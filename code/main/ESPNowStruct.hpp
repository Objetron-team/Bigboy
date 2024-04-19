#pragma once

enum command
{
    START,
    AUTO_ON,
    AUTO_OFF,

    YELLOW_ONE,
    YELLOW_TWO,
    BLUE_ONE,
    BLUE_TWO,
};

typedef struct command_data
{
    command cmd_1;
    command cmd_2;
    command cmd_3;
} command_data;

typedef struct slave_data
{
    uint8_t mac_address[6];
    command_data cmd_data;
} slave_data;