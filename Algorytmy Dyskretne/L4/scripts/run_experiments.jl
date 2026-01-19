using Base.Threads
using Dates
using Printf
using Random
using Plots

const BIN_DIR = joinpath(@__DIR__, "..")
const MAXFLOW_BIN = joinpath(BIN_DIR, "maxflow")
const MATCHING_BIN = joinpath(BIN_DIR, "matching")
const PLOTS_DIR = joinpath(BIN_DIR,"plots")
const MODELS_DIR = joinpath(BIN_DIR,"models")

const reps = 25

wanted = 10
if Threads.nthreads() < wanted
    println("Restarting Julia with $wanted threads...")
    julia_exe = joinpath(Sys.BINDIR, "julia")
    cmd = `$(julia_exe) -t $wanted $(abspath(PROGRAM_FILE))`
    run(cmd)
    exit()
end

struct FlowResult
    size::Int
    algorithm::String
    flow::Float64
    time::Float64
    augmenting_paths::Float64
end

struct MatchingResult
    size::Int
    degree::Int
    match_size::Float64
    time::Float64
end

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

# Task 1
function run_flow_experiments()
    sizes = 1:16
    #reps = 3

    results = FlowResult[]
    results_lock = ReentrantLock()

    println("Running Flow Experiments (Task 1 & 4) — averaging $reps runs per config...")

    tasks = []
    for k in sizes
        for algo in ["ek", "dinic"]
            push!(tasks, (k, algo))
        end
    end

    @threads for (k, algo) in tasks
        sum_flow = 0
        sum_time = 0.0
        sum_paths = 0
        for r in 1:reps
            (flow, time, paths) = run_maxflow(k, algo)
            sum_flow += flow
            sum_time += time
            sum_paths += paths
            print("Done $r reps of $algo\r")
        end

        avg_flow = sum_flow / reps
        avg_time = sum_time / reps
        avg_paths = sum_paths / reps

        lock(results_lock) do
            push!(results, FlowResult(k, algo, avg_flow, avg_time, avg_paths))
            println("Completed k=$k algo=$algo: Avg Time=$(round(avg_time, digits=6)) microseconds")
        end
    end

    return results
end

# Task 2
function run_matching_experiments()
    ks = 3:10
    #reps=3
    
    results = MatchingResult[]
    results_lock = ReentrantLock()

    println("Running Matching Experiments (Task 2)...")

    work_items = []
    for k in ks
        for deg in 1:k
            push!(work_items, (k, deg))
        end
    end

    @threads for (k, deg) in work_items
        sum_match_size = 0
        sum_time = 0.0

        for rep in 1:reps
            (match_size, time, _) = run_matching(k, deg)

            sum_match_size += match_size
            sum_time += time

            print("Done $rep reps for k = $k, i = $deg\r")
        end

        avg_match_size = sum_match_size / reps
        avg_time = sum_time / reps

        lock(results_lock) do
            push!(results, MatchingResult(k, deg, avg_match_size, avg_time))
            println("Completed matching k=$k i=$deg: Avg Time=$(round(avg_time,digits=6)) s")
        end
    end
    
    return results
end

# Task 3 Comparison
function run_comparison()
    println("Running Comparison with JuMP for task 1")
    for k in 2:4
        file_lp = joinpath(MODELS_DIR, "model_ek_$k.jl")
        
        # Generate model from C++ and capture its output
        cmd_gen = `$MAXFLOW_BIN --size $k --algorithm ek --glpk $file_lp`
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
        println("C++ Value: $cpp_flow, Time: $cpp_time microseconds")
        
        if abs(jump_val - cpp_flow) < 1e-5
            println("SUCCESS: Results match!")
        else
            println("WARNING: Results differ!")
        end
    end

    println("Running Comparison with JuMP for task 2")
    for k in 2:4
        file_lp = joinpath(MODELS_DIR, "model_matching_$k.jl")
        
        # Generate model from C++ and capture its output
        cmd_gen = `$MATCHING_BIN --size $k --degree 2 --glpk $file_lp`
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
        println("C++ Value: $cpp_flow, Time: $cpp_time microseconds")
        
        if abs(jump_val - cpp_flow) < 1e-5
            println("SUCCESS: Results match!")
        else
            println("WARNING: Results differ!")
        end
    end
    
    #rm(file_lp, force=true)
