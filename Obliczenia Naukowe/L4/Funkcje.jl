#Jakub Kowal

module Funkcje

export ilorazyRoznicowe, warNewton, naturalna, rysujNnfx

using Plots

function ilorazyRekurencja(a::Int,b::Int,f::Vector{Float64},x::Vector{Float64})
    if a==b
        return f[a]
    end
    return ilorazyRekurencja(a+1,b,f,x)-ilorazyRekurencja(a,b-1,f,x)/(x[b]-x[a])
end

function ilorazyRoznicowe(x::Vector{Float64}, f::Vector{Float64})
    n = length(x) - 1
    fx = zeros(Float64,n+1);1
    
    for i in 1:n+1
        fx[i]=ilorazyRekurencja(1,i,f,x)
    end

    return fx
end

function warNewton(x::Vector{Float64}, fx::Vector{Float64}, t::Float64)
    n = length(fx) - 1
    nt = fx[n+1]
    
    for i in n:-1:1
        nt = fx[i] + (t - x[i]) * nt
    end
    return nt
end

"""
Algorytm działa w ten sposób: pokolei przenosi cn coraz bardziej w prawo i wykonuje na nich obliczenia, ktore sie stackuja dla kazdego z a w kazdej iteracji (oprocz an bo an = fxn), jak na kartce
"""
function naturalna(x::Vector{Float64}, fx::Vector{Float64})
    n = length(fx) - 1
    a = zeros(Float64, n + 1)
    
    a[1] = fx[n+1] 
    
    for i in n:-1:1
        for j in (n+1):-1:2
            a[j] = a[j-1] - x[i] * a[j]
        end
        
        a[1] = fx[i] - x[i] * a[1]
    end
    
    return a
end

function rysujNnfx(f, a::Float64, b::Float64, n::Int; wezly::Symbol = :rownoodlegle)
    x_nodes = Vector{Float64}(undef, n + 1)
    
    if wezly == :rownoodlegle
        h = (b - a) / n
        for k in 0:n
            x_nodes[k+1] = a + k * h
        end
    elseif wezly == :czebyszew
        T = Vector{Function}(undef, n)

        T[1] = x -> 1.0
        T[2] = x -> x

        for i in 3:n
            T[i] = x -> 2*x*T[i-1](x) - T[i-2](x)
        end
    end
    
    y_nodes = f.(x_nodes)
    
    ilorazy = ilorazyRoznicowe(x_nodes, y_nodes)

    plot_x = range(a, b, length=500)
    
    plot_y = f.(plot_x)
    
    plot_y_wiel = [warNewton(x_nodes, ilorazy, t) for t in plot_x]
    
    p = plot(plot_x, plot_y, label="Funkcja f(x)", lw=2, title="Interpolacja n=$n, węzły: $wezly")
    plot!(p, plot_x, plot_y_wiel, label="Wielomian W_n(x)", ls=:dash, lw=2)
    scatter!(p, x_nodes, y_nodes, label="Węzły", color=:red, markersize=4)
    
    display(p)
end

end