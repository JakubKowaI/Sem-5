using JuMP
import GLPK
using JSON

function BudujGrafC(A,N)
    Gc=zeros(Int,length(N),length(N))
    for a in eachrow(A)
        Gc[a[1],a[2]]=a[3]
    end
    return Gc
end

function BudujGrafT(A,N)
    Gt=zeros(Int,length(N),length(N))
    for a in eachrow(A)
        Gt[a[1],a[2]]=a[4]
    end
    return Gt
end

model = Model(GLPK.Optimizer)

## {
##   "nodes": 10,
##   "start": 1,
##   "end": 10,
##   "T": 15,
##   "arcs": [ [1,2,3,4],  ... ]
## }

function load_from_json(path::String)
    s = read(path, String)
    j = JSON.parse(s)

    raw_arcs = j["arcs"]
    arcs = Vector{NTuple{4,Int}}()
    for a in raw_arcs
        if isa(a, Array)
            vals = (Int(a[1]), Int(a[2]), Int(a[3]), Int(a[4]))
        else
            error("Blad w krawedziach")
        end
        push!(arcs, vals)
    end

    nnodes = haskey(j, "nodes") ? Int(j["nodes"]) : maximum([max(a[1], a[2]) for a in arcs])
    start = haskey(j, "start") ? Int(j["start"]) : 1
    stop = haskey(j, "end") ? Int(j["end"]) : nnodes
    T = haskey(j, "T") ? Int(j["T"]) : typemax(Int)

    A = zeros(Int, length(arcs), 4)
    for (idx, a) in enumerate(arcs)
        A[idx, :] = collect(a)
    end

    return nnodes, start, stop, T, A
end

#uzyj domyslnych jesli nie podano sciezki
input = length(ARGS) >= 1 ? try
        load_from_json(ARGS[1])
    catch e
        @warn "Nie podano sciezki";
        nothing
    end : nothing

if input === nothing
    N = 1:10
    N_Start = 1
    N_End = 10
    T = 15
    A = [1 2 3 4; 1 3 4 9; 1 4 7 10; 1 5 8 12; 2 3 2 3; 3 4 4 6; 3 5 2 2;3 10 6 11;4 5 1 1;4 7 3 5;5 6 5 6; 5 7 3 3;5 10 5 8; 6 1 5 8; 6 7 2 2;6 10 7 11;7 3 4 6;7 8 3 5;7 9 1 1;8 9 1 2;9 10 2 2]
else
    nnodes, ns, ne, T, A = input
    N = 1:nnodes
    N_Start = ns
    N_End = ne
end

B = zeros(Int, length(N))
B[N_Start] = 1
B[N_End] = -1
Gc = BudujGrafC(A, N)
Gt = BudujGrafT(A, N)

@variable(model, x[N,N],Bin)
@constraint(model, [i in N, j in N;Gc[i,j]==0],x[i,j]==0)
@constraint(model, [i in N],sum(x[i,:])-sum(x[:,i])==B[i])
@constraint(model, sum(Gt .* x)<=T)
@objective(model, Min, sum(Gc .* x))

optimize!(model)
println("Wynik: ",objective_value(model))
# print("    ")
# for t in N
#     print(t,"   ")
# end
# print("\n")
# for i in N
#     print(i," - ")
#     for j in N
#         print(value(x[i,j])," ")
#     end
#     print("\n")
# end
for i in N
    for j in N
        if value(x[i,j])!=0
            println(i,"->",j," c: ",Gc[i,j]," t: ",Gt[i,j])
        end
    end
end