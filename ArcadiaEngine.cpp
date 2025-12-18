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

///PlayerTable (Double Hashing)
class ConcretePlayerTable : public PlayerTable {
private:
    /// Structure to represent each player entry in the hash table
    struct Player {
        int key;        // Shirt number (unique identifier for player)
        string value;   // Player's name
        bool used;      // Marks whether this slot is occupied in the table

        /// Default constructor initializes an empty slot
        Player() : key(-1), used(false) {}
    };

    /// Fixed-size hash table array (size 101 as required)
    Player table[101];

    /// PRIMARY HASH FUNCTION
    int hash1(int key) {
        return key % 101;
    }

    /// SECOND HASH FUNCTION (FOR DOUBLE HASHING)
    /// Ensures probing in case of collision
    int hash2(int key) {
        return 97 - (key % 97); // Prime < 101 to reduce clustering
    }

public:
    /// INSERT FUNCTION USING DOUBLE HASHING
    /// Inserts a playerID-name pair into the hash table
    void insert(int playerID, string name) override {
        int h1 = hash1(playerID); // Primary hash
        int h2 = hash2(playerID); // Secondary hash for collision resolution

        /// Try up to 101 slots (entire table)
        for (int i = 0; i < 101; i++) {
            int index = (h1 + i * h2) % 101; // Double hashing formula

            /// Case 1: Key already exists → update value
            if (table[index].used && table[index].key == playerID) {
                table[index].value = name; // update name
                return;
            }

            /// Case 2: Empty slot → insert new key
            if (!table[index].used) {
                table[index].key = playerID;
                table[index].value = name;
                table[index].used = true;
                return;
            }
        }

        /// If table is completely full, print error
        cout << "Table is Full\n";
    }

    /// SEARCH FUNCTION USING DOUBLE HASHING
    /// Looks for a player by shirt number
    string search(int playerID) override {
        int h1 = hash1(playerID); // Primary hash
        int h2 = hash2(playerID); // Secondary hash for probing

        /// Try up to 101 slots
        for (int i = 0; i < 101; i++) {
            int index = (h1 + i * h2) % 101;

            if (!table[index].used) // Empty slot → player not found
                return "";

            if (table[index].key == playerID) // Found the player
                return table[index].value;
        }

        return ""; // Not found after full probing
    }

