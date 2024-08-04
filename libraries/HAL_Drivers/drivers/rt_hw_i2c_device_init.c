#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define I2C_BUS_NAME "i2c1"   // I2C总线设备名称
#define I2C_DEVICE_ADDR 0x50  // I2C设备地址

struct rt_i2c_bus_device *i2c_bus = RT_NULL;

static rt_size_t i2c_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    struct rt_i2c_msg msgs[2];
    rt_uint8_t addr = pos;

    msgs[0].addr = I2C_DEVICE_ADDR;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf = &addr;
    msgs[0].len = 1;

    msgs[1].addr = I2C_DEVICE_ADDR;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf = buffer;
    msgs[1].len = size;

    if (rt_i2c_transfer(i2c_bus, msgs, 2) == 2)
    {
        return size;
    }
    else
    {
        return 0;
    }
}

static rt_size_t i2c_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    struct rt_i2c_msg msg;
    rt_uint8_t data[size + 1];

    data[0] = pos;
    rt_memcpy(&data[1], buffer, size);

    msg.addr = I2C_DEVICE_ADDR;
    msg.flags = RT_I2C_WR;
    msg.buf = data;
    msg.len = size + 1;

    if (rt_i2c_transfer(i2c_bus, &msg, 1) == 1)
    {
        return size;
    }
    else
    {
        return 0;
    }
}

static rt_err_t i2c_init(rt_device_t dev)
{
    i2c_bus = rt_i2c_bus_device_find(I2C_BUS_NAME);
    if (i2c_bus == RT_NULL)
    {
        return -RT_ENOSYS;
    }
    return RT_EOK;
}

static rt_err_t i2c_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

static rt_err_t i2c_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t i2c_control(rt_device_t dev, int cmd, void *args)
{
    return RT_EOK;
}

static struct rt_device i2c_device;

int rt_hw_i2c_device_init(void)
{
    i2c_device.type = RT_Device_Class_I2CBUS;
    i2c_device.init = i2c_init;
    i2c_device.open = i2c_open;
    i2c_device.close = i2c_close;
    i2c_device.read = i2c_read;
    i2c_device.write = i2c_write;
    i2c_device.control = i2c_control;

    i2c_device.user_data = RT_NULL;

    return rt_device_register(&i2c_device, "i2c_dev", RT_DEVICE_FLAG_RDWR);
}
INIT_DEVICE_EXPORT(rt_hw_i2c_device_init,rt_hw_i2c_device_init);
