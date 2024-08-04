#include "snake.h"
 
_border_limit 	border_limit;
_food_Info 		food_Info;
_snake_Info 	snake_Info;			//蛇的身体信息
_snake_Info 	snake_Info_next;	//蛇下一坐标信息（存放蛇下一步要走的位置）
 
//	( BORDER_LIMIT_DOWN - BORDER_LIMIT_UP - 1 ) / SNAKE_LENGTH_PIXELS *
//	( BORDER_LIMIT_RIGHT - BORDER_LIMIT_LEFT - 1 ) / SNAKE_LENGTH_PIXELS;	蛇身数组大小为这两个的乘积
//int	snake_body [2080][2] = {0};			//蛇全身坐标
 
u8 keyFlag;				//按键
u8 startGame;			//开始游戏标志位（0不可以开始 1可以开始）
u8 speed = 50;
 
struct List* snake_list;		// 创建双向循环链表
 
 
//链表结构体，节点
struct Node
{
	u16 x;
	u16 y;
	struct Node* left;
	struct Node* right;
 
};
 
 
//创建节点
struct Node* createNode( u16 x, u16 y )
{
	struct Node* newNode = (struct Node*)malloc( sizeof(struct Node) );
	newNode->x = x;
	newNode->y = y;
	newNode->left = newNode->right = NULL;
	
	return newNode;
};
 
 
//链表
//用指针来存储链表哪一个节点是第一、最后一个节点
struct List
{
	int size;
	
	struct Node* firstNode;		//第一个节点
	struct Node* lastNode;		//最后一个节点
};
 
 
//创造双向链表 
static struct List* createList()
{
	//申请内存
	struct List* list = (struct List*)malloc( sizeof(struct List) );
	//初始化变量
	list->size = 0;
	list->firstNode = list->lastNode = NULL;
	
