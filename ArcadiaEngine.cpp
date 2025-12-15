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
};

// --- 2. Leaderboard (Skip List) ---
class ConcreteLeaderboard : public Leaderboard {
private:
    // Skip list node structure representing each player in the leaderboard
    struct SkipListNode {
        int playerID;            // Unique identifier for the player
        int score;               // Current score of the player
        vector<SkipListNode*> forward;  // Forward pointers for each level
        int level;               // Maximum level this node participates in

        // Constructor: creates a node with given ID, score, and level
        // The forward vector is initialized with (level + 1) nullptr pointers
        SkipListNode(int id, int sc, int lvl) : playerID(id), score(sc), level(lvl) {
            forward.resize(lvl + 1, nullptr);  // Allocate forward pointers for each level
        }
    };

    SkipListNode* head;    // Head node of the skip list (sentinel node)
    int maxLevel;          // Maximum allowed level in the skip list
    float probability;     // Probability for random level generation (typically 0.5)

    /// Simple random number generator for determining node levels
    /// Returns: A random level between 0 and maxLevel-1
    /// Method: Uses coin flip analogy - each level has 50% chance to continue
    int randomLevel() {
        int lvl = 0;  // Start at level 0 (always included)

        // Continue flipping coin while we get heads (probability) and below maxLevel
        // rand()/RAND_MAX gives a value between 0 and 1
        // We continue while this value < probability (e.g., 0.5)
        while (((float)rand() / RAND_MAX) < probability && lvl < maxLevel) {
            lvl++;  // Increase level for each successful coin flip
        }
        return lvl;  // Return the final level
    }

    /// Comparison function for skip list ordering
    /// Determines whether newNode should be placed before existingNode
    /// Ordering rules:
    ///   1. Higher scores come first (descending)
    ///   2. For equal scores, lower playerID comes first (ascending)
    /// Parameters:
    ///   newNode - The node being inserted
    ///   existingNode - The node currently in the list
    /// Returns: true if newNode should come before existingNode
    bool shouldInsertBefore(SkipListNode* newNode, SkipListNode* existingNode) {
        // Case 1: existingNode is null (end of list) - newNode should definitely be inserted
        if (existingNode == nullptr) return true;

        // Case 2: newNode has higher score than existingNode - comes before
        if (newNode->score > existingNode->score) return true;

        // Case 3: Scores are equal, but newNode has lower playerID - comes before
        // This implements the tie-breaking rule
        if (newNode->score == existingNode->score && newNode->playerID < existingNode->playerID)
            return true;

        // Case 4: Otherwise, newNode should come after existingNode
        return false;
    }

public:
    /// Constructor: Initializes an empty skip list
    ConcreteLeaderboard() {
        maxLevel = 16;            // Maximum of 16 levels (can be adjusted based on expected size)
        probability = 0.5;        // 50% chance to go to next level
        // Create head sentinel node with minimum possible values
        // INT_MIN ensures it's always first in the list
        head = new SkipListNode(-1, INT_MIN, maxLevel);
        srand(time(nullptr));     // Seed random number generator with current time
    }

    /// Destructor: Cleans up all allocated memory to prevent leaks
    ~ConcreteLeaderboard() {
        // Start from the first real node (level 0)
        SkipListNode* current = head->forward[0];

        // Traverse the bottom level and delete all nodes
        while (current != nullptr) {
            SkipListNode* next = current->forward[0];  // Save next pointer before deletion
            delete current;                            // Delete current node
            current = next;                            // Move to next node
        }

        delete head;  // Finally delete the head sentinel node
    }

