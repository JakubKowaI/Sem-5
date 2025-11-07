function rek(n::Int,x0::Float64,c::Int)
    if n==0
        return x0
    end
    println(rek(n-1,x0,c))
    return rek(n-1,x0,c)^2+c
end

println("1. ",rek(40,1.0,-2))
println("2. ",rek(40,2.0,-2))
println("3. ",rek(40,1.99999999999999,-2))
println("4. ",rek(40,1.0,-1))
println("5. ",rek(40,-1.0,-1))
println("6. ",rek(40,0.75,-1))
println("7. ",rek(40,0.25,-1))