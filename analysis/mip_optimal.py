# Offline MIP Optimal Solver for the RSiC Problem
import pandas as pd
import numpy as np
import time
from ortools.linear_solver import pywraplp

# Server capacity per dimension
E = 1000

#Data
def load_batch(filepath, batch_size, window_start=0):
    df = pd.read_csv(filepath)
    df = df.sort_values("start").reset_index(drop=True)
    df = df.iloc[window_start : window_start + batch_size]
    return df


#Greedy
def run_greedy(jobs):
    # servers stored as [open_time, max_end, remaining_capacity]
    # Greedy: assign job to eligible server with minimum extension
    # open new server if no eligible server exists
    servers = []

    for _, job in jobs.iterrows():
        start = int(job["start"])
        end   = int(job["end"])
        r     = [int(job["core"]), int(job["memory"]), int(job["ssd"]), int(job["nic"])]

        eligible = [i for i, s in enumerate(servers) if all(s[2][k] >= r[k] for k in range(4))]

        if eligible:
            # pick server with minimum extension
            best = min(eligible, key=lambda i: max(0, end - servers[i][1]))
            servers[best][1] = max(servers[best][1], end)
            for k in range(4):
                servers[best][2][k] -= r[k]
        else:
            servers.append([start, end, [E - r[k] for k in range(4)]])

    total_cost = sum(s[1] - s[0] for s in servers)
    return total_cost, len(servers)


#Optimal lower bound 
def compute_lower_bound(jobs):
    events = sorted(set(jobs["start"].tolist() + jobs["end"].tolist()))
    lb = 0
    for i in range(len(events) - 1):
        t  = events[i]
        dt = events[i + 1] - t
        active = jobs[(jobs["start"] <= t) & (jobs["end"] > t)]
        if active.empty:
            continue
        load = [active["core"].sum(),   active["memory"].sum(),
                active["ssd"].sum(),    active["nic"].sum()]
        lb += dt * int(np.ceil(max(load) / E))
    return lb


#MIP
def solve_mip(jobs, n_servers_ub, time_limit_seconds=300):
    # Variables:
    #   x[i][j] in {0,1}  job j assigned to server i
    #   e[i] >= 0          latest end time of jobs on server i
    #   o[i] >= 0          earliest start time of jobs on server i
    #   t[i] >= 0          rental duration of server i (= e[i] - o[i])
    #   y[i] in {0,1}      1 if server i is used
    #
    # Objective: minimize sum(t[i])
    #
    # Constraints:
    #   (1) each job assigned to exactly one server
    #   (2) time-aware capacity: at each event time t, jobs active on
    #       server i must not exceed E in any dimension
    #   (3) e[i] >= end[j] * x[i][j]             (server end time)
    #   (4) o[i] <= start[j] + M*(1 - x[i][j])   (server open time, big-M)
    #   (5) e[i] - o[i] <= M * y[i]               (cost zero if unused)
    #   (6) y[i] >= x[i][j]                       (server used if job assigned)
    #   (7) t[i] >= e[i] - o[i]                   (rental duration, non-negative)

    n = len(jobs)
    m = min(n, n_servers_ub + 5)

    jobs_r = jobs.reset_index(drop=True)
    starts = jobs_r["start"].tolist()
    ends = jobs_r["end"].tolist()
    res = jobs_r[["core", "memory", "ssd", "nic"]].values.tolist()
    M = int(max(ends)) #big M

    # precompute active jobs at each event time
    event_times = sorted(set(starts + ends))
    active_at   = {t: [j for j in range(n) if starts[j] <= t < ends[j]] for t in event_times}

    solver = pywraplp.Solver.CreateSolver("SCIP")
    if not solver:
        raise RuntimeError("SCIP solver not available")
    solver.SetTimeLimit(time_limit_seconds * 1000)

    x = [[solver.BoolVar(f"x_{i}_{j}") for j in range(n)] for i in range(m)]
    e = [solver.NumVar(0.0, M, f"e_{i}") for i in range(m)]
    o = [solver.NumVar(0.0, M, f"o_{i}") for i in range(m)]
    rent = [solver.NumVar(0.0, M, f"rent_{i}") for i in range(m)]
    y = [solver.BoolVar(f"y_{i}") for i in range(m)]

    #Constraint #1: each job assigned exactly once
    for j in range(n):
        solver.Add(sum(x[i][j] for i in range(m)) == 1)

    #Constraint #2: time-aware capacity constraints
    for t in event_times:
        if not active_at[t]:
            continue
        for i in range(m):
            for k in range(4):
                solver.Add(sum(int(res[j][k]) * x[i][j] for j in active_at[t]) <= E)

    #Constraint #3: server end time
    for i in range(m):
        for j in range(n):
            solver.Add(e[i] >= ends[j] * x[i][j])

    #Constraint #4: server open time (big-M)
    for i in range(m):
        for j in range(n):
            solver.Add(o[i] <= starts[j] + M * (1 - x[i][j]))

    #Constraint #5: unused servers have zero cost
    for i in range(m):
        solver.Add(e[i] - o[i] <= M * y[i])

    #Constraint #6: server marked used if any job assigned
    for i in range(m):
        for j in range(n):
            solver.Add(y[i] >= x[i][j])

    #Constraint #7: rental duration = e[i] - o[i], kept non-negative via rent[i]
    for i in range(m):
        solver.Add(rent[i] >= e[i] - o[i])

    solver.Minimize(sum(rent[i] for i in range(m)))

    t0 = time.time()
    status = solver.Solve()
    solve_time = time.time() - t0

    if status in (pywraplp.Solver.OPTIMAL, pywraplp.Solver.FEASIBLE):
        return solver.Objective().Value(), solve_time, status
    return None, solve_time, status


