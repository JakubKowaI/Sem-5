import os
import glob
import pandas as pd
import matplotlib.pyplot as plt
import re

def parse_res_file(filepath):
    """Parsuje pojedynczy plik .res i zwraca słownik z danymi."""
    data = {}
    with open(filepath, 'r') as f:
        lines = f.readlines()
        
    for line in lines:
        parts = line.strip().split()
        if not parts:
            continue
            
        if parts[0] == 'p':
            # Format: p res sp ss [algorytm]
            # Czasami algorytm może być zapisany inaczej, np. 'dikstra' zamiast 'dijkstra'
            algo = parts[4]
            if algo == 'dikstra': algo = 'dijkstra' # Normalizacja nazwy
            data['algorithm'] = algo
            
        elif parts[0] == 'f':
            # Format: f [ścieżka_grafu] [ścieżka_źródeł]
            # Wyciągamy nazwę rodziny z ścieżki grafu, np. USA-road-d.W.gr -> USA-road-d.W
            graph_path = parts[1]
            filename = os.path.basename(graph_path)
            family = filename.replace('.gr', '')
            data['family'] = family
            
        elif parts[0] == 't':
            # Format: t [czas]
            data['time'] = float(parts[1])
            
    return data

def main():
    # Ścieżka do katalogu z wynikami - dostosuj jeśli pliki są gdzie indziej
    results_dir = 'wyniki' 
    
    # Wzorzec plików do szukania (rekurencyjnie)
    files = glob.glob(os.path.join(results_dir, '**/*.res'), recursive=True)
    
    records = []
    
    for file in files:
        try:
            parsed = parse_res_file(file)
            # Filtrowanie tylko dla rodziny USA-road-d
            if 'family' in parsed and 'USA-road-d' in parsed['family']:
                records.append(parsed)
        except Exception as e:
            print(f"Błąd przetwarzania pliku {file}: {e}")

    if not records:
        print("Nie znaleziono pasujących danych.")
        return

    df = pd.DataFrame(records)
    
    # Grupowanie i uśrednianie (na wypadek wielu plików dla tego samego testu)
    df_grouped = df.groupby(['family', 'algorithm'])['time'].mean().unstack()

    # Rysowanie wykresu
    ax = df_grouped.plot(kind='bar', figsize=(12, 6), width=0.8)
    
    plt.title('Porównanie czasów działania algorytmów dla rodziny USA-road-d')
    plt.ylabel('Czas [s]')
    plt.xlabel('Rodzina danych')
    plt.xticks(rotation=45, ha='right')
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.legend(title='Algorytm')
    plt.tight_layout()
    
    # Zapisanie wykresu
    plt.savefig('wykres_czasow.png')
    print("Wykres zapisano jako wykres_czasow.png")
    plt.show()

if __name__ == "__main__":
    main()