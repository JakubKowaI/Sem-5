using Base.Threads
using Dates
using Printf
using Random

# Check if Plots is installed, otherwise warn
try
    using Plots
    ENV["GKSwstype"] = "100" # For headless environments
catch e
    println("Plots.jl not found. Please install it to see charts.")
end

# Paths to binaries
const BIN_DIR = joinpath(@__DIR__, "..")
const MAXFLOW_BIN = joinpath(BIN_DIR, "maxflow")
const MATCHING_BIN = joinpath(BIN_DIR, "matching")

# Data structures for results
struct FlowResult
    size::Int
    algorithm::String
    flow::Int64
    time::Float64
    augmenting_paths::Int64
end

struct MatchingResult
    size::Int
    degree::Int
    match_size::Int64
    time::Float64
end

# Parser function
function parse_output(stdout_str, stderr_str)
    flow = parse(Int64, strip(stdout_str))
    
    time_match = match(r"Time: ([\d\.]+)s", stderr_str)
    path_match = match(r"AugmentingPaths: (\d+)", stderr_str)
    
    time = isnothing(time_match) ? 0.0 : parse(Float64, time_match.captures[1])
    paths = isnothing(path_match) ? 0 : parse(Int64, path_match.captures[1])
    
    return (flow, time, paths)
end

function run_maxflow(k, algo)
    cmd = `$MAXFLOW_BIN --size $k --algorithm $algo`
    out_buf = IOBuffer()
    err_buf = IOBuffer()
    
    # Run command
    # We define a pipeline to capture outputs
    proc = run(pipeline(cmd, stdout=out_buf, stderr=err_buf), wait=true)
    
    return parse_output(String(take!(out_buf)), String(take!(err_buf)))
end

function run_matching(k, degree)
    cmd = `$MATCHING_BIN --size $k --degree $degree`
    out_buf = IOBuffer()
    err_buf = IOBuffer()
    
    proc = run(pipeline(cmd, stdout=out_buf, stderr=err_buf), wait=true)
    
    return parse_output(String(take!(out_buf)), String(take!(err_buf)))
end

# Task 1 & 4 Experiments
function run_flow_experiments()
    sizes = 1:12 # Up to 12 for reasonable time in this environment, task asks for 1..16
    # 16 might be too slow for EK interactive session, but user asked for 1..16.
    # I'll restrict to 12 for safety in demonstration, user can change range.
    # Actually, instructions say "Warunkiem koniecznym... wykonanie dla wszystkich k... 1..16".
    # I will try to run all, but beware of timeouts. C++ is fast.
    # But running all in a script might take long. I'll default to 1:12 but comment.
    sizes = 1:16
    
    results = FlowResult[]
    results_lock = ReentrantLock()

    println("Running Flow Experiments (Task 1 & 4)...")

    # Parallelize over sizes and algorithms
    tasks = []
    for k in sizes
        for algo in ["ek", "dinic"]
            push!(tasks, (k, algo))
        end
    end

    @threads for (k, algo) in tasks
        (flow, time, paths) = run_maxflow(k, algo)
        
        lock(results_lock) do
            push!(results, FlowResult(k, algo, flow, time, paths))
            println("Completed k=$k algo=$algo: Time=$time microseconds")
        end
    end
    
    return results
end

# Task 2 Experiments
function run_matching_experiments()
    ks = 3:10 # Task says 3..10
    
    results = MatchingResult[]
    results_lock = ReentrantLock()

    println("Running Matching Experiments (Task 2)...")

    work_items = []
    for k in ks
        for i in 1:k
            push!(work_items, (k, i))
        end
    end

    @threads for (k, i) in work_items
        (match_size, time, _) = run_matching(k, i)
        
        lock(results_lock) do
            push!(results, MatchingResult(k, i, match_size, time))
            println("Completed matching k=$k i=$i: Time=$time s")
        end
    end
    
    return results
end