#Experiment
def run_experiment(mu, filepath, batch_size):
    print(f"\n{'='*60}")
    print(f"mu = {mu}  |  batch = {batch_size} jobs")
    print(f"{'='*60}")

    jobs = load_batch(filepath, batch_size)

    lb = compute_lower_bound(jobs)
    print(f"Lower bound:  {lb:>15,.0f}")

    greedy_cost, n_servers = run_greedy(jobs)
    print(f"Greedy cost:            {greedy_cost:>15,.0f}  ({n_servers} servers)")
    print(f"Greedy / LB:            {greedy_cost / lb:>15.4f}")

    print(f"Solving MIP ...")
    mip_cost, solve_time, status = solve_mip(jobs, n_servers, time_limit_seconds=300)

    status_str = "NO_SOLUTION"
    if mip_cost is not None:
        status_str = "OPTIMAL" if status == pywraplp.Solver.OPTIMAL else "FEASIBLE"
        print(f"MIP cost ({status_str}): {mip_cost:>15,.0f}  ({solve_time:.1f}s)")
        print(f"MIP / LB: {mip_cost / lb:>15.4f}")
        print(f"Greedy / MIP:{greedy_cost / mip_cost:>15.4f}")
    else:
        print(f"MIP: no solution found within time limit ({solve_time:.1f}s)")   

    return {
        "mu": mu,
        "batch_size": len(jobs),
        "lower_bound": lb,
        "greedy_cost": greedy_cost,
        "greedy_lb": greedy_cost / lb,
        "mip_cost": mip_cost,
        "mip_lb": mip_cost / lb if mip_cost else None,
        "greedy_mip": greedy_cost / mip_cost if mip_cost else None,
        "solve_time_s": solve_time,
        "status": status_str if mip_cost else "NO_SOLUTION",
    }



if __name__ == "__main__":
    # update base_path to the location of your mu-filtered CSV files
    base_path  = "/Users/m_masoor/Desktop/Plot/"
    batch_size = 20
    experiments = [
        (2,   base_path + "mip_jobs_mu2.csv"),
        (5,   base_path + "mip_jobs_mu5.csv"),
        (10,  base_path + "mip_jobs_mu10.csv"),
        (100, base_path + "mip_jobs_mu100.csv"),
    ]

    results = []
    for mu, filepath in experiments:
        results.append(run_experiment(mu, filepath, batch_size))

    print(f"\n{'='*60}")
    print("SUMMARY")
    print(f"{'='*60}")
    print(f"{'mu':>6} {'n':>5} {'LB':>15} {'Greedy':>15} {'MIP':>15} "
          f"{'G/LB':>7} {'MIP/LB':>8} {'G/MIP':>7} {'time(s)':>8} {'status':>10}")
    print("-" * 110)
    for r in results:
        mip_str = f"{r['mip_cost']:>15,.0f}" if r["mip_cost"] else f"{'N/A':>15}"
        mlb_str = f"{r['mip_lb']:>8.4f}"     if r["mip_lb"]   else f"{'N/A':>8}"
        gm_str  = f"{r['greedy_mip']:>7.4f}" if r["greedy_mip"] else f"{'N/A':>7}"
        print(f"{r['mu']:>6} {r['batch_size']:>5} {r['lower_bound']:>15,.0f} "
              f"{r['greedy_cost']:>15,.0f} {mip_str} "
              f"{r['greedy_lb']:>7.4f} {mlb_str} {gm_str} "
              f"{r['solve_time_s']:>8.1f} {r['status']:>10}")
