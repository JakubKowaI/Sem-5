using JuMP
import GLPK
using JSON

function load_from_json(path::String)
    s = read(path, String)
    j = JSON.parse(s)

    F = Int(j["F"])
    L = Int(j["L"])
    Zasoby = [Int(x) for x in j["Zasoby"]]
    Potrzeby = [Int(x) for x in j["Potrzeby"]]

    raw_Koszt = j["Koszt"]
    Koszt = zeros(Int, length(raw_Koszt), L)
    for (idx, a) in enumerate(raw_Koszt)
        if isa(a, Array)
            Koszt[idx, :] = [Int(x) for x in a]
        else
            error("Blad w koszcie")
        end
    end

    return 1:F, 1:L, Zasoby, Potrzeby, Koszt
end

#uzyj domyslnych jesli nie podano sciezki
input = length(ARGS) >= 1 ? try
        load_from_json(ARGS[1])
    catch e
        @warn "Nie podano sciezki";
        nothing
    end : nothing

if input === nothing
    F=1:3
    L=1:4
    Zasoby=[275000,550000,660000]
    Potrzeby=[110000,220000,330000,440000]
    Koszt=[10 10 9 11; 7 11 12 13;8 14 4 9]
else
    F, L, Zasoby, Potrzeby, Koszt = input
end

model=Model(GLPK.Optimizer)

@variable(model, x[F, L]>=0)
@constraint(model, [f in F],sum(x[f, :])<=Zasoby[f])
@constraint(model, [l in L],sum(x[:,l])==Potrzeby[l])
@objective(model,Min,sum(Koszt[f,l]*x[f,l] for l in L, f in F))
optimize!(model)
model
println("Status: ", termination_status(model))
println("Wynik: ", objective_value(model))
for f in F
    println("F: ",f," ",sum(value(x[f,:])))
end