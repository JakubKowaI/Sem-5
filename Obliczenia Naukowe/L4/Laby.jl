include("Funkcje.jl")
using .Funkcje

x=[-1.0,0.0,1.0,2.0]
fx=[-1.0,0.0,-1.0,2.0]

iloraz=ilorazyRoznicowe(x,fx)
print(naturalna(x,iloraz))