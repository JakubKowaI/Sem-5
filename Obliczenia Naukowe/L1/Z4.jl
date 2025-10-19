function findx()
    x=1.0

    while x<=2.0
        # println("sprawdzam x: ", x)
        temp=1.0/x
        if temp*x!=1.0
            println("znaleziono x", x)
            break
        end
        # x=x+1.1102230246251565e-16
        x=nextfloat(x)
    end
    
end

findx()