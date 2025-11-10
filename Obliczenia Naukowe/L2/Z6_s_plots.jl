#Jakub Kowal i Gemini 2.5 Pro
using Plots
using Printf

"""
    stworz_diagram_pajeczynowy(c, x0, n_iteracji; plik_wyjsciowy=nothing)

Generuje i wyświetla diagram pajęczynowy (cobweb plot) dla równania x_{n+1} = x_n^2 + c,
z adnotacją dla punktu startowego x₀.

# Argumenty
- `c`: Stała w równaniu.
- `x0`: Wartość początkowa x₀.
- `n_iteracji`: Liczba iteracji do wykonania.
- `plik_wyjsciowy`: Opcjonalna nazwa pliku (np. "wykres.png"), do którego zostanie zapisany wykres.
"""
function stworz_diagram_pajeczynowy(c::Float64, x0::Float64, n_iteracji::Int; plik_wyjsciowy::Union{String, Nothing}=nothing)
    
    # 1. Obliczanie sekwencji iteracji
    # -----------------------------------
    x_seq = zeros(Float64, n_iteracji + 1)
    x_seq[1] = x0
    for i in 2:n_iteracji
        x_seq[i] = x_seq[i-1]^2 + c
    end

    # 2. Tworzenie ścieżki "pajęczyny"
    # ---------------------------------
    path_x = [x0]
    path_y = [0.0]
    for i in 1:n_iteracji
        push!(path_x, x_seq[i])
        push!(path_y, x_seq[i+1])
        push!(path_x, x_seq[i+1])
        push!(path_y, x_seq[i+1])
    end

    # 3. Definiowanie zakresu wykresu
    # --------------------------------
    min_val = min(minimum(x_seq), 0.0) # Uwzględniamy 0, żeby adnotacja była widoczna
    max_val = max(maximum(x_seq), 0.0)
    margin = (max_val - min_val) * 0.15
    zakres_x = (min_val - margin, max_val + margin)
    zakres_y = (min_val - margin, max_val + margin)

    # 4. Rysowanie
    # ------------
    p = plot(
        title = @sprintf("Diagram pajęczynowy: c=%.3f, x₀=%.3f", c, x0),
        xlabel = "xₙ",
        ylabel = "xₙ₊₁",
        aspect_ratio = :equal,
        legend = :outertopright,
        xlims = zakres_x,
        ylims = zakres_y
    )

    f(x) = x^2 + c
    plot!(p, f, zakres_x..., label="y = x² + c", lw=2, color=:blue)
    plot!(p, x -> x, zakres_x..., label="y = x", lw=2, color=:red, linestyle=:dash)
    plot!(p, path_x, path_y, label="Iteracje", lw=1.5, color=:green, markershape=:circle, markersize=2)

    # 5. Dodanie adnotacji dla x₀ (NOWA CZĘŚĆ)
    # ----------------------------------------
    # Dodajemy marker w punkcie startowym na osi X
    scatter!(p, [x0], [0], color=:purple, markersize=5, label="")
    
    # Obliczamy mały offset, aby tekst był czytelnie umieszczony pod osią X
    y_offset = (zakres_y[2] - zakres_y[1]) * 0.04 # 4% wysokości wykresu
    
    # Dodajemy etykietę "x₀"
    annotate!(p, [(x0, -y_offset, text("x₀", 10, :center, :top, :purple))])


    # 6. Zapisywanie i wyświetlanie
    # -------------------------------
    if plik_wyjsciowy !== nothing
        savefig(p, plik_wyjsciowy)
        println("Wykres został zapisany do pliku: $plik_wyjsciowy")
    end
    
    display(p)
end

# --- Przykłady użycia ---

# Przykład 1: Cykl dla c=-1, x₀=0.75
stworz_diagram_pajeczynowy(-1.0, 0.75, 20, plik_wyjsciowy="wykres_z_adnotacja_1.png")

# Przykład 2: Jeden z Twoich przypadków: c=-2, x₀=1.99999999999999
stworz_diagram_pajeczynowy(-2.0, 1.99999999999999, 40, plik_wyjsciowy="wykres_z_adnotacja_2.png")