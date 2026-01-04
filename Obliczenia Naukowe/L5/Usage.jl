#Jakub Kowal
include("blocksys.jl")
using .blocksys
using Plots

# A=load_matrix("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane16_1_1/A.txt")
# print(A)
# printMatrix(A)
# println("\n")
# b=load_vector("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane16_1_1/b.txt")
# print(b)
# println("\n")
# afterA,afterb=Gauss(A,b)
# afterA,afterb=Gauss_pivot(A,b)
# printMatrix(afterA)
# println("\n")
# print(afterb)
# println("\n")
# x=solve_gauss(afterA,afterb)
# print(x)
# println("\n")
#lu=LU_pivot(A)
#solved_lu=solve_LU(lu,b)
#println(solved_lu)

# print(get_b_from_ones("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane16_1_1/A.txt"))
# test_solve_Gauss("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane16_1_1/A.txt")
#test_solve_Gauss_pivot("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane1000000_1_1/A.txt")
#test_solve_LU("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane16_1_1/A.txt")
#test_solve_LU_pivot("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane16_1_1/A.txt")

Ns =[16,10000,50000,100000,500000,750000,1000000]

# Tablice na wyniki
times_gauss = Float64[]
mem_gauss = Float64[]
times_gauss_pivot = Float64[]
mem_gauss_pivot = Float64[]
times_lu = Float64[]
mem_lu = Float64[]
times_lu_pivot = Float64[]
mem_lu_pivot = Float64[]

for n in Ns
    println("Przetwarzanie n = $n ...")
    Apath=string("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane",n,"_1_1/A.txt")
    bpath=string("/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane",n,"_1_1/b.txt")
    
    A_orig = load_matrix(Apath)
    b_orig = load_vector(bpath)

    A = deepcopy(A_orig)
    b = copy(b_orig)
    stats = @timed begin
        Agauss, bgauss = Gauss(A,b)
        solve_gauss(Agauss, bgauss)
    end
    push!(times_gauss, stats.time)
    push!(mem_gauss, stats.bytes / 1024^2)

    A = deepcopy(A_orig)
    b = copy(b_orig)
    stats = @timed begin
        AgaussPivot, bgaussPivot = Gauss_pivot(A,b)
        solve_gauss(AgaussPivot, bgaussPivot)
    end
    push!(times_gauss_pivot, stats.time)
    push!(mem_gauss_pivot, stats.bytes / 1024^2)

    A = deepcopy(A_orig)
    b = copy(b_orig)
    stats = @timed begin
        LUmatrix = blocksys.LU(A)
        solve_LU(LUmatrix,b)
    end
    push!(times_lu, stats.time)
    push!(mem_lu, stats.bytes / 1024^2)

    A = deepcopy(A_orig)
    b = copy(b_orig)
    stats = @timed begin
        LUmatrixPivot, LUbPivot = LU_pivot(A,b)
        solve_LU(LUmatrixPivot,LUbPivot)
    end
    push!(times_lu_pivot, stats.time)
    push!(mem_lu_pivot, stats.bytes / 1024^2)
end

p1 = plot(Ns, [times_gauss, times_gauss_pivot, times_lu, times_lu_pivot], dpi=500,
    label=["Gauss" "Gauss Pivot" "LU" "LU Pivot"],
    title="Czas wykonania", xlabel="Rozmiar macierzy (n)", ylabel="Czas [s]",
    marker=:circle, lw=2, legend=:topleft)
savefig(p1, "time_graph.png")

p2 = plot(Ns, [mem_gauss, mem_gauss_pivot, mem_lu, mem_lu_pivot], dpi=500,
    label=["Gauss" "Gauss Pivot" "LU" "LU Pivot"],
    title="Zużycie pamięci (alokacje)", xlabel="Rozmiar macierzy (n)", ylabel="Pamięć [MB]",
    marker=:circle, lw=2, legend=:topleft)
savefig(p2, "mem_graph.png")
