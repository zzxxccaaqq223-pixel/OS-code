#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

struct Record
{
    int id;
    char name[20];
    double value;
};

int main()
{
    fstream file("direct.bin", ios::binary | ios::out | ios::in | ios::trunc);
    if (!file) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    Record records[] = {
        {1, "Alpha", 1.1},
        {2, "Beta", 2.2},
        {3, "Gamma", 3.3},
        {4, "Delta", 4.4},
        {5, "Epsilon", 5.5}
    };

    for (auto &r : records)
        file.write(reinterpret_cast<char *>(&r), sizeof(Record));

    int targetIndex = 2;
    streampos offset = targetIndex * sizeof(Record);
    file.seekp(offset, ios::beg);
    Record updated = {3, "Gamma_UPDATED", 99.9};
    file.write(reinterpret_cast<char *>(&updated), sizeof(Record));

    file.seekg(0, ios::beg);
    Record r;
    while (file.read(reinterpret_cast<char *>(&r), sizeof(Record)))
        cout << "ID: " << r.id << " Name: " << r.name << " Val: " << r.value << endl;

    file.close();
    return 0;
}
