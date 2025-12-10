#!/usr/bin/env python3
"""
plot_results_by_family.py

Skrypt przeszukuje katalog z wynikami (domyślnie "wyniki"), gdzie struktura katalogów
to:
  wyniki/<algorithm>/...pliki_wynikowe...

Każdy plik wynikowy zawiera linie takie jak:
  f ... processed/<family>.<n>.<...>.<random|single>...
  t <time>

Dla każdej rodziny (np. Long-C) rysuje wykres czasu względem n.
Na jednym wykresie umieszczone są serie dla każdej kombinacji:
  <algorithm> <type>  (np. "dijkstra random", "radixHeap single")

Zapisuje wykresy do katalogu wyjściowego (domyślnie "plots").

Użycie:
  python plot_results_by_family.py --input-dir wyniki --output-dir plots --format png

Opcje:
  --logy        : użyj logarytmicznej skali Y
  --show        : pokaż wykresy interaktywnie
  --min-points  : minimalna liczba punktów w serii aby ją narysować (domyślnie 1)
"""
from collections import defaultdict
import argparse
import os
import re
import sys
from typing import Dict, List, Tuple, Optional

import matplotlib.pyplot as plt

# Regex do wydobycia processed/<family>.<n>...<random|single>
PROCESSED_RE = re.compile(
    r"processed/(?P<family>[^./\\]+)\.(?P<n>\d+)(?:\.[^/\\]*)*(?:\.(?P<type>random|single))",
    flags=re.IGNORECASE,
)


def parse_f_line(line: str) -> Optional[Tuple[str, int, str]]:
    """Parsuje linię zaczynającą się od 'f' i zwraca (family, n, type) lub None."""
    parts = line.strip().split()
    for token in parts:
        if "processed/" in token:
            m = PROCESSED_RE.search(token)
            if m:
                family = m.group("family")
                n = int(m.group("n"))
                typ = m.group("type").lower() if m.group("type") else "unknown"
                return family, n, typ
    return None


def parse_result_file(path: str) -> Optional[Tuple[str, int, str, float]]:
    """
    Parsuje pojedynczy plik wynikowy. Zwraca (family, n, type, time) lub None jeżeli brakuje danych.
    """
    family = None
    n = None
    typ = None
    time = None
    try:
        with open(path, "r", encoding="utf-8", errors="replace") as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                if line.startswith("f "):
                    parsed = parse_f_line(line)
                    if parsed:
                        family, n, typ = parsed
                elif line.startswith("t "):
                    tokens = line.split()
                    if len(tokens) >= 2:
                        try:
                            time = float(tokens[1])
                        except ValueError:
                            pass
                if family is not None and n is not None and typ is not None and time is not None:
                    break
    except Exception as e:
        print(f"Warning: nie mogę wczytać pliku {path}: {e}", file=sys.stderr)
        return None

    if family is None or n is None or typ is None or time is None:
        return None
    return family, n, typ, time


def collect_data(input_dir: str) -> Tuple[Dict[str, Dict[Tuple[str, str], List[Tuple[int, float]]]], set]:
    """
    Przeszukuje katalog input_dir. Traktuje pierwszy komponent ścieżki jako 'algorithm'.
    Zwraca strukturę:
      data[family][(algorithm, type)] = [(n, time), ...]
    oraz zbiór znalezionych algorytmów.
    """
    data: Dict[str, Dict[Tuple[str, str], List[Tuple[int, float]]]] = defaultdict(lambda: defaultdict(list))
    algorithms = set()

    if not os.path.isdir(input_dir):
        raise FileNotFoundError(f"Katalog wejściowy '{input_dir}' nie istnieje lub nie jest katalogiem.")

    for root, dirs, files in os.walk(input_dir):
        # algorytm to pierwszy komponent relatywnej ścieżki względem input_dir
        rel = os.path.relpath(root, input_dir)
        if rel == ".":
            # pliki bezpośrednio w input_dir - oznacz jako 'unknown'
            algorithm = "unknown"
        else:
            algorithm = rel.split(os.sep)[0]
        algorithms.add(algorithm)

        for fname in files:
            if fname.startswith("."):
                continue
            full = os.path.join(root, fname)
            parsed = parse_result_file(full)
            if parsed:
                family, n, typ, time = parsed
                key = (algorithm, typ)
                data[family][key].append((n, time))
            else:
                continue
    return data, algorithms


