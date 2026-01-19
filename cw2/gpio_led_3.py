import gpio4
import time


gpio_led = gpio4.SysfsGPIO(27)
gpio_led.export = True
gpio_led.direction = 'out'


gpio_button = gpio4.SysfsGPIO(17)
gpio_button.export = True
gpio_button.direction = 'in'

try:
    while True:
        print(gpio_button.value)
        if gpio_button.value == 1:  
            gpio_led.value = 0  
            print('1')
        else:
            gpio_led.value = 1  
        time.sleep(0.1)  

except KeyboardInterrupt:
    print("Program interrupted by user.")

finally:
    gpio_led.export = False
    gpio_button.export = False