    /// Adds or updates a player's score in the leaderboard
    /// Time Complexity: O(log n) average for insertion + O(n) for finding existing player
    /// Parameters:
    ///   playerID - The ID of the player to add/update
    ///   score - The score to add to the player's current score
    void addScore(int playerID, int score) override {
        /// Step 1: Check if player already exists (linear scan O(n) as allowed)
        /// We need to find existing player to update their cumulative score
        SkipListNode* existing = nullptr;
        SkipListNode* current = head->forward[0];  // Start from first real node

        // Traverse the bottom level to find the player
        while (current != nullptr) {
            if (current->playerID == playerID) {
                existing = current;  // Found the player
                break;
            }
            current = current->forward[0];
        }

        int finalScore;
        if (existing != nullptr) {
            /// Case A: Player exists - update their score cumulatively
            /// The problem says "addScore" means add to existing score
            finalScore = existing->score + score;
            // Remove the old node with old score
            removePlayer(playerID);  // This handles deletion from skip list
        } else {
            /// Case B: New player - use the given score as starting score
            finalScore = score;
        }

        /// Step 2: Create new node with the final score and random level
        int nodeLevel = randomLevel();  // Determine how many levels this node will have
        SkipListNode* newNode = new SkipListNode(playerID, finalScore, nodeLevel);

        /// Step 3: Find insertion position at each level (skip list search)
        /// We need to track the last node at each level that will point to newNode
        vector<SkipListNode*> update(maxLevel + 1, nullptr);
        current = head;  // Start search from head

        // Traverse from highest level down to level 0
        for (int i = maxLevel; i >= 0; i--) {
            // Move forward while:
            // 1. There's a next node at this level, AND
            // 2. newNode should NOT come before the next node (based on ordering rules)
            while (current->forward[i] != nullptr &&
                   !shouldInsertBefore(newNode, current->forward[i])) {
                current = current->forward[i];  // Move to next node at this level
            }
            // When loop exits, current is the node that should point to newNode at level i
            update[i] = current;
        }

        /// Step 4: Insert newNode at all levels from 0 to nodeLevel
        /// For each level i where i <= nodeLevel:
        ///   1. newNode's forward[i] points to what update[i] was pointing to
        ///   2. update[i]'s forward[i] now points to newNode
        for (int i = 0; i <= nodeLevel; i++) {
            newNode->forward[i] = update[i]->forward[i];  // Step 1
            update[i]->forward[i] = newNode;              // Step 2
        }
        // For levels > nodeLevel, newNode doesn't exist, so no updates needed
    }

    /// Removes a player from the leaderboard
    /// Time Complexity: O(n) for linear scan (as allowed) + O(log n) for pointer updates
    /// Parameters:
    ///   playerID - The ID of the player to remove
    void removePlayer(int playerID) override {
        /// Step 1: Linear scan to find the node by playerID (O(n) as specified)
        /// Unlike search by score, we can't use skip list levels efficiently for ID search
        SkipListNode* nodeToDelete = nullptr;
        SkipListNode* current = head->forward[0];  // Start from first real node

        // Traverse bottom level (level 0) until we find the player or reach end
        while (current != nullptr) {
            if (current->playerID == playerID) {
                nodeToDelete = current;  // Found the node to delete
                break;
            }
            current = current->forward[0];
        }

        // If player not found, nothing to do
        if (nodeToDelete == nullptr) return;

        /// Step 2: Find update positions for all levels (similar to insertion search)
        /// We need to find all nodes that point to nodeToDelete at each level
        vector<SkipListNode*> update(maxLevel + 1, nullptr);
        current = head;  // Start search from head

        // Traverse from highest level down to level 0
        for (int i = maxLevel; i >= 0; i--) {
            // Move forward while:
            // 1. There's a next node at this level, AND
            // 2. Next node is not the one we're deleting, AND
            // 3. nodeToDelete should NOT come before next node
            while (current->forward[i] != nullptr &&
                   current->forward[i] != nodeToDelete &&
                   !shouldInsertBefore(nodeToDelete, current->forward[i])) {
                current = current->forward[i];
            }
            // When loop exits, current is the node that points to nodeToDelete at level i
            update[i] = current;
        }

        /// Step 3: Verify we actually found the right node
        /// This is a sanity check - current->forward[0] should be nodeToDelete
        if (current->forward[0] != nodeToDelete) {
            // This shouldn't happen if our linear scan was correct
            // Could indicate a bug in the update[] finding logic
            return;
        }

        /// Step 4: Remove node from skip list at all levels where it exists
        /// For each level from 0 to nodeToDelete->level:
        ///   1. Check if update[i] actually points to nodeToDelete
        ///   2. If yes, make update[i] point to nodeToDelete->forward[i]
        ///   (This "bypasses" nodeToDelete in the linked list at level i)
        for (int i = 0; i <= nodeToDelete->level; i++) {
            // Some levels might not contain the node (if nodeLevel < i)
            // Or update[i] might not point to nodeToDelete due to skip list structure
            if (update[i]->forward[i] != nodeToDelete) {
                // This level doesn't have nodeToDelete, skip it
                continue;
            }
            // Bypass nodeToDelete at this level
            update[i]->forward[i] = nodeToDelete->forward[i];
        }

        // Step 5: Free the memory allocated for the deleted node
        delete nodeToDelete;
    }

    /// Returns the top N player IDs in descending score order
    /// Time Complexity: O(n) worst-case, but O(k) where k = min(n, N)
    /// Parameters:
    ///   n - Number of top players to retrieve
    /// Returns: Vector of player IDs of the top N players
    vector<int> getTopN(int n) override {
        vector<int> result;
        // Start from the first real node (after head sentinel)
        SkipListNode* current = head->forward[0];

        // Traverse the bottom level (level 0) which contains all nodes in sorted order
        // Stop when we've collected n players or reached end of list
        while (current != nullptr && result.size() < n) {
            result.push_back(current->playerID);  // Add player ID to result
            current = current->forward[0];        // Move to next node
        }

        return result;
    }

