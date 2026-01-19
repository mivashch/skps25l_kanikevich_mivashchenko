import numpy as np
from scipy import signal
import matplotlib.pyplot as plt

time = np.linspace(0, 10, 500, endpoint=False)

f_up = np.linspace(30, 300, len(time) // 2)
f_down = np.linspace(300, 30, len(time) // 2)
f_pwm = np.concatenate([f_up, f_down])

modulating_signal = np.sin(2 * np.pi * 0.5 * time)

pwm_signal = signal.square(2 * np.pi * f_pwm * time, duty=(modulating_signal + 1) / 2)

plt.figure(figsize=(12, 9))

plt.subplot(2, 1, 1)
plt.plot(time, modulating_signal, label="Sine Wave (Modulation)")
plt.title("Modulating Sine Wave")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.ylim(-1.5, 1.5)
plt.legend()
plt.grid(True)

plt.subplot(2, 1, 2)
plt.plot(time, pwm_signal, label="PWM Signal with Variable Frequency")
plt.title("PWM Signal with Frequency Modulation")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.ylim(-1.5, 1.5)
plt.legend()
plt.grid(True)

plt.show()
