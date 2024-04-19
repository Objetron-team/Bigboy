
#include "ESPNowMaster.hpp"
#include "ESPNowSlave.hpp"

ESPNowMaster espNowMaster;
ESPNowSlave espNowSlave;

void setup() {
    Serial.begin(115200);
    
    espNowSlave.Init();
    
}

void loop() {
    
}