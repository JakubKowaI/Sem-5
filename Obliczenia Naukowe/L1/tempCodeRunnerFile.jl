function pc(n)
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