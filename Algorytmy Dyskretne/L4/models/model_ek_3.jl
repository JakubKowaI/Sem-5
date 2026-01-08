using JuMP
using GLPK
model = Model(GLPK.Optimizer)
set_silent(model)
G = zeros(Int, 8, 8)
G[1,2]=6
G[1,3]=3
G[1,5]=8
G[2,4]=2
G[2,6]=3
G[3,4]=2
G[3,7]=4
G[4,8]=2
G[5,6]=2
G[5,7]=4
G[6,8]=3
G[7,8]=3
# Nodes: 8, Edges: 12
@variable(model, f[1:8, 1:8] >= 0)
@constraint(model, [i = 1:8, j = 1:8], f[i, j] <= G[i, j])
@constraint(model, [i = 1:8; i != 1 && i != 8], sum(f[i, :]) == sum(f[:, i]))
@objective(model, Max, sum(f[1, :]))
optimize!(model)
println(objective_value(model))
