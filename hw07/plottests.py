import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Plot Coin Tosses ==============================================

df = pd.read_csv("cointosses.csv")

plt.figure(figsize=(8, 5))
plt.axhline(0.5, linewidth=2, color='r')
plt.plot(df["tossIdx"], df["frac"], marker=".", linestyle="-", color="b")
plt.ylim(0.3,0.7)
plt.xscale("log")
plt.xlabel("Toss Index")
plt.ylabel("Fraction of heads")
plt.title("Coin toss test")
plt.grid(True, which="both", linestyle="--", alpha=0.5)
plt.tight_layout()
plt.savefig("cointosses.png")
plt.close()


# Plot Pi calculation ==========================================

df = pd.read_csv("pierrors.csv")

plt.figure(figsize=(8, 5))
plt.plot(df["nIter"], df["error"], marker=".", linestyle="-", color="b")
plt.xscale("log")
plt.yscale("log")
plt.xlabel("Iteration")
plt.ylabel("Absolute error")
plt.title("Pi computation test")
plt.grid(True, which="both", linestyle="--", alpha=0.5)
plt.tight_layout()
plt.savefig("pierrors.png")
plt.close()


# Plot Change of Vars ==========================================

arr = np.loadtxt('changevars.csv')

plt.figure(figsize=(8, 5))
plt.hist(arr, bins=500, density=True, alpha=0.8)
x = np.linspace(0.005, arr.max(), 100)
y = 1 / (2 * np.sqrt(x))
plt.plot(x, y, 'r-', linewidth=2, label=r'$\frac{1}{2\sqrt{x}}$')
plt.yscale("log")
plt.xlabel("y")
plt.title("Random PDF via change of variables")
plt.legend()
plt.tight_layout()
plt.savefig("changevars.png")
plt.close()


# Plot Inverse Transform ==========================================

arr = np.loadtxt('invtransform.csv')
lam = 1.5
plt.figure(figsize=(8, 5))
plt.hist(arr, bins=500, density=True, alpha=0.8)
x = np.linspace(arr.min(), arr.max(), 50)
y = lam * np.exp(-x*1.5)
plt.plot(x, y, 'r-', linewidth=2, label=r'$\lambda e^{-\lambda y}$')
plt.yscale("log")
plt.xlabel("y")
plt.title("Random PDF via inverse transform")
plt.legend()
plt.tight_layout()
plt.savefig("invtransform.png")
plt.close()


# Plot Empirical CDF ==========================================

arr = np.sort(arr)
ix = np.arange(1, len(arr) + 1) / len(arr)

plt.figure(figsize=(8, 5))
plt.plot(arr, ix, marker=".", color="b")
x = np.linspace(arr.min(), arr.max(), 500)
y = 1-np.exp(-x*1.5)
plt.plot(x, y, 'r-', linewidth=2, label="Exact CDF")
# plt.xscale("log")
# plt.yscale("log")
plt.xlabel("y")
plt.ylabel("CDF")
plt.title("Empirical CDF")
plt.legend()
plt.tight_layout()
plt.savefig("empiricalcdf.png")
plt.close()