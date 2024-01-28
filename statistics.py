import re
import numpy as np

objectives = {'FMAX': r'HIGHEST\s*MAKESPAN\s*\:\s*(\d+)', 'TFT': r'TOTAL\s*FLOWTIME\s*\:\s*(\d+)'}
iterations = r'COMPLETED\s*ITERATIONS\s*:\s*(\d+)'
time = r'RUNNING\s*TIME\s*:\s*([\d\.]+)'
run = r'RUN\s*\:\s*(\d+)'

def get_runs_count(content: str):
    return max([int(n) for n in re.findall(run, content)])

def extract_results(content: str, objective: str, runs: int):
    results = re.findall(objectives[objective], content)
    results_per_instance = [results[i:i + runs] for i in range(0, len(results), runs)]
    results_per_instance = [[int(r) for r in instance] for instance in results_per_instance]
    completed_interations = [int(r) for r in re.findall(iterations, content)]
    running_time = [float(r) for r in re.findall(time, content)]
    return results_per_instance, completed_interations, running_time

def generate_statistics(results_per_instance: list, completed_interations: list, running_time: list):
    avg_best_results = np.mean([min(r) for r in results_per_instance])
    avg_std_deviation = np.mean([np.std(r) for r in results_per_instance])
    avg_running_time = np.mean(running_time)
    avg_completed_iterations = np.mean(completed_interations)
    print(f"AVG Results: {avg_best_results}\nAVG STD Dev: {avg_std_deviation}\nAVG Time: {avg_running_time}\nAVG Iterations: {avg_completed_iterations}")
    

if __name__ == '__main__':
    path = 'imp/output/FMAX/output.txt'
    objective = 'FMAX'
    with open(path, 'r') as file:
        content = file.read()
    runs = get_runs_count(content)
    results_per_instance, completed_iterations, running_time = extract_results(content, objective, runs)
    statistics = generate_statistics(results_per_instance, completed_iterations, running_time)