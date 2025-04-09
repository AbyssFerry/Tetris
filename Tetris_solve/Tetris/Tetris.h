#include <graphics.h>
#include "Block.h"
#include <vector>
using namespace std;

#ifndef TETRIS_H
#define TETRIS_H

class Tetris
{
public:
	Tetris(int rows, int cols, int left, int top, int nleft, int ntop, int blockSize);
	// 初始化
	void init();
	// 开始游戏
	void play();

private:
	void keyEvent();
	void updateWindow();
	// 返回距离上一次调用该函数，间隔了多少时间(ms)
	unsigned long long getDelay();

	void drop();
	void clearLine();
	void moveLeftRight(int offset);
	void rotate();
	void drawScore();
	void checkOver();
	void saveScore();
	void displayOver();
	void pause();
	int delay;

	// 是否渲染
	bool update;

	// 游戏是结束
	bool gameOver;

	// 0:没有方块
	// 5：第5种方块
	vector<vector<int>> map;

	int rows;
	int cols;
	int blockSize;

	// 当前分数
	int score;

	// 操作方块初始生成地点
	int leftMargin;
	int topMargin;

	// 预告方块初始生成地点
	int leftMarginNext;
	int topMarginNext;

	// 当前关卡
	int level;

	// 消除的行数
	int desLineCount;

	// 最高分
	int highestScore;

	// 背景
	IMAGE imgBg;

	// 失败胜利图片
	IMAGE imgOver;
	IMAGE imgWin;

	// 当前以及预告方块
	Block *curBlock;
	Block *nextBlock;

	Block bakBlock;

	void cheat_getStrip();
};

#endif
