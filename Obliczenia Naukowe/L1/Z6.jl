#Jakub Kowal
function f(x)
    println(sqrt(x^2+1.0)-1.0)
end

function g(x)
    println(x^2/(sqrt(x^2+1.0)+1.0))
end

for x in -1:-1:-10
    f(8.0^x)
    g(8.0^x)
end

println(convert(Float32,Float32(2.0)^-52))
println(eps(Float32))