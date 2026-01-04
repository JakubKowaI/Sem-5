
import sys

def parse_matrix(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    header = lines[0].split()
    n = int(header[0])
    l = int(header[1])
    
    non_zeros = set()
    for line in lines[1:]:
        parts = line.split()
        if len(parts) < 3: continue
        i, j = int(parts[0]), int(parts[1])
        non_zeros.add((i, j))
        
    return n, l, non_zeros

def is_in_blocks(i, j, n, l):
    # 1-based indices
    block_i = (i - 1) // l + 1
    block_j = (j - 1) // l + 1
    
    # A block: block_i == block_j
    # B block: block_i == block_j + 1 (Wait, B is super diagonal? Let's check blocksys.jl)
    # blocksys.jl: 
    # A block: blocki == blockj
    # B block: blocki == blockj + 1  -> This looks like sub-diagonal in terms of block indices if row is i?
    # Let's check load_matrix in blocksys.jl
    # blocki = ceil(i/l), blockj = ceil(j/l)
    # if blocki == blockj: A
    # elseif blocki == blockj + 1: B (This means block row > block col, so sub-diagonal blocks?)
    # elseif blocki == blockj - 1: C (This means block row < block col, so super-diagonal blocks?)
    
    # Usually B is super, C is sub, or vice versa.
    # In blocksys.jl:
    # B is stored in Bs vector.
    # C is stored in Cs vector.
    
    block_i = (i - 1) // l + 1
    block_j = (j - 1) // l + 1
    
    if block_i == block_j: return True
    if block_i == block_j + 1: return True
    if block_i == block_j - 1: return True
    return False

def generate_latex(n, l, initial_non_zeros):
    frames = []
    
    current_non_zeros = set(initial_non_zeros)
    
    # We will simulate steps.
    # Step k: 1 to n-1
    
    # Helper to draw matrix
    def draw_frame(k, active_pivot=None, eliminating=None):
        # k is the step number (0 for initial)
        s = "\\begin{tikzpicture}[scale=0.4, yscale=-1]\n"
        s += f"\\draw[step=1cm,gray!20,very thin] (0,0) grid ({n},{n});\n"
        
        # Draw blocks outline
        # v = n // l
        # for b in range(v):
        #     # A block (b, b)
        #     # Top-left: (b*l, b*l)
        #     s += f"\\draw[blue!30, thick] ({b*l},{b*l}) rectangle ({(b+1)*l},{(b+1)*l});\n"
        #     # C block (b, b+1) -> super
        #     if b < v-1:
        #         s += f"\\draw[green!30, thick] ({(b+1)*l},{b*l}) rectangle ({(b+2)*l},{(b+1)*l});\n"
        #     # B block (b+1, b) -> sub
        #     if b < v-1:
        #         s += f"\\draw[red!30, thick] ({b*l},{(b+1)*l}) rectangle ({(b+1)*l},{(b+2)*l});\n"

        # Draw non-zeros
        for (r, c) in current_non_zeros:
            color = "black"
            if active_pivot and (r, c) == active_pivot:
                color = "red"
            elif eliminating and (r, c) in eliminating:
                color = "orange"
            
            s += f"\\fill[{color}] ({c-1},{r-1}) rectangle ({c},{r});\n"
            
        s += "\\end{tikzpicture}"
        return s

    # Frame 0: Initial
    frames.append(draw_frame(0))
    
    for k in range(1, n):
        # Identify pivot
        pivot = (k, k)
        
        # Identify elements to eliminate in this step
        # i from k+1 to min(n, k + 2*l)
        limit = min(n, k + 2*l)
        to_eliminate = []
        for i in range(k + 1, limit + 1):
            if (i, k) in current_non_zeros:
                to_eliminate.append((i, k))
        
        # Frame for start of step k
        frames.append(draw_frame(k, active_pivot=pivot, eliminating=set(to_eliminate)))
        
        # Perform elimination (update non-zeros)
        # Remove eliminated
        for (r, c) in to_eliminate:
            current_non_zeros.remove((r, c))
            
        # Fill-in
        # For i in rows being processed
        for i in range(k + 1, limit + 1):
            # For j in cols being processed
            for j in range(k + 1, limit + 1):
                # val = matrix[i,j] - factor * matrix[k,j]
                # If matrix[k,j] is non-zero, we might have fill-in at [i,j]
                if (k, j) in current_non_zeros:
                    # Check if (i,j) is within valid blocks
                    if is_in_blocks(i, j, n, l):
                        current_non_zeros.add((i, j))
                        
    # Final frame
    frames.append(draw_frame(n))
    
    return frames

def main():
    filename = "/home/kuba/Projects/Sem-5/Obliczenia Naukowe/L5/dane/Dane16_1_1/A.txt"
    n, l, non_zeros = parse_matrix(filename)
    frames = generate_latex(n, l, non_zeros)
    
    print("\\begin{animateinline}[controls,autoplay,loop, poster=first]{2}")
    print(frames[0])
    for f in frames[1:]:
        print("\\newframe")
        print(f)
    print("\\end{animateinline}")

if __name__ == "__main__":
    main()
