#include <iostream>
#include <filesystem>
#include <string>

using namespace std;
namespace fs = std::filesystem;

int main()
{
    fs::path dirPath = "."; // Current directory
    
    cout << "Directory listing for: " << fs::absolute(dirPath) << endl;
    cout << string(50, '-') << endl;

    try {
        for (const auto &entry : fs::directory_iterator(dirPath))
        {
            if (entry.is_regular_file())
            {
                cout << "[FILE] " << entry.path().filename().string() 
                     << " (" << entry.file_size() << " bytes)" << endl;
            }
            else if (entry.is_directory())
            {
                cout << "[DIR]  " << entry.path().filename().string() << "/" << endl;
            }
            else
            {
                cout << "[OTHER] " << entry.path().filename().string() << endl;
            }
        }
    }
    catch (const fs::filesystem_error& e)
    {
        cerr << "Error: " << e.what() << endl;
    }

    return 0;
}
