// Base.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

void loadData(vector<string> &dataFile, string file);

void printData(vector<string>& dataFile);

int main()
{
    string file1, file2, line;
    
    ofstream outFile;
    vector<string> dataFile1;
    vector<string> dataFile2;

    cout << "Enter first file name.\n";
    getline(cin, file1);
    cout << "Enter second file name.\n";
    getline(cin, file2);

    loadData(dataFile1, file1);
    loadData(dataFile2, file2);

    printData(dataFile1);
    printData(dataFile2);

    //for (int i = 0; i < dataFile1.size(); i++)
    //{
    //    cout << dataFile1[i] << "\n";
    //}

    //for (int i = 0; i < dataFile2.size(); i++)
    //{
    //    cout << dataFile2[i] << "\n";
    //}


}

void loadData(vector<string> &dataFile, string file)
{
    ifstream inData;
    string line;

    inData.open(file);
    while (getline(inData, line))
    {
        dataFile.push_back(line);
    }
    inData.close();

}

void printData(vector<string>& dataFile)
{
    for (int i = 0; i < dataFile.size(); i++)
    {
        cout << dataFile[i] << "\n";
    }
    cout << "#################################\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
