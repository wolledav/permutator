import numpy as np

MAX_FLOW = 10000
MIN_FLOW = 0
MAX_DIST = 1000
MIN_DIST = 1
SIZES = [90, 100, 125, 150]

for size in SIZES:
    D = np.random.randint(low=MIN_DIST, high=MAX_DIST, size=(size, size))
    F = np.random.randint(low=MIN_FLOW, high=MAX_FLOW, size=(size, size))
    data = np.concatenate((D, F), axis=0)
    with open(f"inst{size}.txt", "w") as f:
        f.write(f"{size} 0")
        np.savetxt(f, data, fmt="%d")