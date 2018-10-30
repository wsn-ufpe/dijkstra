import os

os.chdir("/home/hcabral/Permanent/UFPE/Research/Topicos/WSN/Routing/batteries/workspaces/dijkstra_out_2018/")
d = []
fn = "log_n25a_exp1e9"
with open(fn) as f:
    for line in f:
        if not line.startswith("Iter") or " " not in line:
            continue
        if line.startswith("Iter#0"):
            d.append({})
        _, line = line.split(maxsplit=1)
        line = line.strip()
        if not line or not line.startswith("Origem:"):
            continue
        _, src, line = line.split(maxsplit=2)
        src = int(src)
        idx1, idx2 = line.find("Pos:  "), line.find("  Node ")
        line = line[idx1+6:idx2] if idx2 != -1 else line[idx1+6:]
        d[-1].setdefault(src, set()).add(line)

cvt = lambda n: 1 if n == 24 else n + 2

print("\nArquivo {}\n".format(fn))
for l in d:
    print("#### Redistribution ####")
    for k, v in l.items():
        v = [str(k + 2) + " " + " ".join(str(cvt(int(n))) for n in s.split()) for s in v]
        print(v)
