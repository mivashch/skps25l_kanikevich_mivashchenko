#ifndef ENCODER_H
#define ENCODER_H

#include <string>

class Encoder
{
private:
    int pinA_fd;
    int pinB_fd;
    int last_state_A;
    int last_state_B;
    int position;

public:
    Encoder(const std::string& gpioA, const std::string& gpioB);
    ~Encoder();
    void initialize_gpio(const std::string& gpio);
    int read_gpio(int fd);
    void update();
    int get_position();
};

#endif
