include("blocksys.jl")
using .blocksys
b=ones(Float64,4)
blockmatrix=BlockMatrix(4,4)
blockmatrix.As[1][1, 1]=0
blockmatrix.As[1][1, 2]=2
blockmatrix.As[1][1, 3]=-1
blockmatrix.As[1][1, 4]=-2
blockmatrix.As[1][2, 1]=2
blockmatrix.As[1][2, 2]=-2
blockmatrix.As[1][2, 3]=4
blockmatrix.As[1][2, 4]=-1
blockmatrix.As[1][3, 1]=1
blockmatrix.As[1][3, 2]=1
blockmatrix.As[1][3, 3]=1
blockmatrix.As[1][3, 4]=1
blockmatrix.As[1][4, 1]=-4
blockmatrix.As[1][4, 2]=2
blockmatrix.As[1][4, 3]=-4
blockmatrix.As[1][4, 4]=2

#m,bb=LU_pivot(blockmatrix,b)
#println(m.As)
#println(bb)

# test_solve_Gauss("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane1000000_1_1/A.txt")
# test_solve_Gauss_pivot("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane1000000_1_1/A.txt")
# test_solve_LU("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane1000000_1_1/A.txt")
# test_solve_LU_pivot("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane1000000_1_1/A.txt")

n=1000000
Apath=string("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane",n,"_1_1/A.txt")
bpath=string("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane",n,"_1_1/b.txt")

A_orig = load_matrix(Apath)
b_orig = load_vector(bpath)
A, b = Gauss(A,b)
solve_gauss(A, b)

A_orig = load_matrix(Apath)
b_orig = load_vector(bpath)
A, b = Gauss_pivot(A,b)
solve_gauss(A, b)

A_orig = load_matrix(Apath)
b_orig = load_vector(bpath)
A = blocksys.LU(A)
solve_LU(A,b)

A_orig = load_matrix(Apath)
b_orig = load_vector(bpath)
A = blocksys.LU_pivot(A)
solve_LU(A,b)