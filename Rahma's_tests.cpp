#include <bits/stdc++.h>
#include "ArcadiaEngine.h"
#include "ArcadiaEngine.cpp"
using namespace std;

// Utility for comparing vectors (order matters)
bool vecEquals(const vector<int>& a, const vector<int>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) if (a[i] != b[i]) return false;
    return true;
}

// Pretty print vector<int>
string vecToStr(const vector<int>& v) {
    ostringstream oss;
    oss << "{";
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) oss << ", ";
        oss << v[i];
    }
    oss << "}";
    return oss.str();
}

// Test runner helpers
int passed = 0, failed = 0;
void assertEq(const string& name, const vector<int>& actual, const vector<int>& expected) {
    bool ok = vecEquals(actual, expected);
    cout << left << setw(40) << name << " | Actual: " << setw(20) << vecToStr(actual)
         << " Expected: " << setw(20) << vecToStr(expected)
         << " -> " << (ok ? "PASS" : "FAIL") << "\n";
    if (ok) ++passed; else ++failed;
}
void assertEq(const string& name, int actual, int expected) {
    bool ok = (actual == expected);
    cout << left << setw(40) << name << " | Actual: " << setw(6) << actual
         << " Expected: " << setw(6) << expected
         << " -> " << (ok ? "PASS" : "FAIL") << "\n";
    if (ok) ++passed; else ++failed;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "========================================\n";
    cout << " PART C — ConcreteLeaderboard (Skip List)\n";
    cout << "========================================\n\n";

    // ---------- Leaderboard Tests ----------
    {   // Test LB-1: Empty leaderboard -> getTopN(5) == {}
        ConcreteLeaderboard lb;
        vector<int> expected = {};
        assertEq("LB-1 Empty getTopN(5)", lb.getTopN(5), expected);
    }

    {   // Test LB-2: Single player
        ConcreteLeaderboard lb;
        lb.addScore(1, 100);
        vector<int> expected = {1};
        assertEq("LB-2 Single player top1", lb.getTopN(1), expected); // Expected: {1}
    }

    {   // Test LB-3: Different scores ordering
        ConcreteLeaderboard lb;
        lb.addScore(1, 100);
        lb.addScore(2, 300);
        lb.addScore(3, 200);
        vector<int> expected = {2,3,1}; // 300,200,100
        assertEq("LB-3 Different scores top3", lb.getTopN(3), expected);
    }

    {   // Test LB-4: Tie-break by playerID (ascending)
        ConcreteLeaderboard lb;
        lb.addScore(5, 200);
        lb.addScore(2, 200);
        lb.addScore(9, 200);
        vector<int> expected = {2,5,9}; // same score -> IDs ascending
        assertEq("LB-4 Tie-break by ID", lb.getTopN(3), expected);
    }

    {   // Test LB-5: Mixed scores + ties
        ConcreteLeaderboard lb;
        lb.addScore(3,150);
        lb.addScore(1,200);
        lb.addScore(2,200);
        lb.addScore(5,120);
        lb.addScore(4,150);
        vector<int> expected = {1,2,3,4,5}; // 200,200,150,150,120 (IDs tie-order)
        assertEq("LB-5 Mixed scores + ties", lb.getTopN(5), expected);
    }

    {   // Test LB-6: Cumulative addScore (update)
        ConcreteLeaderboard lb;
        lb.addScore(1,50);
        lb.addScore(2,100);
        lb.addScore(1,60); // now player1 = 110
        vector<int> expected = {1,2}; // 110,100
        assertEq("LB-6 Cumulative addScore", lb.getTopN(2), expected);
    }

    {   // Test LB-7: Remove player
        ConcreteLeaderboard lb;
        lb.addScore(1,50);
        lb.addScore(2,100);
        lb.removePlayer(1);
        vector<int> expected = {2}; // only 2 remains
        assertEq("LB-7 Remove existing", lb.getTopN(2), expected);
    }

    {   // Test LB-8: Remove non-existent (should be no-op)
        ConcreteLeaderboard lb;
        lb.addScore(10,10);
        lb.removePlayer(9999); // no crash
        vector<int> expected = {10};
        assertEq("LB-8 Remove non-existent", lb.getTopN(1), expected);
    }

    {   // Test LB-9: Large player IDs with same score
        ConcreteLeaderboard lb;
        lb.addScore(1000000, 50);
        lb.addScore(999999, 50);
        vector<int> expected = {999999, 1000000};
        assertEq("LB-9 Large IDs tie-score", lb.getTopN(2), expected);
    }

    {   // Test LB-10: All same score -> IDs ascending
        ConcreteLeaderboard lb;
        for (int i = 10; i >= 1; --i) lb.addScore(i, 500);
        vector<int> expected;
        for (int i = 1; i <= 10; ++i) expected.push_back(i);
        assertEq("LB-10 All same score sorted by ID", lb.getTopN(10), expected);
    }

    {   // Test LB-11: Top-K where K < N
        ConcreteLeaderboard lb;
        for (int i = 1; i <= 8; ++i) lb.addScore(i, i*10); // scores increasing with id
        // expected: highest three ids 8,7,6
        vector<int> expected = {8,7,6};
        assertEq("LB-11 Top-K subset", lb.getTopN(3), expected);
    }

    {   // Test LB-12: Repeated add/remove sequence
        ConcreteLeaderboard lb;
        lb.addScore(1,100);
        lb.addScore(2,100);
        lb.addScore(3,90);
        lb.addScore(2,50);    // 2 -> 150
        lb.removePlayer(1);   // remove id 1
        vector<int> expected = {2,3};
        assertEq("LB-12 Add/remove sequence", lb.getTopN(3), expected);
    }

    cout << "\n========================================\n";
    cout << " PART D — ServerKernel (Greedy Scheduler)\n";
    cout << "========================================\n\n";

    // ---------- ServerKernel Tests ----------
    auto runSched = [&](const string &name, const vector<char>& tasks, int n, int expected) {
        vector<char> t = tasks; // function signature expects non-const ref
        int actual = ServerKernel::minIntervals(t, n);
        assertEq(name, actual, expected);
    };

    // S-1: Single Task
    // Tasks: {A}, n=3 -> Expected: 1
    runSched("S-1 Single task", {'A'}, 3, 1);

    // S-2: n = 0 (no cooldown)
    // Tasks: A A B C -> n=0 -> Expected: 4 (all consecutive)
    runSched("S-2 n=0 no cooldown", {'A','A','B','C'}, 0, 4);

    // S-3: All different tasks
    // Tasks: A B C D -> n=3 -> Expected: 4
    runSched("S-3 All different", {'A','B','C','D'}, 3, 4);

    // S-4: Classic example
    // Tasks: A A A B B C, n=3 -> Expected: 9
    runSched("S-4 Classic example", {'A','A','A','B','B','C'}, 3, 9);

    // S-5: Equal max frequency
    // Tasks: A A B B, n=2 -> Expected: 5
    runSched("S-5 Equal max freq", {'A','A','B','B'}, 2, 5);

    // S-6: Multiple max tasks
    // Tasks: A A A B B B, n=2 -> Expected: 8
    runSched("S-6 Multiple max tasks", {'A','A','A','B','B','B'}, 2, 8);

    // S-7: Formula < tasks.size()
    // Tasks: A B C D E F, n=2 -> Expected: 6
    runSched("S-7 Formula < tasks.size()", {'A','B','C','D','E','F'}, 2, 6);

    // S-8: High frequency single task
    // Tasks: A A A A, n=3 -> Expected: (4-1)*(3+1)+1 = 13
    runSched("S-8 High freq single", {'A','A','A','A'}, 3, 13);

    // S-9: Large n, few tasks
    // Tasks: A B, n=100 -> Expected: 2
    runSched("S-9 Large n few tasks", {'A','B'}, 100, 2);

    // S-10: One heavy dominant
    // Tasks: A A A A A B C, n=1 -> Expected: (5-1)*(1+1)+1=9
    runSched("S-11 One heavy dominant", {'A','A','A','A','A','B','C'}, 1, 9);

    // S-11: Stress case (100 x 'A')
    // Expected: (100-1)*(2+1)+1 = 298
    vector<char> manyA(100, 'A');
    runSched("S-12 Stress 100x A", manyA, 2, 298);

    // S-12: Edge case: empty tasks vector
    // Correct expected logically: 0 (no tasks)
    // Note: current implementation may not return 0 (it's a known edge behaviour),
    // so we print actual value and compare against expected 0 to reveal mismatch if any.
    vector<char> emptyTasks;
    int actualEmpty = ServerKernel::minIntervals(emptyTasks, 2);
    cout << left << setw(40) << "S-13 Empty tasks (expected 0)"
         << " | Actual: " << setw(6) << actualEmpty
         << " Expected: " << setw(6) << 0
         << " -> " << ((actualEmpty==0) ? "PASS" : "NOTE)") << "\n";
    if (actualEmpty == 0) ++passed; else ++failed;

    // Summary
    cout << "\n========================================\n";
    cout << " TEST SUMMARY: Passed = " << passed << " , Failed = " << failed << "\n";
    cout << "========================================\n";

    return (failed == 0) ? 0 : 1;
}
