#include <bits/stdc++.h>
#include <omp.h>
#include <chrono> // Include for timing
using namespace std;
using namespace chrono; // Use chrono namespace for convenience

// Katz score updater function
void katz_updater(vector<int> &nodeptrs, vector<int> &destptrs, double alpha, vector<double> &katzscores, int source)
{
    if (katzscores[source] != -1)
        return;
    katzscores[source] = 0;
    for (int i = nodeptrs[source]; i < nodeptrs[source + 1]; i++)
    {
        int v = destptrs[i];
        if (katzscores[v] == -1)
            katz_updater(nodeptrs, destptrs, alpha, katzscores, v);
        katzscores[source] += alpha * katzscores[v];
        katzscores[source] += alpha;
    }
}

void affected_nodes_finder(vector<vector<int>> &adj, vector<double> &katzscores, vector<int> &edgeendsonly)
{
    int n = edgeendsonly.size();
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        int source = edgeendsonly[i];
        int iterations = 5;
        queue<int> curr;
        curr.push(source);
        katzscores[source] = -1;
        while (iterations--)
        {
            queue<int> nextqueue;
            while (!curr.empty())
            {
                int curnode = curr.front();
                curr.pop();
                for (int v : adj[curnode])
                {
                    if (katzscores[v] == -1)
                        continue;
                    katzscores[v] = -1;
                    nextqueue.push(v);
                }
            }
            curr = nextqueue;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <graph_file> [<update_file_del> <update_file_add> ...]" << endl;
        return 1;
    }

    vector<string> fileNames;
    for (int i = 1; i < argc; ++i)
    {
        fileNames.push_back(argv[i]);
    }

    omp_set_num_threads(128);

    // Get the graph filename from the user
    cout << "Please input the filename containing the graph: ";
    string filename = fileNames[0];
    cout << filename << endl;
    int iter = 1;

    ifstream infile(filename);
    if (!infile)
    {
        cerr << "Error: Unable to open file!" << endl;
        return 1;
    }

    cout << "Reading graph from file..." << endl;

    int number_of_vertices, number_of_edges;
    infile >> number_of_vertices >> number_of_edges;

    vector<vector<int>> adj(number_of_vertices);       // Normal adjacency list
    vector<vector<int>> edge_list(number_of_vertices); // Edge list with reversed edges

    for (int i = 0; i < number_of_edges; i++)
    {
        int src, dst;
        infile >> src >> dst;
        edge_list[dst].push_back(src);
        adj[src].push_back(dst);
    }
    infile.close();

    double alpha = 0.2;
    cout << "Attenuation factor is: " << alpha << endl;

    vector<int> nodeptrs;
    vector<int> destptrs(number_of_edges);

    int tracker = 0;
    for (int i = 0; i < number_of_vertices; i++)
    {
        nodeptrs.push_back(tracker);
        for (int k = 0; k < edge_list[i].size(); k++)
        {
            destptrs[tracker++] = edge_list[i][k];
        }
    }
    nodeptrs.push_back(tracker);

    vector<double> katzscores(nodeptrs.size() - 1, -1);

    // Timing initial Katz computation
    auto start_initial = high_resolution_clock::now();

    for (int i = 0; i < nodeptrs.size() - 1; i++)
    {
        if (katzscores[i] == -1)
            katz_updater(nodeptrs, destptrs, alpha, katzscores, i);
    }

    auto end_initial = high_resolution_clock::now();
    duration<double> initial_duration = end_initial - start_initial;
    cout << "Time taken for initial Katz score computation: " << initial_duration.count() << " seconds" << endl;

    // File to write initial Katz scores
    // ofstream initial_katz_file("katz_scores_initial.txt");
    // if (!initial_katz_file)
    // {
    //     cerr << "Error: Unable to open file for writing initial Katz scores!" << endl;
    //     return 1;
    // }

    // for (int i = 0; i < katzscores.size(); i++)
    // {
    //     initial_katz_file << katzscores[i] << endl;
    // }
    // initial_katz_file.close();

    int file_counter = 1;
    char continue_updates;

    do
    {
        // Prompt for batch updates file
        cout << "Please input the filename containing the batch updates: ";
        string updates_filename = fileNames[iter];
        cout << updates_filename << endl;

        ifstream updates_file(updates_filename);
        if (!updates_file)
        {
            cerr << "Error: Unable to open batch updates file!" << endl;
            return 1;
        }

        unordered_set<int> edgeends;

        // Read the batch updates from the file
        int no_edge_updates;
        updates_file >> no_edge_updates;

        while (no_edge_updates--)
        {
            bool op;
            int src, dst;
            updates_file >> op >> src >> dst;
            edgeends.insert(dst);

            if (op == 0)
            {
                number_of_edges++;
                edge_list[dst].push_back(src);
                adj[src].push_back(dst);
            }
            else
            {
                bool flag = true;
                for (int i = 0; i < edge_list[dst].size(); i++)
                {
                    if (edge_list[dst][i] == src)
                    {
                        flag = false;
                        edge_list[dst].erase(edge_list[dst].begin() + i);
                        break;
                    }
                }
                for (int i = 0; i < adj[src].size(); i++)
                {
                    if (adj[src][i] == dst)
                    {
                        adj[src].erase(adj[src].begin() + i);
                        break;
                    }
                }
                if (flag)
                {
                    cout << "Invalid deletion: Edge not present" << endl;
                }
                else
                {
                    number_of_edges--;
                }
            }
        }
        updates_file.close();

        vector<int> nodeptrsnew;
        vector<int> destptrsnew(number_of_edges);

        tracker = 0;
        for (int i = 0; i < number_of_vertices; i++)
        {
            nodeptrsnew.push_back(tracker);
            for (int j = 0; j < edge_list[i].size(); j++)
            {
                destptrsnew[tracker++] = edge_list[i][j];
            }
        }
        nodeptrsnew.push_back(tracker);

        // Timing batch updates

        vector<int> edgeendsonly;
        for (auto &i : edgeends)
            edgeendsonly.push_back(i);

        auto start_updates = high_resolution_clock::now();

        affected_nodes_finder(adj, katzscores, edgeendsonly);

        for (int i = 0; i < nodeptrs.size() - 1; i++)
        {
            if (katzscores[i] == -1)
                katz_updater(nodeptrsnew, destptrsnew, alpha, katzscores, i);
        }

        auto end_updates = high_resolution_clock::now();
        duration<double> update_duration = end_updates - start_updates;
        cout << "Time taken for batch updates: " << update_duration.count() << " seconds" << endl;

        // Write the updated Katz scores to a new file
        // string updated_filename = "katz_scores_updated_" + to_string(file_counter) + ".txt";
        // ofstream updated_katz_file(updated_filename);
        // if (!updated_katz_file)
        // {
        //     cerr << "Error: Unable to open file for writing updated Katz scores!" << endl;
        //     return 1;
        // }

        // for (int i = 0; i < katzscores.size(); i++)
        // {
        //     updated_katz_file << katzscores[i] << endl;
        // }
        // updated_katz_file.close();
        file_counter++;

        // Ask if the user wants to continue with more batch updates
        // cout << "Do you want to continue with more batch updates? (y/n): ";
        // cin >> continue_updates;

        // } while (continue_updates == 'y' || continue_updates == 'Y');
        iter++;
    } while (iter < fileNames.size());

    return 0;
}
