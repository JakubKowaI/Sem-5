using JuMP
import GLPK

P=1:4
M=1:3
Czasy=[5 3 4 4; 10 6 5 2; 6 4 3 1]#Czasy produkcji p dla m - [m,p]
Czas=60*60#Czas dostępny w tygodniu w minutach
Ceny=[9 7 6 5]#ceny dla p
KosztCzas=[2/60 2/60 3/60]#koszt dla m
KosztMat=[4 1 1 1]#koszt dla p
MaxPopyt=[400 100 150 500]#popyt dla p
Zysk=zeros(Float64,3,4)
model=Model(GLPK.Optimizer)

for m in M
    for p in P
        Zysk[m,p]=Ceny[p]-KosztMat[p]-KosztCzas[m]*Czasy[m,p]
    end
end

@variable(model,x[M,P]>=0)
@constraint(model,[p in P],sum(x[:,p])<=MaxPopyt[p])
@constraint(model, [m in M],sum(x[m,p]*Czasy[m,p] for p in P)<=Czas)
# @objective(model, Max, sum(x[m,p]*Zysk[m,p] for m in M,p in P))
@objective(model, Max, sum(Ceny[p]*sum(x[m,p] for m in M) for p in P)-sum(x[m,p]*KosztCzas[m]*Czasy[m,p] for m in M,p in P)-sum(x[m,p]*KosztMat[p] for m in M,p in P))
optimize!(model)
model
println(objective_value(model))
for m in M
    czas=sum(value(x[m,p])*Czasy[m,p] for p in P)
    println("Czas dla ",m," : ",czas)
end
for m in M, p in P
    println("x[",m,",",p,"] = ", value(x[m,p]))
end