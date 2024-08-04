#include <rtthread.h>
#include <rtdevice.h>
#if defined(BSP USING TEST)
#define DRV DEBUG
#define LOG TAG     "drv.test"
#include <drv_log.h>  


static rt_err_t dev_test_init(rt_device_t dev)
{
    LOG_I("test dev init");
    return RT_EOK;
}

static rt_err_t dev_test_open(rt_device_t dev, rt_uint16_t oflag)
{
    ("test dev open flag = %d", oflag);
    return RT_EOK;
}

static rt_err_t dev_test_close(rt_device_t dev)
{
    LOG_I("test dev close");
    return RT_EOK;
}

static rt_ssize_t dev_test_read(rt_device_t dev, rt_off_t_pos, void *buffer, rt_size_t size)
{
    LOG_I("test dev read pos = %d, size = %d", pos, size);
    return RT_EOK;
}

static rt_ssize_t dev_test_write(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    LOG_I("test dev write pos =%d, size = %d", pos, size);
    return RT_EOK;
}

static rt_err_t dev_test_control(rt_device_t dev, int cmd, void *args)
{
    LOG_I("test dev control cmd %d", cmd);
    return RT_EOK;
}
