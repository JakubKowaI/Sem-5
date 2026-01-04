#Jakub Kowal
module blocksys
using LinearAlgebra

export BlockMatrix, load_matrix, load_vector, save_solution, printMatrix, Gauss, Gauss_pivot, solve_gauss, LU, LU_pivot, solve_LU, get_b_from_ones, test_solve_Gauss, test_solve_LU, test_solve_Gauss_pivot, test_solve_LU_pivot

function LU end

struct MatrixB
    first_row::Vector{Float64} # Pierwszy wiersz bloku (1, :)
    last_col::Vector{Float64}  # Ostatnia kolumna bloku (:, l)
end

struct BlockMatrix
    n::Int
    l::Int
    block_count::Int
    
    As::Vector{Matrix{Float64}} 
    Bs::Vector{Matrix{Float64}}        
    Cs::Vector{Matrix{Float64}} 
end

function BlockMatrix(n::Int, l::Int)
    v = div(n, l)
    
    As = [zeros(Float64, l, l) for _ in 1:v]
    
    Bs = [zeros(Float64, l, l) for _ in 1:v]
    
    Cs = [zeros(Float64, l, l) for _ in 1:(v-1)]
    
    return BlockMatrix(n, l, v, As, Bs, Cs)
end

function load_matrix(filename::String)::BlockMatrix
    open(filename, "r") do file
        header = readline(file)
        params = split(header)
        n = parse(Int, params[1])
        l = parse(Int, params[2])
        
        A = BlockMatrix(n, l)
        
        for line in eachline(file)
            if isempty(strip(line)) continue end
            parts = split(line)
            if length(parts) < 3 continue end
            
            i = parse(Int, parts[1])
            j = parse(Int, parts[2])
            val = parse(Float64, parts[3])
            
            blocki = Int(ceil(i/A.l))
            blockj = Int(ceil(j/A.l))
            insidei = mod(i,A.l)
            insidej = mod(j,A.l)

            if insidei == 0
                insidei = A.l
            end
            if insidej == 0
                insidej = A.l
            end

            #A block
            if blocki == blockj
                A.As[blocki][insidei, insidej]=val
            #B block
            elseif blocki == blockj + 1
                A.Bs[blocki][insidei, insidej]=val
            #C block
            elseif blocki == blockj - 1
                A.Cs[blocki][insidei, insidej]=val
            #Error
            else
                println("Something is wrong", blocki," : ",blockj)
            end
        end#for
        
        return A
    end #dofile
end #function

function printMatrix(block::BlockMatrix)
    for a in block.As
        for i in 1:block.l
            for j in 1:block.l
                print(a[i, j], "   ")
            end
            println() 
        end
        println("---")
    end

    for b in block.Bs
        for i in 1:block.l
            for j in 1:block.l
                print(b[i, j], "   ")
            end
            println() 
        end
        println("---")
    end

    for c in block.Cs
        for i in 1:block.l
            for j in 1:block.l
                print(c[i, j], "   ")
            end
            println() 
        end
        println("---")
    end
end

function load_vector(filename::String)::Vector{Float64}
    open(filename, "r") do file
        n = parse(Int, readline(file))
        b = Vector{Float64}(undef, n)
        
        idx = 1
        for line in eachline(file)
            if isempty(strip(line)) continue end
            b[idx] = parse(Float64, strip(line))
            idx += 1
        end
        return b
    end
end

function save_solution(filename::String, x::Vector{Float64}, error::Union{Float64, Nothing}=nothing)
    open(filename, "w") do file
        if error !== nothing
            println(file, error)
        end
        
        for val in x
            println(file, val)
        end
    end
end

