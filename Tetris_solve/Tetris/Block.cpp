#include "Block.h"
#include <stdlib.h>

const int BLOCK_SIZE = 36;

IMAGE* Block::imgs[7] = { NULL, };
int Block::size = BLOCK_SIZE;
int Block::blocks[7][4] = {
	1,3,5,7, // I
	2,4,5,7, // Z 1型
	3,5,4,6, // Z 2型
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // 田
};


Block::Block()
{
	// 初始化所有类型方块图片
	if (imgs[0] == NULL) {
		IMAGE imgTmp;
		// 导入待切割图形
		loadimage(&imgTmp, "res\\tiles.png");

		// 开始切割
		SetWorkingImage(&imgTmp);
		for (int i = 0; i < 7; i++) {
			// 为切割方块分配内存
			imgs[i] = new IMAGE;

			// 切割方块， 参数：保存地址， 起点x， 起点y， 宽x， 高y
			getimage(imgs[i], i * size, 0, size, size);
		}
		// 恢复工作区
		SetWorkingImage();
	}



	// 随机生成一种俄罗斯方块

	blockType = 1 + rand() % 7; // 1...7

	// 初始化smallBlocks
	for (int i = 0; i < 4; i++) {
		int value = blocks[blockType - 1][i];

		// 用8块表示法，值除于2就是行，取余2就是列
		smallBlocks[i].row = value / 2;
		smallBlocks[i].col = value % 2;
	}

	img = imgs[blockType - 1];
}

void Block::drop()
{
	for (int i = 0; i < 4; i++) {
		smallBlocks[i].row++;
	}
}

void Block::moveLeftRight(int offset)
{
	for (int i = 0; i < 4; i++) {
		smallBlocks[i].col += offset;
	}
}

void Block::rotate()
{
	// 下面是方块旋转算法

	// 配置旋转中心
	Point p = smallBlocks[1];

	for (int i = 0; i < 4; i++) {
		Point temp = smallBlocks[i];
		smallBlocks[i].col = p.col - temp.row + p.row;
		smallBlocks[i].row = p.row + temp.col - p.col;
	}
}

void Block::draw(int leftMargin, int topMargin)
{
	for (int i = 0; i < 4; i++) {
		int x = leftMargin + smallBlocks[i].col * size;
		int y = topMargin + smallBlocks[i].row * size;

		// 参数 x像素点， y像素点， 图片地址
		putimage(x, y, img);
	}
}

Block Block::operator=(const Block other)
{
	if (this == &other) return *this;
	this->blockType = other.blockType;
	this->img = other.img;

	for (int i = 0; i < 4; i++) {
		this->smallBlocks[i] = other.smallBlocks[i];
	}

	return *this;
}

bool Block::blockInMap(const vector<vector<int>>& map)
{
	int rows = map.size();
	int cols = map[0].size();
	for (int i = 0; i < 4; i++) {
		if (smallBlocks[i].row < 0 || smallBlocks[i].row >= rows ||
			smallBlocks[i].col < 0 || smallBlocks[i].col >= cols ||
			map[smallBlocks[i].row][smallBlocks[i].col] != 0) {
			return false;
		}

	}
	return true;
}

void Block::solidify(vector<vector<int>>& map)
{
	for (int i = 0; i < 4; i++) {
		map[smallBlocks[i].row][smallBlocks[i].col] = blockType;
	}
}

IMAGE** Block::getImage()
{
	return imgs;
}

int Block::getBlockType()
{
	return blockType;
}

void Block::_cheat_changeType(int type)
{
	for (int i = 0; i < 4; i++) {
		int value = blocks[type - 1][i];

		// 用8块表示法，值除于2就是行，取余2就是列
		smallBlocks[i].row = value / 2;
		smallBlocks[i].col = value % 2;
	}
}
