using JuMP
import GLPK

Okresy=1:4
MaxTowar=100
KosztProdukcji=[6000 4000 8000 9000]
ProdukcjaPonadwymiarowa=[60 65 70 60]
KosztProdukcjiPonadwymiarowej=[8000 6000 10000 11000]
Popyt=[130 80 125 195]
Magazyn=70
KosztMagazynowania=1500
TowarPoczatkowy=15

model = Model(GLPK.Optimizer)

@variable(model,MaxTowar>=x[Okresy]>=0)
@variable(model,o[Okresy]>=0)
@variable(model,Magazyn>=m[Okresy]>=0)

@constraint(model, [j in Okresy],o[j]<=ProdukcjaPonadwymiarowa[j])

for j in Okresy
    if j == 1
        @constraint(model, TowarPoczatkowy + x[1] + o[1] - Popyt[1] == m[1])
    else
        @constraint(model, m[j-1] + x[j] + o[j] - Popyt[j] == m[j])
    end
end

@objective(model, Min, sum(KosztProdukcji[j]*x[j] + KosztProdukcjiPonadwymiarowej[j]*o[j] + KosztMagazynowania*m[j] for j in Okresy))

optimize!(model)
objective_value(model)