function matrix_get(matrix::BlockMatrix,i::Int,j::Int)::Float64
    blocki = Int(ceil(i/matrix.l))
    blockj = Int(ceil(j/matrix.l))
    insidei = mod(i,matrix.l)
    insidej = mod(j,matrix.l)

    if insidei == 0
        insidei = matrix.l
    end
    if insidej == 0
        insidej = matrix.l
    end

    #A block
    if blocki == blockj
        #matrix.As[blocki][insidei, insidej]=val
        return matrix.As[blocki][insidei, insidej]
    #B block
    elseif blocki == blockj + 1
        return matrix.Bs[blocki][insidei, insidej]
    #C block
    elseif blocki == blockj - 1
        return matrix.Cs[blocki][insidei, insidej]
    #Error
    else
        #println("Something is wrong", blocki," : ",blockj)
        return 0.0
    end
end

function matrix_put(matrix::BlockMatrix,i::Int,j::Int,val::Float64)
    blocki = Int(ceil(i/matrix.l))
    blockj = Int(ceil(j/matrix.l))
    insidei = mod(i,matrix.l)
    insidej = mod(j,matrix.l)

    if insidei == 0
        insidei = matrix.l
    end
    if insidej == 0
        insidej = matrix.l
    end

    #A block
    if blocki == blockj
        matrix.As[blocki][insidei, insidej]=val
    #B block
    elseif blocki == blockj + 1
        matrix.Bs[blocki][insidei, insidej]=val
    #C block
    elseif blocki == blockj - 1
        matrix.Cs[blocki][insidei, insidej]=val
    #Error
    else
        #println("Something is wrong", blocki," : ",blockj)
    end
    return matrix
end

function matrix_swap(matrix::BlockMatrix,k::Int,index::Int)
    for j in k:min(matrix.n, k + 2*matrix.l)
        temp=matrix_get(matrix,k,j)
        matrix_put(matrix,k,j,matrix_get(matrix,index,j))
        matrix_put(matrix,index,j,temp)
    end
end

function matrix_LU_swap(matrix::BlockMatrix,k::Int,index::Int)
    for j in max(1,k-2*matrix.l):min(matrix.n, k + 2*matrix.l)
        temp=matrix_get(matrix,k,j)
        matrix_put(matrix,k,j,matrix_get(matrix,index,j))
        matrix_put(matrix,index,j,temp)
    end
end

function Gauss(matrix::BlockMatrix,b::Vector)
    for k in 1:matrix.n-1
        for i in k+1:min(matrix.n, k + 2*matrix.l)
            I=matrix_get(matrix,i,k)/matrix_get(matrix,k,k)
            matrix_put(matrix,i,k,0.0)
            for j in k+1:min(matrix.n, k + 2*matrix.l)
                val=matrix_get(matrix,i,j)- I * matrix_get(matrix,k,j)
                matrix=matrix_put(matrix,i,j,val)
            end
            b[i]=b[i]-I*b[k]
        end
    end
    return matrix, b
end

function Gauss_pivot(matrix::BlockMatrix,b::Vector)
    for k in 1:matrix.n-1
        pivot=abs(matrix_get(matrix,k,k))
        index=k
        for t in k+1:min(matrix.n, k + 2*matrix.l)
            if abs(matrix_get(matrix,t,k))>pivot
                pivot=abs(matrix_get(matrix,t,k))
                index=t
            end
        end
        if index!=k
            matrix_swap(matrix,k,index)
            temp=b[k]
            b[k]=b[index]
            b[index]=temp
        end
        for i in k+1:min(matrix.n, k + 2*matrix.l)
            I=matrix_get(matrix,i,k)/matrix_get(matrix,k,k)
            matrix_put(matrix,i,k,0.0)
            for j in k+1:min(matrix.n, k + 2*matrix.l)
                val=matrix_get(matrix,i,j)- I * matrix_get(matrix,k,j)
                matrix=matrix_put(matrix,i,j,val)
            end
            b[i]=b[i]-I*b[k]
        end
    end
    return matrix, b
end

function solve_gauss(matrix::BlockMatrix,b::Vector{Float64})
    x = zeros(Float64, matrix.n)
    for i in matrix.n:-1:1
        sum_ax = 0.0
        for j in i+1 : min(matrix.n, i + 2*matrix.l)
            sum_ax += matrix_get(matrix, i, j) * x[j]
        end
        x[i] = (b[i] - sum_ax) / matrix_get(matrix, i, i)
    end

    return x
