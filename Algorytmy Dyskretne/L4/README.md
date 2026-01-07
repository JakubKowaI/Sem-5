# Rozwiązania zadań - Algorytmy Dyskretne L4

## Wymagania
* Kompilator C++ (g++)
* Julia
* Pakiety Julia: `JuMP`, `GLPK`, `Plots`

## Budowanie
W katalogu `L4` uruchom:
```bash
make
```
Zostaną utworzone pliki wykonywalne `maxflow` oraz `matching`.

## Uruchamianie testów
Skrypt `scripts/run_experiments.jl` uruchamia algorytmy na wielu wątkach, porównuje wyniki z JuMP/GLPK (dla małych instancji) oraz generuje wykresy.

Uruchomienie:
```bash
export JULIA_NUM_THREADS=4  # Ustawienie liczby wątków
julia scripts/run_experiments.jl
```

## Opis plików
* `src/maxflow.cpp`: Implementacja Zadania 1 (Edmonds-Karp) oraz Zadania 4 (Dinic). Generuje też model LP (Zadanie 3).
* `src/matching.cpp`: Implementacja Zadania 2 (Skojarzenie w grafie dwudzielnym). Generuje też model LP (Zadanie 3).
* `src/flow_utils.hpp`: Wspólne struktury grafu i algorytmy przepływu.
* `scripts/run_experiments.jl`: Skrypt testujący, uruchamiający obliczenia równolegle i tworzący wykresy.
