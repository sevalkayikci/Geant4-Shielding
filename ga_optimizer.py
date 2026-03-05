#!/usr/bin/env python3
"""
Melanin Shield — Genetik Algoritma Optimizasyonu
=================================================
Mevcut ROOT sweep verisini seed olarak kullanir,
sonra yeni Geant4 simuasyonlari kosturarak en iyi
(cfg, fungus_kalinligi) kombinasyonunu bulur.

Kullanim:
  cd build/
  python3 ../ga_optimizer.py

Gereksinimler:
  pip install uproot numpy
"""

import subprocess, re, json, random, os, sys
from pathlib import Path

# =============== AYARLAR ===============
GEANT4_EXEC   = "./exampleB1"          # build klasoru icinde
ROOT_FILE     = "results/melanin_results.root"
GA_JSON       = "results/ga_results.json"

POP_SIZE      = 20      # populasyon buyuklugu
N_GEN         = 30      # nesil sayisi
MUTATION_RATE = 0.25    # mutasyon olasiligi
TOURNAMENT_K  = 3       # tournament secim buyuklugu
N_EVENTS_GA   = 20000   # GA evaluasyonu icin event sayisi (hizli)
N_EVENTS_BEST = 80000   # En iyi sonuc dogrulama icin

CFG_MIN, CFG_MAX     = 0, 15
THICK_MIN, THICK_MAX = 1.0, 20.0  # cm

CONFIG_NAMES = {
    0:  "Single-MelaninFungi",
    1:  "HDPE-Fungus-HDPE",
    2:  "Al-Fungus-Al",
    3:  "Concrete-Fungus-Concrete",
    4:  "Al-HDPE-Fungus-HDPE-Al",
    5:  "Pb-Fungus-Pb",
    6:  "HDPE-Water-Fungus-HDPE",
    7:  "Al-Water-Fungus-Al",
    8:  "Pb-Only",
    9:  "Al-Water-Fungus-Water-Al",
    10: "Concrete-Water-Fungus-Water-Concrete",
    11: "Pb-Water-Fungus-Water-Pb",
    12: "Steel-Fungus-Steel",
    13: "Steel-Water-Fungus-Water-Steel",
    14: "Al-Concrete-Water-Fungus-Water-Concrete-Al",
    15: "Pb-Concrete-Water-Fungus-Water-Concrete-Pb",
}

# =============== BIREY ===============
class Individual:
    def __init__(self, cfg=None, thickness=None):
        self.cfg       = cfg       if cfg       is not None else random.randint(CFG_MIN, CFG_MAX)
        self.thickness = thickness if thickness is not None else round(random.uniform(THICK_MIN, THICK_MAX), 1)
        self.fitness      = None   # 1 - transmission (yukseği iyi)
        self.transmission = None
        self.mean_edep    = None

    def __repr__(self):
        return (f"Cfg={self.cfg:>2} ({CONFIG_NAMES.get(self.cfg,'?'):<42}) "
                f"t={self.thickness:>5.1f}cm  "
                f"trans={self.transmission:.4f}  fit={self.fitness:.4f}")

# =============== GEANT4 CALISTIR ===============
def run_geant4(cfg, thickness, n_events=N_EVENTS_GA):
    """./exampleB1 --single kosutur, stdout'u parse et."""
    cmd = [GEANT4_EXEC, "--single",
           "--cfg",       str(cfg),
           "--thickness", f"{thickness:.1f}",
           "--events",    str(n_events)]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=600)
        stdout = result.stdout
        m_trans = re.search(r"Transmission:([\d.eE+\-]+)", stdout)
        m_edep  = re.search(r"Mean Edep:\s+([\d.eE+\-]+)", stdout)
        if m_trans:
            trans = float(m_trans.group(1))
            edep  = float(m_edep.group(1)) if m_edep else 0.0
            return trans, edep
        else:
            print(f"  [WARN] Geant4 cikti parse edilemedi! Stdout:\n{stdout[-500:]}")
            return 1.0, 0.0
    except subprocess.TimeoutExpired:
        print(f"  [ERROR] Timeout: cfg={cfg} t={thickness}")
        return 1.0, 0.0
    except FileNotFoundError:
        print(f"  [ERROR] '{GEANT4_EXEC}' bulunamadi. 'build/' klasorunde misiniz?")
        sys.exit(1)