end

function LU(matrix::BlockMatrix)
    for k in 1:matrix.n-1
        for i in k+1:min(matrix.n, k + 2*matrix.l)
            I=matrix_get(matrix,i,k)/matrix_get(matrix,k,k)
            matrix_put(matrix,i,k,I)
            for j in k+1:min(matrix.n, k + 2*matrix.l)
                val=matrix_get(matrix,i,j)- I * matrix_get(matrix,k,j)
                matrix=matrix_put(matrix,i,j,val)
            end
        end
    end
    return matrix
end

function LU_pivot(matrix::BlockMatrix,b::Vector{Float64})
    for k in 1:matrix.n-1
        pivot=abs(matrix_get(matrix,k,k))
        index=k
        for t in k+1:min(matrix.n, k + 2*matrix.l)
            if abs(matrix_get(matrix,t,k))>pivot
                pivot=abs(matrix_get(matrix,t,k))
                index=t
            end
        end
        if index!=k
            matrix_LU_swap(matrix,k,index)
            temp=b[k]
            b[k]=b[index]
            b[index]=temp
        end
        for i in k+1:min(matrix.n, k + 2*matrix.l)
            I=matrix_get(matrix,i,k)/matrix_get(matrix,k,k)
            matrix_put(matrix,i,k,I)
            for j in k+1:min(matrix.n, k + 2*matrix.l)
                val=matrix_get(matrix,i,j)- I * matrix_get(matrix,k,j)
                matrix=matrix_put(matrix,i,j,val)
            end
        end
    end
    return matrix,b
end

function solve_LU(matrix::BlockMatrix, b::Vector{Float64})
    #y = copy(b)
    for k in 1:matrix.n-1
        for i in k+1:min(matrix.n, k + 2*matrix.l)
            b[i] = b[i] - matrix_get(matrix, i, k) * b[k]
        end
    end
    return solve_gauss(matrix, b)
end

function get_b_from_ones(filename::String)
    matrix=load_matrix(filename)
    b=zeros(Float64,matrix.n)

    for i in 1:matrix.n
        k = Int(ceil(i / matrix.l))
        start_col = max(1, (k-2)*matrix.l + 1)
        end_col = min(matrix.n, (k+1)*matrix.l)
        
        for j in start_col:end_col
            b[i] += matrix_get(matrix, i, j)
        end
    end

    return b
end

function test_solve_Gauss(filename::String)
    matrix=load_matrix(filename)
    b=get_b_from_ones(filename)
    matrix_after,b_after = Gauss(matrix,b)
    x=solve_gauss(matrix_after,b_after)

    err = norm(ones(Float64,matrix.n) - x) / norm(ones(Float64,matrix.n))
    save_solution("wynikGauss.txt", x, err)
end

function test_solve_Gauss_pivot(filename::String)
    matrix=load_matrix(filename)
    b=get_b_from_ones(filename)
    matrix_after,b_after = Gauss_pivot(matrix,b)
    x=solve_gauss(matrix_after,b_after)

    err = norm(ones(Float64,matrix.n) - x) / norm(ones(Float64,matrix.n))
    save_solution("wynikGaussPivot.txt", x, err)
end

function test_solve_LU(filename::String)
    matrix=load_matrix(filename)
    b=get_b_from_ones(filename)
    matrix_after = LU(matrix)
    x=solve_LU(matrix_after,b)

    err = norm(ones(Float64,matrix.n) - x) / norm(ones(Float64,matrix.n))
    save_solution("wynikLU.txt", x, err)
end

function test_solve_LU_pivot(filename::String)
    matrix=load_matrix(filename)
    b=get_b_from_ones(filename)
    matrix_after,b_after = LU_pivot(matrix,b)
    x=solve_LU(matrix_after,b_after)

    err = norm(ones(Float64,matrix.n) - x) / norm(ones(Float64,matrix.n))
    save_solution("wynikLUPivot.txt", x, err)
end


end # module