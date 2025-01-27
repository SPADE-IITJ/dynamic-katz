# Fast Katz Centrality on Dynamic Graphs

This project contains scripts and C++ programs to compute and update Katz centrality on dynamic graphs. The updates involve edge additions and deletions processed in batches. The project uses OpenMP to parallelize computations and optimise performance.

## Project Structure

- ``1m_edges.sh`` and ``cit-Patents_new.sh``: A shell script that compiles and runs the Katz centrality C++ programs (`katz_dynamic.cpp` and `katz_threads.cpp`) on a specified graph file with multiple batch updates.

- ``katz_dynamic.cpp``: A C++ program that computes Katz centrality and updates it dynamically as the graph is modified. Uses DFS-based propagation.

- ``katz_threads.cpp``: Parallel version of the update Katz Centrality, which parallelizes affected node updates using OpenMP for efficiency.

- ``Graphs/``: A folder containing graph files and batch update files.

## Prerequisites
``g++``: Ensure you have the GNU compiler installed. The project requires support for `C++17` and `OpenMP`.
`OpenMP`: Make sure your system supports `OpenMP` for parallel execution.

## Setup Instructions

1. Clone the repository and navigate to the project folder.

    Prepare the graph and batch update files:
    Place your initial graph file and all batch update files inside a folder named `Graphs/`.
    Make sure the filenames in `1m_edges.sh` are updated to reflect the correct file paths.

2. Running the Project

    Make the shell script executable:
    ```shell
    chmod +x 1m_edges.sh
    ```

3. Run the shell script:
    ```shell 
    ./1m_edges.sh
    ```

The script will:

Compile `katz_dynamic.cpp` and `katz_threads.cpp`.
Run each program sequentially with the specified graph and batch update files.
Output the initial and updated Katz centrality scores along with the time taken for each computation.

## Files Explained

`1m_edges.sh`:

Specifies the graph and batch update files.
Compiles the C++ programs if not already compiled.
Runs the compiled programs, passing the graph file and batch updates as arguments.
Reports any compilation or runtime errors.

``katz_dynamic.cpp``:

Reads the graph structure and computes initial Katz centrality scores.
Applies each batch of updates (edge additions and deletions) and recomputes affected scores.
Outputs the time taken for both the initial computation and each batch update.

`katz_threads.cpp`:

Similar to katz_dynamic.cpp, it optimises update propagation using multiple threads for improved performance.
Outputs speedups on computation and updates.
