include("blocksysgem.jl")
using .blocksys

function run_tests()
    # Przykładowe ścieżki (należy podmienić na faktyczne pliki z danymi)
    matrix_file = "dane/Dane16_1_1/A.txt" 
    vector_file = "dane/Dane16_1_1/b.txt"
    output_gauss = "x_gauss.txt"
    output_gen_b = "x_gen_b.txt"

    println("--- Test 1: Wczytanie A i b z pliku, Gauss bez wyboru ---")
    if isfile(matrix_file) && isfile(vector_file)
        A = load_matrix(matrix_file)
        b = load_vector(vector_file)
        
        # Rozwiązanie Gaussem
        x = solve_gauss(A, b)
        write_result(output_gauss, x)
        println("Wynik zapisano do $output_gauss")
    else
        println("Brak plików wejściowych A.txt lub b.txt - pomijam test 1.")
    end

    println("\n--- Test 2: Wczytanie A, generowanie b, Gauss z błędem ---")
    if isfile(matrix_file)
        A = load_matrix(matrix_file)
        # Generujemy b dla x = [1, 1, ..., 1]
        b_gen = compute_b_from_x(A)
        
        # Rozwiązujemy układ
        x_calc = solve_gauss(A, b_gen)
        
        # Wektor dokładny (same jedynki)
        x_exact = ones(Float64, A.n)
        
        write_result_with_error(output_gen_b, x_calc, x_exact)
        println("Wynik zapisano do $output_gen_b")
    else
        println("Brak pliku A.txt - pomijam test 2.")
    end
    
    println("\n--- Test 3: Rozkład LU i rozwiązanie ---")
    if isfile(matrix_file) && isfile(vector_file)
        A = load_matrix(matrix_file)
        b = load_vector(vector_file)
        
        LU = blocksys.get_lu(A)
        x_lu = blocksys.solve_lu_matrix(LU, b)
        
        # Dla porównania wyświetlamy pierwsze 5 elementów
        println("Pierwsze 5 elementów rozwiązania LU:")
        println(x_lu[1:min(5, length(x_lu))])
    end

end

run_tests()