    /// DISPLAY FUNCTION
    /// For testing/debugging: prints table contents
    void display() {
        cout << "\n--- Player Hash Table (Double Hashing) ---\n";
        cout << "Index\tPlayerID\tName\n";
        cout << "----------------------------------------\n";

        for (int i = 0; i < 101; i++) {
            if (table[i].used) {
                cout << i << "\t" << table[i].key
                     << "\t\t" << table[i].value << endl;
            } else {
                cout << i << "\tEMPTY\n"; // Slot is empty
            }
        }
        cout << "----------------------------------------\n\n";
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
// Problem:
// Divide the coins into two groups such that the absolute
// difference between the sums of the two groups is minimized.
//
// Core Idea (DP):
// - Let total be the sum of all coins.
// - If one group has sum S, the other has sum (total - S).
// - The difference is |total - 2*S|.
// - To minimize this difference, we want S as close as possible
//   to total/2.
// - This reduces to a 0/1 Subset Sum problem.

int InventorySystem::optimizeLootSplit(int n, vector<int>& coins) {

    // If there are no coins, the difference is zero
    if (n <= 0 || coins.empty()) return 0;

    // Calculate the total sum of all coins
    long long total = 0;
    for (int v : coins) {
        total += v;
    }

    // We only need to consider sums up to total/2
    // Any sum larger than that would create a larger difference
    long long half = total / 2;

    // dp[j] = true if it is possible to form a subset of coins
    // whose sum is exactly j
    vector<char> dp(half + 1, 0);

    // Base case:
    // A sum of 0 is always achievable by selecting no coins
    dp[0] = 1;

    // Build the DP table
    // Each coin can be used at most once (0/1 property)
    for (int val : coins) {

        // Iterate backwards to avoid reusing the same coin
        // in the same iteration
        for (long long j = half; j >= val; --j) {

            // If we could previously make sum (j - val),
            // then by adding this coin we can now make sum j
            if (dp[j - val]) {
                dp[j] = 1;
            }
        }
    }

    // Find the largest achievable sum that does not exceed half
    // This gives the closest possible partition to total/2
    long long best = 0;
    for (long long j = half; j >= 0; --j) {
        if (dp[j]) {
            best = j;
            break;
        }
    }

    // The second group gets the remaining sum
    long long other = total - best;

    // The minimum difference between the two groups
    long long diff = llabs(other - best);

    return (int)diff;
}

int InventorySystem::maximizeCarryValue(int capacity, vector<pair<int, int>>& items) {
    // TODO: Implement 0/1 Knapsack using DP
    // items = {weight, value} pairs
    // Return maximum value achievable within capacity
    return 0;
}

/// STRING DECODING USING DYNAMIC PROGRAMMING
// Counts all valid sequences of a string where "uu" and "nn" can optionally merge
long long InventorySystem::countStringPossibilities(string s) {
    /// MOD to prevent integer overflow for very large numbers of combinations
    const int MOD = 1e9 + 7;
    int n = s.size();

    /// dp[i] stores the number of ways to decode first i characters of string
    vector<long long> dp(n + 1, 0);
    dp[0] = 1;  // Base case: empty string has exactly 1 way

    /// Iterate through characters of the string
    for (int i = 1; i <= n; i++) {

        /// Single-character contribution: each character can stand alone as a valid sequence
        dp[i] = dp[i - 1] % MOD;

        /// Two-character merge check: only possible if at least 2 characters
        if (i >= 2) {
            /// Take the last 2 characters to see if they form a valid merge pair
            string pair = s.substr(i - 2, 2);

            /// Merge allowed only for "uu" or "nn"
            /// If valid, add the number of sequences from dp[i-2]
            if (pair == "uu" || pair == "nn") {
                dp[i] = (dp[i] + dp[i - 2]) % MOD;  // Merge update
            }
        }

        /**
        Example for s = "uunu" (n = 4):
        dp array evolution:
        i=0: dp[0] = 1 (empty string)
        i=1: dp[1] = dp[0] = 1   // "u"
        i=2: dp[2] = dp[1] + dp[0] = 1 + 1 = 2  // "uu" can merge
        i=3: dp[3] = dp[2] = 2   // "uun", no merge possible
        i=4: dp[4] = dp[3] = 2   // "uunu", last 2 = "nu", no merge

        Valid sequences for "uunu":
        1) "u u n u"  (no merges)
        2) "uu n u"   (first two 'u's merged)
        **/
    }

    /// Return total number of valid sequences for entire string
    return dp[n];
}

// =========================================================
// PART C: WORLD NAVIGATOR (Graphs)
// =========================================================

// === Path Existence (Undirected Graph) ===
//
// Problem:
// Determine whether there exists at least one path
// from a given source node to a destination node
// in an undirected graph.
//
// Approach:
// - Represent the graph using an adjacency list.
// - Perform Breadth-First Search (BFS) starting from the source.
// - If the destination is reached during BFS, a path exists.

bool WorldNavigator::pathExists(int n, vector<vector<int>>& edges, int source, int dest) {

    // =========================
    // Input validation
    // =========================

    // If the graph has no nodes, no path can exist
    if (n <= 0) return false;

    // Check that source and destination are valid node indices
    if (source < 0 || source >= n || dest < 0 || dest >= n)
        return false;

    // If source and destination are the same node,
    // a path exists trivially
    if (source == dest) return true;

    // =========================
    // Build adjacency list
    // =========================
    // adj[i] contains all nodes directly connected to node i
    vector<vector<int>> adj(n);

    // Each edge connects two nodes bidirectionally
    for (auto &e : edges) {
        if (e.size() >= 2) {
            int u = e[0];
            int v = e[1];

            // Ignore edges with invalid node indices
            if (u >= 0 && u < n && v >= 0 && v < n) {
                adj[u].push_back(v);
                adj[v].push_back(u);
            }
        }
    }

    // =========================
    // Breadth-First Search (BFS)
    // =========================

    // seen[i] indicates whether node i has already been visited
    vector<char> seen(n, 0);

    // Queue used to explore the graph level by level
    queue<int> q;

    // Start BFS from the source node
    q.push(source);
    seen[source] = 1;

    // Explore all reachable nodes
    while (!q.empty()) {

        // Get the next node to process
        int u = q.front();
        q.pop();

        // Visit all neighbors of the current node
        for (int v : adj[u]) {

            // Process only unvisited nodes
            if (!seen[v]) {

                // If destination is reached, a path exists
                if (v == dest)
                    return true;

                // Mark the node as visited
                seen[v] = 1;

                // Add the node to the queue for further exploration
                q.push(v);
            }
        }
    }

    // If BFS completes without reaching destination,
    // then no path exists
    return false;
}

/// MINIMUM SPANNING TREE USING PRIM'S ALGORITHM
/// Computes the minimum total cost to connect all cities
/// goldRate, silverRate: cost multipliers for each road
/// roadData[i] = {u, v, goldCost, silverCost} for each road
long long WorldNavigator::minBribeCost(int n, int m, long long goldRate, long long silverRate,
                                       vector<vector<int>>& roadData) {
    /// --- Step 1: Build adjacency list
    /// Each city (node) has a vector of pairs {neighbor, cost}
    /// This allows fast access to all connected roads from a city
    vector<vector<pair<int,long long>>> adj(n);

    for (int i = 0; i < m; i++) {
        int u = roadData[i][0];
        int v = roadData[i][1];
        /// Compute total cost using gold and silver rates
        long long c = roadData[i][2] * goldRate + roadData[i][3] * silverRate;

        /// Undirected graph → add connection in both directions
        adj[u].push_back({v, c});
        adj[v].push_back({u, c});
    }

    /// --- Step 2: Initialize visited array for cities
    /// visited[i] = true if city i is already included in the MST
    vector<bool> visited(n, false);

    /// --- Step 3: Priority queue (min-heap) to select next minimum-cost edge
    /// Each element: {cost to reach node, node index}
    /// greater<pair<...>> ensures the smallest cost edge is at the top
    priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<pair<long long,int>>> pq;

    /// Start MST from node 0 with cost 0
    pq.push({0, 0});
    long long totalCost = 0;  // Total MST cost accumulator
    int visitedCount = 0;     // Number of cities added to MST

    /// --- Step 4: Prim's main loop
    /// Continue until all cities are visited or no edges left
    while (!pq.empty() && visitedCount < n) {
        /// Pick the smallest-cost edge from the heap
        auto [c, u] = pq.top();
        pq.pop();

        /// Skip if city already included in MST
        if (visited[u]) continue;

        /// Include city u in MST
        visited[u] = true;
        totalCost += c;  // Add edge cost to total
        visitedCount++;  // Increment count of cities in MST

        /// Push all edges from newly included city u to the heap
        for (auto &[v, w] : adj[u]) {
            if (!visited[v]) {
                pq.push({w, v});
            }
        }
    }

    /// --- Step 5: Check if all cities are connected
    /// If not all cities were visited, the graph is disconnected → return -1
    if (visitedCount != n) return -1;

    /// --- Step 6: Return total MST cost
    return totalCost;

    /**
    EXAMPLE:

    n = 3 cities (0,1,2)
    m = 3 roads: roadData = {
        {0, 1, 1, 0},  // cost = 1*goldRate + 0*silverRate
        {1, 2, 1, 0},  // cost = 1*goldRate + 0*silverRate
        {0, 2, 10, 0}  // cost = 10*goldRate + 0*silverRate
    }
    goldRate = 10
    silverRate = 0

    Step-by-step execution:

    1. Build adjacency list (compute actual costs):
       adj[0] = {(1,10), (2,100)}
       adj[1] = {(0,10), (2,10)}
       adj[2] = {(1,10), (0,100)}

    2. Start MST at city 0, priority queue pq = {(0,0)}

    3. Pop (0,0) → visit city 0, totalCost = 0
       Push edges from 0 → pq = {(10,1),(100,2)}

    4. Pop (10,1) → visit city 1, totalCost = 10
       Push edges from 1 → pq = {(10,2),(100,2)}
       (edge to 0 ignored since 0 is already visited)

    5. Pop (10,2) → visit city 2, totalCost = 20
       Push edges from 2 → pq = {(100,2)}
       (edges to visited cities ignored)

    6. All cities visited → MST complete
       Return totalCost = 20

    Notes:
    - Only the smallest-cost edges that connect unvisited nodes are chosen.
    */
}


// === All-Pairs Shortest Path + Sum in Binary ===
//
// Problem:
// Given an undirected weighted graph, compute the shortest
// distance between every pair of nodes.
// Then sum the shortest distances for all unique pairs (i < j)
// and return the result as a binary string.
//
// Approach:
// - Use Floyd–Warshall to compute all-pairs shortest paths.
// - Sum distances for unique reachable node pairs.
// - Convert the sum to binary representation.

string WorldNavigator::sumMinDistancesBinary(int n, vector<vector<int>>& roads) {

    // If there are no nodes, there are no distances to sum
    if (n <= 0) return string("0");

    // A very large value representing "infinite distance"
    const long long INF = (1LL << 60);

    // dist[i][j] stores the shortest distance from node i to node j
    // Initially, all distances are set to INF (unreachable)
    vector<vector<long long>> dist(n, vector<long long>(n, INF));

    // Distance from any node to itself is zero
    for (int i = 0; i < n; ++i) {
        dist[i][i] = 0;
    }

    // Populate the distance matrix using the given roads
    // Each road connects two nodes with a given weight
    for (auto &r : roads) {
        if (r.size() < 3) continue;

        int u = r[0];
        int v = r[1];
        long long w = r[2];

        // Ignore invalid node indices
        if (u < 0 || u >= n || v < 0 || v >= n) continue;

        // Since the graph is undirected, update both directions
        // Keep the smallest weight if multiple edges exist
        dist[u][v] = min(dist[u][v], w);
    }

    // =========================
    // Floyd–Warshall Algorithm
    // =========================
    // Try all nodes as intermediate points to improve paths
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {

            // If node i cannot reach k, skip
            if (dist[i][k] == INF) continue;

            for (int j = 0; j < n; ++j) {

                // If k cannot reach j, skip
                if (dist[k][j] == INF) continue;

                // Check whether the path i -> k -> j is shorter
                long long newDist = dist[i][k] + dist[k][j];

                if (newDist < dist[i][j]) {
                    dist[i][j] = newDist;
                }
            }
        }
    }

    // =========================
    // Sum shortest distances
    // =========================
    // Use a larger integer type to avoid overflow
    __int128 total = 0;

    // Sum distances only for unique pairs (i < j)
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {

            // Only include pairs that are reachable
            if (dist[i][j] != INF) {
                total += dist[i][j];
            }
        }
    }

    // If the sum is zero, return binary zero
    if (total == 0) return string("0");

    // =========================
    // Convert sum to binary
    // =========================
    string binary;
    __int128 value = total;

    // Repeatedly extract least significant bits
    while (value > 0) {
        binary.push_back((value & 1) ? '1' : '0');
        value >>= 1;
    }

    // Reverse to obtain correct binary order
    reverse(binary.begin(), binary.end());

    return binary;
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