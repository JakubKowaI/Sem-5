using JuMP
import GLPK
using JSON

function load_from_json(path::String)
    s = read(path, String)
    j = JSON.parse(s)

    P = Int(j["P"])
    Z = Int(j["Z"])
    Dost = Array(j["Dostepne"])
    Pow = Array(j["Powinno"])

    raw_Min = j["Min"]
    Min = zeros(Int, length(raw_Min), Z)
    for (idx, a) in enumerate(raw_Min)
        if isa(a, Array)
            Min[idx, :] = [Int(x) for x in a]
        else
            error("Blad w koszcie")
        end
    end

    raw_Max = j["Max"]
    Max = zeros(Int, length(raw_Max), Z)
    for (idx, a) in enumerate(raw_Max)
        if isa(a, Array)
            Max[idx, :] = [Int(x) for x in a]
        else
            error("Blad w koszcie")
        end
    end

    return 1:P,1:Z,Min,Max,Dost,Pow
end

#uzyj domyslnych jesli nie podano sciezki
input = length(ARGS) >= 1 ? try
        load_from_json(ARGS[1])
    catch e
        @warn "Nie podano sciezki";
        nothing
    end : nothing

if input === nothing
    P=1:3
    Z=1:3
    MinRadiowozy=[2 4 3; 3 6 5; 5 7 6]#[p,z]
    MaxRadiowozy=[3 7 5; 5 7 10; 8 12 10]#[p,z]
    DostepneRadiowozy=[10 20 18]#[z]
    Powinno=[10 14 13]#[p] conajmniej
else
    P,Z,MinRadiowozy,MaxRadiowozy,DostepneRadiowozy,Powinno = input
end


# Gc=[
#     1 1 1;
#     1 1 1;
#     1 1 1
# ]

model=Model(GLPK.Optimizer)

@variable(model,source[p in P]>=Powinno[p])
@variable(model,x[P,Z]>=0)
@variable(model, sink>=0)
# source przydziela dzielnicy dzielnica przydziela zmianom

@constraint(model,[p in P,z in Z],MinRadiowozy[p,z]<=x[p,z]<=MaxRadiowozy[p,z])
@constraint(model, [z in Z],sum(x[:,z])>=DostepneRadiowozy[z])
@constraint(model, sink==sum(x[:,:]))
@constraint(model, sum(source[:])-sink==0)

@objective(model,Min,sum(source[:]))

optimize!(model)

println(objective_value(model))

for p in P, z in Z
    println("P: ",p," Z: ",z," ",value(x[p,z]))
end

println(MinRadiowozy[1,2])