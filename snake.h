#ifndef __SNAKE_H
#define __SNAKE_H
 
 
#include "stm32f10x.h"
#include "stdlib.h"
#include "tft2_4.h"
#include "usart1_dma.h"
#include "general_time.h"
 
 
#pragma diag_suppress 177	//屏蔽变量、函数未使用警告
#pragma diag_suppress 550
 
 
#define SNAKE_LENGTH_PIXELS		5		//蛇的其中一节的长度像素（关节是正方形的）
#define SNAKE_INIT_LENGTH		30		//除蛇头外蛇身初始化的关节数量
#define SNAKE_MOVE_DISTANCE		SNAKE_LENGTH_PIXELS		//蛇每走一步的长度，必须为蛇一节长度的整数倍
 
#define SNAKE_COLOR				RED						//蛇体颜色
#define Snake_Erase_COLOR		TFT2_4_Info.bColor		//消除蛇身的颜色
#define SNAKE_HEAD_COLOR		GREEN					//蛇头坐标的颜色
#define SNAKE_DIE_COLOR			YELLOW					//蛇头触碰边界时的颜色
#define FOOD_COLOR				BLUE					//食物颜色
 
#define SNAKE_UP				1						//蛇头方向，依次为上下左右
#define SNAKE_DOWN				2
#define SNAKE_LEFT				3
#define SNAKE_RIGHT				4	
 
//在此可以直接更改游戏区域大小，其余参数可以自适应（其长宽距离必须要是蛇关节长度的倍数）
//												比如：(240-19-20)=201，实际的像素宽度只有200，200是蛇关节像素长度的整数倍
//												比如：(320-19-40)=261，实际的像素宽度只有260，260是蛇关节像素长度的整数倍
#define BORDER_LIMIT_UP			40							//边界上限位
#define BORDER_LIMIT_DOWN		TFT2_4_Info.height - 19		//边界下限位（屏幕长-）（320）
#define BORDER_LIMIT_LEFT		20							//边界左限位
#define BORDER_LIMIT_RIGHT		TFT2_4_Info.width - 19		//边界右限位（屏幕宽-）（240）
#define BORDER_WIDTH			5							//边界宽度
#define BORDER_COLOR			WHITE						//边界颜色
 
#define FOOD_LENGTH_PIXELS		SNAKE_LENGTH_PIXELS		//食物的宽度
 
 
//调用的外部函数,移植基本只需在这改调用到的外部函数
#define Snake_printf 				USART1_printf			//printf函数，蛇信息输出，不需要可以把相关代码注释掉
#define Snake_LCD_ShowNum			TFT2_4_ShowNum			//屏幕显示数字（分数）函数，可能需要修改函数的使用
#define Snake_LCD_String			TFT2_4_String			//屏幕显示字符串函数
#define Snake_LCD_DrawPoint			TFT2_4_DrawPoint		//屏幕显示点函数
#define Snake_LCD_Fill				TFT2_4_Fill				//屏幕指定区域填充颜色函数
#define Snake_LCD_DrawRectangle		TFT2_4_DrawRectangle	//屏幕画矩形函数，此函数是可以画边有厚度的矩形，如不同可能需要修改
 
#define	Snake_FOOD					GENERAL_TIM_GetCapturex(GENERAL_TIM)		//食物坐标随机种子，默认种子为：读取定时器的值
 
 
//蛇的位置信息结构体
typedef struct
{
	u8  sHeadDir;		//蛇头朝向（1上 2下 3左 4右）
	u8  sEndDir;		//蛇尾朝向（1上 2下 3左 4右）【暂时没用到】
	u8  sFraction;		//得分数
	u8  sLength;		//蛇的关节数量
	u8  sLife;			//蛇的生命（0死 1活）
	u16 sHeadx;			//蛇头的坐标值				
	u16 sHeady;			//蛇头的坐标值
	u16 sEndx;			//蛇尾的坐标值			
	u16 sEndy;			//蛇尾的坐标值
	
}_snake_Info;
extern _snake_Info 	snake_Info;
extern _snake_Info 	snake_Info_leader;
 
//游戏边界结构体
typedef struct
{
	u16 xUpLimit;		//上限位
	u16 xDownLimit;		//下限位
	u16 yLeftLimit;		//左限位
	u16 yRightLimit;	//右限位
	
}_border_limit;
extern _border_limit 	border_limit;
 
 
 
//食物结构体
typedef struct
{
	u16 xFood;
	u16 yFood;
	u16 validity;		//食物坐标的合法性（0不合法， 1合法）
	
}_food_Info;
extern _food_Info 	food_Info;
 
 
 
 
 
//static void Delay_ms( u32 ms );
//static void Snake_Draw_Erase_OneJoint( u16 sX, u16 sY, u16 color );
//static void Snake_Draw_Erase(  u16 sX, u16 sY, u8 dir, u16 color );
//static void Snake_Update_HeadXY( u8 i );
//static void Snake_Update_EndXY( u8 i );
//static void Snake_Update_BodyXY(void);
//static void Snake_Display_Init(void);
//static void Border_Display_Init(void);
 
//static u8 Snake_HTW(void);
//static u8 Snake_TouchMyself(void);
//static u8 Snake_Move(void);
void Create_Food(void);
 
void Snake_Game(void);
 
 
 
 
 
#endif /* __SNAKE_H */
 
 