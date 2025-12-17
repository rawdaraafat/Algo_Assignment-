#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include "ArcadiaEngine.cpp"

using namespace std;

/// ===================================================
/// PATH EXISTENCE (BFS) TESTS
/// ===================================================
void testPathExists() {
    cout << "\n[TEST] pathExists (BFS)" << endl;

    WorldNavigator nav;

    // Basic connected graph
    {
        vector<vector<int>> edges = {{0,1},{1,2}};
        cout << nav.pathExists(3, edges, 0, 2) << endl; // true
    }

    // Disconnected graph
    {
        vector<vector<int>> edges = {{0,1},{2,3}};
        cout << nav.pathExists(4, edges, 0, 3) << endl; // false
    }

    // Single node, source == destination
    {
        vector<vector<int>> edges;
        cout << nav.pathExists(1, edges, 0, 0) << endl; // true
    }

    // Invalid source
    {
        vector<vector<int>> edges = {{0,1}};
        cout << nav.pathExists(3, edges, -1, 1) << endl; // false
    }

    // Invalid destination
    {
        vector<vector<int>> edges = {{0,1}};
        cout << nav.pathExists(3, edges, 0, 5) << endl; // false
    }

    // Large chain (stress)
    {
        int n = 1000;
        vector<vector<int>> edges;
        for (int i = 0; i < n - 1; i++) edges.push_back({i, i + 1});
        cout << nav.pathExists(n, edges, 0, 999) << endl; // true
    }

    cout << "PathExists tests passed" << endl;
}

/// ===================================================
/// ALL-PAIRS SHORTEST PATH + BINARY SUM
/// ===================================================
void testSumMinDistancesBinary() {
    cout << "\n[TEST] sumMinDistancesBinary (APSP)" << endl;

    WorldNavigator nav;

    // Sample from assignment
    {
        vector<vector<int>> roads = {{0,1,1},{1,2,2}};
        cout << nav.sumMinDistancesBinary(3, roads) << endl; // 110
    }

    // Single edge
    {
        vector<vector<int>> roads = {{0,1,4}};
        cout << nav.sumMinDistancesBinary(2, roads) << endl; // 100
    }

    // Disconnected graph
    {
        vector<vector<int>> roads = {{0,1,2},{0,2,8}};
        cout << nav.sumMinDistancesBinary(3, roads) << endl; // 1010
    }

    // Multiple edges (min chosen)
    {
        vector<vector<int>> roads = {{0,1,10},{0,1,1},{1,2,2}};
        cout << nav.sumMinDistancesBinary(3, roads) << endl; // 110
    }

    // Single node
    {
        vector<vector<int>> roads;
        cout << nav.sumMinDistancesBinary(1, roads) << endl; // 0
    }

    // No nodes
    {
        vector<vector<int>> roads;
        cout << nav.sumMinDistancesBinary(0, roads) << endl; // 0
    }

    cout << "APSP Binary Sum tests passed" << endl;
}

/// ===================================================
/// LOOT SPLIT (PARTITION DP)
/// ===================================================
void testOptimizeLootSplit() {
    cout << "\n[TEST] optimizeLootSplit" << endl;

    InventorySystem inv;

    // Assignment examples
    {
        vector<int> coins = {1,2,4};
        cout << inv.optimizeLootSplit(3, coins) << endl; // 1
    }

    {
        vector<int> coins = {2,2};
        cout << inv.optimizeLootSplit(2, coins) << endl; // 0
    }

    {
        vector<int> coins = {1,5,11,5};
        cout << inv.optimizeLootSplit(4, coins) << endl; // 0
    }

    // Empty input
    {
        vector<int> coins;
        cout << inv.optimizeLootSplit(0, coins) << endl; // 0
    }

    // Single coin
    {
        vector<int> coins = {10};
        cout << inv.optimizeLootSplit(1, coins) << endl; // 10
    }

    // Large equal values
    {
        vector<int> coins(100, 1);
        cout << inv.optimizeLootSplit(100, coins) << endl; // 0
    }

    cout << "Loot Split tests passed" << endl;
}

/// ===================================================
/// MAIN
/// ===================================================
int main() {
    cout << "===== RUNNING TESTS =====" << endl;

    testPathExists();
    testSumMinDistancesBinary();
    testOptimizeLootSplit();

    cout << "\nALL TESTS PASSED SUCCESSFULLY" << endl;
    return 0;
}
