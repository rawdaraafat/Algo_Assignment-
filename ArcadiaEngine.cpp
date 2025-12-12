// ArcadiaEngine.cpp - STUDENT TEMPLATE
// TODO: Implement all the functions below according to the assignment requirements

#include "ArcadiaEngine.h"
#include <algorithm>
#include <queue>
#include <numeric>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <set>

using namespace std;

// =========================================================
// PART A: DATA STRUCTURES (Concrete Implementations)
// =========================================================

///PlayerTable (Mid square - Double Hashing)
class ConcretePlayerTable : public PlayerTable {
private:
    struct Player {
        int key; //shirt number
        string value; //name
        bool used;
        Player() : key(-1), used(false) {}
    };

    vector<Player> table;

    ///Mid-Square Hash Function (r = 2)
    int midSquare(int key) {
        int sq = key * key;
        string s = to_string(sq);
        int n = s.size();

        int mid = n / 2;
        int start, len;

        if (n % 2 == 0) {
            /// Even length → take 2 middle digits
            start = mid - 1;
            len = 2;
        } else {
            /// Odd length → take 3 middle digits
            start = max(0, mid - 1);
            len = 3;
        }

        /// Extract substring and convert to integer
        int midDigits = stoi(s.substr(start, len));
        return midDigits % 101;     // hash index
    }

    /// Second hash for double hashing
    int hash2(int key) {
        return 97 - (key % 97); //first prime < 101
    }

public:
    ConcretePlayerTable() {
        table.resize(101);
    }

    /// INSERT using Double Hashing
    void insert(int playerID, string name) override {
        int h1 = midSquare(playerID);
        int h2 = hash2(playerID);

        for (int i = 0; i < 101; i++) {
            int index = (h1 + i * h2) % 101;

            if (!table[index].used) {
                table[index].key = playerID;
                table[index].value = name;
                table[index].used = true;
                return;
            }
        }
        cout << "Hash Table is FULL! Cannot insert.\n";
    }

    /// SEARCH using Double Hashing
    string search(int playerID) override {
        int h1 = midSquare(playerID);
        int h2 = hash2(playerID);

        for (int i = 0; i < 101; i++) {
            int index = (h1 + i * h2) % 101;

            if (!table[index].used)
                return "";  //key not found

            if (table[index].key == playerID)
                return table[index].value;
        }
        return "";
    }

    //todo: this function is just for testing
    // so we will need to remove it later
    // as it's not an assignment requirement
    void display() {
        cout << "\n--- Player Hash Table (Double Hashing) ---\n";
        cout << "Index\tPlayerID\tName\n";
        cout << "----------------------------------------\n";

        for (int i = 0; i < 101; i++) {
            if (table[i].used) {
                cout << i << "\t" << table[i].key
                     << "\t\t" << table[i].value << endl;
            } else {
                cout << i << "\tEMPTY\n";
            }
        }
        cout << "----------------------------------------\n\n";
    }
};

// --- 2. Leaderboard (Skip List) ---

class ConcreteLeaderboard : public Leaderboard {
private:
    // TODO: Define your skip list node structure and necessary variables
    // Hint: You'll need nodes with multiple forward pointers

public:
    ConcreteLeaderboard() {
        // TODO: Initialize your skip list
    }

    void addScore(int playerID, int score) override {
        // TODO: Implement skip list insertion
        // Remember to maintain descending order by score
    }

    void removePlayer(int playerID) override {
        // TODO: Implement skip list deletion
    }

    vector<int> getTopN(int n) override {
        // TODO: Return top N player IDs in descending score order
        return {};
    }
};

// --- 3. AuctionTree (Red-Black Tree) ---

class ConcreteAuctionTree : public AuctionTree {
private:
    // TODO: Define your Red-Black Tree node structure
    // Hint: Each node needs: id, price, color, left, right, parent pointers

public:
    ConcreteAuctionTree() {
        // TODO: Initialize your Red-Black Tree
    }

    void insertItem(int itemID, int price) override {
        // TODO: Implement Red-Black Tree insertion
        // Remember to maintain RB-Tree properties with rotations and recoloring
    }

    void deleteItem(int itemID) override {
        // TODO: Implement Red-Black Tree deletion
        // This is complex - handle all cases carefully
    }
};

// =========================================================
// PART B: INVENTORY SYSTEM (Dynamic Programming)
// =========================================================

