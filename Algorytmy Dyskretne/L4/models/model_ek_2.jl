using JuMP
using GLPK
model = Model(GLPK.Optimizer)
set_silent(model)
G = zeros(Int, 4, 4)
G[1,2]=1
G[1,3]=3
G[2,4]=3
G[3,4]=4
# Nodes: 4, Edges: 4
@variable(model, f[1:4, 1:4] >= 0)
@constraint(model, [i = 1:4, j = 1:4], f[i, j] <= G[i, j])
@constraint(model, [i = 1:4; i != 1 && i != 4], sum(f[i, :]) == sum(f[:, i]))
@objective(model, Max, sum(f[1, :]))
optimize!(model)
println(objective_value(model))
