#include <iostream>
#include <fstream>
#include <unistd.h> // access()
#include <sys/stat.h>
#include <cerrno>
#include <cstring>

using namespace std;

void checkPermissions(const char *path)
{
    cout << "Checking permissions for: " << path << endl;

    // access() checks real UID/GID permissions
    cout << "Read(R_OK) : " 
         << (access(path, R_OK) == 0 ? "ALLOWED" : "DENIED") << endl;
    
    cout << "Write(W_OK) : " 
         << (access(path, W_OK) == 0 ? "ALLOWED" : "DENIED") << endl;
    
    cout << "Execute(X_OK) : " 
         << (access(path, X_OK) == 0 ? "ALLOWED" : "DENIED") << endl;
    
    cout << "Exists(F_OK) : " 
         << (access(path, F_OK) == 0 ? "YES" : "NO") << endl;
}

void safeOpen(const char *path)
{
    if (access(path, R_OK) != 0)
    {
        cerr << "Permission denied: cannot read '" << path << "'" << endl;
        cerr << "Reason: " << strerror(errno) << endl;
        return;
    }

    ifstream f(path);
    if (f.is_open()) {
        cout << "File opened successfully." << endl;
        f.close();
    } else {
        cerr << "Failed to open file despite access check." << endl;
    }
}

int main()
{
    const char* filename = "test_perms.txt";

    // Create test file with write-only permissions
    ofstream f(filename);
    if (!f) {
        perror("ofstream");
        return 1;
    }
    f << "test content";
    f.close();

    // Set to write-only: no read
    if (chmod(filename, S_IWUSR) == -1) {
        perror("chmod");
        return 1;
    }

    checkPermissions(filename);
    cout << endl;

    safeOpen(filename); // Should be denied

    // Restore permissions so we can delete it or read it later
    if (chmod(filename, S_IRUSR | S_IWUSR) == -1) {
        perror("chmod restore");
    }

    // Cleanup
    unlink(filename);

    return 0;
}
// access() vs open(): access() checks permissions WITHOUT opening;
// useful for pre-checks (though TOCTOU race conditions can occur)
