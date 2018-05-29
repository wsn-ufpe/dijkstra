from pathlib import Path
import json
import numpy as np
import matplotlib.pyplot as plt
import scipy.spatial as spatial

NBR_NODES, FILE_NBR = 200, 1
filename = "n{0}_P1e-8_{1:03d}.dat".format(NBR_NODES, FILE_NBR)

with open(filename) as fp:
    data = json.load(fp)

energy_profile1 = 1000*np.array(data["lifetime"]["energy_profile"])
lifetime_profile1 = np.array(data["lifetime"]["lifetime_profile"])

NBR_NODES, FILE_NBR = 200, 0
filename = "n{0}_P5e-8_{1:03d}.dat".format(NBR_NODES, FILE_NBR)

with open(filename) as fp:
    data = json.load(fp)

energy_profile2 = 1000*np.array(data["lifetime"]["energy_profile"])
lifetime_profile2 = np.array(data["lifetime"]["lifetime_profile"])
xs = np.array(data["xs"])
ys = np.array(data["ys"])

#plt.figure(figsize=(16, 12))

plt.subplot(121)
plt.plot(lifetime_profile)
plt.title("Lifetime Profile")
plt.subplot(122)
plt.plot(energy_profile)
plt.title("Energy Profile")

#plt.savefig('sink_position.png', bbox_inches="tight", dpi=200)
#plt.clf()

plt.show()

plt.figure(figsize=(16, 12))

lifetime_profile = np.concatenate((lifetime_profile1, lifetime_profile2))
energy_profile = np.concatenate((energy_profile1, energy_profile2))
points = np.stack((lifetime_profile, energy_profile), axis=-1)
hull = spatial.ConvexHull(points)
xh, yh = points[hull.vertices].T
xh = np.concatenate((xh, xh[:1]))
yh = np.concatenate((yh, yh[:1]))

plt.scatter(lifetime_profile1, energy_profile1, "y")
plt.scatter(lifetime_profile2, energy_profile2, "b")
plt.plot(xh, yh, "ro-")
plt.xlabel("Lifetime")
plt.ylabel("Energy (mW)")
plt.title("Pareto front (N = {})".format(NBR_NODES))

plt.savefig('pareto_front_n200_p5e-8_{:03d}.png'.format(FILE_NBR), bbox_inches="tight", dpi=200)
plt.clf()

plt.show()

plt.plot(lifetime_profile, energy_profile)
plt.title("Pareto front")

#plt.savefig('network_n200_16x16.png', bbox_inches="tight", dpi=200)
#plt.clf()

plt.show()

print(min(consumptions_me), max(consumptions_me))
print(min(consumptions_ml), max(consumptions_ml))
print(min(lifetimes_me), max(lifetimes_me))
print(min(lifetimes_ml), max(lifetimes_ml))

