import matplotlib.pyplot as plt
import pandas as pd

# Load the CSV file
data = pd.read_csv('results.csv', header=None, names=["Iteration", "AES", "RSA"])

# Plot the graph
plt.figure(figsize=(10, 6))
plt.plot(data["Iteration"], data["AES"], label="AES-CTR DRBG", marker='o')
plt.plot(data["Iteration"], data["RSA"], label="RSA-based DRBG", marker='x')
plt.xlabel("Iteration")
plt.ylabel("Time (seconds)")
plt.title("DRBG Performance Comparison (AES vs RSA)")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("drbg_comparison.png")
plt.show()
