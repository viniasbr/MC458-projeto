import csv
import matplotlib.pyplot as plt

rows = []
with open("size_experiments.csv", newline="") as f:
    reader = csv.DictReader(f)
    for row in reader:
        rows.append({
            "n": int(row["n"].strip()),
            "sparsity": float(row["sparsity"].strip()),
            "dense": float(row["dense_bytes"].strip()),
            "avl": float(row["avl_bytes"].strip()),
            "hash": float(row["hash_bytes"].strip()),
        })

by_n = {}
for r in rows:
    by_n.setdefault(r["n"], []).append(r)

for n_val, group in by_n.items():
    group = sorted(group, key=lambda r: r["sparsity"])
    s = [r["sparsity"] for r in group]
    d = [r["dense"] for r in group]
    a = [r["avl"] for r in group]
    h = [r["hash"] for r in group]

    plt.figure()
    plt.plot(s, d, marker="o", label="Dense")
    plt.plot(s, a, marker="o", label="AVL")
    plt.plot(s, h, marker="o", label="Hash")
    plt.title(f"Memória vs. esparsidade (n={n_val})")
    plt.xlabel("Esparsidade (escala log)")
    plt.ylabel("Bytes (escala log)")
    plt.xscale("log")
    plt.yscale("log")
    plt.legend()
    plt.tight_layout()
    plt.savefig(f"memory_vs_sparsity_n{n_val}.png")
    plt.close()
