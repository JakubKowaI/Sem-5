using Plots

# przygotowanie danych
h = [2.0^-n for n in 0:54]   # h = 1, 1/2, 1/4, ..., 2^-54
one_plus_h = 1 .+ h

# znajdź pierwsze h takie, że 1.0 + h == 1.0
idx = findfirst(x -> 1.0 + x == 1.0, h)
h_equal = idx === nothing ? nothing : h[idx]

# wykres
plot(h, one_plus_h,
    xscale = :log10,
    xlabel = "h",
    ylabel = "1 + h",
    title = "Zachowanie wartości 1 + h w arytmetyce Float64",
    legend = :outertopright,
    linewidth = 2,
    marker = :circle,
    color = :blue,
)

# linia referencyjna y=1
hline!([1.0], color=:red, linestyle=:dash, label="1.0")

# pionowa linia przy h = 2^-52 (jak w oryginale)
vline!([2.0^-52], color=:green, linestyle=:dot, label="h = 2⁻⁵²")

# pionowa, czarna, przerywana linia w momencie kiedy 1.0 + h == 1.0
if h_equal !== nothing
    vline!([h_equal], color=:black, linestyle=:dash, linewidth=2,
           label="1.0 + h == 1.0 (h = $(h_equal))")
else
    @info "Nie znaleziono h takiego, że 1.0 + h == 1.0 w zadanym zakresie"
end