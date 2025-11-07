import numpy as np
import matplotlib.pyplot as plt

def f(x):
    return np.exp(x) * np.log(1 + np.exp(-x))

x = np.linspace(-10, 10, 400)

y = f(x)

plt.figure(figsize=(8, 5))
plt.plot(x, y, label=r"$f(x) = e^x \ln(1 + e^{-x})$", color="blue", linewidth=2)
plt.axhline(1, color="red", linestyle="--", label=r"$\lim_{x \to \infty} f(x) = 1$")
plt.xlabel("x")
plt.ylabel("f(x)")
plt.title("Wykres funkcji f(x) = e^x ln(1 + e^{-x})")
plt.legend()
plt.grid(True)

plt.savefig("wykres_funkcji.png", dpi=300, bbox_inches="tight")

plt.show()
