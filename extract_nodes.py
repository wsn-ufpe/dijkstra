import os

os.chdir("/home/hcabral/Permanent/UFPE/Research/Topicos/WSN/Routing/batteries/workspaces/dijkstra_out_2018/")
d = {}
fn = "log_n16_p1e8"
with open(fn) as f:
    for line in f:
        if not line.startswith("Iter") or " " not in line:
            continue
        _, line = line.split(maxsplit=1)
        line = line.strip()
        if not line or not line.startswith("Origem:"):
            continue
        _, src, line = line.split(maxsplit=2)
        src = int(src)
        idx1, idx2 = line.find("Pos:  "), line.find("  Node ")
        d.setdefault(src, set()).add(line[idx1+6:idx2] if idx2 != -1 else line[idx1+6:])

cvt = lambda n: 1 if n == 15 else n + 2

for k, v in d.items():
    v = [str(k + 2) + " " + " ".join(str(cvt(int(n))) for n in s.split()) for s in v]
    print(v)
