#include "PWM.h"

void PWM::export_channel()
{
    std::ofstream export_file("/sys/class/pwm/pwmchip0/export");
    if (export_file.is_open())
    {
        export_file << channel;
        export_file.close();
    }
    else
    {
        std::cerr << "Error while exporting channel! Channel:" << channel << std::endl;
    }
}

void PWM::set_period(int period_ns)
{
    std::ofstream period_file(pwm_path + "period");
    if (period_file.is_open())
    {
        period_file << period_ns;
        period_file.close();
    }
    else
    {
        std::cerr << "Error while setting PWM period! Channel:" << channel << std::endl;
    }
}

void PWM::set_duty_cycle(int duty_ns)
{
    std::ofstream duty_file(pwm_path + "duty_cycle");
    if (duty_file.is_open())
    {
        duty_file << duty_ns;
        duty_file.close();
    }
    else
    {
        std::cerr << "Error while setting duty_cycle! Channel: " << channel << std::endl;
    }
}

void PWM::enable()
{
    std::ofstream enable_file(pwm_path + "enable");
    if (enable_file.is_open())
    {
        enable_file << 1;
        enable_file.close();
    }
    else
    {
        std::cerr << "Error while trying to enable! Channel: " << channel << std::endl;
    }
}

void PWM::disable()
{
    std::ofstream enable_file(pwm_path + "enable");
    if (enable_file.is_open())
    {
        enable_file << 0;
        enable_file.close();
    }
    else
    {
        std::cerr << "Error while trying to disable! Channel: " << channel << std::endl;
    }
}

void PWM::unexport_channel()
{
    std::ofstream unexport_file("/sys/class/pwm/pwmchip0/unexport");
    if (unexport_file.is_open())
    {
        unexport_file << channel;
        unexport_file.close();
    }
    else
    {
        std::cerr << "Error while unexporting channel! Channel: " << channel << std::endl;
    }
}