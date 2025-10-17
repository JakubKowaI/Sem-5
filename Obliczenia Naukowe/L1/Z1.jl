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
