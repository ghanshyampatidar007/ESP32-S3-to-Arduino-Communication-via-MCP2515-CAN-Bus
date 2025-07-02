#include <SPI.h>
#include <mcp2515.h>

struct can_frame rx_frame;
struct can_frame tx_frame;

MCP2515 mcp2515(10);  // CS pin (use pin 10)

void setup() {
  Serial.begin(115200);

  // Initialize SPI for MCP2515: SCK=13, MISO=12, MOSI=11, CS=10 (Arduino Uno default)
  SPI.begin();

  if (mcp2515.reset() != MCP2515::ERROR_OK) {
    Serial.println("MCP2515 Reset FAILED");
    while (1);
  }

  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);  // Match ESP32-S3 config
  mcp2515.setNormalMode();  // Set MCP2515 to normal mode

  Serial.println("CAN Receiver Ready");
}

void loop() {
 //while(1){
  
  if (mcp2515.readMessage(&rx_frame) == MCP2515::ERROR_OK) {
    Serial.print("Received CAN ID: 0x");
    Serial.println(rx_frame.can_id, HEX);

    Serial.print("Received Data: ");
    for (int i = 0; i < rx_frame.can_dlc; i++) {
      Serial.print("0x");
      Serial.print(rx_frame.data[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
   
    // Send back acknowledgment
    tx_frame.can_id = 0x30;  // ID expected by ESP32-S3
    tx_frame.can_dlc = 8;
    uint8_t payload[8] = {0x23, 0xAB, 0xCD, 0x98, 0x23, 0xAB, 0xCD, 0x98};
    memcpy(tx_frame.data, payload, 8);

    if (mcp2515.sendMessage(&tx_frame) == MCP2515::ERROR_OK) {
      Serial.print("Sent ID: ");
      Serial.println(tx_frame.can_id, HEX);

      Serial.print("Sent data : ");
        for(char i=0;i<tx_frame.can_dlc;i++)
        {   Serial.print("0x");
            Serial.print(tx_frame.data[i],HEX);
            Serial.print(" ");
        }
        Serial.println("");  
    }
    else {
      Serial.println("Failed to send ACK");
    }
    
 }

  delay(2000);
//}
}

