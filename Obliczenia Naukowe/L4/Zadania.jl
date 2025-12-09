#Jakub Kowal

include("Funkcje.jl")
using .Funkcje

using Plots

    # Zadanie 5
    # (a) f(x) = e^x na [0, 1]
    f1(x) = exp(x)

    for n in [5, 10, 15]
        p= Funkcje.rysujNnfx(f1, 0.0, 1.0, n; wezly=:rownoodlegle)
        title!("e^x stopnia $n")
        p[1][1][:label] = "Funckja e^x"
        savefig("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L4/Plots/Z5_a_$n.pdf")
        #display(p)
        
    end

    # (b) f(x) = x^2 * sin(x) na [-1, 1]
    f2(x) = x^2 * sin(x)

    for n in [5, 10, 15]
        p= Funkcje.rysujNnfx(f2, -1.0, 1.0, n; wezly=:rownoodlegle)
        title!("x^2*sin(x) stopnia $n")
        p[1][1][:label] = "Funckja x^2*sin(x)"
        savefig("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L4/Plots/Z5_b_$n.pdf")
        #display(p)
    end

    # Zadanie 6
    # (a) f(x) = |x| na [-1, 1]
    f3(x) = abs(x)

    for n in [5, 10, 15]
        p= Funkcje.rysujNnfx(f3, -1.0, 1.0, n; wezly=:rownoodlegle)
        title!("|x| stopnia $n z węzłami równoodległymi")
        p[1][1][:label] = "Funckja |x|"
        savefig("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L4/Plots/Z6_a_r_$n.pdf")
        #display(p)
        
        p= Funkcje.rysujNnfx(f3, -1.0, 1.0, n; wezly=:czebyszew)
        title!("|x| stopnia $n z węzłami Czebyszewa")
        p[1][1][:label] = "Funckja |x|"
        savefig("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L4/Plots/Z6_a_c_$n.pdf")
        #display(p)
    end

    # (b) f(x) = 1 / (1 + x^2) na [-5, 5] (Funkcja Rungego)
    f4(x) = 1.0 / (1.0 + x^2)

    for n in [5, 10, 15]
        p= Funkcje.rysujNnfx(f4, -5.0, 5.0, n; wezly=:rownoodlegle)
        title!("1/(1+x^2) stopnia $n dla węzłów równoodległych")
        p[1][1][:label] = "Funkcja 1/(1+x^2)"
        savefig("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L4/Plots/Z6_b_r_$n.pdf")
        #display(p)
        
        p= Funkcje.rysujNnfx(f4, -5.0, 5.0, n; wezly=:czebyszew)
        title!("1/(1+x^2) stopnia $n dla węzłów Czebyszewa")
        p[1][1][:label] = "Funkcja 1/(1+x^2)"
        savefig("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L4/Plots/Z6_b_c_$n.pdf")
        #display(p)
    end

    println("Done")