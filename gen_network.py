import numpy as np
import matplotlib.pyplot as plt
import json

xs_unscaled = np.array([0, -1, 1, -2, 0, 2, -1, 1, 0])
ys_unscaled = np.array([0, 1, 1, 2, 2, 2, 3, 3, 4])*np.sqrt(3)

disturb_pos = lambda x: x + (2*np.random.random(ys.shape) - 1)*0.1
scale = 20
xs, ys = scale*disturb_pos(xs_unscaled), scale*disturb_pos(ys_unscaled)
batteries = np.ones_like(xs)*6

print(json.dumps(dict([("xs", list(xs)), ("ys", list(ys)), ("batteries", list(batteries))])))

plt.scatter(xs, ys)
plt.gca().set_aspect("equal")
plt.show()
