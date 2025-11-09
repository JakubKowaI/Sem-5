#Jakub Kowal
include("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/listy/L2/hilb.jl")
include("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/listy/L2/matcond.jl")

Ns=[5,10,20]
Cs=[1,10,10e3,10e7,10e12,10e16]

function gauss(A, n)
    local x=ones(Float64,n)
    b=A*x
    #println("Rzad macierzy: ",rank(A)," N: ", n)
    return A\b
end

function inversion(A, n)
    local x=ones(Float64,n)
    b=A*x
    return Base.inv(A)*b
end

println("Hilb")
for n::Int in 1:20
    local x =ones(Float64,n)
    println("N: ",n)
    println("Gauss: ",norm(x-gauss(hilb(n),n))/norm(x))
    println("Inv: ",norm(x-inversion(hilb(n),n))/norm(x))
end

println("Matcond")
for n in Ns
    local x = ones(Float64,n)
    println("N: ",n)
    for c in Cs
        println("C: ",c)
        println("Gauss: ",norm(x-gauss(matcond(n,c),n))/norm(x))
        println("Inv: ",norm(x-inversion(matcond(n,c),n))/norm(x))
    end
end

println("Cond A hilb: ",cond(hilb(6)))
println("Cond A matcond: ",LinearAlgebra.cond(matcond(6,10e7)))