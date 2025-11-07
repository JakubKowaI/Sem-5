using Plots

f(x) = exp(x) * log(1 + exp(-x))

x = -10:0.1:10

plot(x, f.(x), label="f(x) = e^x * ln(1 + e^{-x})", linewidth=2, color=:blue)
hline!([1], linestyle=:dash, color=:red, label="lim x→∞ f(x) = 1")

xlabel!("x")
ylabel!("f(x)")
title!("Wykres funkcji f(x) = e^x * ln(1 + e^{-x})")