	return list;
}
 
 
//求大小,判断是否为NULL
static u8 listSize( struct List* list )
{
	return list->size;
}
static u8 emptyList( struct List* list )
{
	if( list->size == 0 )
		return 0;
	else
		return 1;
}
//检查链表中数据是否有与传入的数据相同，0 没有相同的数据，1 有相同的数据
static u8 listCheck(  struct List* list, u16 x, u16 y )
{
	if( list->size == 0 )
		Snake_printf( "无法比较，链表为空\n" );
	else
	{
		struct Node* pMove = list->firstNode;		//创建移动链表指针
		list->lastNode->right = NULL;				//断开双向循环链表
		while( pMove )
		{
			if( pMove->x == x && pMove->y == y )
			{
				list->lastNode->right = list->firstNode;
				return 1;							//重新建立双向循环链表,并返回1
			}
			else
				pMove = pMove->right;
		}
 
		list->lastNode->right = list->firstNode;	//重新建立双向循环链表,并返回0
		return 0;
	}
	return NULL;
}
 
 
//打印链表
static void printfList( struct List* list )
{
	if( list->size == 0 )
		Snake_printf( "无法打印，链表为空\n" );
	else
	{
		struct Node* pMove = list->firstNode;
		list->lastNode->right = NULL;				//断开双向循环链表
		while( pMove )
		{
			Snake_printf( "%d, %d \n", pMove->x, pMove->y );
			pMove = pMove->right;
		}
		Snake_printf( "打印完成\n\n" );
		list->lastNode->right = list->firstNode;	//重新建立双向循环链表
	}
}
 
 
//销毁双向链表
static void ListDestory( struct List* list )
{
	if( list->size == 0)
		Snake_printf( "无法销毁，链表为空\n" );
	else
	{
		struct Node* pMove = list->firstNode;
		list->lastNode->right = NULL;	//断开双向循环链表
		while( pMove )
		{
			Snake_printf( "%d, %d  销毁\n", pMove->x, pMove->y );
			free( pMove );
			pMove = pMove->right;
		}
		Snake_printf( "销毁完成，链表已为空\n\n" );
	}
}
 
 
//表头插入
static void InsertNodeByHead( struct List* list, u16 x, u16 y )
{
	struct Node* newNode = createNode( x, y );
 
	if( list->firstNode == NULL )		//如果链表为空，则头尾节点都是新节点
	{
		list->lastNode = newNode;
	}
	else	//否则则头节点的左边是新节点，新节点右边是头节点
	{
		list->firstNode->left = newNode;
		newNode->right = list->firstNode;
	}
	
	list->firstNode = newNode;					//设新节点为头节点
	list->firstNode->left = list->lastNode;		//建立表头表尾循环
	list->lastNode->right = list->firstNode;
	
	list->size++;
}
 
 
//表尾插入
static void InsertNodeByTail( struct List* list, u16 x, u16 y )
{
	struct Node* newNode = createNode( x, y );
 
	if( list->lastNode == NULL )		//如果链表为空，则头尾节点都是新节点
	{
		list->firstNode = newNode;
	}
	else	//否则则尾节点的右边是新节点，新节点左边是尾节点
	{
		list->lastNode->right = newNode;
		newNode->left = list->lastNode;
	}
	
	list->lastNode = newNode;					//设新节点为尾节点
	list->firstNode->left = list->lastNode;		//建立表头表尾循环
	list->lastNode->right = list->firstNode;
	
	list->size++;
}
 
 
//双向链表移动，原表尾移动为新表头，并重新赋值	
static void ListTailMoveToHead( struct List* list, u16 x, u16 y )
{
	list->firstNode = list->lastNode;			//把表头赋值为现表尾
	list->lastNode = list->lastNode->left;		//把表尾重新赋值为现表尾的左边那个节点
 
	list->firstNode->x = x;		//对新表头重新赋值
	list->firstNode->y = y;
}
 
 
// 本地MS粗略延时函数，减少移植时对外部文件依赖；
static void Delay_ms( u32 ms )
{
    ms=ms*6500;                  
    for(u32 i=0; i<ms; i++);	// 72MHz系统时钟下，多少个空循环约耗时1ms
}
 
 
/*****************************************************************
 * 函  数： Snake_Draw_Erase_OneJoint
 * 功  能： 蛇的一个关节/食物绘画，一个关节/食物擦除
 * 参  数： u16 sX, sY			起点坐标	
 *          u16 color			绘画，或者擦除成的颜色	
 *
 * 返回值： 无
 *
 * 备  注： 
******************************************************************/
static void Snake_Draw_Erase_OneJoint( u16 sX, u16 sY, u16 color )
{
	u16 sX2, sY2;
 
	sX2 = sX + SNAKE_LENGTH_PIXELS-1;
	sY2 = sY + SNAKE_LENGTH_PIXELS-1;
 
	Snake_LCD_Fill( sX, sY, sX2, sY2, color );
}
 
 
/*****************************************************************
 * 函  数： Snake_Draw_Erase
 * 功  能： 绘画，擦除蛇一个关节的一格像素
 * 参  数： u16 sX, sY			起点坐标	
 *          u16 color			绘画，或者擦除成的颜色	
 *
 * 返回值： 无
 *
 * 备  注： 为了蛇移动画面显示流畅，所以一个关节画一个像素头边擦除一个尾边
******************************************************************/
static void Snake_Draw_Erase(  u16 sX, u16 sY, u8 dir, u16 color )
{
	u16 sX2 = sX, sY2 = sY;
 
	if( dir == 3 || dir == 4 )
		sY2 = sY + SNAKE_LENGTH_PIXELS-1;
	else
		sX2 = sX + SNAKE_LENGTH_PIXELS-1;
	
	Snake_LCD_Fill( sX, sY, sX2, sY2, color );
}
 
 
/*****************************************************************
 * 函  数： Snake_Update_HeadXY
 * 功  能： 更新蛇头显示，显示蛇头
 * 参  数： u8 i		显示蛇头关节的第几个像素块	
 *
 * 返回值： 无
 *
 * 备  注：
******************************************************************/
static void Snake_Update_HeadXY( u8 i )
{
	if( snake_Info.sHeadDir == 4 )				//蛇头向左
		Snake_Draw_Erase( snake_Info_next.sHeadx + i, snake_Info_next.sHeady, snake_Info.sHeadDir, SNAKE_COLOR );	//显示蛇头
 
	else if( snake_Info.sHeadDir == 3 )			//蛇头向右
		Snake_Draw_Erase( snake_Info_next.sHeadx + SNAKE_MOVE_DISTANCE - i -1, snake_Info_next.sHeady, snake_Info.sHeadDir, SNAKE_COLOR );
 
	else if( snake_Info.sHeadDir == 2 )			//蛇头向下
		Snake_Draw_Erase(  snake_Info_next.sHeadx, snake_Info_next.sHeady + i, snake_Info.sHeadDir, SNAKE_COLOR );
 
	else if( snake_Info.sHeadDir == 1 )			//蛇头向上
		Snake_Draw_Erase(  snake_Info_next.sHeadx, snake_Info_next.sHeady + SNAKE_MOVE_DISTANCE - i -1, snake_Info.sHeadDir, SNAKE_COLOR );
}
 
 
/*****************************************************************
 * 函  数： Snake_Update_EndXY
 * 功  能： 更新蛇尾显示，擦除蛇尾
 * 参  数： u8 i		擦除蛇尾关节的第几个像素块	
 *
 * 返回值： 无
 *
 * 备  注：
******************************************************************/
static void Snake_Update_EndXY( u8 i )		//更新蛇尾显示，擦除蛇尾
{
//	if( snake_body[snake_Info.sLength-2][1] == snake_Info.sEndy )			// 蛇尾如果和上一节蛇身在同一y轴，则是往左右走
//	{
//		if( snake_body[snake_Info.sLength-2][0] - snake_Info.sEndx > 0 )	// 如果蛇尾的上一节蛇身x坐标减去蛇尾x坐标大于0，则是左走
//			Snake_Draw_Erase( snake_Info.sEndx + i, snake_Info.sEndy, 4, Snake_Erase_COLOR );		//擦除蛇尾
//		else																// 反之右走
//			Snake_Draw_Erase( snake_Info.sEndx + SNAKE_MOVE_DISTANCE - i -1, snake_Info.sEndy, 3, Snake_Erase_COLOR );	//擦除蛇尾
//	}
//	else																	// 蛇尾如果和上一节蛇身在同一x轴，则是往左右走
//	{
//		if( snake_body[snake_Info.sLength-2][1] - snake_Info.sEndy > 0 )	// 如果蛇尾的上一节蛇身y坐标减去蛇尾y坐标大于0，则是左走下走
//			Snake_Draw_Erase( snake_Info.sEndx , snake_Info.sEndy + i, 2, Snake_Erase_COLOR );		//擦除蛇尾
//		else																// 反之上走
//			Snake_Draw_Erase( snake_Info.sEndx, snake_Info.sEndy + SNAKE_MOVE_DISTANCE - i -1, 1, Snake_Erase_COLOR );	//擦除蛇尾
//	}
	
	if( snake_list->lastNode->y == snake_list->lastNode->left->y )			// 蛇尾如果和上一节蛇身在同一y轴，则是往左右走
	{
		if( snake_list->lastNode->left->x - snake_list->lastNode->x > 0 )	// 如果蛇尾的上一节蛇身x坐标减去蛇尾x坐标大于0，则是左走
			Snake_Draw_Erase( snake_Info.sEndx + i, snake_Info.sEndy, 4, Snake_Erase_COLOR );		//擦除蛇尾
		else																// 反之右走
			Snake_Draw_Erase( snake_Info.sEndx + SNAKE_MOVE_DISTANCE - i -1, snake_Info.sEndy, 3, Snake_Erase_COLOR );	//擦除蛇尾
	}
	else																	// 蛇尾如果和上一节蛇身在同一x轴，则是往左右走
	{
		if( snake_list->lastNode->left->y - snake_list->lastNode->y > 0 )	// 如果蛇尾的上一节蛇身y坐标减去蛇尾y坐标大于0，则是左走下走
			Snake_Draw_Erase( snake_Info.sEndx , snake_Info.sEndy + i, 2, Snake_Erase_COLOR );		//擦除蛇尾
		else																// 反之上走
			Snake_Draw_Erase( snake_Info.sEndx, snake_Info.sEndy + SNAKE_MOVE_DISTANCE - i -1, 1, Snake_Erase_COLOR );	//擦除蛇尾
	}
}
 
 
/*****************************************************************
 * 函  数： Snake_Update_BodyXY
 * 功  能： 更新蛇身坐标
 * 参  数：
 *
 * 返回值： 无
 *
 * 备  注：蛇每移动完一个关节，更新一次蛇身坐标
******************************************************************/
static void Snake_Update_BodyXY(void)
{					//		移动前						移动后					吃到食物增加关节
					//  4   3   2   1   0	 |     4   3   2   1   0		  5   4   3   2   1   0
//	int i;			// 0.1 0.2 0.3 0.4 0.5   |    0.2 0.3 0.4 0.5 0.5        0.1 0.2 0.3 0.4 0.5 0.5
	
//	for( i=snake_Info.sLength-1; i>0; i-- )    	//从尾巴开始，每一个点的位置等于它前面一个点的位置,蛇头在数组[0][]
//	{
//		snake_body[i][0] = snake_body[i-1][0];
//		snake_body[i][1] = snake_body[i-1][1];
//	}
			
	//更新蛇头坐标,把确认可以走的下一步蛇头坐标赋值给现蛇头坐标和蛇身数组
//	snake_body[0][0] = snake_Info.sHeadx = snake_Info_next.sHeadx;
//	snake_body[0][1] = snake_Info.sHeady = snake_Info_next.sHeady;
	
//	snake_Info.sEndx = snake_body[snake_Info.sLength-1][0];		//更新现蛇尾坐标
//	snake_Info.sEndy = snake_body[snake_Info.sLength-1][1];
	
 
	
	if( snake_list->size < snake_Info.sLength )			//如果吃到食物，则只增加表头
		InsertNodeByHead( snake_list, snake_Info_next.sHeadx, snake_Info_next.sHeady );
	else
		//更新蛇头蛇尾链表坐标，消除原蛇尾坐标，把链表表尾移到表头，并赋值为新蛇头坐标
		ListTailMoveToHead( snake_list, snake_Info_next.sHeadx, snake_Info_next.sHeady );	
	
	snake_Info.sHeadx = snake_Info_next.sHeadx;		//更新现蛇头坐标
	snake_Info.sHeady = snake_Info_next.sHeady;	
	
	snake_Info.sEndx = snake_list->lastNode->x;		//更新现蛇尾坐标
	snake_Info.sEndy = snake_list->lastNode->y;
}
 
 
/*****************************************************************
 * 函  数： Snake_Display_Init
 * 功  能： 蛇的信息及显示初始化
 * 参  数：
 *
 * 返回值： 无
 *
 * 备  注：如果改蛇初始化位置及方向状态则在这改
******************************************************************/
static void Snake_Display_Init(void)
{
	Snake_printf( "*****! 正在初始化蛇体信息 !*****\n" );
	
	snake_Info.sHeadDir = 4;		//初始化蛇头朝向（1上 2下 3左 4右）
	snake_Info.sEndDir = 4;			//初始化蛇尾朝向（1上 2下 3左 4右）
	snake_Info.sLife = 1;			//初始化蛇生命
	snake_Info.sFraction = 0;		//得分初始化
	food_Info.validity = 0;			//食物初始化
	
	snake_Info.sLength = SNAKE_INIT_LENGTH + 1 ;			//蛇身长度
	
	//蛇头蛇尾坐标初始化(一开始的位置在左上角，往右走)
	snake_Info.sHeadx = border_limit.yLeftLimit + SNAKE_LENGTH_PIXELS*(1+SNAKE_INIT_LENGTH) + SNAKE_LENGTH_PIXELS + 1; 
	snake_Info.sHeady = border_limit.xUpLimit + SNAKE_LENGTH_PIXELS + 1;
	snake_Info.sEndx = snake_Info.sHeadx;
	snake_Info.sEndy = snake_Info.sHeady;
	
	snake_Info_next.sHeadx = snake_Info.sHeadx;						
	snake_Info_next.sHeady = snake_Info.sHeady;
	
//	snake_body[0][0] = snake_Info.sHeadx;	// 蛇头坐标，x
//	snake_body[0][1] = snake_Info.sHeady;	// y坐标
	
	snake_list = createList();		// 创建双向循环链表
	InsertNodeByHead( snake_list, snake_Info.sHeadx, snake_Info.sHeady );		//表头插入蛇头位置
 
	//增加蛇头（此关节是必需的）
	Snake_Draw_Erase_OneJoint( snake_Info.sHeadx, snake_Info.sHeady, SNAKE_COLOR );
	
	//增加 SNAKE_INIT_LENGTH 个关节
	for( int count=1; count <= SNAKE_INIT_LENGTH; count++ )
	{
		snake_Info.sEndx -= SNAKE_LENGTH_PIXELS;
//		snake_body[count][0] = snake_Info.sEndx;
//		snake_body[count][1] = snake_Info.sEndy;
		InsertNodeByTail( snake_list, snake_Info.sEndx, snake_Info.sEndy );		//表尾循环插入蛇身数据
		Snake_Draw_Erase_OneJoint( snake_Info.sEndx, snake_Info.sEndy, SNAKE_COLOR );	//添加蛇身
	}
 
	//蛇身信息
	Snake_printf( "** 蛇头坐标信息：x=%d, y=%d\n", snake_Info.sHeadx, snake_Info.sHeady );
	Snake_printf( "** 蛇尾坐标信息：x=%d, y=%d\n", snake_Info.sEndx, snake_Info.sEndy );
	Snake_printf( "** 蛇身长度：%d节\n", snake_Info.sLength );
	Snake_printf( "** 蛇头朝向：%d\n", snake_Info.sHeadDir );
	
	//分数初始化显示
	Snake_LCD_ShowNum( border_limit.yRightLimit/2, border_limit.xUpLimit-30, snake_Info.sFraction, 24, WHITE, BLACK );
 
	Snake_printf( "*****! 蛇体信息初始化完成 !*****\n\n" );
}
 
 
/*****************************************************************
 * 函  数： Border_Display_Init
 * 功  能： 游戏边界初始化及显示
 * 参  数：
 *
 * 返回值： 无
 *
 * 备  注：
******************************************************************/
static void Border_Display_Init(void)
{
	Snake_printf("*****! 正在初始化游戏界面 !*****\n");
 
	border_limit.xUpLimit = BORDER_LIMIT_UP;
	border_limit.xDownLimit = BORDER_LIMIT_DOWN;	
	border_limit.yLeftLimit = BORDER_LIMIT_LEFT;
	border_limit.yRightLimit = BORDER_LIMIT_RIGHT;
	
	//游戏界面检查（界面长宽距离必须要是蛇关节长度的倍数）
	if( (BORDER_LIMIT_DOWN - BORDER_LIMIT_UP - 1) % SNAKE_LENGTH_PIXELS != 0 ||
	    (BORDER_LIMIT_RIGHT - BORDER_LIMIT_LEFT - 1) % SNAKE_LENGTH_PIXELS != 0 )
	{
		Snake_printf("xxxxx! 游戏界面初始化失败 !xxxxx\n");
		while(1);
	}
	
	Snake_LCD_DrawRectangle	( border_limit.yLeftLimit,
							  border_limit.xUpLimit,  
							  border_limit.yRightLimit, 
						      border_limit.xDownLimit, BORDER_WIDTH, 0, BORDER_COLOR );
	
	Snake_printf("*****! 游戏界面初始化完成 !*****\n\n");
}
 
 
/*****************************************************************
 * 函  数： Snake_HTW
 * 功  能： 判断蛇下一步是否撞墙
 * 参  数：
 *
 * 返回值： 0死亡	1正常
 *
 * 备  注：
******************************************************************/
static u8 Snake_HTW(void)
{
	u8 row, col;
	
	if( snake_Info_next.sHeady <= border_limit.xUpLimit   || 
		snake_Info_next.sHeady >= border_limit.xDownLimit ||
		snake_Info_next.sHeadx <= border_limit.yLeftLimit ||
		snake_Info_next.sHeadx >= border_limit.yRightLimit )
	{
		Snake_printf("触碰边界，死亡！\n");
		
		//显示撞墙蛇头
		Snake_Draw_Erase_OneJoint( snake_Info.sHeadx, snake_Info.sHeady, SNAKE_DIE_COLOR );
		
		col = ( BORDER_LIMIT_RIGHT - BORDER_LIMIT_LEFT - 1 - 9*12 )/2 + 9;
		row = ( BORDER_LIMIT_DOWN - BORDER_LIMIT_UP - 1 )/2 + 16;
		
		Snake_LCD_String( col, row, "Game Over", 32, WHITE, BLACK );
 
		//更新蛇生命状态
		snake_Info.sLife = 0;
	}
	
	return snake_Info.sLife;
}
 
 
/*****************************************************************
 * 函  数： Snake_TouchMyself
 * 功  能： 判断蛇是否吃到自己
 * 参  数：
 *
 * 返回值： 0死亡	1正常
 *
 * 备  注：
******************************************************************/
static u8 Snake_TouchMyself(void)
{
	u8 snakeSta = 1, row, col;
	
//	for( int i=0; i<snake_Info.sLength; i++ )    	//遍历蛇身数组，检查蛇头是否碰到蛇身
//	{
//		if( snake_Info_next.sHeadx == snake_body[i][0] && snake_Info_next.sHeady == snake_body[i][1] )
//		{
//			snakeSta = 0;
//			break;
//		}
//	}
	
	//遍历链表数据，检查蛇头是否碰到蛇身，因为返回值1为有重复值，所以1-运算，赋值为0
	snakeSta = 1 - listCheck( snake_list, snake_Info_next.sHeadx, snake_Info_next.sHeady );
	
	if(snakeSta == 0)
	{	
		Snake_printf("吃到自己，死亡！\n");
		
		//显示吃到自己的蛇头
		Snake_Draw_Erase_OneJoint( snake_Info.sHeadx, snake_Info.sHeady, SNAKE_DIE_COLOR );
		
		col = ( BORDER_LIMIT_RIGHT - BORDER_LIMIT_LEFT - 1 - 9*12 )/2 + 9;
		row = ( BORDER_LIMIT_DOWN - BORDER_LIMIT_UP - 1 )/2 + 16;
		
		Snake_LCD_String( col, row, "Game Over", 32, WHITE, BLACK );
		
		//更新蛇生命状态
		snake_Info.sLife = 0;
	}	
	
	return snake_Info.sLife;
}
 
 
/*****************************************************************
 * 函  数： Snake_Move
 * 功  能： 蛇身移动
 * 参  数：
 *
 * 返回值： 0死亡	1正常
 *
 * 备  注：
******************************************************************/
static u8 Snake_Move(void)
{
	u8 upDateSnakeEnd = 0;	//更新尾巴标志位（0更新，1不更新）
	
//	for( int i=0; i<snake_Info.sLength; i++ )	//打印全部蛇身
//	{
//		Snake_printf( "%d: %d , %d\n\n", i, snake_body[i][0], snake_body[i][1] );
//	}
	
	//蛇当前信息
	Snake_printf( "*****! 移动信息 !*****\n");
	Snake_printf( "** 蛇头坐标信息：x = %d, y = %d\n", snake_Info.sHeadx, snake_Info.sHeady );
	Snake_printf( "** 蛇尾坐标信息：x = %d, y = %d\n", snake_Info.sEndx, snake_Info.sEndy );
	Snake_printf( "** 蛇身长度：%d节\n", snake_Info.sLength );
	Snake_printf( "** 蛇头朝向：%d\n", snake_Info.sHeadDir );
	Snake_printf( "** 当前食物信息：x = %d, y = %d\n", food_Info.xFood, food_Info.yFood );
	Snake_printf( "**********************\n\n");
	
	//刷新分数界面
	Snake_LCD_ShowNum( border_limit.yRightLimit/2, border_limit.xUpLimit-30, snake_Info.sFraction, 24, WHITE, BLACK );
	
	//判断蛇下一步坐标是否会撞墙
	u8 SnakeSta = Snake_HTW();
	if( SnakeSta == 0 )
		return SnakeSta;	
 
	//判断先导坐标是否会吃到自己
	SnakeSta = Snake_TouchMyself();
	if( SnakeSta == 0 )
		return SnakeSta;
	
	//判断蛇下一步坐标是否与食物一样，代表吃到食物，则此次将不删除尾巴
	if( snake_Info_next.sHeadx == food_Info.xFood && snake_Info_next.sHeady == food_Info.yFood )
		upDateSnakeEnd = 1;	
	
	//更新蛇的显示，为了显示流畅，选择显示一格蛇头消除一格蛇尾，循环一节蛇身次
	for( int i=0; i<SNAKE_MOVE_DISTANCE; i++ )
	{
		Snake_Update_HeadXY( i );		//更新蛇头	
		
		if( upDateSnakeEnd == 0 )		//如果吃到食物则不更新蛇尾
			Snake_Update_EndXY( i );
		
		Delay_ms( speed );				//延时
	}
	
	Snake_LCD_DrawPoint( snake_Info_next.sHeadx, snake_Info_next.sHeady, SNAKE_HEAD_COLOR );	//在蛇头位置画点	
	
	if(upDateSnakeEnd == 1)
	{	
		snake_Info.sLength++;		//更新蛇身长度
		snake_Info.sFraction++;		//加分数
		food_Info.validity = 0;		//更新食物状态
		
		if( snake_Info.sFraction%2 == 0 && speed>0 )	//分数每多2分速度增加
			speed--;
	}
		
	//更新蛇身坐标
	Snake_Update_BodyXY();
	
	return 1;
}
 
 
/*****************************************************************
 * 函  数： Create_Food
 * 功  能： 生成食物
 * 参  数：
 *
 * 返回值： 无
 *
 * 备  注：
******************************************************************/
void Create_Food(void)
{
	uint16_t temp1=0, rowTotal, colTotal, row, col;
 
	if( food_Info.validity == 0 )		//如果食物已经被吃掉
	{
		//计算食物的行列范围
		colTotal = ( BORDER_LIMIT_RIGHT - BORDER_LIMIT_LEFT - 1 ) / FOOD_LENGTH_PIXELS;
		rowTotal = ( BORDER_LIMIT_DOWN - BORDER_LIMIT_UP - 1 ) / FOOD_LENGTH_PIXELS;		
		
		do
		{
			temp1 += Snake_FOOD;					//读取定时器的值
	
			//随机生成某行某列
			srand( temp1 );							//随机种子
			
			col = rand() % colTotal;
			row = rand() % rowTotal;
			
			//根据某行某列计算出食物的实际像素坐标		
			food_Info.xFood = (col * FOOD_LENGTH_PIXELS) + BORDER_LIMIT_LEFT + 1;
			food_Info.yFood = (row * FOOD_LENGTH_PIXELS) + BORDER_LIMIT_UP + 1;
			
			food_Info.validity = 1;			//已生成食物
			
			Snake_printf( "xfood = %d\n", food_Info.xFood );
			Snake_printf( "yfood = %d\n", food_Info.yFood );
			
			//检查随机数是否会超出边界
			if( food_Info.xFood <= BORDER_LIMIT_LEFT 	|| 
			    food_Info.xFood >= BORDER_LIMIT_RIGHT	||
			    food_Info.yFood <= BORDER_LIMIT_UP		||
			    food_Info.yFood >= BORDER_LIMIT_DOWN	)
			{
				food_Info.validity = 0;
			}
			else 
			{
//				for( int i=0; i<snake_Info.sLength; i++ )    	//遍历蛇身数组，检查随机数是否与蛇体重合
//				{
//					if( snake_body[i][0] == food_Info.xFood && snake_body[i][1] == food_Info.yFood )
//					{
//						food_Info.validity = 0;		//随机位置与蛇体重合，重新生成食物
//						continue;
//					}
//				}
				
				//遍历链表数据，检查随机数是否与蛇体重合，因为返回值1为有重复值，所以1-运算，赋值为0
				food_Info.validity = 1 - listCheck( snake_list, food_Info.xFood, food_Info.yFood );
			}
			
		}while( food_Info.validity == 0 );
		
		Snake_Draw_Erase_OneJoint( food_Info.xFood, food_Info.yFood, FOOD_COLOR );		//生成新的食物
		
//		Snake_LCD_DrawPoint( food_Info.xFood, food_Info.yFood, GREEN );
	}	
}
 
 
/*****************************************************************
 * 函  数： Snake_Game
 * 功  能： 贪吃蛇游戏主运行函数
 * 参  数：
 *
 * 返回值： 无
 *
 * 备  注：
******************************************************************/
void Snake_Game(void)
{
	u8 snakeSta = 1;
	startGame = 0;
	
	Border_Display_Init();			//初始化游戏边界
	Snake_Display_Init();			//初始化蛇体
	
	while( startGame == 1 );		//按下key_up键游戏开始
	keyFlag = snake_Info.sHeadDir;
	
	while(1)
	{
		while( snakeSta != 0 )			
		{	
			switch(keyFlag)
			{
				case SNAKE_UP:				//向上走
					if( snake_Info.sHeadDir != 2 )
					{
						snake_Info_next.sHeady -= SNAKE_MOVE_DISTANCE;
						snake_Info.sHeadDir = 1;
					} else {
						keyFlag = 2;
						continue;
					}
					break;
					
				case SNAKE_DOWN:			//向下走
					if( snake_Info.sHeadDir != 1 )
					{
						snake_Info_next.sHeady += SNAKE_MOVE_DISTANCE;
						snake_Info.sHeadDir = 2;
					} else {
						keyFlag = 1;
						continue;
					}
					break;
					
				case SNAKE_LEFT:			//向左走
					if( snake_Info.sHeadDir != 4 )
					{
						snake_Info_next.sHeadx -= SNAKE_MOVE_DISTANCE;
						snake_Info.sHeadDir = 3;
					} else {
						keyFlag = 4;
						continue;
					}
					break;
					
				case SNAKE_RIGHT:			//向右走
					if( snake_Info.sHeadDir != 3 )
					{
						snake_Info_next.sHeadx += SNAKE_MOVE_DISTANCE;
						snake_Info.sHeadDir = 4;
					} else {
						keyFlag = 3;
						continue;
					}
					break;
			}			
			//蛇体移动
			snakeSta = Snake_Move();
			
			//检查是否需要更新食物
			Create_Food();			
			
			//移动速度
//			Delay_ms( speed );
		}
		
		if( snakeSta == 0 )		//游戏结束
		{
			ListDestory( snake_list );		//摧毁双向链表
			startGame = 0;
			break;
		}
	}
}	
 