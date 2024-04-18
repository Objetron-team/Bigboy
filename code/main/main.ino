
#include "ESPNowMaster.hpp"

ESPNowMaster espNowMaster;

void setup() {
    Serial.begin(115200);
    
    command_data cmd_data;
    cmd_data.cmd_1 = YELLOW_ONE;
    cmd_data.cmd_2 = AUTO_ON;
    cmd_data.cmd_3 = START;
    
    uint8_t mac[] = {0xE4, 0x65, 0xB8, 0x77, 0x18, 0x98};
    
    
    slave_data slave;
    memcpy(slave.mac_address, mac, 6);
    slave.cmd_data = cmd_data;
    
    espNowMaster.AddSlave(slave);
    
    espNowMaster.Init();
    espNowMaster.Start();
}

void loop() {
    
    espNowMaster.Update();
}