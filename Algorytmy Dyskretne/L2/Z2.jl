using JuMP
import GLPK
using JSON

function load_from_json(path::String)
    s = read(path, String)
    j = JSON.parse(s)

    P=Int(j["P"])
    M=Int(j["M"])
    Czas=Int(j["Czas"])
    Ceny=Array(j["Ceny"])
    KosztCzas=[Float64(x/60) for x in j["KosztCzas"]]
    KosztMat=[Int(x) for x in j["KosztMat"]]
    MaxPopyt=[Int(x) for x in j["MaxPopyt"]]

    raw_Czasy = j["Czasy"]
    Czasy = zeros(Int, length(raw_Czasy), M)
    for (idx, a) in enumerate(raw_Czasy)
        if isa(a, Array)
            Czasy[idx, :] = [Int(x) for x in a]
        else
            error("Blad w koszcie")
        end
    end

    return 1:P, 1:M, Czasy, Czas, Ceny, KosztCzas, KosztMat, MaxPopyt 
end

#uzyj domyslnych jesli nie podano sciezki
input = length(ARGS) >= 1 ? try
        load_from_json(ARGS[1])
    catch e
        @warn "Nie podano sciezki";
        nothing
    end : nothing

if input === nothing
    P=1:4
    M=1:3
    Czasy=[5 3 4 4; 10 6 5 2; 6 4 3 1]#Czasy produkcji p dla m - [m,p]
    Czas=60*60#Czas dostępny w tygodniu w minutach
    Ceny=[9 7 6 5]#ceny dla p
    KosztCzas=[2/60 2/60 3/60]#koszt dla m
    KosztMat=[4 1 1 1]#koszt dla p
    MaxPopyt=[400 100 150 500]#popyt dla p
else
    P, M, Czasy, Czas, Ceny, KosztCzas, KosztMat, MaxPopyt = input
end

Zysk=zeros(Float64,length(M),length(P))
model=Model(GLPK.Optimizer)

for m in M
    for p in P
        Zysk[m,p]=Ceny[p]-KosztMat[p]-KosztCzas[m]*Czasy[m,p]
    end
end

@variable(model,x[M,P]>=0)

@constraint(model,[p in P],sum(x[:,p])<=MaxPopyt[p])
@constraint(model, [m in M],sum(x[m,p]*Czasy[m,p] for p in P)<=Czas)

@objective(model, Max, sum(Ceny[p]*sum(x[m,p] for m in M) for p in P)-sum(x[m,p]*KosztCzas[m]*Czasy[m,p] for m in M,p in P)-sum(x[m,p]*KosztMat[p] for m in M,p in P))

optimize!(model)

println(objective_value(model))
for m in M
    czas=sum(value(x[m,p])*Czasy[m,p] for p in P)
    println("Czas dla ",m," : ",czas)
end
for m in M, p in P
    println("x[",m,",",p,"] = ", value(x[m,p]))
end