import json
import math
import numpy as np
import sys
import os
from itertools import combinations

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




os.chdir("/home/hcabral/Permanent/UFPE/Research/Topicos/WSN/Routing/batteries/workspaces/dijkstra_out_2018/networks/")
#with open("network_diamond4_b.json") as f:
with open("network_diamond5_a.json") as f:
    root = json.load(f)
xs, ys = root["xs"], root["ys"]
xs, ys = xs[-1:] + xs[:-1], ys[-1:] + ys[:-1]

dists = np.array([[math.hypot(x0 - x1, y0 - y1) for x1, y1 in zip(xs, ys)]
                  for x0, y0 in zip(xs, ys)])

ep, a, b, SCALE = 50e-9, 100e-12, 40e-15, 10000
for path_exp in [2.0, 2.5, 3.0, 3.5, 4.0]:
    print("Path exponent = {}".format(path_exp))
    print("param et[*,*]:")
    print(" "*4 + " ".join("{}".format(n + 1).center(12) for n in range(len(xs))).rstrip() + ":=")
    print("\n".join("{:-3d} ".format(n + 1) +
                    " ".join("{:10.6e}".format(SCALE*a*d**path_exp) if d > 0 else ".".center(12)
                             for d in ds) for n, ds in enumerate(dists)).rstrip() + ";\n")

print("param Q[*]:= 1 0 {};".format(" ".join("{} 100".format(n) for n in range(2, len(xs) + 1))))
print("param epr[*]:= {};".format(" ".join("{} {:.10}".format(n + 1, SCALE*ep)
                                           for n in range(len(xs)))))
print("param ept[*]:= {};".format(" ".join("{} {:.10}".format(n + 1, SCALE*ep)
                                           for n in range(len(xs)))))




with open("network_diamond4_b.json") as f:
    root = json.load(f)
xs = root["xs"]
ys = [0.45*y for y in root["ys"]]
xs, ys = xs[-1:] + xs[:-1], ys[-1:] + ys[:-1]

dists = np.array([[math.hypot(x0 - x1, y0 - y1) for x1, y1 in zip(xs, ys)]
                  for x0, y0 in zip(xs, ys)])

ep, a, b = 50e-9, 100e-12, 40e-15
for path_exp in [2.0, 2.5, 3.0, 3.5, 4.0]:
    print("Path exponent = {}".format(path_exp))
    print("param et[*,*]:")
    print(" "*4 + " ".join("{}".format(n + 1).center(12) for n in range(len(xs))).rstrip() + ":=")
    print("\n".join("{:-3d} ".format(n + 1) +
                    " ".join("{:10.6e}".format(1000000*a*d**path_exp) if d > 0 else ".".center(12)
                             for d in ds) for n, ds in enumerate(dists)).rstrip() + ";\n")





costs = [(a*math.hypot(x0 - x1, y0 - y1)**2, (i, j)) for (i, (x0, y0)), (j, (x1, y1)) in
         combinations(enumerate(zip(xs, ys)), 2)]
cost_diffs = [(abs(d1 - d2), p1, p2) for (d1, p1), (d2, p2) in combinations(costs, 2)]







os.chdir("/home/hcabral/Permanent/UFPE/Research/Topicos/WSN/Routing/batteries/workspaces/dijkstra_out_2018/networks/")
#json_file = "network_diamond5_a.json"
#json_file = "network_diamond4_b.json"
json_file = "network_diamond4_c.json"

with open("tempodevida_template.txt") as f:
    template = "".join(["# Arquivo que define a rede: {}\n\n".format(json_file)] + f.readlines())

with open(json_file) as f:
    root = json.load(f)
xs, ys = root["xs"], root["ys"]
xs, ys = xs[-1:] + xs[:-1], ys[-1:] + ys[:-1]
dists = np.array([[math.hypot(x0 - x1, y0 - y1) for x1, y1 in zip(xs, ys)]
                  for x0, y0 in zip(xs, ys)])

nbr_nodes = len(xs)
out_file = "tempodevida_{}nodes_exp{}.txt"
CMax, NO = 7*(nbr_nodes - 1), " ".join(str(n + 1) for n in range(nbr_nodes))
ENLACES_HDR = ["\t".join([""] + [str(n + 1) for n in range(nbr_nodes)]) + ":="]
ENLACES_LINES = ["\t".join([str(row + 1)] +
                           ["+" if row != col else "-" for col in range(nbr_nodes)])
                 for row in range(nbr_nodes)]
ENLACES = "\n".join(ENLACES_HDR + ENLACES_LINES)

ep, a, b, path_exp, SCALE = 50e-9, 100e-12, 40e-15, 2.0, 100000
ET_LINES = ["\t".join([str(row + 1)] +
                           ["{:10.6e}".format(SCALE*a*d**path_exp) if row != col else "."
                            for col, d in enumerate(ds)])
                      for row, ds in enumerate(dists)]
ET = "\n".join(ENLACES_HDR + ET_LINES)

Q = "1 0 {}".format(" ".join("{} 100".format(n) for n in range(2, len(xs) + 1)))
EPR = " ".join("{} {:.10}".format(n + 1, SCALE*ep) for n in range(len(xs)))
EPT = " ".join("{} {:.10}".format(n + 1, SCALE*ep) for n in range(len(xs)))

path_exp_str = "{:d}p{:1d}".format(int(path_exp), int(round(10*(path_exp - int(path_exp)))))
with open(out_file.format(nbr_nodes, path_exp_str), "wt", newline="\r\n") as f:
    f.write(template.format(CMax, NO, ENLACES, ET, Q, EPR, EPT))


## Raul
#json_file = "network_diamond4_b.json"
json_file = "network_diamond5_a.json"
with open(json_file) as f:
    root = json.load(f)
xs, ys = root["xs"], root["ys"]
xs, ys = xs[-1:] + xs[:-1], ys[-1:] + ys[:-1]
dists = np.array([[math.hypot(x0 - x1, y0 - y1) for x1, y1 in zip(xs, ys)]
                  for x0, y0 in zip(xs, ys)])
ep, a, b, path_exp, SCALE = 50e-9, 100e-12, 40e-15, 2.0, 10000
print("\n".join("\t".join("{:10.6e}".format(SCALE*a*d**path_exp) for col, d in enumerate(ds))
                for row, ds in enumerate(dists)))
print("\nEp = {}".format(ep*SCALE))