data_me = iterations[len(iterations)//2]["energy"]
data_ml = iterations[len(iterations)//2]["lifetime"]

lifetime1 = data_me["lifetime"]
lifetime2 = data_ml["lifetime"]
consumption1 = data_me["consumption"]
consumption2 = data_ml["consumption"]
print("Minimum lifetime: ", min(lifetime1[:-1]))
print("Minimum lifetime: ", min(lifetime2[:-1]))
print("Global consumption: ", 1000*sum(consumption1))
print("Global consumption: ", 1000*sum(consumption2))

plt.violinplot([lifetime1[:-1], lifetime2[:-1]], [1, 3], widths=0.7, showmeans=True,
               showextrema=True, showmedians=True)
plt.show()

plt.scatter(xs[:-1], ys[:-1], s=20)
plt.scatter(xs[-1], ys[-1], s=50, marker="s")

plt.scatter(xs[11], ys[11], s=100, marker="p")
plt.scatter(xs[28], ys[28], s=100, marker="h")
plt.scatter(xs[39], ys[39], s=100, marker="*")
plt.scatter(xs[50], ys[50], s=100, marker="^")
plt.scatter(xs[61], ys[61], s=100, marker="<")
plt.scatter(xs[95], ys[95], s=100, marker=">")

plt.show()

a = 100/(np.log(min(lifetime2[:-1])) - np.log(max(lifetime2[:-1])))
b = 5 - a*np.log(max(lifetime2[:-1]))
scales = [a*np.log(x) + b for x in lifetime2[:-1]]
xs, ys = data_me["xs"], data_me["ys"]

long_routes = [[idx] + r for idx, r in enumerate(data_ml["routes"]) if r]# and len(r) > 4]
for route in long_routes:
    xx, yy = zip(*[(xs[idx], ys[idx]) for idx in route])
    plt.plot(xx, yy)

plt.scatter(xs[:-1], ys[:-1], s=scales)
plt.scatter(xs[-1], ys[-1], s=100, marker="s")

plt.show()

a = 100/(np.log(min(data1["lifetime"])) - np.log(max(data1["lifetime"])))
b = 5 - a*np.log(max(data1["lifetime"]))
scales = [a*np.log(x) + b for x in data1["lifetime"]]
xs, ys = data1["xs"], data1["ys"]

long_routes = [[idx] + r for idx, r in enumerate(data1["routes"]) if r]# and len(r) > 4]
for route in long_routes:
    xx, yy = zip(*[(xs[idx], ys[idx]) for idx in route])
    plt.plot(xx, yy)

plt.scatter(xs[:-1], ys[:-1], s=scales)
plt.scatter(xs[-1], ys[-1], s=100, marker="s")

plt.show()

def calculate_consumption(prefix, p):
    variables = ["max_lifetime", "min_energy"]
    max_file_nbr = max(int(b.stem.split("_")[-1]) for b in p.glob(variables[0] + "*.dat"))
    data = []
    for n in range(max_file_nbr+1):
        files = [v + prefix + "{:03d}.dat".format(n) for v in variables]
        with (p / files[0]).open("rt") as lifetime_file, (p / files[1]).open("rt") as energy_file:
            lifetime_data, energy_data = json.load(lifetime_file), json.load(energy_file)
            consumption_lifetime = sum(lifetime_data["consumption"])
            consumption_energy = sum(energy_data["consumption"])
            consumption_ratio = (consumption_lifetime/consumption_energy - 1)*100
            data.append((consumption_ratio, consumption_lifetime, consumption_energy))

    return [np.array(l) for l in zip(*data)]

def calculate_lifetime(prefix, p):
    variables = ["max_lifetime", "min_energy"]
    max_file_nbr = max(int(b.stem.split("_")[-1]) for b in p.glob(variables[0] + "*.dat"))
    data = []
    for n in range(max_file_nbr+1):
        files = [v + prefix + "{:03d}.dat".format(n) for v in variables]
        with (p / files[0]).open("rt") as lifetime_file, (p / files[1]).open("rt") as energy_file:
            lifetime_data, energy_data = json.load(lifetime_file), json.load(energy_file)
            min_lifetime_lifetime = min(lifetime_data["lifetime"][:-1])
            min_lifetime_energy = min(energy_data["lifetime"][:-1])
            min_ratio = (min_lifetime_lifetime/min_lifetime_energy - 1)*100
            data.append((min_ratio, min_lifetime_lifetime, min_lifetime_energy))

    return [np.array(l) for l in zip(*data)]

files = [("_n_100_", Path("../data/20180501_n100")), ("_n_200_", Path("../data/20180501_n200")),
         ("_n_300_", Path("../data/20180501_n300")), ("_n_500_", Path("../data/20180501_n500"))]
# files = [("_n_100_", Path("../data/20180501_n100")),
#          ("_n100_x500_y500_b5_m100_p1e-7_", Path("../data/20180507_n100a")),
#          ("_n100_x500_y500_b5_m100_p5e-8_", Path("../data/20180507_n100b"))]
ratio_lt_n100, lifetime1_n100, lifetime2_n100 = calculate_lifetime(*files[0])
ratio_lt_n200, lifetime1_n200, lifetime2_n200 = calculate_lifetime(*files[1])
ratio_lt_n300, lifetime1_n300, lifetime2_n300 = calculate_lifetime(*files[2])
ratio_lt_n500, lifetime1_n500, lifetime2_n500 = calculate_lifetime(*files[3])

plt.figure(figsize=(12, 9))
plt.violinplot([ratio_lt_n100, ratio_lt_n200, ratio_lt_n300, ratio_lt_n500], [1, 2, 3, 5], widths=0.7,
               showmeans=True, showextrema=True, showmedians=False)
fontsize, labelsize = 24, 24
plt.xlabel("Número de nós na rede", fontsize=fontsize)
plt.ylabel("Aumento no tempo de vida (%)", fontsize=fontsize)
plt.gca().set_xticklabels(["0", "100", "200", "300", "400", "500"])
plt.tick_params(labelsize=labelsize)

plt.savefig('lifetime.pdf', bbox_inches="tight", dpi=300)
plt.clf()

plt.show()

ratio_en_n100, energy1_n100, energy2_n100 = calculate_consumption(*files[0])
ratio_en_n200, energy1_n200, energy2_n200 = calculate_consumption(*files[1])
ratio_en_n300, energy1_n300, energy2_n300 = calculate_consumption(*files[2])
ratio_en_n500, energy1_n500, energy2_n500 = calculate_consumption(*files[3])

plt.figure(figsize=(12, 9))
plt.violinplot([ratio_en_n100, ratio_en_n200, ratio_en_n300, ratio_en_n500], [1, 2, 3, 5], widths=0.7,
               showmeans=True, showextrema=True, showmedians=False)
plt.xlabel("Número de nós na rede", fontsize=fontsize)
plt.ylabel("Aumento no consumo de energia (%)", fontsize=fontsize)
plt.gca().set_xticklabels(["0", "100", "200", "300", "400", "500"])
plt.tick_params(labelsize=labelsize)

plt.savefig('energy.pdf', bbox_inches="tight", dpi=300)
plt.clf()

plt.show()

plt.gca().set_aspect("equal")
plt.scatter(lifetime2_n100, lifetime1_n100, c=ratio_en_n100)
plt.xlim(plt.ylim())
plt.show()

plt.scatter(1000*energy1_n100, 1000*energy2_n100, c="red")
plt.scatter(1000*energy1_n200, 1000*energy2_n200, c="yellow")
plt.scatter(1000*energy1_n300, 1000*energy2_n300, c="blue")
plt.scatter(1000*energy1_n500, 1000*energy2_n500, c="green")
plt.show()

def read_json_file(prefix, p, n):
    variables = ["max_lifetime", "min_energy"]
    files = [v + prefix + "{:03d}.dat".format(n) for v in variables]
    with (p / files[0]).open("rt") as lifetime_file, (p / files[1]).open("rt") as energy_file:
        data = json.load(lifetime_file), json.load(energy_file)
    return data

def plot_network(ax, json_data):
    a = 200/(np.log(min(json_data["lifetime"])) - np.log(max(json_data["lifetime"])))
    b = 2 - a*np.log(max(json_data["lifetime"]))
    scales = np.array([a*np.log(x) + b for x in json_data["lifetime"]])
    xs, ys = np.array(json_data["xs"]), np.array(json_data["ys"])

    routes = [[idx] + r for idx, r in enumerate(json_data["routes"]) if r]
    for route in routes:
        xx, yy = zip(*[(xs[idx], ys[idx]) for idx in route])
        ax.plot(xx, yy, zorder=0, c="gray")

    ax.scatter(xs[:-1], ys[:-1], s=scales, zorder=0.4)
    ax.scatter(xs[-1], ys[-1], s=200, marker="s", zorder=0.5)

best_lifetime = list(sorted(enumerate(ratio_lt_n100), key=lambda t: t[1], reverse=True))
network_lt, network_en = read_json_file(n=best_lifetime[1][0], *files[0])

plt.figure(figsize=(12, 9))

plt.gca().set_aspect("equal")
plot_network(plt.gca(), network_lt)

plt.xlabel("Coordenada x (m)", fontsize=fontsize)
plt.ylabel("Coordenada y (m)", fontsize=fontsize)
plt.tick_params(labelsize=labelsize)

plt.savefig("exemplo_lifetime.pdf", bbox_inches="tight", dpi=300)
plt.clf()

plt.figure(figsize=(12, 9))

plt.gca().set_aspect("equal")
plot_network(plt.gca(), network_en)

plt.xlabel("Coordenada x (m)", fontsize=fontsize)
plt.ylabel("Coordenada y (m)", fontsize=fontsize)
plt.tick_params(labelsize=labelsize)

plt.savefig("exemplo_energia.pdf", bbox_inches="tight", dpi=300)
plt.clf()


fig, (ax1, ax2) = plt.subplots(1, 2, sharey=False, figsize=(36, 16))
ax1.set_aspect("equal")
ax2.set_aspect("equal")
    
plot_network(ax1, network_lt)
plot_network(ax2, network_en)

ax1.set_title("Otimizado para tempo de vida", fontdict={"fontsize": 40})
ax1.tick_params(labelsize=40)

ax2.set_title("Otimizado para energia apenas", fontdict={"fontsize": 40})
ax2.tick_params(labelsize=40)

fig.savefig("teste.png", dpi=150)
plt.close(fig)

##

lifetime1 = network_lt["lifetime"]
consumption1 = network_lt["consumption"]
print("Minimum lifetime: ", min(lifetime1[:-1]))
print("Global consumption: ", 1000*sum(consumption1))

lifetime2 = network_en["lifetime"]
consumption2 = network_en["consumption"]
print("Minimum lifetime: ", min(lifetime2[:-1]))
print("Global consumption: ", 1000*sum(consumption2))

plt.violinplot([lifetime1[:-1], lifetime2[:-1]], [1, 3], widths=0.7, showmeans=True,
               showextrema=True, showmedians=False)
plt.show()
