using JuMP
using GLPK
model = Model(GLPK.Optimizer)
set_silent(model)
G = zeros(Int, 10, 10)
G[1,2]=1
G[1,3]=1
G[1,4]=1
G[1,5]=1
G[2,9]=1
G[2,8]=1
G[3,8]=1
G[3,9]=1
G[4,8]=1
G[4,6]=1
G[5,9]=1
G[5,6]=1
G[6,10]=1
G[7,10]=1
G[8,10]=1
G[9,10]=1
# Nodes: 10, Edges: 16
@variable(model, f[1:10, 1:10] >= 0)
@constraint(model, [i = 1:10, j = 1:10], f[i, j] <= G[i, j])
@constraint(model, [i = 1:10; i != 1 && i != 10], sum(f[i, :]) == sum(f[:, i]))
@objective(model, Max, sum(f[1, :]))
optimize!(model)
println(objective_value(model))