// === Partition (Minimize difference) ===
int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {
    // Edge cases
    if (n <= 0 || coins.empty()) return 0;

    long long total = 0;
    for (int v : coins) total += v;

    long long half = total / 2;

    // dp[j] = true if sum j is achievable
    vector<char> dp(half + 1, 0);
    dp[0] = 1;

    for (int val : coins) {
        // iterate backwards to keep 0/1 property
        for (long long j = half; j >= val; --j) {
            if (dp[j - val]) dp[j] = 1;
        }
    }

    // find best achievable sum <= half
    long long best = 0;
    for (long long j = half; j >= 0; --j) {
        if (dp[j]) { best = j; break; }
    }

    long long other = total - best;
    long long diff = llabs(other - best);
    return (int)diff;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    // TODO: Implement 0/1 Knapsack using DP
    // items = {weight, value} pairs
    // Return maximum value achievable within capacity
    return 0;
}

///string decoding(DP)
long long InventorySystem::countStringPossibilities(string s)   {
    ///This is a large prime number to avoid integer overflow.
    const int MOD = 1e9 + 7;
    int n = s.size();
    vector<long long> dp(n + 1, 0);
    dp[0] = 1;  // empty string

    ///it counts all valid sequences of s where "uu" and "nn" can optionally merge.
    for (int i = 1; i <= n; i++) {
        /// Single character
        dp[i] = dp[i - 1] % MOD;

        /// Check two-character substitution
        if (i >= 2) {
            string pair = s.substr(i - 2, 2);
            if (pair == "uu" || pair == "nn") {
                dp[i] = (dp[i] + dp[i - 2]) % MOD;
            }
        }
    }
    return dp[n];
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

// === Path existence (undirected) ===
bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {
    // Validate nodes
    if (n <= 0) return false;
    if (source < 0 || source >= n || dest < 0 || dest >= n) return false;
    if (source == dest) return true;

    vector<vector<int>> adj(n);
    for (auto &e : edges) {
        if (e.size() >= 2) {
            int u = e[0], v = e[1];
            if (u >= 0 && u < n && v >= 0 && v < n) {
                adj[u].push_back(v);
                adj[v].push_back(u);
            }
        }
    }

    vector<char> seen(n, 0);
    queue<int> q;
    q.push(source);
    seen[source] = 1;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : adj[u]) {
            if (!seen[v]) {
                if (v == dest) return true;
                seen[v] = 1;
                q.push(v);
            }
        }
    }
    return false;
}

///MST with Prim's
long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>>& roadData) {
    /// Build adjacency list
    /// adj is a vector of vectors,
    /// used to store the adjacency list of the graph.
    /// Example:
    ///    If adj[0] = {{1,10}, {2,20}},
    ///    it means city 0 is connected to city 1 with cost 10, and city 2 with cost 20.
    vector<vector<pair<int,long long>>> adj(n);
    for(int i = 0 ; i < m ; i++){
        int u = roadData[i][0], v = roadData[i][1];
        long long c = roadData[i][2]*goldRate + roadData[i][3]*silverRate;

        ///Since the roads are undirected, the connection works both ways.
        adj[u].push_back({v,c});
        adj[v].push_back({u,c});
    }

    /// Prim's algorithm
    vector<bool> visited(n,false);

    ///Each element is a pair {cost, node}:
    ///    first = cost of reaching node from the MST
    ///    second = node itself
    ///greater<pair<...>> → makes it a min-heap, so the smallest cost edge is always on top.
    priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<pair<long long,int>>> pq;

    pq.push({0,0}); // {cost, node}
    long long totalCost = 0;
    int visitedCount = 0;

    /// The loop picks the minimum-cost edge from the priority queue connecting the MST to an unvisited node.
    /// If the node has already been visited, the edge is skipped
    /// Otherwise, the node with it's cost is added to the MST, and all edges from this new node are candidate edges for the next iterations.
    while(!pq.empty() && visitedCount < n){
        auto [c,u] = pq.top();
        pq.pop();

        if(visited[u]) continue;

        visited[u] = true;
        totalCost += c;
        visitedCount++;

        for(auto &[v,w]: adj[u]){
            if(!visited[v]){
                pq.push({w,v});
            }
        }
    }

    if(visitedCount != n) return -1;
    return totalCost;
}

