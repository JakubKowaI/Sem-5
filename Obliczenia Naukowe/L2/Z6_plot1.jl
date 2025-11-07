using Plots

function cobweb(f, x0; n=40, xrange=(-1.25, 1.0))
    xs = range(xrange[1], xrange[2], length=400)
    p = plot(xs, f.(xs), label="f(x)", legend=:bottomright)
    plot!(p, xs, xs, label="y = x", color=:black, linestyle=:dot)

    x = x0
    for i in 1:n
        y = f(x)
        plot!(p, [x, x], [x, y], color=:red)
        plot!(p, [x, y], [y, y], color=:red)
        x = y
    end

    display(p)  # <- to wymusza pokazanie wykresu
    return p
end


cobweb(x -> x^2 - 1, 0.25)
