using JuMP
import GLPK
using JSON

function load_from_json(path::String)
    s = read(path, String)
    j = JSON.parse(s)
    
    M = Int(j["M"])
    N = Int(j["N"])
    k = Int(j["k"])

    return 1:M,1:N,k
end

#uzyj domyslnych jesli nie podano sciezki
input = length(ARGS) >= 1 ? try
        load_from_json(ARGS[1])
    catch e
        @warn "Nie podano sciezki";
        nothing
    end : nothing

if input === nothing
    M=1:20
    N=1:15
    k=4
else
    M, N, k =input
end

model = Model(GLPK.Optimizer)



@variable(model,x[M,N],Bin)

@constraint(model,[m in M,n in N],sum(x[max(1,m-k):min(m,length(M)+k),max(1,n-k):min(length(N),n+k)])>=1)

@objective(model,Min,sum(x[:,:]))

optimize!(model)

println(objective_value(model))

for n in N
    for m in M
        print(value(x[m,n])," ")
    end
    print("\n")
end