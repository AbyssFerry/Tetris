#include "Tetris.h"
#include <time.h>
#include <stdlib.h>
#include "Block.h"
#include <conio.h>
#include <fstream>
#include <iostream>

//#include <Mmsystem.h>
//#pragma comment(lib, "winmm.lib")
// 导入音效头文件，不过好像不行
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define MAX_LEVEL 5
#define RECORDER_FILE "recorder.txt"

//const int SPEED_NORMAL = 500; //ms
const int SPEED_NORMAL[MAX_LEVEL] = { 500, 300, 150, 100, 80 };
const int SPEED_QUICK = 30;

Tetris::Tetris(int rows, int cols, int left, int top, int nleft, int ntop, int blockSize)
{
	this->rows = rows;
	this->cols = cols;
	this->leftMargin = left;
	this->topMargin = top;
	this->leftMarginNext = nleft;
	this->topMarginNext = ntop;
	this->blockSize = blockSize;

	// 这些变量在之后运用前都会被初始化，所以在这里初始化只是为了避免一些神奇的错误
	this->delay = 0;
	this->update = true;
	this->curBlock = nullptr;
	this->nextBlock = nullptr;

	// 初始化map
	for (int i = 0; i < rows; i++) {
		vector<int> mapRow;
		for (int j = 0; j < cols; j++) {
			mapRow.push_back(0);
		}
		map.push_back(mapRow);

	}
}

void Tetris::init()
{
	// 背景音乐
	mciSendString("open res/bg.mp3", 0, 0, 0);
	mciSendString("play res/bg.mp3 repeat", 0, 0, 0);


	delay = SPEED_NORMAL[0];

	// 设置随机种子
	srand(time(NULL));

	// 创建游戏窗口
	initgraph(938, 896);

	// 加载背景图片
	loadimage(&imgBg, "res/bg2.png");

	// 加载胜利失败图片
	loadimage(&imgWin, "res/win.png");
	loadimage(&imgOver, "res/over.png");

	// 初始化游戏区数据
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			map[i][j] = 0;
		}
	}

	// 创建初始的当前和预告方块
	nextBlock = new Block;
	curBlock = nextBlock;
	nextBlock = new Block;

	// 初始化分数
	score = 0;

	// 初始化等级,消除的行,最高分数
	level = 1;
	desLineCount = 0;
	highestScore = 0;

	// 初始化游戏是否结束
	gameOver = false;

	// 导入最高分
	ifstream file(RECORDER_FILE);
	if (!file.is_open()) {
		cout << RECORDER_FILE << "打开失败" << endl;
		highestScore = 0;
	}
	else {
		file >> highestScore;
	}
	file.close();
}

void Tetris::play()
{
	init();


	int timer = 0;
	while (1) {
		// 接受用户输入
		keyEvent();

		//设置刷新间隔
		timer += getDelay();
		if (timer > delay) {
			timer = 0;

			// 方块下降
			drop();

			// 检查游戏是否结束
			checkOver();

			// 清除行
			clearLine();
			update = true;
		}

		// 判断游戏是否结束
		if (gameOver) {
			// 保存分数
			saveScore();

			// 游戏结束画面
			displayOver();

			// 重新开始游戏
			system("pause");
			init();
		}

		if (update) {
			update = false;
			// 渲染游戏画面
			updateWindow();

			// 更新游戏数据

		}



	}
}


// 终端必须在上层才可以读取键盘输入， 后期优化 @@
void Tetris::keyEvent()
{
	unsigned char ch;

	// kbhit()只有用于输入了才为真
	if (_kbhit())
	{
		// 直接读取用户输入（不用回车）
		ch = _getch();


		// 是否执行旋转
		bool rotateFlag = false;

		// 移动偏量
		int dx = 0;

		// 用户按下方向键会返回两个字符
		// 如果按下 向上方向键，会先后返回：224 72
		// 如果按下 向下方向键，会先后返回：224 80
		// 如果按下 向左方向键，会先后返回：224 75
		// 如果按下 向右方向键，会先后返回：224 77

		if (ch == 224) {
			ch = _getch();
			switch (ch) {
			case 72:
				rotateFlag = true;
				break;
			case 80:
				delay = SPEED_QUICK;
				break;
			case 75:
				dx = -1;
				break;
			case 77:
				dx = 1;
				break;
			default:
				break;
			}

			if (rotateFlag) {
				rotate();
				update = true;
			}

			if (dx != 0) {
				moveLeftRight(dx);
				update = true;
			}
		}
		else if(ch == 32) {
			pause();
			update = true;
		}
		else if(ch == 'I'){
			cheat_getStrip();
		}
	}

}

