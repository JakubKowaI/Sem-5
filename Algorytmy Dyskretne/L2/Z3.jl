using JuMP
import GLPK
using JSON

function load_from_json(path::String)
    s = read(path, String)
    j = JSON.parse(s)

    Okresy=1:Int(j["Okresy"])
    MaxTowar=Int(j["MaxTowar"])
    KosztProdukcji=Array(j["KosztProdukcji"])
    ProdukcjaPonadwymiarowa=Array(j["ProdukcjaPonadwymiarowa"])
    KosztProdukcjiPonadwymiarowej=Array(j["KosztProdukcjiPonadwymiarowej"])
    Popyt=Array(j["Popyt"])
    Magazyn=Int(j["Magazyn"])
    KosztMagazynowania=Int(j["KosztMagazynowania"])
    TowarPoczatkowy=Int(j["TowarPoczatkowy"])

    return Okresy, MaxTowar, KosztProdukcji, ProdukcjaPonadwymiarowa, KosztProdukcjiPonadwymiarowej, Popyt, Magazyn, KosztMagazynowania, TowarPoczatkowy
end

#uzyj domyslnych jesli nie podano sciezki
input = length(ARGS) >= 1 ? try
        load_from_json(ARGS[1])
    catch e
        @warn "Nie podano sciezki";
        nothing
    end : nothing

if input === nothing
    Okresy=1:4
    MaxTowar=100
    KosztProdukcji=[6000 4000 8000 9000]
    ProdukcjaPonadwymiarowa=[60 65 70 60]
    KosztProdukcjiPonadwymiarowej=[8000 6000 10000 11000]
    Popyt=[130 80 125 195]
    Magazyn=70
    KosztMagazynowania=1500
    TowarPoczatkowy=15
else
    Okresy, MaxTowar, KosztProdukcji, ProdukcjaPonadwymiarowa, KosztProdukcjiPonadwymiarowej, Popyt, Magazyn, KosztMagazynowania, TowarPoczatkowy = input
end


model = Model(GLPK.Optimizer)

@variable(model,MaxTowar>=x[Okresy]>=0)
@variable(model,o[Okresy]>=0)
@variable(model,Magazyn>=m[Okresy]>=0)

@constraint(model, [j in Okresy],o[j]<=ProdukcjaPonadwymiarowa[j])

for j in Okresy
    if j == 1
        @constraint(model, TowarPoczatkowy + x[1] + o[1] - Popyt[1] == m[1])
    else
        @constraint(model, m[j-1] + x[j] + o[j] - Popyt[j] == m[j])
    end
end

@objective(model, Min, sum(KosztProdukcji[j]*x[j] + KosztProdukcjiPonadwymiarowej[j]*o[j] + KosztMagazynowania*m[j] for j in Okresy))

optimize!(model)
objective_value(model)

for j in Okresy
    println("j: ",j," ", value(x[j])," ",value(o[j])," ",value(m[j]))
end