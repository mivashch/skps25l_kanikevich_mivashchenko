import gpio4
import time
import math


buzzer = gpio4.SysfsGPIO(27)
buzzer.export = True  
buzzer.direction = 'out'  


notes = {
    'C4': 261.63,
    'D4': 293.66,
    'E4': 329.63,
    'F4': 349.23,
    'G4': 392.00,
    'A4': 440.00,
    'B4': 493.88,
    'C5': 523.25,
    'D5': 587.33,
    'E5': 659.25,
    'F5': 698.46,
    'G5': 783.99
}


def play_tone(frequency, duration):
    period = 1.0 / frequency  
    half_period = period / 2  


    end_time = time.time() + duration
    while time.time() < end_time:
        buzzer.value = 1  
        time.sleep(half_period)
        buzzer.value = 0  
        time.sleep(half_period)


    for note in notes.values():
        play_tone(note, 0.5)  
        time.sleep(0.1)
