using JuMP
using GLPK
model = Model(GLPK.Optimizer)
set_silent(model)
G = zeros(Int, 16, 16)
G[1,2]=10
G[1,3]=12
G[1,5]=13
G[1,9]=15
G[2,4]=7
G[2,6]=8
G[2,10]=1
G[3,4]=7
G[3,7]=3
G[3,11]=7
G[4,8]=6
G[4,12]=1
G[5,6]=5
G[5,7]=2
G[5,13]=3
G[6,8]=8
G[6,14]=6
G[7,8]=1
G[7,15]=8
G[8,16]=11
G[9,10]=7
G[9,11]=4
G[9,13]=1
G[10,12]=5
G[10,14]=3
G[11,12]=4
G[11,15]=6
G[12,16]=3
G[13,14]=6
G[13,15]=5
G[14,16]=14
G[15,16]=3
# Nodes: 16, Edges: 32
@variable(model, f[1:16, 1:16] >= 0)
@constraint(model, [i = 1:16, j = 1:16], f[i, j] <= G[i, j])
@constraint(model, [i = 1:16; i != 1 && i != 16], sum(f[i, :]) == sum(f[:, i]))
@objective(model, Max, sum(f[1, :]))
optimize!(model)
println(objective_value(model))
