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
    i =1.0 
    temp=1.0
    print("F64:")
    println(typeof(i))
    while true
        i = 1.0+ temp
        if i== 1.0
            break
        end
        temp = temp / 2.0
        # println(i)
    end
    println(i)
    print("eps: ")
    println(temp)
end

function F32()
    i =convert(Float32,1.0)
    temp=convert(Float32,1.0)
    print("F32:")
    println(typeof(i))
    while true
        i = Float32(1)+ temp
        if i== Float32(1)
            break
        end
        temp = temp / Float32(2)
        # println(i)
    end
    # println(typeof(i))
    println(i)
    # println(typeof(temp))
    print("eps: ")
    println(temp)
end

function F16()
    i =convert(Float16,1.0)
    temp=convert(Float16,1.0)
    print("F16:")
    println(typeof(i))
    while true
        i = Float16(1)+ temp
        if i== Float16(1)
            break
        end
        temp = temp / Float16(2)
        # println(i)
    end
    println(i)
    print("eps: ")
    println(temp)
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
    # println(temp)
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
    print("Max: ")
    println(temp)
end

println("MaxFloat64: ", floatmax(Float64))
println("MaxFloat32: ", floatmax(Float32))
println("MaxFloat16: ", floatmax(Float16))
M64()
M32()
M16()