using JuMP
using GLPK
model = Model(GLPK.Optimizer)
set_silent(model)
G = zeros(Int, 16, 16)
G[1,2]=16
G[1,3]=2
G[1,5]=2
G[1,9]=4
G[2,4]=2
G[2,6]=3
G[2,10]=5
G[3,4]=3
G[3,7]=8
G[3,11]=6
G[4,8]=4
G[4,12]=2
G[5,6]=7
G[5,7]=1
G[5,13]=6
G[6,8]=4
G[6,14]=4
G[7,8]=7
G[7,15]=8
G[8,16]=16
G[9,10]=8
G[9,11]=3
G[9,13]=8
G[10,12]=5
G[10,14]=3
G[11,12]=8
G[11,15]=2
G[12,16]=16
G[13,14]=8
G[13,15]=7
G[14,16]=3
G[15,16]=15
# Nodes: 16, Edges: 32
@variable(model, f[1:16, 1:16] >= 0)
@constraint(model, [i = 1:16, j = 1:16], f[i, j] <= G[i, j])
@constraint(model, [i = 1:16; i != 1 && i != 16], sum(f[i, :]) == sum(f[:, i]))
@objective(model, Max, sum(f[1, :]))
optimize!(model)
println(objective_value(model))
