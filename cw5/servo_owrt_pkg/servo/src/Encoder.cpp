#include "Encoder.h"
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

Encoder::Encoder(const std::string& gpioA, const std::string& gpioB)
{
    initialize_gpio(gpioA);
    initialize_gpio(gpioB);

    std::string pathA = "/sys/class/gpio/" + gpioA + "/value";
    std::string pathB = "/sys/class/gpio/" + gpioB + "/value";

    pinA_fd = open(pathA.c_str(), O_RDONLY);
    pinB_fd = open(pathB.c_str(), O_RDONLY);

    if (pinA_fd < 0 || pinB_fd < 0) {
        std::cerr << "Failed to open GPIO files" << std::endl;
    }

    last_state_A = read_gpio(pinA_fd);
    last_state_B = read_gpio(pinB_fd);

    position = 0;
}

Encoder::~Encoder()
{
    if (pinA_fd >= 0) close(pinA_fd);
    if (pinB_fd >= 0) close(pinB_fd);
}

void Encoder::initialize_gpio(const std::string& gpio)
{
    std::ofstream export_file("/sys/class/gpio/export");
    export_file << gpio.substr(4, 2);
    std::cout << "num" << gpio.substr(4, 2) << std::endl;
    export_file.close();

    std::ofstream direction_file("/sys/class/gpio/" + gpio + "/direction");
    direction_file << "in";
    direction_file.close();
}

int Encoder::read_gpio(int fd)
{
    if (fd < 0) return -1;
    lseek(fd, 0, SEEK_SET);
    char value;
    read(fd, &value, 1);
    return value == '1' ? 1 : 0;
}

void Encoder::update()
{
    int current_A = read_gpio(pinA_fd);
    int current_B = read_gpio(pinB_fd);
    //std::cout << "A: " << current_A << " B: " << current_B << std::endl;

    int current_state = (current_A << 1) | current_B;
    int last_state    = (last_state_A << 1) | last_state_B;

    int transition = (last_state << 2) | current_state;

    switch (transition) {
    case 0b1110: // 11 -> 10
    case 0b1000: // 10 -> 00
    case 0b0001: // 00 -> 01
    case 0b0111: // 01 -> 11
        position++;
        break;
    case 0b1101: // 11 -> 01
    case 0b0100: // 01 -> 00
    case 0b0010: // 00 -> 10
    case 0b1011: // 10 -> 11
        position--;
        break;
    default:
        break;
    }

    last_state_A = current_A;
    last_state_B = current_B;
}


int Encoder::get_position()
{
    return position;
}
