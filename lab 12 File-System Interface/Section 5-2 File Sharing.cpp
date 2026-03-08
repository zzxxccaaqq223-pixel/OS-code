#include <iostream>
#include <fstream>
#include <unistd.h> 
#include <sys/stat.h>


using namespace std;

int main()
{
    // Create original file
    ofstream f("original.txt");
    f << "File sharing via links!";
    f.close();

    // Hard link: same inode, different directory entry
    link("original.txt", "hardlink.txt");

    // Symbolic link: separate inode pointing to path string
    symlink("original.txt", "symlink.txt");

    // Check inode numbers
    struct stat s1, s2, s3;
    stat("original.txt", &s1);
    stat("hardlink.txt", &s2);
    lstat("symlink.txt", &s3);

    cout << "Original inode : " << s1.st_ino << endl;
    cout << "Hard link inode : " << s2.st_ino 
         << " (same? " << (s1.st_ino == s2.st_ino ? "YES" : "NO") << ")" << endl;
    cout << "Symlink inode : " << s3.st_ino 
         << " (same? " << (s1.st_ino == s3.st_ino ? "YES" : "NO") << ")" << endl;

    // Delete original
    unlink("original.txt");

    // Hard link still works (reference count &gt; 0)
    ifstream h("hardlink.txt");
    cout << "\nHard link after delete : " 
         << (h.good() ? "ACCESSIBLE" : "BROKEN") << endl;

    // Symbolic link is now dangling
    ifstream sym("symlink.txt");
    cout << "Symbolic link after delete : " 
         << (sym.good() ? "ACCESSIBLE" : "DANGLING / BROKEN") << endl;

    // Cleanup
    unlink("hardlink.txt");
    unlink("symlink.txt");

    return 0;
}
// Hard link: file data survives until ALL hard links deleted (link count = 0)
// Symbolic link: dangling reference once target is removed
