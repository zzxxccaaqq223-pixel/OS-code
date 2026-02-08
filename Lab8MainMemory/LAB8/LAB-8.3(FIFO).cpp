#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>
#include <iomanip>
using namespace std;
class PageReplacementFIFO
{
private:
    int numFrames;
    queue<int> frameQueue;
    unordered_set<int> frameSet;
    int pageFaults;
    vector<vector<int>> frameHistory; // Track frame contents over time
public:
    PageReplacementFIFO(int frames) : numFrames(frames), pageFaults(0), frameQueue(), frameSet(), frameHistory() {}
    void referencePage(int page)
    {
        cout << "\nReferencing page : " << page << endl;
        // Check if page is already in memory
        if (frameSet.find(page) != frameSet.end())
        {
            cout << "Page " << page << " already in memory(HIT) " << endl;
        }
        else
        {
            // Page fault occurs
            pageFaults++;
            cout << "PAGE FAULT #" << pageFaults << endl;
            // If frames are full, remove oldest page (FIFO)
            if (frameQueue.size() >= numFrames)
            {
                int removedPage = frameQueue.front();
                frameQueue.pop();
                frameSet.erase(removedPage);
                cout << "Removed page " << removedPage << " (oldest) " << endl;
            }
            // Add new page
            frameQueue.push(page);
            frameSet.insert(page);
            cout << "Loaded page " << page << " into memory " << endl;
        }
        // Record current frame contents
        vector<int> currentFrames;
        queue<int> tempQueue = frameQueue;
        while (!tempQueue.empty())
        {
            currentFrames.push_back(tempQueue.front());
            tempQueue.pop();
        }
        frameHistory.push_back(currentFrames);
        // Display current frames
        cout << "    Frames: [";
        for (size_t i = 0; i < currentFrames.size(); i++) {
            cout << currentFrames[i];
            if (i < currentFrames.size() - 1)
                cout << ", ";
        }
        cout << "] " << endl;
    }
    void simulate(vector<int> referenceString)
    {
        cout << "\n ========== FIFO PAGE REPLACEMENT SIMULATION ========== " << endl;
        cout << "Number of Frames : " << numFrames << endl;
        cout << "Reference String : ";
        for (int page : referenceString)
        {
            cout << page << " ";
        }
        cout << "\n " << endl;
        // Process each page reference
        for (int page : referenceString)
        {
            referencePage(page);
        }
        displayResults();
        displayTable(referenceString);
    }

    void displayResults()
    {
        cout << "\n ========== RESULTS ========== " << endl;
        cout << "Total Page Faults : " << pageFaults << endl;
        int totalReferences = frameHistory.size();
        double faultRate = (double)pageFaults / totalReferences * 100;
        double hitRate = 100.0 - faultRate;
        cout << "Total References : " << totalReferences << endl;
        cout << "Page Fault Rate : " << fixed << setprecision(2) << faultRate << " % " << endl;
        cout << "Hit Rate : " << hitRate << " % " << endl;
    }
    void displayTable(vector<int> referenceString)
    {
        cout << "\n ========== FRAME CONTENTS TABLE ========== " << endl;
        cout << setw(8) << "Step" << "| ";
        for (size_t i = 0; i < referenceString.size(); i++)
        {
            cout << setw(4) << referenceString[i];
        }
        cout << endl;
        cout << string(8 + 5 + referenceString.size() * 4, '-') << endl;
        for (int frame = 0; frame < numFrames; frame++)
        {
            cout << setw(8) << (" Frame " + to_string(frame)) << "| ";
            for (size_t step = 0; step < frameHistory.size(); step++)
            {
                if (frame < frameHistory[step].size())
                {
                    cout << setw(4) << frameHistory[step][frame];
                }
                else
                {
                    cout << setw(4) << "-";
                }
            }
            cout << endl;
        }
    }
};
int main()
{
    cout << "PAGE REPLACEMENT ALGORITHM - FIFO " << endl;
    cout << "================================== " << endl;
    // Test Case 1
    cout << "\n--- TEST CASE 1 ---" << endl;
    PageReplacementFIFO fifo1(3);
    vector<int> refString1 = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2};
    fifo1.simulate(refString1);
    // Test Case 2
    cout << "\n\n--- TEST CASE 2 ---" << endl;
    PageReplacementFIFO fifo2(4);
    vector<int> refString2 = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};
    fifo2.simulate(refString2);
    // Interactive mode
    char choice;
    cout << "\n\nRun custom test ? (y/n) : ";
    cin >> choice;
    if (choice == 'y' || choice == 'Y')
    {
        int frames;
        cout << "Enter number of frames : ";
        cin >> frames;
        PageReplacementFIFO customFifo(frames);
        cout << "Enter reference string(space-separated, -1 to end) : " << endl;

        vector<int> customRef;
        int page;
        while (cin >> page && page != -1)
        {
            customRef.push_back(page);
        }
        customFifo.simulate(customRef);
    }
    return 0;
}