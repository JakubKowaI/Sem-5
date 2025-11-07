#Jakub Kowal
using Plots

function Pn(p0,r,obciecie::Bool)
    x=zeros(typeof(p0),40)
    x[1]=p0
    for n in 2:40
        if n==12
            x[11]=floor(x[11],digits=3)
        end
        x[n]=x[n-1]+r*x[n-1]*(1-x[n-1])
    end
    return x
end

plt = plot(title="Wykresy Funkcji Pn",
           xlabel="n",
           ylabel="Wynik",
           legend=:outerright)

y1=Pn(Float64(0.01),3,false)
y2=Pn(Float32(0.01),3,false)
y3=Pn(Float32(0.01),3,true)

plot!(plt, 1:40, y1, label="Float64", color=:blue, linestyle=:solid, linewidth=6)
plot!(plt, 1:40, y2, label="Float32", color=:red, linestyle=:dash, linewidth=4)
plot!(plt, 1:40, y3, label="Float32 + Obcięcie", color=:green, linestyle=:dot, linewidth=2)

display(plt)