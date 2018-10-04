import json
import math
import numpy as np
import sys

if len(sys.argv) != 2:
    print("Usage: {} json_file".format(sys.argv[0]))
    sys.exit(-1)

with open(sys.argv[1]) as f:
    root = json.load(f)
xs, ys = root["xs"], root["ys"]
xs, ys = xs[-1:] + xs[:-1], ys[-1:] + ys[:-1]

dists = np.array([[math.hypot(x0 - x1, y0 - y1) for x1, y1 in zip(xs, ys)]
                  for x0, y0 in zip(xs, ys)])
print("\n".join(" ".join("{:10.6f}".format(d) for d in ds) for ds in dists))

ep, a, b = 50e-9, 100e-12, 40e-15
costs = a*dists**2
costs_str = [["{:10.6e}".format(a*d**2) if d > 0 else "-".center(12) for d in ds]
             for ds in dists]

print("\nCustos dos enlaces")
print("\n".join(" ".join(c for c in cs) for cs in costs_str))

print("\nKarcius")
print("param et[*,*]:")
print(" "*4 + " ".join("{}".format(n + 1).center(12) for n in range(len(xs))).rstrip() + ":=")
print("\n".join("{:-3d} ".format(n + 1) +
                " ".join("{:10.6e}".format(1000000*a*d**2) if d > 0 else ".".center(12) for d in ds)
                for n, ds in enumerate(dists)).rstrip() + ";")

print("\nRaul")
print("\n".join(" ".join("{:10.6e}".format(a*d**2) if d > 0 else "-1".center(12) for d in ds)
                for ds in dists))
