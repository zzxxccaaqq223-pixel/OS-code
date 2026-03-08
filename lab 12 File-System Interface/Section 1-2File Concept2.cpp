#include <iostream>
#include <sys/stat.h>
#include <ctime>

using namespace std;

int main()
{
    struct stat fileInfo;
    const char *filename = "data.txt";

    if (stat(filename, &fileInfo) != 0)
    {
        cerr << "Error: Cannot retrieve file attributes" << endl;
        return 1;
    }

    cout << "File: " << filename << endl;
    cout << "Size: " << fileInfo.st_size << " bytes" << endl;
    cout << "Last modified: " << ctime(&fileInfo.st_mtime);
    cout << "Is regular file: " << (S_ISREG(fileInfo.st_mode) ? "Yes" : "No") << endl;

    return 0;
}
// Key attributes: st_size (size), st_mtime (modification time),
// st_mode (type/permissions), st_uid (owner), st_nlink (link count)
