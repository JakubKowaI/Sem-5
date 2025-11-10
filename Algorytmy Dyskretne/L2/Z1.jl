using JuMP
import GLPK

F=1:3
L=1:4
Zasoby=[275000,550000,660000]
Potrzeby=[110000,220000,330000,440000]
Koszt=[10 10 9 11; 7 11 12 13;8 14 4 9]
model=Model(GLPK.Optimizer)

@variable(model, x[F, L]>=0)
@constraint(model, [f in F],sum(x[f, :])<=Zasoby[f])
@constraint(model, [l in L],sum(x[:,l])==Potrzeby[l])
@objective(model,Min,sum(Koszt[f,l]*x[f,l] for l in L, f in F))
optimize!(model)
model
println("Status: ", termination_status(model))
println("Wartość celu: ", objective_value(model))
for f in F, l in L
    println("x[",f,",",l,"] = ", value(x[f,l]))
end