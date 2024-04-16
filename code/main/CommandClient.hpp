#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h> // Include this header for esp_wifi_get_mac

class CommandClient
{
private:
    esp_now_peer_info_t master_info; // Information about the master device
    byte my_mac[6];                  // Store this ESP32's MAC address
public:
    // Constructor (optional, you can leave it empty)
    CommandClient(String name) {}

    void Init()
    {
        WiFi.mode(WIFI_STA); // Required for ESP-NOW
        esp_now_init();

        // Get this ESP32's MAC address
        esp_wifi_get_mac(WIFI_IF_STA, my_mac);

        // Set up callback to receive commands
        esp_now_register_recv_cb(OnCommandReceived);
    }

    static void OnCommandReceived(const uint8_t *mac_addr, const uint8_t *data, int data_len)
    {
        Command received_command;
        memcpy(&received_command, data, sizeof(received_command)); // Copy received data

        // Process the received command based on its value (e.g., control outputs, update sensors)
        Serial.print("Command value: ");
        Serial.println(received_command.value); // Print the integer value

        // Your code to handle the received command goes here
    }
};
