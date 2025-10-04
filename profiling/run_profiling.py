import subprocess
import psutil
import os
import time
import sys

test_files = ["graph_10_edges_19.graphml", 
              "graph_15_edges_14.graphml", "graph_15_edges_33.graphml", "graph_15_edges_50.graphml", "graph_15_edges_105.graphml", 
              "graph_20_edges_19.graphml", "graph_20_edges_30.graphml", "graph_20_edges_50.graphml", "graph_20_edges_100.graphml" , "graph_20_edges_190.graphml", 
              "graph_30_edges_29.graphml", "graph_30_edges_50.graphml", "graph_30_edges_78.graphml", 
              "graph_45_edges_120.graphml",
              "graph_90_edges_252.graphml", "graph_150.graphml", "graph_200.graphml", "graph_500.graphml"]

def run_and_measure(command, insert_param=None):
    # If we need to insert a custom parameter after the first argument
    if insert_param:
        # command[0] is the program name, command[1] is algorithm name, input_file name, everything else are its args
        command = [command[0], command[1], insert_param] + command[2:]
    # Start external process
    process = subprocess.Popen(command, stdout=None, stderr=None, text=True)
    ps_process = psutil.Process(process.pid)

    peak_memory = 0
    start = time.perf_counter()

    was_killed = False

    # While the process is still running, check its memory usage
    while process.poll() is None:
        try:
            mem = ps_process.memory_info().rss  # Resident Set Size (bytes)
            peak_memory = max(peak_memory, mem)
        except psutil.NoSuchProcess:
            break
        time.sleep(0.05)  # Avoid busy waiting (check every 50 ms)

        if (time.perf_counter() - start) > 120:  # Timeout after 2 minutes
            process.kill()
            process.wait()
            was_killed = True
            break

    end = time.perf_counter()
    stdout = ""
    stderr = ""

    return {
        "exit_code": -10 if was_killed else process.returncode,  # Exit code of the process
        "stdout": stdout,
        "stderr": stderr,
        "time_sec": end - start,                     # Total execution time
        "peak_mem_mb": peak_memory / 1024 / 1024     # Peak memory usage in MB
    }

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python measure.py <command> [arguments...]")
        sys.exit(1)

    # The external command to run is taken from command-line arguments
    command = sys.argv[1:]

    results = []

    for input_file in test_files:
        result = run_and_measure(command, insert_param=input_file)
    
        # Print results
        print(f"File: {input_file}")
        print(f"Exit code: {result['exit_code']}")
        print(f"Execution time: {result['time_sec']:.2f} sec")
        print(f"Peak memory usage: {result['peak_mem_mb']:.2f} MB")

        result = {"filename": input_file, "exit_code": result['exit_code'], "time_sec": result['time_sec'], "peak_mem_mb": result['peak_mem_mb']}
        results.append(result)

    # Output results as table
    print(f"{'File':<20} | {'Res':<3} | {'Time (sec)':<10} | {'Peak Mem (MB)':<15}")
    print("-" * 50)
    for res in results:
        print(f"{res['filename']:<20} | {res['exit_code']:<3} | {res['time_sec']:10.5f} | {res['peak_mem_mb']:<15}")
