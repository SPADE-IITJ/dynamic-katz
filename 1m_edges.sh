#!/bin/bash

# Specify the graph file and batch update files here
graph_file="Graphs/1m_edges.txt"  # Replace with your graph file name
update_files=(
    "Graphs/1m_edges_1_del.txt" "Graphs/1m_edges_1_add.txt"  # Pair 1
    "Graphs/1m_edges_10_del.txt" "Graphs/1m_edges_10_add.txt"  # Pair 2
    "Graphs/1m_edges_100_del.txt" "Graphs/1m_edges_100_add.txt"
    "Graphs/1m_edges_1000_del.txt" "Graphs/1m_edges_1000_add.txt"
    "Graphs/1m_edges_10000_del.txt" "Graphs/1m_edges_10000_add.txt"
)


# Check if the program has been compiled
if [ ! -f katz_dynamic ]; then
    echo "Compiling the C++ program..."
    g++ -std=c++17 -fopenmp -o katz_dynamic katz_dynamic.cpp
    if [ $? -ne 0 ]; then
        echo "Compilation failed. Please check the C++ code for errors."
        exit 1
    fi
fi

# Ensure the graph file exists
if [ ! -f "$graph_file" ]; then
    echo "Error: Graph file '$graph_file' not found!"
    exit 1
fi

# Check if the number of update files is even (pairs of deletion and addition)
if [ $((${#update_files[@]} % 2)) -ne 0 ]; then
    echo "Error: Please provide pairs of deletion and addition update files."
    exit 1
fi

# Run the program with the graph file and all update files
echo "PROGRAM 1: katz_dynamic"
./katz_dynamic "$graph_file" "${update_files[@]}"

# Check if the program ran successfully
if [ $? -eq 0 ]; then
    echo "Program executed successfully!"
    echo ""
    echo ""
else
    echo "Program execution failed. Please check for runtime errors."
    exit 1
fi

# Check if the program has been compiled
if [ ! -f katz_threads ]; then
    echo "Compiling the C++ program..."
    g++ -std=c++17 -fopenmp -o katz_threads katz_threads.cpp
    if [ $? -ne 0 ]; then
        echo "Compilation failed. Please check the C++ code for errors."
        exit 1
    fi
fi

# Run the program with the graph file and all update files
echo "PROGRAM 1: katz_threads"
./katz_threads "$graph_file" "${update_files[@]}"

# Check if the program ran successfully
if [ $? -eq 0 ]; then
    echo "Program executed successfully!"
else
    echo "Program execution failed. Please check for runtime errors."
    exit 1
fi