# Task 3 Comparison
function run_comparison()
    println("Running Comparison with JuMP (Task 3)...")
    k = 4
    file_lp = joinpath(@__DIR__, "model_temp.jl")
    
    # Generate model from C++ and capture its output
    cmd_gen = `$MAXFLOW_BIN --size $k --algorithm dinic --glpk $file_lp`
    out_buf = IOBuffer()
    err_buf = IOBuffer()
    run(pipeline(cmd_gen, stdout=out_buf, stderr=err_buf))
    
    # Parse C++ result from the run that generated the file
    (cpp_flow, cpp_time, _) = parse_output(String(take!(out_buf)), String(take!(err_buf)))
    
    # Run generated Julia model
    println("Solving generated JuMP model for k=$k...")
    t_start = time()
    cmd_run = `julia $file_lp`
    out = read(cmd_run, String)
    t_end = time()
    
    # Parse output, looking for the last numerical value
    lines = split(strip(out), "\n")
    jump_val = 0.0
    for l in reverse(lines)
        try
            jump_val = parse(Float64, strip(l))
            break
        catch
            continue
        end
    end
    jump_time = t_end - t_start
    
    # (cpp_flow already obtained from the first run)
    
    println("Comparison k=$k:")
    println("JuMP Value: $jump_val, Time: $jump_time s")
    println("C++ Value: $cpp_flow, Time: $cpp_time s")
    
    if abs(jump_val - cpp_flow) < 1e-5
        println("SUCCESS: Results match!")
    else
        println("WARNING: Results differ!")
    end
    
    rm(file_lp, force=true)
end

# Plotting
function generate_plots(flow_results, matching_results)
    println("Generating plots...")
    
    # Filter flow results
    ek_res = filter(r -> r.algorithm == "ek", flow_results)
    dinic_res = filter(r -> r.algorithm == "dinic", flow_results)
    
    sort!(ek_res, by = x -> x.size)
    sort!(dinic_res, by = x -> x.size)
    
    # Plot 1: Flow Time vs K
    p1 = plot(
        [r.size for r in ek_res], [r.time for r in ek_res], 
        label="Edmonds-Karp", xlabel="k", ylabel="Time (microseconds)",
        title="MaxFlow Execution Time", marker=:circle, yscale=:log10, legend=:topleft
    )
    plot!(p1, [r.size for r in dinic_res], [r.time for r in dinic_res], label="Dinic", marker=:square)
    savefig(p1, joinpath(@__DIR__, "flow_time.png"))

    # Plot 2: Augmenting Paths vs K
    p2 = plot(
        [r.size for r in ek_res], [r.augmenting_paths for r in ek_res], 
        label="Edmonds-Karp", xlabel="k", ylabel="Paths",
        title="Augmenting Paths Count", marker=:circle, yscale=:log10
    )
    plot!(p2, [r.size for r in dinic_res], [r.augmenting_paths for r in dinic_res], label="Dinic", marker=:square)
    savefig(p2, joinpath(@__DIR__, "flow_paths.png"))

    # Plot 3: Matching Time for constant i, varying k
    # Pick i=3
    m_i3 = filter(r -> r.degree == 3, matching_results)
    sort!(m_i3, by = x -> x.size)
    # Pick i=k (degree grows)
    m_ik = filter(r -> r.degree == r.size, matching_results)
    sort!(m_ik, by = x -> x.size)

    p3 = plot(
        [r.size for r in m_i3], [r.time for r in m_i3],
        label="Degree i=3", xlabel="k", ylabel="Time (s)",
        title="Matching Time", marker=:circle
    )
    plot!(p3, [r.size for r in m_ik], [r.time for r in m_ik], label="Degree i=k", marker=:square)
    savefig(p3, joinpath(@__DIR__, "matching_time.png"))
    
    println("Plots saved in scripts directory.")
end

function main()
    flow_res = run_flow_experiments()
    match_res = run_matching_experiments()
    run_comparison()
    
    if isdefined(Main, :Plots)
        generate_plots(flow_res, match_res)
    end
end

main()
