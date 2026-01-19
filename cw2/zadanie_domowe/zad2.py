import numpy as np
import matplotlib.pyplot as plt

def generate_alternating_array(n, p):
    k = int(n * p / 100)
    arr = np.zeros(n, dtype=int)

    if k == 0:
        return arr

    indices = np.linspace(0, n - 1, k, dtype=int)
    arr[indices] = 1
    return arr


frequency = 43
time_duration = 10
t = np.linspace(0, time_duration, 20)

duty_cycle_sinusoidal = 10 + (90 - 10) * (0.5 * (1 + np.sin(np.pi * (t / time_duration) * 2 - np.pi / 2)))

N = (frequency * time_duration) // t.size

repeat_factor = (frequency * time_duration) // t.size

expanded_duty_cycle = np.repeat(duty_cycle_sinusoidal, repeat_factor)

total_points = len(expanded_duty_cycle)

# pwm_signal_sinusoidal = np.array([    # uncomment for more nubbly signal
#     1 if (((n % N) / N * 100) < inets) else 0
#     for n, inets in zip(range(total_points), expanded_duty_cycle)
# ])

pwm_signal_sinusoidal = np.array([])

for n, inets in zip(range(total_points), duty_cycle_sinusoidal):    # uncomment for more alternating signal
    pwm_signal_sinusoidal = np.concatenate((pwm_signal_sinusoidal,generate_alternating_array(N, inets)))


plt.figure(figsize=(12, 6))

plt.subplot(2, 1, 1)
plt.plot(t, duty_cycle_sinusoidal, label="sinus changes", color="orange")
plt.title("signal depends on time")
plt.xlabel("time(seconds)")
plt.ylabel("Cycle(%)")
plt.legend()
plt.grid()


plt.subplot(2, 1, 2)
plt.plot(range(0, pwm_signal_sinusoidal.size), pwm_signal_sinusoidal, label="PWM signal", color="orange")
plt.title("PWM depends on time")
plt.xlabel("cycles)")
plt.ylabel("signal")
plt.legend()
plt.grid()

plt.tight_layout()
plt.savefig("sinusoidal_pwm_signal.png")
plt.show()
