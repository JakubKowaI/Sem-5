using Plots

# przygotowanie danych
h = [2.0^-n for n in 0:54]   # h = 1, 1/2, 1/4, ..., 2^-60
one_plus_h = 1 .+ h

# wykres
plot(h, one_plus_h,
    xscale = :log10,
    xlabel = "h",
    ylabel = "1 + h",
    title = "Zachowanie wartości 1 + h w arytmetyce Float64",
    legend = false,
    linewidth = 2,
    marker = :circle,
    color = :blue,
)

# linia referencyjna y=1
hline!([1.0], color=:red, linestyle=:dash, label="1.0")

# pionowa linia przy h = 2^-52
vline!([2.0^-52], color=:green, linestyle=:dot, label="h = 2⁻⁵²")
