
function onetwo()
    o=2.0^-52
    i =1.0

    while i<=2.0
        i = i + o
        println(bitstring(i)," : ", i)
    end
end

function halfone()
    o=2.0^-52
    i =0.5

    while i<=1.0
        i = i + o
        println(bitstring(i)," : ", i)
    end
end

function twofour()
    o=2.0^-52
    i =2.0

    while i<=4.0
        i = i + o
        println(bitstring(i)," : ", i)
    end
end

function newhalfone()
    o=2.0^-52
    i =0.5

    println("halfone test")
    while i<=1.0
        # println("i: ", i, " nextfloat(i): ", nextfloat(i), " i+o: ", i+o)
        if(i+o!=nextfloat(i))
            println("o jest niepoprawne")
            step=nextfloat(i)-i
            println("poprawny krok to: ", step)
            break
        end
        i = i + o
        
        # println(bitstring(i)," : ", i)
        # println("o jest poprawne")
    end
    
end

function newonetwo()
    o=2.0^-52
    i =1.0
    println("onetwo test")

    while i<=2.0
        if(i+o!=nextfloat(i))
            println("o jest niepoprawne")
            step=nextfloat(i)-i
            println("poprawny krok to: ", step)
            break
        end
        i = i + o
        
        # println(bitstring(i)," : ", i)
        # println("o jest poprawne")
    end
    
end

function newtwofour()
    o=2.0^-52
    i =2.0
    
    println("twofour test")
    while i<=4.0
        if(i+o!=nextfloat(i))
            println("o jest niepoprawne")
            step=nextfloat(i)-i
            println("poprawny krok to: ", step)
            break
        end
        i = i + o
        
        # println(bitstring(i)," : ", i)
        # println("o jest poprawne")
    end
    
end

# halfone()
# onetwo()
#twofour()
newhalfone()
newtwofour()
newonetwo()

function test()
    one=0b1
    println(one+1.0)
    println(bitstring(one+1.0))
    println(bitstring(2.0^(-52)))
end

# test()