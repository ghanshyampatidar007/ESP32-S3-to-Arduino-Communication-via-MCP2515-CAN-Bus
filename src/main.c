#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(can_tx_app, LOG_LEVEL_INF);

#define CAN_ID_TX 0x35
#define CAN_ID_RX 0x30

const struct device *can_dev;
static volatile bool rx_flag = false;


static void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data)
{   
    if (frame->id == CAN_ID_RX)
    {
        rx_flag = true;
        printk("Received id: 0x%02X \n", frame->id);
        printk("Received byte : ");
        for(uint8_t i=0;i<frame->dlc;i++)
            printk(" 0x%02X", frame->data[i]);
        printk("\n");
    }
   
}

int main(void)
{
    can_dev = DEVICE_DT_GET(DT_NODELABEL(mcp2515));
    if (!device_is_ready(can_dev)) {
        LOG_ERR("CAN device not ready");
        return 0;
    }

    LOG_INF("CAN device ready");

    if (can_start(can_dev) != 0) {
        LOG_ERR("CAN start failed");
        return 0;
    }

    struct can_filter filter = {
        .id = CAN_ID_RX,
        .mask = CAN_STD_ID_MASK,
        .flags = 0,
    };
    can_add_rx_filter(can_dev, can_rx_callback, NULL, &filter);

    struct can_frame tx_frame = {
        .id = CAN_ID_TX,
        .dlc = 8,
        .flags = 0,
        .data = {0xAB,0xCD,0x98,0x32,0x23,0xAB,0xCD,0x98}
    };

    while (1) {
    rx_flag = false;
       
    if (can_send(can_dev, &tx_frame, K_MSEC(100), NULL, NULL) == 0) {
       printk("Sent id: 0x%02X \n", tx_frame.id);
       printk("Sent data : ");
        for(uint8_t i=0;i<tx_frame.dlc;i++)
            printk(" 0x%02X", tx_frame.data[i]);
        printk("\n");    
    } 
    else {
            LOG_ERR("Failed to send");
        }    
        int64_t start = k_uptime_get();
        while (!rx_flag && k_uptime_get() - start < 3000)
        {
            k_msleep(10);
        }

        if (!rx_flag) {
            LOG_WRN("No response received");
        }

        k_sleep(K_SECONDS(3));
    }
}


