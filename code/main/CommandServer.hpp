#include <esp_now.h>
#include <WiFi.h>

class CommandeServer
{
private:
    /* data members */

    esp_now_peer_info_t slave_info[2];

    bool started = false;

    long initTime;
    long lastCommandTime;

    const long COMMAND_WAIT = 5000;     // Wait time before sending first command (milliseconds)
    const long COMMAND_INTERVAL = 1000; // Interval between commands (milliseconds)

    void SendCommand()
    {
        Command command;
        command.value = 10; // Example command value

        // Send command to both slaves
        esp_now_send(this->slave_info[0].peer_addr, (uint8_t *)&command, sizeof(command));
        esp_now_send(this->slave_info[1].peer_addr, (uint8_t *)&command, sizeof(command));
    }

public:
    // Constructor
    CommandeServer(String name) {}

    void Init()
    {

        byte slave1_mac[] = {0xE4, 0x65, 0xB8, 0x79, 0x83, 0x16}; // MyLittlePami
        byte slave2_mac[] = {0xE4, 0x65, 0xB8, 0x79, 0x94, 0x46}; // MySweetPami

        WiFi.mode(WIFI_STA); // Required for ESP-NOW
        esp_now_init();
        esp_now_register_send_cb(OnDataSent);

        // Add slave MAC addresses to peer list
        memcpy(this->slave_info[0].peer_addr, slave1_mac, 6);
        this->slave_info[0].channel = 0; // Use a common channel (can be adjusted)

        memcpy(this->slave_info[1].peer_addr, slave2_mac, 6);
        this->slave_info[1].channel = 0;

        esp_now_add_peer(&this->slave_info[0]); // Pass address of struct
        esp_now_add_peer(&this->slave_info[1]); // Pass address of struct

        Serial.println("Master ESP32 ready");
    }

    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
    {
        if (status != ESP_NOW_SEND_SUCCESS)
        {
            Serial.print("Failed to send data to: ");
            Serial.println(mac_addr[0], HEX);
        }
    }

    // Function to send commands to connected slaves
    void Update()
    {

        if (!started)
        {
            return;
        }

        unsigned long currentTime = millis();

        // Check if COMMAND_WAIT period has passed and interval has elapsed
        if (currentTime - initTime >= COMMAND_WAIT && currentTime - lastCommandTime >= COMMAND_INTERVAL)
        {
            SendCommand();

            lastCommandTime = currentTime;
        }
    }

    void Start()
    {
        initTime = millis();
        lastCommandTime = initTime;
        started = true;
    }
};