void Tetris::updateWindow()
{
	// 绘制背景图片
	putimage(0, 0, &imgBg);

	// 获得map里面数值所对应的方块类型
	IMAGE** ings = Block::getImage();

	// 开始画一批
	BeginBatchDraw();

	// 绘制已经固化的方块
	for (int i = 0; i < rows; i++) {
		;
		for (int j = 0; j < cols; j++) {
			if (map[i][j] == 0) continue;
			int x = j * blockSize + leftMargin;
			int y = i * blockSize + topMargin;
			putimage(x, y, ings[map[i][j] - 1]);
		}
	}


	// 绘制当前和预告方块
	curBlock->draw(leftMargin, topMargin);
	nextBlock->draw(leftMarginNext, topMarginNext);

	// 绘制分数
	drawScore();

	//输出画的全部
	EndBatchDraw();
}

// 第一次返回0
// 返回距离上一次，间隔了多少时间 (ms)
unsigned long long Tetris::getDelay()
{
	static unsigned long long lastTime = 0;
	unsigned long long currentTime = GetTickCount();

	if (lastTime == 0) {
		lastTime = currentTime;
		return 0;
	}
	else {
		unsigned long long ret = currentTime - lastTime;
		lastTime = currentTime;
		return ret;
	}
}

void Tetris::drop()
{
	bakBlock = *curBlock;
	curBlock->drop();

	if (!curBlock->blockInMap(map)) {
		// 固化方块
		bakBlock.solidify(map);

		delete curBlock;
		curBlock = nextBlock;
		nextBlock = new Block;

	}

	// 按了 向下方向键后要恢复速度
	delay = SPEED_NORMAL[level - 1];
}

void Tetris::clearLine()
{
	// 记录满的行数
	int fullLines = 0;

	int k = rows - 1;
	for (int i = rows - 1; i >= 0; i--) {
		// 检测i行是否满行
		int count = 0;
		for (int j = 0; j < cols; j++) {
			if (map[i][j]) count++;
			// 边读边存
			map[k][j] = map[i][j];
		}
		// 行没满
		if (count < cols) {
			k--;
		}
		else {
			fullLines++;
		}
	}

	if (fullLines > 0) {

		// 加分, 消除的行平方乘十
		score += fullLines * fullLines * 10;

		// 播放音效，头文件用不了
		mciSendString("play res/xiaochu1.mp3", 0, 0, 0);


		// 每100分一个级别 101-200第二关
		level = (score + 99) / 100;
		if (level > 5) {
			gameOver = true;
		}

		// 设置消除了多少行
		desLineCount += fullLines;
	}
}

void Tetris::moveLeftRight(int offset)
{
	bakBlock = *curBlock;
	curBlock->moveLeftRight(offset);

	// 越界检测
	if (!curBlock->blockInMap(map)) {

		// 复原
		*curBlock = bakBlock;
	}
}

void Tetris::rotate()
{
	// 如果是田方块旋转没有意义
	if (curBlock->getBlockType() == 7) return;

	bakBlock = *curBlock;
	curBlock->rotate();

	// 越界检测
	if (!curBlock->blockInMap(map)) {

		// 复原
		*curBlock = bakBlock;
	}
}

void Tetris::drawScore()
{
	// 看不懂
	char scoreText[32];
	sprintf_s(scoreText, sizeof(scoreText), "%d", score);


	setcolor(RGB(180, 180, 180));

	LOGFONT f;
	gettextstyle(&f);
	f.lfHeight = 60;
	f.lfWeight = 30;

	// 消除锯齿
	f.lfQuality = ANTIALIASED_QUALITY;
	strcpy_s(f.lfFaceName, sizeof(f.lfFaceName), _T("Segoe UI Black"));
	settextstyle(&f);

	// 设置背景为透明
	setbkmode(TRANSPARENT);

	// 绘制分数
	outtextxy(670, 727, scoreText);

	// 绘制消除了多少行
	sprintf_s(scoreText, sizeof(scoreText), "%d", desLineCount);
	gettextstyle(&f);
	int xPos = 224 - f.lfWeight * strlen(scoreText);
	outtextxy(xPos, 817, scoreText);

	// 绘制关卡级别
	sprintf_s(scoreText, sizeof(scoreText), "%d", level);
	outtextxy(194, 727, scoreText);

	// 绘制最高分数
	sprintf_s(scoreText, sizeof(scoreText), "%d", highestScore);
	outtextxy(670, 817, scoreText);
}

void Tetris::checkOver()
{
	gameOver = (curBlock->blockInMap(map) == false);
}

void Tetris::saveScore()
{
	if (score > highestScore) {
		highestScore = score;
		ofstream file(RECORDER_FILE);
		file << highestScore;
		file.close();
	}
}

void Tetris::displayOver()
{
	mciSendString("stop res/bg.mp3", 0, 0, 0);

	// 绘制胜利失败图片
	if (level <= MAX_LEVEL) {

		putimage(262, 361, &imgOver);
		mciSendString("play res/over.mp3", 0, 0, 0);
	}
	else {
		putimage(262, 361, &imgWin);
		mciSendString("play res/win.mp3", 0, 0, 0);		
	}
}

void Tetris::pause()
{
	// 等待读取到空格就结束
	while(1) {
		if(_getch() == 32) return;
	}
}

void Tetris::cheat_getStrip()
{
	nextBlock->_cheat_changeType(1);
	
}
