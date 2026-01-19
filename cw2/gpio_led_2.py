import numpy as np
import gpio4


def generate_alternating_array(n, p):
    k = int(n * p / 100)
    arr = np.zeros(n, dtype=int)

    if k == 0:
        return arr

    indices = np.linspace(0, n - 1, k, dtype=int)
    arr[indices] = 1
    return arr


frequency = 60000
time_duration = 10
t = np.linspace(0, time_duration, 100)


gpio_pin_27 = 27
gpio_led_27 = gpio4.SysfsGPIO(gpio_pin_27)
gpio_led_27.export = True 
gpio_led_27.direction = 'out' 


duty_cycle_sinusoidal = 0 + 100 * (0.5 * (1 + np.sin(np.pi * (t / time_duration) * 2 - np.pi / 2)))

repeat_factor = (frequency * time_duration) // t.size

expanded_duty_cycle = np.repeat(duty_cycle_sinusoidal, repeat_factor)

total_points = len(expanded_duty_cycle)

# pwm_signal_sinusoidal = np.array([    # uncomment for more nubbly signal
#     1 if (((n % N) / N * 100) < inets) else 0
#     for n, inets in zip(range(total_points), expanded_duty_cycle)
# ])


pwm_signal_sinusoidal = np.array([])

for n, inets in zip(range(total_points), duty_cycle_sinusoidal):    # uncomment for more alternating signal
    pwm_signal_sinusoidal = np.concatenate((pwm_signal_sinusoidal,generate_alternating_array(repeat_factor, inets)))


while True:
    for pwm_value in pwm_signal_sinusoidal:

        if pwm_value > 0.5:
            gpio_led_27.value = 1  
        else:
            gpio_led_27.value = 0  
