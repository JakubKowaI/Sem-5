#Jakub Kowal
function findx()
    x=2.0
    i=1.0
    while true
        x=2.0
        while x>1.0
            temp=1.0/x
            println("Testing x: ", x, " i: ", i)
            if x*(1.0/x)!=1.0
                println("Found x: ", x)
                return 0
            end
            x=x-2.0^-i
        end
        println("Finished x: ", x)
        i+=1.0
        if i>20.0
            break
        end
    end
end

#findx()

function findxb()
    x=2.0
    i=52.0
    temp=2.0
    while i>1.0
        x=1.0
        while x<1.0+2.0^-(i-1.0)
            #println("Testing x: ", x, " i: ", i)
            if x*(1.0/x)!=1.0
                #println("Found x: ", x)
                if x<temp
                    temp=x
                end
                #return 0
            end
            x=x+2.0^-i
        end
        #println("Finished x: ", x)
        i-=1.0
        
    end
    println("Smallest x found: ", temp)
end

function findxc()
    x=1.0
    while x*(1/x)==1.0 && x<2.0
        x=nextfloat(x)
    end
    println(x)
    println(bitstring(x))
end

findxc()

# println("Nextfloat of 1.0: ", nextfloat(0.5))
# println("Bitstring of nextfloat(1.0): ", bitstring(nextfloat(0.5)))
# println("1.0 + eps", 0.5 + eps(Float64))
# println("Bitstring of 1.0 + eps: ", bitstring(0.5 + eps(Float64)))
# println(1.0+2.0^-53)
# println(0.5+2.0^-53)

