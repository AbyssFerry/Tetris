// cpp俄罗斯方块游戏
// 跟着教程敲

/*
	日志：
	2024 5/26: 完成基本API和界面显示
	2024 5/27: 完成到方块移动
	2024 5/28: 除了开头结尾全部完成
*/


#include "Tetris.h"

int main()
{
	Tetris game(20, 10, 263, 133, 689, 133, 36);
	game.play();
	return 0;
}
