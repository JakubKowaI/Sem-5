#Jakub Kowal
#3(4/3−1)−1

function R64()
    three= Float64(3)
    four= Float64(4)
    one= Float64(1)

    result= three*(four/three - one) - one
    println(result)
end

function R32()
    three= Float32(3)
    four= Float32(4)
    one= Float32(1)

    result= three*(four/three - one) - one
    println(result)
end

function R16()
    three= Float16(3)
    four= Float16(4)
    one= Float16(1)

    result= three*(four/three - one) - one
    println(result)
end

println("eps64: ", eps(Float64))
R64()
println("eps32: ", eps(Float32))
R32()
println("eps16: ", eps(Float16))
R16()