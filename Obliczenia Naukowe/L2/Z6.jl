#Jakub Kowal
using Plots

function rek_iter(n::Int, x0::Float64, c::Float64)
    res = zeros(Float64, n)
    res[1] = x0
    for i in 2:n
        res[i] = res[i-1]^2 + c
    end
    return res
end

n = 40
x0 = 1.0
c_values = [-2.0, -1.0, -0.25, 0.25, 0.75]

plt = plot(title="Iteracje",
           xlabel="n",
           ylabel="xn",
           legend=:outerright)


plot!(plt,1:n,rek_iter(40,1.0,-2.0),label="x0=1.0 r=-2.0")
plot!(plt,1:n,rek_iter(40,2.0,-2.0),label="x0=2.0 r=-2.0")
plot!(plt,1:n,rek_iter(40,1.99999999999999,-2.0),label="x0=1.99999999999999 r=-2.0")
plot!(plt,1:n,rek_iter(40,1.0,-1.0),label="x0=1.0 r=-1.0")
plot!(plt,1:n,rek_iter(40,-1.0,-1.0),label="x0=-1.0 r=-1.0")
plot!(plt,1:n,rek_iter(40,0.75,-1.0),label="x0=0.75 r=-1.0")
plot!(plt,1:n,rek_iter(40,0.25,-1.0),label="x0=0.25 r=-1.0")

display(plt)


# println("1. ",rek(40,1.0,-2))
# println("2. ",rek(40,2.0,-2))
# println("3. ",rek(40,1.99999999999999,-2))
# println("4. ",rek(40,1.0,-1))
# println("5. ",rek(40,-1.0,-1))
# println("6. ",rek(40,0.75,-1))
# println("7. ",rek(40,0.25,-1))