def plot_family(
    family: str,
    series: Dict[Tuple[str, str], List[Tuple[int, float]]],
    out_path: str,
    fmt: str = "png",
    logy: bool = False,
    show: bool = False,
    min_points: int = 1,
):
    """
    Rysuje wykres dla jednej rodziny. series: (algorithm, type) -> [(n, time), ...]
    Zapisuje do out_path (bez rozszerzenia).
    """
    plt.figure(figsize=(9, 6))
    plotted_any = False
    for (algorithm, typ), points in sorted(series.items(), key=lambda x: (x[0][0], x[0][1])):
        if not points or len(points) < min_points:
            continue
        pts_sorted = sorted(points, key=lambda x: x[0])
        ns = [p[0] for p in pts_sorted]
        times = [p[1] for p in pts_sorted]
        label = f"{algorithm} {typ}"
        plt.plot(ns, times, marker="o", linestyle="-", label=label)
        plotted_any = True

    if not plotted_any:
        print(f"Uwaga: brak wystarczających danych dla rodziny '{family}'. Pomijam.", file=sys.stderr)
        plt.close()
        return

    plt.xlabel("n")
    plt.ylabel("czas [s]")
    plt.title(f"Czas wykonania vs n — {family}")
    plt.grid(True, which="both", linestyle="--", linewidth=0.5)
    plt.legend()
    if logy:
        plt.yscale("log")
    os.makedirs(os.path.dirname(out_path), exist_ok=True)
    save_path = f"{out_path}.{fmt}"
    plt.tight_layout()
    plt.savefig(save_path)
    print(f"Zapisano wykres: {save_path}")
    if show:
        plt.show()
    plt.close()


def main():
    parser = argparse.ArgumentParser(description="Rysuje wykresy czasu względem n dla wyników (katalogi = algorytmy).")
    parser.add_argument("--input-dir", "-i", default="wyniki", help="Katalog wejściowy (domyślnie 'wyniki').")
    parser.add_argument("--output-dir", "-o", default="plots", help="Katalog wyjściowy (domyślnie 'plots').")
    parser.add_argument("--format", "-f", default="png", choices=["png", "pdf", "svg"], help="Format wykresów.")
    parser.add_argument("--logy", action="store_true", help="Użyć logarytmicznej skali Y.")
    parser.add_argument("--show", action="store_true", help="Pokaż wykresy lokalnie.")
    parser.add_argument("--min-points", type=int, default=1, help="Minimalna liczba punktów w serii, aby ją narysować.")
    args = parser.parse_args()

    try:
        data, algorithms = collect_data(args.input_dir)
    except FileNotFoundError as e:
        print(f"Błąd: {e}", file=sys.stderr)
        sys.exit(2)

    if not data:
        print("Brak danych do narysowania (nie znaleziono pasujących plików).", file=sys.stderr)
        sys.exit(1)

    print(f"Znalezione rodziny: {len(data)}, algorytmy: {sorted(list(algorithms))}")

    for family, series in sorted(data.items()):
        safe_family = family.replace("/", "_").replace("\\", "_")
        out_base = os.path.join(args.output_dir, safe_family, f"{safe_family}_time_vs_n")
        plot_family(
            family,
            series,
            out_base,
            fmt=args.format,
            logy=args.logy,
            show=args.show,
            min_points=args.min_points,
        )


if __name__ == "__main__":
    main()