end

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
        title="MaxFlow Execution Time", marker=:circle, yscale=:log10, legend=:topleft, dpi=500
    )
    plot!(p1, [r.size for r in dinic_res], [r.time for r in dinic_res], label="Dinic", marker=:square)
    savefig(p1, joinpath(PLOTS_DIR, "flow_time.png"))

    # Plot 2: Augmenting Paths vs K
    p2 = plot(
        [r.size for r in ek_res], [r.augmenting_paths for r in ek_res], 
        label="Edmonds-Karp", xlabel="k", ylabel="Paths",
        title="Agmented Paths Value", marker=:circle, yscale=:log10, dpi=500
    )
    plot!(p2, [r.size for r in dinic_res], [r.augmenting_paths for r in dinic_res], label="Dinic", marker=:square)
    savefig(p2, joinpath(PLOTS_DIR, "flow_paths.png"))

    # Plot 3: Average flow vs K
    p3 = plot(
        [r.size for r in ek_res], [r.flow for r in ek_res], 
        label="Edmonds-Karp", xlabel="k", ylabel="Paths",
        title="Augmenting Paths Count", marker=:circle, yscale=:log10, dpi=500
    )
    plot!(p3, [r.size for r in dinic_res], [r.flow for r in dinic_res], label="Dinic", marker=:square)
    savefig(p3, joinpath(PLOTS_DIR, "flow_count.png"))

    # Plot 4: Matching Time for constant i, varying k
    # Pick i=3
    #m_i3 = filter(r -> r.degree == 3, matching_results)
    #sort!(m_i3, by = x -> x.size)
    # Pick i=k (degree grows)
    #m_ik = filter(r -> r.degree == r.size, matching_results)
    #sort!(m_ik, by = x -> x.size)

    # Iterate over unique degrees present in results
    degrees = sort(unique([r.degree for r in matching_results]))
    for i in degrees
        m_temp = filter(r -> r.degree == i, matching_results)
        sort!(m_temp,by = x -> x.size)
        if isempty(m_temp)
            continue
        end
        p_temp = plot(
            [r.size for r in m_temp], [r.time for r in m_temp],
            label="Time for i=$i", xlabel="k", ylabel="Time (ms)",
            title="Matching Time for degree = $i", marker=:circle, dpi=500
        )
        savefig(p_temp, joinpath(PLOTS_DIR, "matching_time_i_$i.png"))
    end

    # Iterate over unique sizes present in results
    sizes = sort(unique([r.size for r in matching_results]))
    for k in sizes
        m_temp = filter(r -> r.size == k, matching_results)
        sort!(m_temp,by = x -> x.degree)
        if isempty(m_temp)
            continue
        end
        p_temp = plot(
            [r.degree for r in m_temp], [r.match_size for r in m_temp],
            label="k=$k", xlabel="i", ylabel="Match size",
            title="Matching size for k = $k", marker=:circle, dpi=500
        )
        savefig(p_temp, joinpath(PLOTS_DIR, "matching_size_k_$k.png"))
    end

    # p4 = plot(
    #     [r.size for r in m_i3], [r.time for r in m_i3],
    #     label="Degree i=3", xlabel="k", ylabel="Time (s)",
    #     title="Matching Time", marker=:circle, dpi=500
    # )
    # plot!(p4, [r.size for r in m_ik], [r.time for r in m_ik], label="Degree i=k", marker=:square)
    # savefig(p4, joinpath(@__DIR__, "matching_time.png"))

    # p5 = plot(
    #     [r.size for r in m_i3], [r.match_size for r in m_i3],
    #     label="Degree i=3", xlabel="k", ylabel="Time (s)",
    #     title="Matching Time", marker=:circle, dpi=500
    # )
    # plot!(p5, [r.size for r in m_ik], [r.match_size for r in m_ik], label="Degree i=k", marker=:square)
    # savefig(p5, joinpath(@__DIR__, "matching_size.png"))
    
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
