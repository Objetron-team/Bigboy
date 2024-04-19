
#include "ESPNowMaster.hpp"

ESPNowMaster espNowMaster;

/*
Pami gris -> E4:65:B8:79:83:14

Pami noir -> E4:65:B8:76:94:44
*/

void setup() {
    Serial.begin(115200);
    
    
    command_data pami_gris_cmd;
    pami_gris_cmd.cmd_1 = BLUE_ONE;
    pami_gris_cmd.cmd_2 = AUTO_ON;
    pami_gris_cmd.cmd_3 = START;
    
    uint8_t mac_gris[] = {0xE4, 0x65, 0xB8, 0x79, 0x83, 0x14};
    
    slave_data pami_gris;
    memcpy(pami_gris.mac_address, mac_gris, 6);
    pami_gris.cmd_data = pami_gris_cmd;
    
    command_data pami_noir_cmd;
    pami_noir_cmd.cmd_1 = BLUE_TWO;
    pami_noir_cmd.cmd_2 = AUTO_ON;
    pami_noir_cmd.cmd_3 = START;
    
    uint8_t mac_noir[] = {0xE4, 0x65, 0xB8, 0x76, 0x94, 0x44};
    
    slave_data pami_noir;
    memcpy(pami_noir.mac_address, mac_noir, 6);
    pami_noir.cmd_data = pami_noir_cmd;
    
    
    espNowMaster.AddSlave(pami_gris);
    espNowMaster.AddSlave(pami_noir);
    
    espNowMaster.Init();
    espNowMaster.Start();
}

void loop() {
    
    espNowMaster.Update();
}