    /// Debug helper function: Prints the entire skip list structure
    /// Shows each level from highest to lowest with all nodes at that level
    void printList() {
        // Print from highest level down to level 0
        for (int i = maxLevel; i >= 0; i--) {
            cout << "Level " << i << ": ";
            SkipListNode* current = head->forward[i];  // Start from first node at this level

            // Traverse all nodes at this level
            while (current != nullptr) {
                cout << "[" << current->playerID << "," << current->score << "] ";
                current = current->forward[i];  // Move to next node at same level
            }
            cout << endl;
        }
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
// Problem: divide coins into two groups such that the sum difference is minimized
// Example:
// coins = {1, 11, 5, 6}
// Total sum = 1+11+5+6 = 23
// Goal: split into groups with sums as close as possible
// Closest split: {11,6} sum=17, {1,5} sum=6  → diff = 17-6 = 11? Actually let's trace carefully below

int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {

    // Edge case: no coins → difference is 0
    if (n <= 0 || coins.empty()) return 0;

    // Compute total sum of coins
    long long total = 0;
    for (int v : coins) total += v;
    // Example: total = 1+11+5+6 = 23

    // We want to get as close as possible to half of total
    long long half = total / 2; // 23/2 = 11 (integer division)

    // dp[j] = 1 if it's possible to make sum j using some coins
    vector<char> dp(half + 1, 0);
    dp[0] = 1; // sum 0 is always possible (take no coins)

    // =========================
    // Fill dp table
    // =========================
    for (int val : coins) {
        // We iterate backwards to ensure we use each coin at most once
        for (long long j = half; j >= val; --j) {
            // If sum (j-val) was achievable, then sum j is also achievable by adding val
            if (dp[j - val]) dp[j] = 1;

            /*
                Example tracing with coins = {1, 11, 5, 6}:

                Step 1: val = 1
                dp[1] = dp[1-1] = dp[0] = 1 → sum 1 is possible
                dp = [1,1,0,0,0,...,11]

                Step 2: val = 11
                Since half=11, j only = 11
                dp[11] = dp[11-11]=dp[0]=1 → sum 11 is possible

                Step 3: val = 5
                j=11: dp[11] |= dp[6] (currently 0) → no change
                j=10: dp[10] |= dp[5] (0) → no change
                j=5: dp[5] |= dp[0]=1 → dp[5]=1
                Now achievable sums <=11: 0,1,5,11

                Step 4: val=6
                j=11: dp[11] |= dp[5]=1 → dp[11]=1 (already 1)
                j=10: dp[10] |= dp[4]=0 → no change
                j=6: dp[6] |= dp[0]=1 → dp[6]=1

                Final achievable sums <=11: 0,1,5,6,11
            */
        }
    }

    // =========================
    // Find the best achievable sum ≤ half
    // =========================
    long long best = 0;
    for (long long j = half; j >= 0; --j) {
        if (dp[j]) { best = j; break; }
        /*
            For the example:
            dp[11]=1 → best=11
            So one group can sum to 11
        */
    }

    // The other group will have the remaining coins
    long long other = total - best; // 23-11=12

    // Absolute difference between the two groups
    long long diff = llabs(other - best); // |12-11|=1

    return (int)diff; // returns 1
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

// === Path existence (undirected graph) ===
// This function checks:
// "Is there ANY path from source to dest?"
//
// Example:
// n = 5
// edges = {{0,1}, {1,2}, {3,4}}
// source = 0, dest = 2
//
// Graph:
// 0 -- 1 -- 2      3 -- 4
//
// Expected answer: true

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {

    // =========================
    // Basic validation
    // =========================

    // No nodes → no path
    if (n <= 0) return false;

    // Invalid source or destination
    if (source < 0 || source >= n || dest < 0 || dest >= n)
        return false;

    // Same node → path exists trivially
    // Example: source=2, dest=2
    if (source == dest) return true;

    // =========================
    // Build adjacency list
    // =========================
    // adj[i] will store all nodes connected to i
    vector<vector<int>> adj(n);

    for (auto &e : edges) {
        if (e.size() >= 2) {
            int u = e[0], v = e[1];

            // Ignore invalid edges
            if (u >= 0 && u < n && v >= 0 && v < n) {
                // Undirected graph → add both directions
                adj[u].push_back(v);
                adj[v].push_back(u);
            }
        }
    }

    /*
        After building adj for the example:

        adj[0] = {1}
        adj[1] = {0, 2}
        adj[2] = {1}
        adj[3] = {4}
        adj[4] = {3}
    */

    // =========================
    // BFS setup
    // =========================

    // seen[i] = 1 means "we already visited node i"
    vector<char> seen(n, 0);

    // Queue for BFS
    queue<int> q;

    // Start BFS from source
    q.push(source);
    seen[source] = 1;

    /*
        Initially:
        queue = [0]
        seen  = [1,0,0,0,0]
    */

    // =========================
    // Breadth-First Search (BFS)
    // =========================
    while (!q.empty()) {

        // Take the front node
        int u = q.front();
        q.pop();

        /*
            First iteration:
            u = 0
            queue = []
        */

        // Visit all neighbors of u
        for (int v : adj[u]) {

            // If we haven't visited v yet
            if (!seen[v]) {

                // If v is destination → path found!
                if (v == dest)
                    return true;

                // Mark v as visited
                seen[v] = 1;

                // Add v to queue for further exploration
                q.push(v);

                /*
                    From u = 0:
                    v = 1
                    queue = [1]
                    seen = [1,1,0,0,0]
                */
            }
        }
    }

    // If BFS finishes and we never reached dest
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

// === All-Pairs Shortest Path + sum -> binary ===
// Example:
// n = 3
// roads = {{0,1,1}, {1,2,2}}
//
// Graph:
// 0 --(1)-- 1 --(2)-- 2
//
// Shortest distances:
// 0-1 = 1
// 1-2 = 2
// 0-2 = 3  (0 -> 1 -> 2)
//
// Sum = 1 + 2 + 3 = 6
// Binary = "110"

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {

    // If there are no nodes, no distances exist
    if (n <= 0) return string("0");

    // A very large number representing "no path"
    const long long INF = (1LL << 60);

    // dist[i][j] = shortest distance from node i to node j
    // Initially, we assume everything is unreachable (INF)
    vector<vector<long long>> dist(n, vector<long long>(n, INF));

    // Distance from a node to itself is always 0
    // Example: dist[0][0] = 0, dist[1][1] = 0, dist[2][2] = 0
    for (int i = 0; i < n; ++i)
        dist[i][i] = 0;

    // Read roads: each road is {u, v, length}
    // Example road {0,1,1} means:
    // dist[0][1] = 1 and dist[1][0] = 1
    for (auto &r : roads) {
        if (r.size() < 3) continue;

        int u = r[0], v = r[1];
        long long w = r[2];

        // Ignore invalid nodes
        if (u < 0 || u >= n || v < 0 || v >= n) continue;

        // If multiple roads exist between same nodes,
        // keep the smallest distance
        dist[u][v] = min(dist[u][v], w);
        dist[v][u] = min(dist[v][u], w);
    }

    /*
        After reading roads for the example:

            0   1   2
        0 [ 0   1  INF ]
        1 [ 1   0   2  ]
        2 [ INF 2   0  ]
    */

    // =========================
    // Find shortest paths using intermediate nodes
    // =========================
    // k = intermediate node
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {

            // If i cannot reach k, skip
            if (dist[i][k] == INF) continue;

            for (int j = 0; j < n; ++j) {

                // If k cannot reach j, skip
                if (dist[k][j] == INF) continue;

                // Try path: i -> k -> j
                long long nd = dist[i][k] + dist[k][j];

                /*
                    Example when k = 1:
                    i = 0, j = 2

                    dist[0][1] = 1
                    dist[1][2] = 2

                    nd = 1 + 2 = 3

                    This discovers path:
                    0 --(1)-- 1 --(2)-- 2
                */

                // If going through k is shorter, update
                if (nd < dist[i][j])
                    dist[i][j] = nd;
            }
        }
    }

    /*
        Final dist table for the example:

            0   1   2
        0 [ 0   1   3 ]
        1 [ 1   0   2 ]
        2 [ 3   2   0 ]
    */

    // =========================
    // Sum shortest distances (only once per pair)
    // =========================
    __int128 total = 0;  // Big type to avoid overflow

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {

            // Only count reachable pairs
            if (dist[i][j] != INF)
                total += dist[i][j];

            /*
                Example additions:
                i=0, j=1 -> +1
                i=0, j=2 -> +3
                i=1, j=2 -> +2

                total = 6
            */
        }
    }

    // If total distance is 0, binary is "0"
    if (total == 0) return string("0");

    // =========================
    // Convert total to binary
    // =========================
    string bits;
    __int128 t = total;

    // Example: total = 6
    // 6 % 2 = 0
    // 3 % 2 = 1
    // 1 % 2 = 1
    // Binary = 110
    while (t > 0) {
        bits.push_back((t & 1) ? '1' : '0');
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