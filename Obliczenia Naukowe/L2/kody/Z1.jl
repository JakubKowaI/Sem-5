#Jakub Kowal
x = [2.718281828,−3.141592654,1.414213562,0.577215664,0.301029995]
y = [1486.2497,878366.9879,−22.37492,4773714.647,0.000185049]
xog = [2.718281828,−3.141592654,1.414213562,0.5772156649,0.3010299957]
yog = [1486.2497,878366.9879,−22.37492,4773714.647,0.000185049] 
n = 5

function pa(n)
    sum = 0.0
    for i in 1:n
        sum += x[i]*y[i]
    end
    # println("pa: ", sum)
    return sum
end

function pb(n)
    sum = 0.0
    for i in n:-1:1
        sum += x[i]*y[i]
    end
    # println("pb: ", sum)
    return sum
end

function pc(n)
    sum = 0.0
    minus=Vector{Float64}()
    plus=Vector{Float64}()
    for i in 1:n
        if x[i]*y[i]<0.0
            push!(minus,x[i]*y[i])
        else
            push!(plus,x[i]*y[i])
        end
    end
    sorted_plus=sort(plus,rev = true)
    sorted_minus=sort(minus)
    sum_plus=0.0
    sum_minus=0.0
    for i in sorted_plus
        sum_plus=sum_plus + i
    end
    for i in sorted_minus
        sum_minus= sum_minus + i
    end
    sum=sum_plus+sum_minus
    # println("pc: ", sum)
    return sum
end

function pd(n)
    sum = 0.0
    minus=Vector{Float64}()
    plus=Vector{Float64}()
    for i in 1:n
        if x[i]*y[i]<0.0
            push!(minus,x[i]*y[i])
        else
            push!(plus,x[i]*y[i])
        end
    end
    sorted_plus=sort(plus)
    sorted_minus=sort(minus,rev = true)
    sum_plus=0.0
    sum_minus=0.0
    for i in sorted_plus
        sum_plus=sum_plus + i
    end
    for i in sorted_minus
        sum_minus= sum_minus + i
    end
    # println("sum_plus: ", sum_plus)
    # println("sum_minus: ", sum_minus)
    sum=sum_plus+sum_minus
    # println("pd: ", sum)
    return sum
end

# pa(n)
# pb(n)
# pc(n)
# pd(n)

function pa32(n)
    sum = Float32(0.0)
    for i in 1:n
        sum += convert(Float32,x[i])*convert(Float32,y[i])
    end
    # println("pa32: ", sum)
    return sum
end

function pb32(n)
    sum = Float32(0.0)
    for i in n:-1:1
        sum += convert(Float32,x[i])*convert(Float32,y[i])
    end
    # println("pb32: ", sum)
    return sum
end

function pc32(n)
    sum = Float32(0.0)
    minus=Vector{Float32}()
    plus=Vector{Float32}()
    for i in 1:n
        if convert(Float32,x[i])*convert(Float32,y[i])<Float32(0.0)
            push!(minus,convert(Float32,x[i])*convert(Float32,y[i]))
        else
            push!(plus,convert(Float32,x[i])*convert(Float32,y[i]))
        end
    end
    sorted_plus=sort(plus,rev = true)
    sorted_minus=sort(minus)
    sum_plus=Float32(0.0)
    sum_minus=Float32(0.0)
    for i in sorted_plus
        sum_plus=sum_plus + i
    end
    for i in sorted_minus
        sum_minus= sum_minus + i
    end
    # println("sum_plus: ", sum_plus)
    # println("sum_minus: ", sum_minus)
    sum=sum_plus+sum_minus
    # println("pc32: ", sum)
    return sum
end

function pd32(n)
    sum = Float32(0.0)
    minus=Vector{Float32}()
    plus=Vector{Float32}()
    for i in 1:n
        if convert(Float32,x[i])*convert(Float32,y[i])<Float32(0.0)
            push!(minus,convert(Float32,x[i])*convert(Float32,y[i]))
        else
            push!(plus,convert(Float32,x[i])*convert(Float32,y[i]))
        end
    end
    sorted_plus=sort(plus)
    sorted_minus=sort(minus,rev = true)
    sum_plus=Float32(0.0)
    sum_minus=Float32(0.0)
    for i in sorted_plus
        sum_plus=sum_plus + i
    end
    for i in sorted_minus
        sum_minus= sum_minus + i
    end
    # println("sum_plus: ", sum_plus)
    # println("sum_minus: ", sum_minus)
    sum=sum_plus+sum_minus
    # println("pd32: ", sum)
    return sum
end

# pa32(n)
# pb32(n)
# pc32(n)
# pd32(n)

function paog(n)
    sum = 0.0
    for i in 1:n
        sum += xog[i]*yog[i]
    end
    # println("pa: ", sum)
    return sum
end

function pbog(n)
    sum = 0.0
    for i in n:-1:1
        sum += xog[i]*yog[i]
    end
    # println("pb: ", sum)
    return sum
end

function pcog(n)
    sum = 0.0
    minus=Vector{Float64}()
    plus=Vector{Float64}()
    for i in 1:n
        if xog[i]*yog[i]<0.0
            push!(minus,xog[i]*yog[i])
        else
            push!(plus,xog[i]*yog[i])
        end
    end
    sorted_plus=sort(plus,rev = true)
    sorted_minus=sort(minus)
    sum_plus=0.0
    sum_minus=0.0
    for i in sorted_plus
        sum_plus=sum_plus + i
    end
    for i in sorted_minus
        sum_minus= sum_minus + i
    end
    sum=sum_plus+sum_minus
    # println("pc: ", sum)
    return sum
