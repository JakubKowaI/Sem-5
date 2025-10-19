
function f(x::Float64)
    return sin(x)+cos(3.0 * x)
end

function fprimtylda(h::Float64, x::Float64)
    return (f(x+h)-f(x))/h
end

function fprim(x::Float64)
    return  cos(x) - 3.0 * sin(3.0 * x)
end

function test()
    for n in 0:54
        global h=2.0^-n
        println("h: ", h, " f'(x): ", fprimtylda(h, 1.0), " f'(x) true: ", fprim(1.0), " error: ", abs(fprimtylda(h, 1.0)-fprim(1.0)))
    end
end
test()