// === All-Pairs Shortest Path (Floyd–Warshall) + sum -> binary ===
string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {
    if (n <= 0) return string("0");

    const long long INF = (1LL<<60);
    // Use long long matrix
    vector<vector<long long>> dist(n, vector<long long>(n, INF));

    for (int i = 0; i < n; ++i) dist[i][i] = 0;

    // roads entries: {u, v, length}
    for (auto &r : roads) {
        if (r.size() < 3) continue;
        int u = r[0], v = r[1];
        long long w = r[2];
        if (u < 0 || u >= n || v < 0 || v >= n) continue;
        // If multiple edges exist, keep the smallest length
        dist[u][v] = min(dist[u][v], w);
        dist[v][u] = min(dist[v][u], w);
    }

    // Floyd-Warshall
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            if (dist[i][k] == INF) continue;
            for (int j = 0; j < n; ++j) {
                if (dist[k][j] == INF) continue;
                long long nd = dist[i][k] + dist[k][j];
                if (nd < dist[i][j]) dist[i][j] = nd;
            }
        }
    }

    // Sum distances for unique pairs i < j, ignoring unreachable (INF)
    __int128 total = 0; // use wider integer to avoid overflow in intermediate sum
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (dist[i][j] != INF) total += dist[i][j];
            // If disconnected, the problem expects we skip those pairs (as in examples).
        }
    }

    // Convert total to binary string
    if (total == 0) return string("0");

    // Convert __int128 to binary
    string bits;
    __int128 t = total;
    while (t > 0) {
        bits.push_back( (t & 1) ? '1' : '0' );
        t >>= 1;
    }
    reverse(bits.begin(), bits.end());
    return bits;
}

// =========================================================
// PART D: SERVER KERNEL (Greedy)
// =========================================================


// A,A,B   n=2
//A,B,IDLE,A
//4

// A,A,A,B,B,C   n=3
//A,B,C,IDLE,A,B,IDLE,IDLE,A
//9


int ServerKernel::minIntervals(vector<char>& tasks, int n) {
    // A,A,A,B,B,B,C   n=0
    // If cooling time is 0, we can execute all tasks consecutively
    if (n == 0) return tasks.size();

    // Step 1: Count frequency of each task (A-Z)
    // We use an array of size 26 since tasks are uppercase English letters
    vector<int> freq(26, 0);


    // Count occurrences of each task
    for (char task : tasks) {
        freq[task - 'A']++;  // Convert char to index (0-25)
        // freq[3,3,1]
    }

    // Step 2: Find the maximum frequency (most frequent task)
    int max_freq = 0;
    for (int count : freq) {
        max_freq = max(max_freq, count);
    }

    // Step 3: Count how many tasks have this maximum frequency
    int count_max_freq = 0;
    for (int count : freq) {
        if (count == max_freq) {
            count_max_freq++;
        }
    }


    // Step 4: Calculate minimum intervals using the scheduling formula
    // Formula explanation:
    // (max_freq - 1) * (n + 1) + count_max_freq
    // - max_freq - 1: Number of gaps between occurrences of most frequent task
    // - n + 1: Each gap can hold n other tasks/idles + 1 for the task itself
    // - count_max_freq: Add the last occurrence of each max-frequency task

    // EXPLAIN THE RULE MATHEMATICALLY
    // A,A,A,B,B,C   n=3
    //A,B,C,IDLE,A,B,IDLE,IDLE,A
    //9
    int calculated_intervals = (max_freq - 1) * (n + 1) + count_max_freq;
    //                          (   3    - 1) * (3 + 1)  +   1            = 9
    // Step 5: Return the maximum of calculated value and total tasks
    // We need to consider cases where we have many tasks and small n
    // Example: tasks={'A','B','C'}, n=2 → calculated=3, tasks.size()=3
    // So we take the maximum to ensure we schedule all tasks
    return max(calculated_intervals, (int)tasks.size());
    // A,B,C,D,E,F     n=3

}


// =========================================================
// FACTORY FUNCTIONS (Required for Testing)
// =========================================================

extern "C" {
    PlayerTable* createPlayerTable() { 
        return new ConcretePlayerTable(); 
    }

    Leaderboard* createLeaderboard() { 
        return new ConcreteLeaderboard(); 
    }

    AuctionTree* createAuctionTree() { 
        return new ConcreteAuctionTree(); 
    }
}

// int main() {
//     // ==== Test Path Existence ====
//     vector<vector<int>> edges = {};
//     cout << "Path from 0 to 1: " << (WorldNavigator::pathExists(1, edges, 0, 0) ? "YES" : "NO") << endl;
//
//     // ==== Test Partition (Optimize Loot Split) ====
//     vector<int> coins = {1,2,4};
//     cout << "Min loot difference: " << InventorySystem::optimizeLootSplit(coins.size(), coins) << endl;
//
//     // ==== Test All-Pairs Shortest Path Sum (Binary) ====
//     vector<vector<int>> roads = {
//         {{0,1,1}, {1,2,2}}
//     };
//     cout << "APSP Sum (Binary): " << WorldNavigator::sumMinDistancesBinary(3, roads) << endl;
//
//     return 0;
// }