#Jakub Kowal
using Test
include("Funkcje.jl")
using .Funkcje

@testset "ilorazyRoznicowe" begin
    # f(x) = x^2
    x = [0.0, 1.0, 2.0]
    f = x .^ 2
    fx = Funkcje.ilorazyRoznicowe(x, f)

    @test length(fx) == 3
    @test fx[1] ≈ 0.0       # f(x0)
    @test fx[2] ≈ 1.0       # (1 - 0) / (1 - 0)
    @test fx[3] ≈ 1.0       # drugi iloraz powinien być stały = 1 dla x^2
end


@testset "warNewton" begin
    # wielomian x^2 — wartości jak wyżej
    x = [0.0, 1.0, 2.0]
    f = x .^ 2
    fx = Funkcje.ilorazyRoznicowe(x, f)

    @test Funkcje.warNewton(x, fx, 0.0) ≈ 0.0
    @test Funkcje.warNewton(x, fx, 1.0) ≈ 1.0
    @test Funkcje.warNewton(x, fx, 2.0) ≈ 4.0
    @test Funkcje.warNewton(x, fx, 3.0) ≈ 9.0
end


@testset "naturalna" begin
    # f(x) = x^2 → a0=0, a1=0, a2=1
    x = [0.0, 1.0, 2.0]
    f = x .^ 2
    fx = Funkcje.ilorazyRoznicowe(x, f)
    a = Funkcje.naturalna(x, fx)

    @test length(a) == 3
    @test a[1] ≈ 0.0   # stała
    @test a[2] ≈ 0.0   # współczynnik przy x
    @test a[3] ≈ 1.0   # współczynnik przy x^2
end


@testset "rysujNnfx" begin
    f(x) = x^2
    a, b = 0.0, 2.0
    n = 3

    p = Funkcje.rysujNnfx(f, a, b, n)

    @test typeof(p) <: Plots.Plot
    @test length(p.series_list) == 3
end