# =============== SEED: MEVCUT ROOT VERISI ===============
def seed_from_root():
    """Mevcut melanin_results.root verisinden en iyi bireyleri al."""
    seeds = []
    if not Path(ROOT_FILE).exists():
        print("  [INFO] ROOT dosyasi bulunamadi, random baslaniyor.")
        return seeds
    try:
        import uproot
        with uproot.open(ROOT_FILE) as f:
            tree = f["T"]
            data = tree.arrays(["cfg","thickness_cm","transmission","mean_edep_MeV"],
                               library="np")
        n = len(data["cfg"])
        print(f"  ROOT'tan {n} kayit okundu, en iyi {POP_SIZE//2} seed olarak aliniyor.")
        order = sorted(range(n), key=lambda i: data["transmission"][i])
        for i in order[:POP_SIZE//2]:
            ind = Individual(int(data["cfg"][i]), float(data["thickness_cm"][i]))
            ind.transmission = float(data["transmission"][i])
            ind.mean_edep    = float(data["mean_edep_MeV"][i])
            ind.fitness      = 1.0 - ind.transmission
            seeds.append(ind)
    except ImportError:
        print("  [WARN] uproot yuklu degil (pip install uproot). ROOT seed atlaniyor.")
    except Exception as e:
        print(f"  [WARN] ROOT seed hatasi: {e}")
    return seeds

# =============== FITNESS ===============
def evaluate(ind):
    print(f"  > cfg={ind.cfg:>2} t={ind.thickness:>5.1f}cm  ...", end="", flush=True)
    trans, edep = run_geant4(ind.cfg, ind.thickness)
    ind.transmission = trans
    ind.mean_edep    = edep
    ind.fitness      = 1.0 - trans   # 0=kotu, 1=mukemmel
    print(f"  trans={trans:.4f}  fit={ind.fitness:.4f}")
    return ind

# =============== GA OPERATORLERI ===============
def tournament(population):
    competitors = random.sample(population, min(TOURNAMENT_K, len(population)))
    return max(competitors, key=lambda x: x.fitness)

def crossover(p1, p2):
    cfg = p1.cfg if random.random() < 0.5 else p2.cfg
    w   = random.random()
    t   = round(w * p1.thickness + (1-w) * p2.thickness, 1)
    t   = max(THICK_MIN, min(THICK_MAX, t))
    return Individual(cfg, t)

def mutate(ind):
    if random.random() < MUTATION_RATE:
        ind.cfg = random.randint(CFG_MIN, CFG_MAX)
    if random.random() < MUTATION_RATE:
        delta = random.gauss(0, 2.5)
        ind.thickness = round(max(THICK_MIN, min(THICK_MAX, ind.thickness + delta)), 1)
    return ind

# =============== ANA GA DONGUSU ===============
def run_ga():
    print("=" * 60)
    print("  MELANIN SHIELD — GENETIK ALGORITMA OPTIMIZASYONU")
    print(f"  Pop={POP_SIZE}  Nesil={N_GEN}  Events/eval={N_EVENTS_GA}")
    print("=" * 60)

    # Populasyon olustur
    print(f"\n[0] Populasyon hazirlaniyor...")
    population = seed_from_root()
    while len(population) < POP_SIZE:
        population.append(Individual())

    # Seed edilmemisleri degerlendir
    print(f"\n[0] Baslangic evaluasyonu ({sum(1 for x in population if x.fitness is None)} birey):")
    for ind in population:
        if ind.fitness is None:
            evaluate(ind)

    history = []

    for gen in range(1, N_GEN + 1):
        population.sort(key=lambda x: x.fitness, reverse=True)
        best    = population[0]
        avg_fit = sum(x.fitness for x in population) / len(population)

        print(f"\n[Gen {gen:>2}/{N_GEN}]  Best: trans={best.transmission:.4f}  "
              f"Avg_fit={avg_fit:.4f}")
        print(f"  {best}")

        history.append({
            "gen":              gen,
            "best_cfg":         best.cfg,
            "best_material":    CONFIG_NAMES.get(best.cfg, "?"),
            "best_thickness":   best.thickness,
            "best_trans":       best.transmission,
            "best_fitness":     best.fitness,
            "avg_fitness":      avg_fit,
        })

        # Bir sonraki nesil
        next_gen = population[:2]  # elit: en iyi 2 dogrudan gecsin
        while len(next_gen) < POP_SIZE:
            child = crossover(tournament(population), tournament(population))
            child = mutate(child)
            child = evaluate(child)
            next_gen.append(child)
        population = next_gen

    # Son siralama
    population.sort(key=lambda x: x.fitness, reverse=True)
    best = population[0]

    # En iyi bireyi yuksek event sayisiyla dogrula
    print(f"\n{'='*60}")
    print(f"  DOGRULAMA: En iyi birey {N_EVENTS_BEST} event ile tekrar kosturuluyor...")
    trans_val, edep_val = run_geant4(best.cfg, best.thickness, N_EVENTS_BEST)
    print(f"  Dogrulama transmission: {trans_val:.4f}")

    print(f"\n{'='*60}")
    print(f"  SONUC — EN IYI KONFIGURASYEON")
    print(f"  Config:       {best.cfg} — {CONFIG_NAMES.get(best.cfg,'?')}")
    print(f"  Fungus kal.:  {best.thickness:.1f} cm")
    print(f"  Transmission: {trans_val:.4f} ({trans_val*100:.2f}%)")
    print(f"  Mean Edep:    {edep_val:.4f} MeV")
    print(f"{'='*60}")

    # Top-5
    top5 = []
    for ind in population[:5]:
        top5.append({
            "cfg":          ind.cfg,
            "material":     CONFIG_NAMES.get(ind.cfg, "?"),
            "thickness_cm": ind.thickness,
            "transmission": ind.transmission,
            "fitness":      ind.fitness,
        })

    # JSON kaydet
    Path("results").mkdir(exist_ok=True)
    with open(GA_JSON, "w") as f:
        json.dump({
            "parametreler": {
                "pop_size":    POP_SIZE,
                "n_gen":       N_GEN,
                "n_events_ga": N_EVENTS_GA,
                "mutation":    MUTATION_RATE,
            },
            "en_iyi": {
                "cfg":          best.cfg,
                "material":     CONFIG_NAMES.get(best.cfg, "?"),
                "thickness_cm": best.thickness,
                "transmission": trans_val,
                "mean_edep_MeV": edep_val,
            },
            "top5":   top5,
            "history": history,
        }, f, indent=2, ensure_ascii=False)

    print(f"\n  GA sonuclari kaydedildi: {GA_JSON}")
    return best

# =============== ROOT ANALIZ (opsiyonel) ===============
def print_sweep_summary():
    """Mevcut sweep verisini ozetle (GA oncesi hizli bakim)."""
    if not Path(ROOT_FILE).exists():
        return
    try:
        import uproot
        with uproot.open(ROOT_FILE) as f:
            data = f["T"].arrays(["cfg","material","thickness_cm","transmission"], library="np")
        n = len(data["cfg"])
        print(f"\n--- Mevcut sweep verisi ({n} run) ---")
        order = sorted(range(n), key=lambda i: data["transmission"][i])
        print(f"  {'#':<4} {'cfg':<4} {'material':<44} {'t(cm)':<7} {'trans'}")
        for rank, i in enumerate(order[:10], 1):
            mat = str(data["material"][i], 'utf-8') if isinstance(data["material"][i], bytes) else str(data["material"][i])
            print(f"  {rank:<4} {int(data['cfg'][i]):<4} {mat:<44} "
                  f"{float(data['thickness_cm'][i]):<7.1f} {float(data['transmission'][i]):.4f}")
        print(f"  ... (ilk 10 gosterildi)")
    except ImportError:
        pass
    except Exception as e:
        print(f"  [WARN] Sweep ozet hatasi: {e}")

# =============== MAIN ===============
if __name__ == "__main__":
    print_sweep_summary()
    best = run_ga()
