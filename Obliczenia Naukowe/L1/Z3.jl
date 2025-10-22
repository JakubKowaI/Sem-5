
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
        i = i + o
        println(bitstring(i)," : ", i)
    
end

function newhalfone()
    o=2.0^-52
    i =0.5

    println("halfone test")
    
        # println("i: ", i, " nextfloat(i): ", nextfloat(i), " i+o: ", i+o)
        if(i+o!=nextfloat(i))
            println("o jest niepoprawne")
            step=nextfloat(i)-i
            println("poprawny krok to: ", step)

        end
        if(i+o==nextfloat(i))
            println("o jest poprawne")
            
        end
        println("i: ",bitstring(i), " : ", i)
        println("o: ",bitstring(o), " : ", o)
        println("i+o: ",bitstring(i+o), " : ", i+o)
        println("step: ",bitstring(nextfloat(i)), " : ", nextfloat(i))
    
        # println(bitstring(i)," : ", i)
        # println("o jest poprawne")
    
    
end

function newonetwo()
    o=2.0^-52
    i =1.0
    println("onetwo test")

    
        if(i+o!=nextfloat(i))
            println("o jest niepoprawne")
            step=nextfloat(i)-i
            println("poprawny krok to: ", step)
       
        end
        if(i+o==nextfloat(i))
            println("o jest poprawne")
        end
        
        println("i: ",bitstring(i), " : ", i)
        println("o: ",bitstring(o), " : ", o)
        println("i+o: ",bitstring(i+o), " : ", i+o)
        println("step: ",bitstring(nextfloat(i)), " : ", nextfloat(i))

        # println(bitstring(i)," : ", i)
        # println("o jest poprawne")
    
    
end

function newtwofour()
    o=2.0^-52
    i =2.0
    
    println("twofour test")
    
        if(i+o!=nextfloat(i))
            println("o jest niepoprawne")
            step=nextfloat(i)-i
            println("poprawny krok to: ", step)
        end
        if(i+o==nextfloat(i))
            println("o jest poprawne")
        end
        println("i: ",bitstring(i), " : ", i)
        println("o: ",bitstring(o), " : ", o)
        println("i+o: ",bitstring(i+o), " : ", i+o)
        println("step: ",bitstring(nextfloat(i)), " : ", nextfloat(i))
        
        # println(bitstring(i)," : ", i)
        # println("o jest poprawne")
    
    
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