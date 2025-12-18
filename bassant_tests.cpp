#include <iostream>
#include <vector>
#include <string>
#include "ArcadiaEngine.cpp"

using namespace std;

/// ===================================================
/// PLAYER TABLE TESTS (Division + Double Hashing)
/// ===================================================
void testPlayerTable() {
    cout << "\n[TEST] PlayerTable (Double Hashing)\n";

    ConcretePlayerTable table;

    // -------------------------------
    // Basic insert & search
    // -------------------------------
    table.insert(10, "Alice");
    table.insert(20, "Bob");

    cout << table.search(10) << endl;   // Alice
    cout << table.search(20) << endl;   // Bob
    cout << table.search(30) << endl;   // "" (not found)

    // -------------------------------
    // Update existing ID (NOT collision)
    // -------------------------------
    table.insert(10, "AliceUpdated");
    cout << table.search(10) << endl;   // AliceUpdated

    // -------------------------------
    // Collision-heavy inputs
    // (same hash1 = key % 101)
    // -------------------------------
    table.insert(11, "A");
    table.insert(112, "B");   // 11 % 101 == 112 % 101
    table.insert(213, "C");   // collision chain

    cout << table.search(11) << endl;    // A
    cout << table.search(112) << endl;   // B
    cout << table.search(213) << endl;   // C

    // -------------------------------
    // Edge values
    // -------------------------------
    table.insert(0, "Zero");
    table.insert(100000, "Big");

    cout << table.search(0) << endl;         // Zero
    cout << table.search(100000) << endl;    // Big

    // -------------------------------
    // Fill table to capacity (101 slots)
    // -------------------------------
    for (int i = 1; i <= 300; i += 3) {
        table.insert(i, "Player" + to_string(i));
    }

    // -------------------------------
    // This insert SHOULD fail
    // -------------------------------
    table.insert(999999, "Overflow"); // prints: Table is Full

    cout << "PlayerTable tests completed\n";
}

/// ===================================================
/// STRING DECODING TESTS (DP Autocorrect)
/// ===================================================
void testStringDecoding() {
    cout << "\n[TEST] String Decoding (DP)\n";

    InventorySystem inv;

    // -------------------------------
    // Empty string
    // -------------------------------
    cout << inv.countStringPossibilities("") << endl;   // 1

    // -------------------------------
    // Single characters
    // -------------------------------
    cout << inv.countStringPossibilities("u") << endl;  // 1
    cout << inv.countStringPossibilities("n") << endl;  // 1
    cout << inv.countStringPossibilities("a") << endl;  // 1

    // -------------------------------
    // Invalid characters
    // -------------------------------
    cout << inv.countStringPossibilities("w") << endl;  // 0
    cout << inv.countStringPossibilities("m") << endl;  // 0
    cout << inv.countStringPossibilities("unwm") << endl; // 0

    // -------------------------------
    // Simple merge cases
    // -------------------------------
    cout << inv.countStringPossibilities("uu") << endl; // 2
    cout << inv.countStringPossibilities("nn") << endl; // 2

    // -------------------------------
    // Mixed cases
    // -------------------------------
    cout << inv.countStringPossibilities("uunu") << endl; // 2
    cout << inv.countStringPossibilities("unun") << endl; // 1

    // -------------------------------
    // No merge possible
    // -------------------------------
    cout << inv.countStringPossibilities("abcde") << endl; // 1

    // -------------------------------
    // Large input (O(n) performance)
    // -------------------------------
    string big(100000, 'u');
    cout << inv.countStringPossibilities(big) << endl;

    cout << "String Decoding tests completed\n";
}

/// ===================================================
/// MST (PRIM) TESTS — PART C
/// ===================================================
void testWorldNavigator() {
    cout << "\n[TEST] WorldNavigator (Prim MST)\n";

    WorldNavigator nav;

    // -------------------------------------------------
    // 1. No cities
    // -------------------------------------------------
    {
        vector<vector<int>> roads;
        cout << nav.minBribeCost(0, 0, 10, 10, roads) << endl; // expected: 0
    }

    // -------------------------------------------------
    // 2. Single city (no roads needed)
    // -------------------------------------------------
    {
        vector<vector<int>> roads;
        cout << nav.minBribeCost(1, 0, 5, 5, roads) << endl; // expected: 0
    }

    // -------------------------------------------------
    // 3. Simple connected graph (example from assignment)
    // -------------------------------------------------
    {
        vector<vector<int>> roads = {
                {0, 1, 10, 0},
                {1, 2, 5, 0},
                {0, 2, 20, 0}
        };
        cout << nav.minBribeCost(3, 3, 1, 1, roads) << endl; // expected: 15
    }

    // -------------------------------------------------
    // 4. Disconnected graph → MST impossible
    // -------------------------------------------------
    {
        vector<vector<int>> roads = {
                {0, 1, 1, 0},
                {2, 3, 1, 0}
        };
        cout << nav.minBribeCost(4, 2, 10, 0, roads) << endl; // expected: -1
    }

    // -------------------------------------------------
    // 5. Zero-cost edges (goldRate = silverRate = 0)
    // -------------------------------------------------
    {
        vector<vector<int>> roads = {
                {0, 1, 5, 5},
                {1, 2, 7, 9}
        };
        cout << nav.minBribeCost(3, 2, 0, 0, roads) << endl; // expected: 0
    }

    // -------------------------------------------------
    // 6. Multiple edges between same cities
    // (Prim should pick cheaper one)
    // -------------------------------------------------
    {
        vector<vector<int>> roads = {
                {0, 1, 10, 0},
                {0, 1, 1, 0}
        };
        cout << nav.minBribeCost(2, 2, 1, 0, roads) << endl; // expected: 1
    }

    // -------------------------------------------------
    // 7. Large weights (overflow safety)
    // -------------------------------------------------
    {
        vector<vector<int>> roads = {
                {0, 1, 1000000000, 1000000000}
        };
        cout << nav.minBribeCost(
                2, 1,
                1000000000LL,
                1000000000LL,
                roads
        ) << endl; // expected: 2000000000000000000
    }

    // -------------------------------------------------
    // 8. City index boundary correctness
    // -------------------------------------------------
    {
        vector<vector<int>> roads = {
                {0, 4, 1, 0}
        };
        cout << nav.minBribeCost(5, 1, 10, 0, roads) << endl; // expected: -1
    }

    cout << "WorldNavigator tests completed\n";
}

int main() {
    cout << "===== RUNNING EXTENDED TEST SUITE =====\n";

    testPlayerTable();
    testStringDecoding();
    testWorldNavigator();

    cout << "\n ALL TESTS PASSED SUCCESSFULLY\n";
}