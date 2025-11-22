#Jakub Kowal
include("Metody.jl")

function f(x)
    return 0.0
end

function fsin(x)
    return -(x/2.0)^2+sin(x)
end

function pfsin(x)
    return cos(x) - x/2.0
end

function fy(x)
    return 3x
end

function fe(x)
    return ℯ^x
end

function f5(x)
    return fy(x)-fe(x)
end

function f1(x)
    return ℯ^(1-x)-1
end

function pf1(x)
    return -ℯ^(1-x)
end

function f2(x)
    return x*ℯ^(-x)
end

function pf2(x)
    return (x-1)*-ℯ^(-x)
end

#Zad 1-3
# try
#     println("Zadania 1-3")
#     println(mbisekcji(f,1.0,10.0,0.5*10^-5,0.5*10^-5))
#     println(mstycznych(f,f,1.0,0.5*10^-5,0.5*10^-5,30))
#     println(msiecznych(f,1.0,10.0,0.5*10^-5,0.5*10^-5,30))
#     #NaN bo 1.0 - 0.0 * Inf
# catch e
#     println("Wystapil blad: ",e)
# end

#Zad 4
# try
#     println("Zadanie 4")
#     println(mbisekcji(fsin,1.5,2.0,0.5*10^-5,0.5*10^-5))
#     println(mstycznych(fsin,pfsin,1.5,0.5*10^-5,0.5*10^-5,30))
#     println(msiecznych(fsin,1.0,2.0,0.5*10^-5,0.5*10^-5,30))
# catch e
#     println("Wystapil blad: ",e)
# end

#Zad 5
# try
#     println("Zadanie 5")
#     w=mbisekcji(f5,0.0,1.0,10^-4,10^-4)
#     println(mbisekcji(f5,0.0,1.0,10^-4,10^-4))
#     println(fy(w[1]))
# catch e
#     println("Wystapil blad: ",e)
# end

#Zad 6
try
    println("Zadanie 6")
    println("f1")
    println(mbisekcji(f1,0.5,1.5,10^-5,10^-5))
    println(mstycznych(f1,pf1,0.5,10^-5,10^-5,30))
    println(msiecznych(f1,0.5,0.75,10^-5,10^-5,30))
    println("f2")
    println(mbisekcji(f2,-1.0,1.0,10^-5,10^-5))
    println(mstycznych(f2,pf2,-1.0,10^-5,10^-5,30))
    println(msiecznych(f2,-1.0,-0.5,10^-5,10^-5,30))
    println("Test")
    println(mstycznych(f1,pf1,1.5,10^-5,10^-5,30))
    println(mstycznych(f2,pf2,0.5,10^-5,10^-5,30))
    println(mstycznych(f2,pf2,1.0,10^-5,10^-5,30))
    #Nie można wybrać x0=1 bo pochodna = 0
catch e
    println("Wystapil blad: ",e)
end