end

function pdog(n)
    sum = 0.0
    minus=Vector{Float64}()
    plus=Vector{Float64}()
    for i in 1:n
        if x[i]*y[i]<0.0
            push!(minus,xog[i]*yog[i])
        else
            push!(plus,xog[i]*yog[i])
        end
    end
    sorted_plus=sort(plus)
    sorted_minus=sort(minus,rev = true)
    sum_plus=0.0
    sum_minus=0.0
    for i in sorted_plus
        sum_plus=sum_plus + i
    end
    for i in sorted_minus
        sum_minus= sum_minus + i
    end
    # println("sum_plus: ", sum_plus)
    # println("sum_minus: ", sum_minus)
    sum=sum_plus+sum_minus
    # println("pd: ", sum)
    return sum
end

# paog(n)
# pbog(n)
# pcog(n)
# pdog(n)

function pa32og(n)
    sum = Float32(0.0)
    for i in 1:n
        sum += convert(Float32,xog[i])*convert(Float32,yog[i])
    end
    # println("pa32: ", sum)
    return sum
end

function pb32og(n)
    sum = Float32(0.0)
    for i in n:-1:1
        sum += convert(Float32,xog[i])*convert(Float32,yog[i])
    end
    # println("pb32: ", sum)
    return sum
end

function pc32og(n)
    sum = Float32(0.0)
    minus=Vector{Float32}()
    plus=Vector{Float32}()
    for i in 1:n
        if convert(Float32,xog[i])*convert(Float32,yog[i])<Float32(0.0)
            push!(minus,convert(Float32,xog[i])*convert(Float32,yog[i]))
        else
            push!(plus,convert(Float32,xog[i])*convert(Float32,yog[i]))
        end
    end
    sorted_plus=sort(plus,rev = true)
    sorted_minus=sort(minus)
    sum_plus=Float32(0.0)
    sum_minus=Float32(0.0)
    for i in sorted_plus
        sum_plus=sum_plus + i
    end
    for i in sorted_minus
        sum_minus= sum_minus + i
    end
    # println("sum_plus: ", sum_plus)
    # println("sum_minus: ", sum_minus)
    sum=sum_plus+sum_minus
    # println("pc32: ", sum)
    return sum
end

function pd32og(n)
    sum = Float32(0.0)
    minus=Vector{Float32}()
    plus=Vector{Float32}()
    for i in 1:n
        if convert(Float32,x[i])*convert(Float32,y[i])<Float32(0.0)
            push!(minus,convert(Float32,xog[i])*convert(Float32,yog[i]))
        else
            push!(plus,convert(Float32,xog[i])*convert(Float32,yog[i]))
        end
    end
    sorted_plus=sort(plus)
    sorted_minus=sort(minus,rev = true)
    sum_plus=Float32(0.0)
    sum_minus=Float32(0.0)
    for i in sorted_plus
        sum_plus=sum_plus + i
    end
    for i in sorted_minus
        sum_minus= sum_minus + i
    end
    # println("sum_plus: ", sum_plus)
    # println("sum_minus: ", sum_minus)
    sum=sum_plus+sum_minus
    # println("pd32: ", sum)
    return sum
end

# pa32og(n)
# pb32og(n)
# pc32og(n)
# pd32og(n)


function Uwarunkowaniea(typ::Type)
    local wynik::typ
    if typ == Float64
        wynik=abs(pa(n)-paog(n))/abs(paog(n))
    elseif typ == Float32
        wynik=abs(pa32(n)-pa32og(n))/abs(pa32og(n))
    end
    return wynik
end

function Uwarunkowanieb(typ::Type)
    local wynik::typ
    if typ == Float64
        wynik=abs(pb(n)-pbog(n))/abs(pbog(n))
    elseif typ == Float32
        wynik=abs(pb32(n)-pb32og(n))/abs(pb32og(n))
    end
    return wynik
end

function Uwarunkowaniec(typ::Type)
    local wynik::typ
    if typ == Float64
        wynik=abs(pc(n)-pcog(n))/abs(pcog(n))
    elseif typ == Float32
        wynik=abs(pc32(n)-pc32og(n))/abs(pc32og(n))
    end
    return wynik
end

function Uwarunkowanied(typ::Type)
    local wynik::typ
    if typ == Float64
        wynik=abs(pd(n)-pdog(n))/abs(pdog(n))
    elseif typ == Float32
        wynik=abs(pd32(n)-pd32og(n))/abs(pd32og(n))
    end
    return wynik
end

println("Cond() dla a ",Uwarunkowaniea(Float64))
println("Cond() dla b ",Uwarunkowanieb(Float64))
println("Cond() dla c ",Uwarunkowaniec(Float64))
println("Cond() dla d ",Uwarunkowanied(Float64))
println("Cond() dla a ",Uwarunkowaniea(Float32))
println("Cond() dla b ",Uwarunkowanieb(Float32))
println("Cond() dla c ",Uwarunkowaniec(Float32))
println("Cond() dla d ",Uwarunkowanied(Float32))