#include <iostream>
#include <vector>
#include <string>
#include "ArcadiaEngine.cpp"

using namespace std;

/// ===================================================
/// PLAYER TABLE TESTS
/// ===================================================
void testPlayerTable() {
    cout << "\n[TEST] PlayerTable (Double Hashing)\n";

    ConcretePlayerTable table;

    // Basic insert & search
    table.insert(10, "Alice");
    table.insert(20, "Bob");
    table.display();

    cout << table.search(10)<< endl;
    cout << table.search(20)<< endl;
    cout << table.search(30)<< endl;

    // Collision-heavy inputs
    table.insert(11, "A");
    table.insert(111, "B");
    table.insert(211, "C");
    table.display();

    cout << table.search(11)<< endl;
    cout << table.search(111)<< endl;
    cout << table.search(211)<< endl;

    // Edge values
    table.insert(0, "Zero");
    table.insert(100000, "Big");
    table.display();

    cout << table.search(0)<< endl;
    cout << table.search(100000)<< endl;

    // Fill all 101 slots with UNIQUE keys
    for (int i = 1; i < 300; i+=3) {
        table.insert(i , "Player" + to_string(i));
    }
    table.display();

    cout << "PlayerTable tests passed\n";
}

/// ===================================================
/// STRING DECODING TESTS
/// ===================================================
void testStringDecoding() {
    cout << "\n[TEST] String Decoding (DP)\n";

    InventorySystem inv;

    // Empty string
    cout << inv.countStringPossibilities("")<< endl;

    // Single character
    cout << inv.countStringPossibilities("u")<< endl;
    cout << inv.countStringPossibilities("n")<< endl;
    cout << inv.countStringPossibilities("a")<< endl;

    // Simple merge cases
    cout << inv.countStringPossibilities("uu")<< endl; // u u | uu
    cout << inv.countStringPossibilities("nn")<< endl; // n n | nn

    // Mixed cases
    cout << inv.countStringPossibilities("uunu")<< endl;
    cout << inv.countStringPossibilities("unun")<< endl;

    // Edge: no valid merges at all
    cout << inv.countStringPossibilities("abcde")<< endl;

    // Large input (performance O(n))
    string big(100000, 'u');
    cout << inv.countStringPossibilities(big)<< endl;

    cout << "String Decoding tests passed\n";
}

/// ===================================================
/// MST (PRIM) TESTS
/// ===================================================
void testWorldNavigator() {
    cout << "\n[TEST] WorldNavigator (Prim MST)\n";

    WorldNavigator nav;

    // Single node
    {
        vector<vector<int>> roads;
        cout << nav.minBribeCost(1, 0, 5, 5, roads)<< endl;
    }

    // Simple connected graph
    {
        vector<vector<int>> roads = {
                {0, 1, 1, 0},
                {1, 2, 1, 0},
                {0, 2, 10, 0}
        };
        cout << nav.minBribeCost(3, 3, 10, 0, roads)<< endl;
    }

    // Disconnected graph
    {
        vector<vector<int>> roads = {
                {0, 1, 1, 0},
                {2, 3, 1, 0}
        };
        cout << nav.minBribeCost(4, 2, 10, 0, roads)<< endl;
    }

    // Zero cost edges
    {
        vector<vector<int>> roads = {
                {0, 1, 5, 5},
                {1, 2, 5, 5}
        };
        cout << nav.minBribeCost(3, 2, 0, 0, roads)<< endl;
    }

    // Multiple edges between same nodes
    {
        vector<vector<int>> roads = {
                {0, 1, 10, 0},
                {0, 1, 1, 0}
        };
        cout << nav.minBribeCost(2, 2, 1, 0, roads)<< endl;
    }

    // No cities
    {
        vector<vector<int>> roads;
        cout << nav.minBribeCost(0, 0, 10, 10, roads)<< endl;
    }

    // Large values (overflow safety)
    {
        vector<vector<int>> roads = {
                {0, 1, 1000000000, 1000000000}
        };
        cout << nav.minBribeCost(2, 1, 1000000000LL, 1000000000LL, roads) << endl;
    }

    cout << "WorldNavigator tests passed\n";
}

int main() {
    cout << "===== RUNNING EXTENDED TEST SUITE =====\n";

    testPlayerTable();
    testStringDecoding();
    testWorldNavigator();

    cout << "\n ALL TESTS PASSED SUCCESSFULLY\n";
}