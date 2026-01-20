using JuMP
using GLPK
model = Model(GLPK.Optimizer)
set_silent(model)
G = zeros(Int, 18, 18)
G[1,2]=1
G[1,3]=1
G[1,4]=1
G[1,5]=1
G[1,6]=1
G[1,7]=1
G[1,8]=1
G[1,9]=1
G[2,12]=1
G[2,11]=1
G[3,12]=1
G[3,13]=1
G[4,13]=1
G[4,13]=1
G[5,12]=1
G[5,14]=1
G[6,10]=1
G[6,13]=1
G[7,12]=1
G[7,11]=1
G[8,11]=1
G[8,13]=1
G[9,14]=1
G[9,14]=1
G[10,18]=1
G[11,18]=1
G[12,18]=1
G[13,18]=1
G[14,18]=1
G[15,18]=1
G[16,18]=1
G[17,18]=1
# Nodes: 18, Edges: 32
@variable(model, f[1:18, 1:18] >= 0)
@constraint(model, [i = 1:18, j = 1:18], f[i, j] <= G[i, j])
@constraint(model, [i = 1:18; i != 1 && i != 18], sum(f[i, :]) == sum(f[:, i]))
@objective(model, Max, sum(f[1, :]))
optimize!(model)
println(objective_value(model))
