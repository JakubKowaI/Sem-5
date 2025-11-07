function Pn(n::Int,r)
    if n==0
        return convert(typeof(r),0.01)
    end
    temp=Pn(n-1,r)
    return temp+r*temp*(1-temp)
end

function PnWithStop(n::Int,r,tick::Int)
    if n==0
        return convert(typeof(r),0.01)
    end
    if tick==0
        floor(r,digits=3)
        println("R: ",r)
    end
    temp=PnWithStop(n-1,r,tick-1)
    return temp+r*temp*(1-temp)
end

println(Pn(40,Float32(3)))
# println(PnWithStop(40,Float64(3),10))
println(trunc(Pn(40,Float64(3)),digits=3,base=10))