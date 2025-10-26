#Jakub Kowal
x = [2.718281828,−3.141592654,1.414213562,0.5772156649,0.3010299957]
y = [1486.2497,878366.9879,−22.37492,4773714.647,0.000185049]
n = 5

function pa(n)
    sum = 0.0
    for i in 1:n
        sum += x[i]*y[i]
    end
    println("pa: ", sum)
end

function pb(n)
    sum = 0.0
    for i in n:-1:1
        sum += x[i]*y[i]
    end
    println("pb: ", sum)
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
    println("pc: ", sum)
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
    println("pd: ", sum)
end

# function pc(n)
#     minus=Vector{Float64}()
#     plus=Vector{Float64}()
#     for i in x
#         if i<0.0
#             push!(minus,i)
#         else
#             push!(plus,i)
#         end
#     end
#     for i in y
#         if i<0.0
#             push!(minus,i)
#         else
#             push!(plus,i)
#         end
#     end
#     sorted_plus=sort(plus,rev = true)
#     sorted_minus=sort(minus)
#     sum_plus=0.0
#     sum_minus=0.0
#     for i in sorted_plus
#         sum_plus+=i
#     end
#     for i in sorted_minus
#         sum_minus+=i
#     end
#     sum=sum_plus+sum_minus
#     println("pc: ", sum)
# end

# function pd(n)
#     minus=Vector{Float64}()
#     plus=Vector{Float64}()
#     for i in x
#         if i<0.0
#             push!(minus,i)
#         else
#             push!(plus,i)
#         end
#     end
#     for i in y
#         if i<0.0
#             push!(minus,i)
#         else
#             push!(plus,i)
#         end
#     end
#     sorted_plus=sort(plus)
#     sorted_minus=sort(minus,rev = true)
#     sum_plus=0.0
#     sum_minus=0.0
#     for i in sorted_plus
#         sum_plus+=i
#     end
#     for i in sorted_minus
#         sum_minus+=i
#     end
#     sum=sum_plus+sum_minus
#     println("pd: ", sum)
# end

pa(n)
pb(n)
pc(n)
pd(n)

function pa32(n)
    sum = Float32(0.0)
    for i in 1:n
        sum += convert(Float32,x[i])*convert(Float32,y[i])
    end
    println("pa32: ", sum)
end

function pb32(n)
    sum = Float32(0.0)
    for i in n:-1:1
        sum += convert(Float32,x[i])*convert(Float32,y[i])
    end
    println("pb32: ", sum)
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
    println("pc32: ", sum)
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
    println("pd32: ", sum)
end

pa32(n)
pb32(n)
pc32(n)
pd32(n)