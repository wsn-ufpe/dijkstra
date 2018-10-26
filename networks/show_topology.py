import json
import numpy as np
import matplotlib.pyplot as plt
import os

os.chdir("/home/hcabral/Permanent/UFPE/Research/Topicos/WSN/Routing/batteries/workspaces/dijkstra_out_2018/networks/")
#with open("network_diamond4_b.json") as f:
with open("network_diamond5_a.json") as f:
    root = json.load(f)
xs, ys = root["xs"], root["ys"]
xs, ys = xs[-1:] + xs[:-1], ys[-1:] + ys[:-1]

plt.scatter(xs[1:], ys[1:])
plt.scatter(xs[0], ys[0], s=100)
for n, pos in enumerate(zip(xs, ys)):
    plt.annotate(s=str(n + 1), xy=np.array(pos) + 1.5)
plt.gca().set_aspect("equal")
plt.show()





os.chdir("/home/hcabral/Permanent/UFPE/Research/Topicos/WSN/Routing/batteries/workspaces/dijkstra_out_2018/networks/")
json_in, json_out = "network_diamond5_a.json", "network_diamond5_b.json"
with open(json_in) as f:
    root = json.load(f)

xs, ys, batteries = root["xs"], root["ys"], root["batteries"]
SCALE = 0.5
ys = [y*SCALE for y in ys]

with open(json_out, "wt") as f:
    json.dump({"xs": xs, "ys": ys, "batteries": batteries}, f)

xs, ys = xs[-1:] + xs[:-1], ys[-1:] + ys[:-1]
plt.scatter(xs[1:], ys[1:])
plt.scatter(xs[0], ys[0], s=100)
for n, pos in enumerate(zip(xs, ys)):
    plt.annotate(s=str(n + 1), xy=np.array(pos) + 1.5)
plt.gca().set_aspect("equal")
plt.show()
