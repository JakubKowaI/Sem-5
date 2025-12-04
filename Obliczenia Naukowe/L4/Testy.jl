#Jakub Kowal

include("Funkcje.jl")
using .Funkcje

# --- Zadanie 5 ---
println("\n--- Zadanie 5: Testy podstawowe (węzły równoodległe) ---")

# (a) f(x) = e^x na [0, 1]
f1(x) = exp(x)
println("Rysowanie e^x, [0, 1]")
for n in [5, 10, 15]
    println("  Stopień n = $n")
    rysujNnfx(f1, 0.0, 1.0, n; wezly=:rownoodlegle)
    sleep(1) # Pauza, żeby wykres zdążył się wyrenderować/zmienić
end

# (b) f(x) = x^2 * sin(x) na [-1, 1]
f2(x) = x^2 * sin(x)
println("Rysowanie x^2 * sin(x), [-1, 1]")
for n in [5, 10, 15]
    println("  Stopień n = $n")
    rysujNnfx(f2, -1.0, 1.0, n; wezly=:rownoodlegle)
    sleep(1)
end

# --- Zadanie 6 ---
# println("\n--- Zadanie 6: Zjawisko rozbieżności i Rungego ---")

# # (a) f(x) = |x| na [-1, 1]
# f3(x) = abs(x)
# println("Rysowanie |x|, [-1, 1]")
# for n in [5, 10, 15]
#     println("  Stopień n = $n, węzły równoodległe")
#     rysujNnfx(f3, -1.0, 1.0, n; wezly=:rownoodlegle)
#     sleep(1)
    
#     println("  Stopień n = $n, węzły Czebyszewa")
#     rysujNnfx(f3, -1.0, 1.0, n; wezly=:czebyszew)
#     sleep(1)
# end

# # (b) f(x) = 1 / (1 + x^2) na [-5, 5] (Funkcja Rungego)
# f4(x) = 1.0 / (1.0 + x^2)
# println("Rysowanie 1/(1+x^2), [-5, 5]")
# for n in [5, 10, 15]
#     println("  Stopień n = $n, węzły równoodległe (oczekuj oscylacji na brzegach!)")
#     rysujNnfx(f4, -5.0, 5.0, n; wezly=:rownoodlegle)
#     sleep(2) # Dłuższa pauza, by zobaczyć efekt Rungego
    
#     println("  Stopień n = $n, węzły Czebyszewa (lepsze przybliżenie)")
#     rysujNnfx(f4, -5.0, 5.0, n; wezly=:czebyszew)
#     sleep(1)
# end