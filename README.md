# RSOC-2024-FinalProject  
本文章是经过一周的培训之后写出的一个小项目，目前暂定的是贪吃蛇，由于在电赛期间，时间比较赶，但会抓紧时间，完善迭代项目！！！！！！111
目前已经完成了main.c的主要编写，后面加上其他头文件的编写  
游戏逻辑  

  
  蛇碰到墙：GAMEOVER
  
  蛇头碰到蛇身：GAMEOVER
  
  蛇头碰到食物会变长一个单位
  
  没有操作时蛇会延记录的方向移动一个单位，设置为500ms的delay
  
  GAMEOVER后会显示分数
当然，以下是用 Markdown 格式解释的代码：



## 头文件

#include <rtthread.h>
#include <lcd.h> // LCD库，根据实际情况选择对应的库
#include <touch.h> // 触摸屏库，根据实际情况选择对应的库
```

引入必要的头文件。`rtthread.h` 是 RT-Thread 操作系统的核心库，`lcd.h` 和 `touch.h` 分别是用于控制 LCD 和触摸屏的库。

## 宏定义


#define SNAKE_LENGTH 100
#define LCD_WIDTH 240
#define LCD_HEIGHT 320
#define SNAKE_SIZE 10
#define FOOD_SIZE 10
```

- `SNAKE_LENGTH`：蛇的最大长度。
- `LCD_WIDTH` 和 `LCD_HEIGHT`：LCD 屏幕的宽度和高度。
- `SNAKE_SIZE`：每个蛇节的尺寸。
- `FOOD_SIZE`：食物的尺寸。

## 数据结构

```c
typedef struct {
    int x;
    int y;
} Point;
```

`Point` 结构体表示屏幕上的一个点，包含 x 和 y 坐标。

## 全局变量

```c
static Point snake[SNAKE_LENGTH];
static Point food;
static int snake_length = 1;
static int direction = 0; // 0: right, 1: down, 2: left, 3: up
```

- `snake`：存储蛇身各节的坐标。
- `food`：食物的位置。
- `snake_length`：当前蛇的长度。
- `direction`：蛇的移动方向（0: 右, 1: 下, 2: 左, 3: 上）。

## 绘制函数

```c
static void draw_snake()
{
    int i;
    for (i = 0; i < snake_length; i++)
    {
        lcd_draw_rectangle(snake[i].x * SNAKE_SIZE, snake[i].y * SNAKE_SIZE, SNAKE_SIZE, SNAKE_SIZE, RED);
    }
}
```

绘制蛇的每一节，使用 `lcd_draw_rectangle` 函数绘制红色矩形。

```c
static void draw_food()
{
    lcd_draw_rectangle(food.x * FOOD_SIZE, food.y * FOOD_SIZE, FOOD_SIZE, FOOD_SIZE, GREEN);
}
```

绘制食物，使用绿色矩形表示。

## 更新蛇的位置

```c
static void update_snake()
{
    int i;
    for (i = snake_length - 1; i > 0; i--)
    {
        snake[i] = snake[i - 1];
    }

    switch (direction)
    {
        case 0: snake[0].x++; break;
        case 1: snake[0].y++; break;
        case 2: snake[0].x--; break;
        case 3: snake[0].y--; break;
    }

    if (snake[0].x == food.x && snake[0].y == food.y)
    {
        snake_length++;
        if (snake_length > SNAKE_LENGTH) snake_length = SNAKE_LENGTH;
        food.x = rand() % (LCD_WIDTH / FOOD_SIZE);
        food.y = rand() % (LCD_HEIGHT / FOOD_SIZE);
    }
}
```

- 移动蛇身的各节，使蛇头的位置根据当前方向进行更新。
- 如果蛇头碰到食物，蛇的长度增加，新的食物位置被随机生成。

## 碰撞检测

```c
static void check_collision()
{
    int i;
    for (i = 1; i < snake_length; i++)
    {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
        {
            rt_kprintf("Game Over\n");
            rt_thread_mdelay(1000);
            rt_system_restart(); // Restart system in case of game over
        }
    }

    if (snake[0].x < 0 || snake[0].x >= LCD_WIDTH / SNAKE_SIZE || snake[0].y < 0 || snake[0].y >= LCD_HEIGHT / SNAKE_SIZE)
    {
        rt_kprintf("Game Over\n");
        rt_thread_mdelay(1000);
        rt_system_restart(); // Restart system in case of game over
    }
}
```

检查蛇是否碰到自己或屏幕边界。如果碰到，打印“Game Over”并重启系统。

## 游戏主线程

```c
static void snake_game_thread_entry(void *parameter)
{
    lcd_init();
    touch_init();
    
    // Initialize snake and food
    snake[0].x = 10;
    snake[0].y = 10;
    food.x = 15;
    food.y = 15;

    while (1)
    {
        lcd_clear(BLACK);
        draw_snake();
        draw_food();
        update_snake();
        check_collision();
        
        rt_thread_mdelay(300); // Control speed
        if (touch_data_available())
        {
            Point touch_point = touch_get_data();
            if (touch_point.x > LCD_WIDTH / 2)
                direction = (direction + 1) % 4;
            else
                direction = (direction + 3) % 4;
        }
    }
}
```

- 初始化 LCD 和触摸屏。
- 设置初始的蛇和食物位置。
- 主循环中清除屏幕，绘制蛇和食物，更新蛇的位置，检测碰撞，控制游戏速度。
- 根据触摸屏输入改变蛇的方向。

## 主函数

```c
int main(void)
{
    rt_thread_t game_thread = rt_thread_create("snake_game", snake_game_thread_entry, RT_NULL, 2048, 10, 10);
    if (game_thread != RT_NULL)
    {
        rt_thread_startup(game_thread);
    }
    return 0;
}
```

- 创建并启动游戏线程。
- `rt_thread_create` 创建一个新的线程用于运行游戏逻辑。
- `rt_thread_startup` 启动线程。

