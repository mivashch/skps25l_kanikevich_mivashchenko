#ifndef PWM_H
#define PWM_H



#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

class PWM
{
private:
    int channel;
    std::string pwm_path;

public:
    PWM(int channel) : channel(channel)
    {
        pwm_path = "/sys/class/pwm/pwmchip0/pwm" + std::to_string(channel) + "/";
    }
    ~PWM()
    {
        unexport_channel();
    }
    void export_channel();
    void set_period(int period_ns);
    void set_duty_cycle(int duty_ns);
    void enable();
    void disable();
    void unexport_channel();
};





#endif //PWM_H
