#include "ArcadiaEngine.cpp"
int main() {
    ///test for hash table
    ConcretePlayerTable table;
    table.display();

    table.insert(101, "Alice");
    table.display();

    table.insert(205, "Bob");
    table.display();

    table.insert(333, "Charlie");
    table.display();

    table.insert(404, "Diana");
    table.display();

    table.insert(505, "Ethan");
    table.display();

    table.insert(606, "Fiona");
    table.display();

    table.insert(707, "George");
    table.display();

    table.insert(808, "Hannah");
    table.display();

    table.insert(909, "Ian");
    table.display();

    table.insert(111, "Jack");
    table.display();

    table.insert(222, "Kara");
    table.display();

    cout << table.search(205) << endl; // Bob
    cout << table.search(500) << endl; // ""


}

