include("Metody.jl")

function f(x)
    return 0.0
end

function fsin(x)
    return (x/2.0)^2+sin(x)
end

function pfsin(x)
    return x/2.0+cos(x)
end

#f(x)=0
try
    println(mbisekcji(f,1.0,10.0,0.5-5.0,eps(Float64)))
    println(mstycznych(f,f,1.0,0.5-5.0,eps(Float64),30))
    println(msiecznych(f,1.0,10.0,0.5-5.0,eps(Float64),30))
catch e
    println("Wystapil blad: ",e)
end

# try
#     println(mbisekcji(fsin,1.5,2.0,0.5-5.0,eps(Float64)))
#     #println(mstycznych(fsin,pfsin,1.5,0.5-5.0,eps(Float64),30))
# catch e
#     println("Wystapil blad: ",e)
# end