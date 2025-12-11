#include "ArcadiaEngine.cpp"
int main() {
    ///test for hash table(PartA1)
    ConcretePlayerTable table;
    cout << "test for hash table(PartA1)" << endl;
    // Insert 10 players and display after each insert
    table.insert(192, "Alice");
    table.insert(7, "Bob");
    table.insert(25, "Charlie");
    table.insert(3, "David");
    table.insert(18, "Eve");
    table.insert(30, "Frank");
    table.insert(21, "Grace");
    table.insert(14, "Hannah");
    table.insert(977, "Ivy");
    table.insert(5, "Jack");
    table.display();

    // Search tests
    cout << "\n--- Search Tests ---\n";
    cout << "Search 205: " << table.search(205) << endl; // Bob (if 205 maps to same slot)
    cout << "Search 500: " << table.search(500) << endl; // ""
    cout << "Search 222: " << table.search(222) << endl; // Kara (if previously inserted)

    ///test string decoding (PartB3)
    InventorySystem inv;
    cout << "\ntest string decoding (PartB3)" << endl;
    cout << "for uu : " << inv.countStringPossibilities("uu") << endl;     // 2
    cout << "for uunn : " << inv.countStringPossibilities("uunn") << endl;   // 4
    cout << "for unnuunn : "<< inv.countStringPossibilities("unnuunn") << endl; // 8
    cout << "for ununnuunnu : " <<inv.countStringPossibilities("ununnuunnu") << endl; // 8
    cout << "for nnununnuunnu : " <<inv.countStringPossibilities("nnununnuunnu") << endl; // 16

    ///MST Prim's(PartC2)
    WorldNavigator navigator;
    cout << "\ntest MST Prim's(PartC2)" << endl;
    int n = 5;       // 5 cities
    int m = 8;       // total number of roads
    long long goldRate = 1;
    long long silverRate = 1;

    // roadData[i] = {u, v, goldCost, silverCost}
    // Each city has 2–3 edges to other cities
    vector<vector<int>> roadData = {
            {0, 1, 10, 2},  // city 0 - 1
            {0, 2, 5, 1},   // city 0 - 2
            {1, 2, 3, 0},   // city 1 - 2
            {1, 3, 8, 2},   // city 1 - 3
            {2, 3, 4, 1},   // city 2 - 3
            {2, 4, 7, 3},   // city 2 - 4
            {3, 4, 6, 2},   // city 3 - 4
            {0, 4, 12, 5}   // city 0 - 4
    };

    long long cost = navigator.minBribeCost(n, m, goldRate, silverRate, roadData);
    cout << "Minimum bribe cost for 5 cities: " << cost << endl;
}

