#include <graphics.h>
#include <vector>
using namespace std;
//避免重复定义
#ifndef BLOCK_H
#define BLOCK_H

class Tetris;

struct Point {
	int row;
	int col;
};

class Block
{
public:
	Block();
	void drop();
	void moveLeftRight(int offset);
	void rotate(); // 旋转
	void draw(int leftMargin, int topMargin);
	Block operator=(const Block other);
	bool blockInMap(const vector<vector<int>>& map);
	void solidify(vector<vector<int>>& map);
	static IMAGE** getImage();
	int getBlockType();

	void _cheat_changeType(int type);

private:
	int blockType; // 方块的类型
	Point smallBlocks[4];

	IMAGE* img;
	static IMAGE* imgs[7];
	static int size;
	static int blocks[7][4];
};

#endif
