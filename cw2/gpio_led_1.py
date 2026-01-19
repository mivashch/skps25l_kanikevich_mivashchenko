import gpio4
import time


gpio40 = gpio4.SysfsGPIO(27)


gpio40.export = True


gpio40.direction = 'out'


for _ in range(10):
    gpio40.value = 1  
    time.sleep(0.5)  
    gpio40.value = 0  
    time.sleep(0.5)  

gpio40.export = False
