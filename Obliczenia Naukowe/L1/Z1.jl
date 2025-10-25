#Jakub Kowal
x = 1.0
y = convert(Float32, x)
z = convert(Float16, x)
# println(typeof(x))
# println(typeof(y))
# println(typeof(z))
# println(x)
# println(y)
# println(z)
# println(eps(x))
# println(eps(y))
# println(eps(z))

function info(x)
    print("EPS for ")
    print(typeof(x))
    print(" = ")
    println(eps(x))
end

info(x)
info(y)
info(z)



function F64()
    temp=1.0
    print("F64:")
    println(typeof(temp))
    while true
        if Float64(1.0) + temp/Float64(2.0)== Float64(1.0)
            break
        end
        temp = temp / Float64(2.0)
    end
    print("Macheps64: ")
    println(temp)
    println(Float64(1.0) + temp)
end

function F32()
    temp=Float32(1.0)
    print("F32:")
    println(typeof(temp))
    while true
        if Float32(1.0) + temp/Float32(2.0)== Float32(1.0)
            break
        end
        temp = temp / Float32(2.0)
    end
    print("Macheps32: ")
    println(temp)
    println(Float32(1.0) + temp)
end

function F16()
    temp=Float16(1.0)
    print("F16:")
    println(typeof(temp))
    while true
        if Float16(1.0) + temp/Float16(2.0)== Float16(1.0)
            break
        end
        temp = temp / Float16(2.0)
    end
    print("Macheps16: ")
    println(temp)
    println(Float16(1.0) + temp)
end
F64()
F32()
F16()

# FLT_MIN         = 1.175494e-38
# FLT_MAX         = 3.402823e+38
# FLT_EPSILON     = 1.192093e-07
# headery z float.h

function E64()
    temp=convert(Float64,1.0)
    print("E64:")
    println(typeof(temp))
    while temp/Float64(2) > Float64(0)
        temp = temp / Float64(2)
        # println(i)
    end
    # println(temp)
    print("eta: ")
    println(temp)
end

function E32()
    temp=convert(Float32,1.0)
    print("E32:")
    println(typeof(temp))
    while temp/Float32(2) > Float32(0)
        temp = temp / Float32(2)
        # println(i)
    end
    # println(temp)
    print("eta: ")
    println(temp)
end

function E16()
    temp=convert(Float16,1.0)
    print("E16:")
    println(typeof(temp))
    while temp/Float16(2) > Float16(0)
        temp = temp / Float16(2)
        # println(i)
    end
    # println(temp)
    print("eta: ")
    println(temp)
end

println("NextFloat64: ", nextfloat(Float64(0.0)))
println("NextFloat32: ", nextfloat(Float32(0.0)))
println("NextFloat16: ", nextfloat(Float16(0.0)))
E64()
E32()
E16()

function M64()
    temp=convert(Float64,1.0)
    print("Max 64:")
    println(typeof(temp))
    while !isinf(temp*Float64(2))
        temp = temp * Float64(2)
        # println(i)
    end
    temp= temp*(Float64(2.0)-(eps(Float64)))
    print("Max: ")
    println(temp)
end

function M32()
    temp=convert(Float32,1.0)
    print("Max 32:")
    println(typeof(temp))
    while !isinf(temp*Float32(2))
        temp = temp * Float32(2)
        # println(i)
    end
    # println(temp)
    temp= temp*(Float32(2.0)-(eps(Float32)))
    print("Max: ")
    println(temp)
end

function M16()
    temp=convert(Float16,1.0)
    print("Max 16:")
    println(typeof(temp))
    while !isinf(temp*Float16(2))
        temp = temp * Float16(2)
        # println(i)
    end
    # println(temp)
    temp= temp*(Float16(2.0)-(eps(Float16)))
    print("Max: ")
    println(temp)
end

println("MaxFloat64: ", floatmax(Float64))
println("MaxFloat32: ", floatmax(Float32))
println("MaxFloat16: ", floatmax(Float16))
M64()
M32()
M16()

println("MinFloat64: ", floatmin(Float64))
println("MinFloat32: ", floatmin(Float32))
println("MINnor Float64: ", Float64(2.0)^(-1022))
println("MINnor Float64: ", Float32(2.0)^(-126))