/*
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(can_tx_app, LOG_LEVEL_INF);

#define CAN_ID_DATA 0x036
#define CAN_ID_ACK  0x037
#define MAX_RETRIES 3
#define CS_GPIO_CONTROLLER DT_NODELABEL(gpio0)
#define CS_PIN 10

static const struct device *can_dev;
static volatile bool ack_received = false;

static void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data)
{
    if (frame->id == CAN_ID_ACK) {
        LOG_INF("ACK received!");
        ack_received = true;
    }
}

int main(void)
{
    struct can_frame tx_frame;
    int ret, retries, filter_id;

    const struct device *cs_gpio = DEVICE_DT_GET(CS_GPIO_CONTROLLER);

      if (!device_is_ready(cs_gpio)) {
        LOG_ERR("SPI or CS GPIO not ready");
        return;
    }

    gpio_pin_configure(cs_gpio, CS_PIN, GPIO_OUTPUT_HIGH);
    gpio_pin_set(cs_gpio, CS_PIN, 0); // CS LOW

    can_dev = DEVICE_DT_GET(DT_NODELABEL(mcp2515));
    if (!device_is_ready(can_dev)) {
        LOG_ERR("MCP2515 device not ready");
        return 0;
    }

    LOG_INF("CAN device ready");

    ret = can_start(can_dev);
    if (ret) {
        LOG_ERR("CAN start failed: %d", ret);
        return 0;
    }

    struct can_filter ack_filter = {
        .id = CAN_ID_ACK,
        .mask = CAN_STD_ID_MASK,
        .flags = 0 // Standard ID
    };

    filter_id = can_add_rx_filter(can_dev, can_rx_callback, NULL, &ack_filter);
    if (filter_id < 0) {
        LOG_ERR("Failed to add RX filter");
        return 0;
    }

    while (1) {
        float temperature = 25.75;
        int tempInt = (int)(temperature * 100);

        tx_frame.id = CAN_ID_DATA;
        tx_frame.dlc = 2;
        tx_frame.flags = 0; // Standard frame
        tx_frame.data[0] = (tempInt >> 8) & 0xFF;
        tx_frame.data[1] = tempInt & 0xFF;

        ack_received = false;
        retries = 0;

        while (!ack_received && retries < MAX_RETRIES) {
            ret = can_send(can_dev, &tx_frame, K_MSEC(100), NULL, NULL);
            if (ret == 0) {
               LOG_INF("Sent: %.2f °C", (double)temperature);

            } else {
                LOG_ERR("CAN send failed: %d", ret);
                retries++;
                continue;
            }

            int64_t start = k_uptime_get();
            while (!ack_received && (k_uptime_get() - start < 500)) {
                k_msleep(10);
            }

            if (!ack_received) {
                LOG_WRN("No ACK, retrying...");
                retries++;
            }
        }

        if (!ack_received) {
            LOG_ERR("Failed to get ACK after %d retries", MAX_RETRIES);
        }

        k_sleep(K_SECONDS(1));
    }
}
*/
/*
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(can_loopback, LOG_LEVEL_INF);

#define CAN_ID_LOOPBACK 0x123

static const struct device *can_dev;
static struct can_frame tx_frame;
static volatile bool loopback_received = false;

static void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(user_data);

    if (frame->id == CAN_ID_LOOPBACK) {
        LOG_INF("Loopback frame received: 0x%02X 0x%02X", frame->data[0], frame->data[1]);
        loopback_received = true;
    }
}

int main(void)
{
    int ret;
    int filter_id;

    can_dev = DEVICE_DT_GET(DT_NODELABEL(mcp2515));

    if (!device_is_ready(can_dev)) {
        LOG_ERR("CAN device not ready");
        return 0;
    }

    LOG_INF("CAN device ready");

    // Enable loopback mode
    ret = can_set_mode(can_dev, CAN_MODE_LOOPBACK);

    if (ret != 0) {
        LOG_ERR("Failed to set loopback mode: %d", ret);
        return 0;
    }

    ret = can_start(can_dev);
    if (ret != 0) {
        LOG_ERR("CAN start failed: %d", ret);
        return 0;
    }

    struct can_filter loopback_filter = {
        .id = CAN_ID_LOOPBACK,
        .mask = CAN_STD_ID_MASK,
        .flags = 0,
    };

    filter_id = can_add_rx_filter(can_dev, can_rx_callback, NULL, &loopback_filter);
    if (filter_id < 0) {
        LOG_ERR("Failed to add RX filter");
        return 0;
    }

    tx_frame.id = CAN_ID_LOOPBACK;
    tx_frame.dlc = 2;
    tx_frame.flags = 0; // Standard frame
    tx_frame.data[0] = 0xAB;
    tx_frame.data[1] = 0xCD;

    while (1) {
        loopback_received = false;

        ret = can_send(can_dev, &tx_frame, K_MSEC(100), NULL, NULL);
        if (ret != 0) {
            LOG_ERR("Failed to send frame: %d", ret);
        } else {
            LOG_INF("Sent loopback frame: 0x%02X 0x%02X", tx_frame.data[0], tx_frame.data[1]);
        }

        //int64_t start = k_uptime_get();
       // while (!loopback_received && (k_uptime_get() - start < 100)) 
        {
            k_msleep(10);
        }

        if (!loopback_received) {
            LOG_WRN("No loopback response");
        }

        k_sleep(K_SECONDS(1));
    }
}
*/

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


