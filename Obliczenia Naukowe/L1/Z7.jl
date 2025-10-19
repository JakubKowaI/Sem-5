
function f(x::Float64)
    sin(x)+cos(3.0 *x)
end

function fprim()
    return (f(x+h)-f(x))/h
end