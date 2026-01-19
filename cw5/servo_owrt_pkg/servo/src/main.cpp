#include "PWM.h"
#include "Encoder.h"
#include <iostream>
#include <thread>
#include <chrono>

int main()
{

    PWM pwm(0);
    pwm.export_channel();
    pwm.set_period(20000000);
    pwm.enable();

    Encoder encoder("gpio23", "gpio24");


    int duty_min = 1000000;
    int duty_max = 2000000;
    int old_pos = 0;

    while (true)
    {
        encoder.update();
        int pos = encoder.get_position();

        //std::cout << pos << std::endl;
        if (pos != old_pos){
        if (pos < -180) pos = -180;
        if (pos > 180) pos = 180;



        int duty = duty_min + (pos + 90) * (duty_max - duty_min) / 180;

        pwm.set_duty_cycle(duty);
        old_pos = pos;

        }
    }

    return 0;
}
