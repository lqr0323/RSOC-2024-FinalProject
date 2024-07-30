#include <rtthread.h>
#include <rtdevice.h>

#define VIRTUAL_DEVICE_NAME "virtual_dev"
#define VIRTUAL_BUFFER_SIZE 128

static char virtual_buffer[VIRTUAL_BUFFER_SIZE];
static struct rt_device virtual_device;

/* 读操作 */
static rt_size_t virtual_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    if (pos + size > VIRTUAL_BUFFER_SIZE)
        size = VIRTUAL_BUFFER_SIZE - pos;

    rt_memcpy(buffer, &virtual_buffer[pos], size);
    return size;
}

/* 写操作 */
static rt_size_t virtual_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    if (pos + size > VIRTUAL_BUFFER_SIZE)
        size = VIRTUAL_BUFFER_SIZE - pos;

    rt_memcpy(&virtual_buffer[pos], buffer, size);
    return size;
}

/* 打开设备 */
static rt_err_t virtual_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

/* 关闭设备 */
static rt_err_t virtual_close(rt_device_t dev)
{
    return RT_EOK;
}

/* 控制设备 */
static rt_err_t virtual_control(rt_device_t dev, int cmd, void *args)
{
    return RT_EOK;
}

int rt_hw_virtual_device_init(void)
{
    /* 初始化设备结构体 */
    virtual_device.type = RT_Device_Class_Char;
    virtual_device.init = RT_NULL;
    virtual_device.open = virtual_open;
    virtual_device.close = virtual_close;
    virtual_device.read = virtual_read;
    virtual_device.write = virtual_write;
    virtual_device.control = virtual_control;
    virtual_device.user_data = RT_NULL;

    return rt_device_register(&virtual_device, VIRTUAL_DEVICE_NAME, RT_DEVICE_FLAG_RDWR);
}
INIT_DEVICE_EXPORT(rt_hw_virtual_device_init,rt_hw_virtual_device_init);
