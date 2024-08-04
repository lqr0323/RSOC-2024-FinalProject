#include <rtthread.h>
#include <lcd.h> 
#include <touch.h> 
#include<snake.h>

#define SNAKE_LENGTH 100
#define LCD_WIDTH 240
#define LCD_HEIGHT 320
#define SNAKE_SIZE 10
#define FOOD_SIZE 10

typedef struct {
    int x;
    int y;
} Point;

static Point snake[SNAKE_LENGTH];
static Point food;
static int snake_length = 1;
static int direction = 0; // 0: right, 1: down, 2: left, 3: up

static void draw_snake()
{
    int i;
    for (i = 0; i < snake_length; i++)
    {
        lcd_draw_rectangle(snake[i].x * SNAKE_SIZE, snake[i].y * SNAKE_SIZE, SNAKE_SIZE, SNAKE_SIZE, RED);
    }
}

static void draw_food()
{
    lcd_draw_rectangle(food.x * FOOD_SIZE, food.y * FOOD_SIZE, FOOD_SIZE, FOOD_SIZE, GREEN);
}

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

int main(void)
{
    rt_thread_t game_thread = rt_thread_create("snake_game", snake_game_thread_entry, RT_NULL, 2048, 10, 10);
    if (game_thread != RT_NULL)
    {
        rt_thread_startup(game_thread);
    }
    return 0;
}