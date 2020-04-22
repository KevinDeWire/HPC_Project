// Base.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono> 

using namespace std;

#define N 4
#define M 1

void LoadData(vector<string>& dataFile, string file);
void PrintData(vector<string>& dataFile);
void SaveData(vector<string>& dataFile, string file);

// Common
void HelixSetup(vector< vector<float> >& helixPoints, vector<string>& dataFile, int& length);
void HelixCoordExtract(vector< vector<float> >& helixPoints, vector<string>& dataFile, string helixChainID, int initSeqNum, int endSeqNum);
void HelixStartPoint(vector< vector<float> >& helixPoints, vector<float>& helixStartPoint);
void HelixEndPoint(vector< vector<float> >& helixPoints, vector<float>& helixEndPoint, int length);
void MoveDistance(vector<float>& moveDistance, vector<float>& point1, vector<float>& point2);

// Matrix
void TranslationMatrix(vector<float> move);
void XRotateMatrix(float thetaX);
void YRotateMatrix(float RyMatrix[][N], float thetaY);
void ZRotateMatrix(float RzMatrix[][N], float thetaZ);
void TransformationMatrix(float TfMatrix[][N], float RxMatrix[][N], float RyMatrix[][N], float RzMatrix[][N], float ToMatrix[][N], float TransMatrix[][N]);
void Multiply4x4(float mat1[][N], float mat2[][N], float res[][N]);

// Linear
void Translation(vector<string>& inputFile, vector<string>& outputFile, float* moveToOrigen, float thetaX, float thetaY, float thetaZ, float* moveToFinal);
void Move(float* moveDist, float& x, float& y, float& z);
void Rotate(float theta, float& coord1, float& coord2);

// Useful
string RecordType(string record);
int HelixLength(string record);
string HelixChainID(string record);
int HelixInitSeqNum(string record);
int HelixEndSeqNum(string record);
string ChainID(string record);
int ResSeq(string record);
string AtomName(string record);
string AltLocInd(string record);
float XCoord(string record);
float YCoord(string record);
float ZCoord(string record);
float CoordAvg(float** helixPoints, int first, int pos);
float Theta(float* helixStartPoint, float* helixEndPoint, int opp, int adj);



int main()
{
    string inFile1, inFile2, outFileName;
    vector<string> dataFile1;
    vector<string> dataFile2;

    ofstream outFile;
    vector<string> outputFile;

    vector< vector<float> > helixPoints1;
    vector< vector<float> > helixPoints2;

    int helixPoints1Length = 0;
    int helixPoints2Length = 0;

    vector<float> helixStartPoint1;
    vector<float> helixEndPoint1;
    vector<float> helixStartPoint2;
    vector<float> helixEndPoint2;
    vector<float> origenPoint;
    vector<float> moveToOrigen;
    vector<float> moveToFinal;

    float thetaX, thetaY, thetaZ;

    vector< vector<float> > ToMatrix; // Translation to Origen
    vector< vector<float> > RzMatrix; // Z rotation
    vector< vector<float> > RyMatrix; // Y rotation
    vector< vector<float> > RxMatrix; // X rotation
    vector< vector<float> > TfMatrix; // Translation to final position
    vector< vector<float> > TransMatrix; // Transformation matrix

    cout << "Enter first file name.\n";
    getline(cin, inFile1);
    cout << "Enter second file name.\n";
    getline(cin, inFile2);
    cout << "Enter output file name.\n";
    getline(cin, outFileName);

    auto start1 = chrono::high_resolution_clock::now();

    LoadData(dataFile1, inFile1);
    LoadData(dataFile2, inFile2);

    HelixSetup(helixPoints1, dataFile1, helixPoints1Length);
    HelixSetup(helixPoints2, dataFile2, helixPoints2Length);

    HelixStartPoint(helixPoints1, helixStartPoint1);
    HelixStartPoint(helixPoints2, helixStartPoint2);

    HelixEndPoint(helixPoints1, helixEndPoint1, helixPoints1Length);
    HelixEndPoint(helixPoints2, helixEndPoint2, helixPoints2Length);

    thetaZ = Theta(helixEndPoint1, helixStartPoint1, 1, 0) - Theta(helixEndPoint2, helixStartPoint2, 1, 0);
    thetaY = Theta(helixEndPoint1, helixStartPoint1, 2, 0) - Theta(helixEndPoint2, helixStartPoint2, 2, 0);
    thetaX = Theta(helixEndPoint1, helixStartPoint1, 2, 1) - Theta(helixEndPoint2, helixStartPoint2, 2, 1);

    origenPoint.assign(3, 0);

    MoveDistance(moveToOrigen, origenPoint, helixStartPoint2);
    MoveDistance(moveToFinal, helixStartPoint1, origenPoint);

    TfMatrix=TranslationMatrix(moveToFinal);
    ToMatrix=TranslationMatrix(moveToOrigen);
    RxMatrix=XRotateMatrix(thetaX);
    RyMatrix=YRotateMatrix(thetaY);
    RzMatrix=ZRotateMatrix(thetaZ);
    TransMatrix=TransformationMatrix(TfMatrix, RxMatrix, RyMatrix, RzMatrix, ToMatrix);

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            cout << TransMatrix[i][j] << "  ";
        }
        cout << endl;
    }

    auto end1 = chrono::high_resolution_clock::now();

    auto start2 = chrono::high_resolution_clock::now();

    //   Translation(dataFile2, outputFile, moveToOrigen, thetaX, thetaY, thetaZ, moveToFinal);

    auto end2 = chrono::high_resolution_clock::now();

    double time_taken1 = chrono::duration_cast<chrono::nanoseconds>(end1 - start1).count();
    double time_taken2 = chrono::duration_cast<chrono::nanoseconds>(end2 - start2).count();
    double time_taken3 = chrono::duration_cast<chrono::nanoseconds>(end2 - start1).count();

    time_taken1 *= 1e-9;
    time_taken2 *= 1e-9;
    time_taken3 *= 1e-9;

    cout << "Prep Time: " << fixed << time_taken1 << setprecision(9) << endl;
    cout << "Translation Time: " << fixed << time_taken2 << setprecision(9) << endl;
    cout << "Total Time: " << fixed << time_taken3 << setprecision(9) << endl;

    //   cout << thetaX << " | " << thetaY << " | " << thetaZ;

       //for (int i = 0; i < 3; i++)
       //{
       //    cout << moveToOrigen[i] << " | ";
       //}
       //cout << endl;

       //for (int i = 0; i < 3; i++)
       //{
       //    cout << moveToFinal[i] << " | ";
       //}
       //cout << endl;

       //PrintData(dataFile1);
       //PrintData(dataFile2);
       //PrintData(outputFile);


//    SaveData(outputFile, outFileName);

}

// Setup Functions
void LoadData(vector<string>& dataFile, string file)
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

void HelixSetup(vector< vector<float> >& helixPoints, vector<string>& dataFile, int& length)
{
    string helixChainID, recordType;
    int initSeqNum, endSeqNum, helixLength;
    bool helixFound = false;

    for (int i = 0; i < dataFile.size(); i++)
    {
        if (RecordType(dataFile[i]) == "HELIX " && HelixLength(dataFile[i]) > 4)
        {
            helixChainID = HelixChainID(dataFile[i]);
            initSeqNum = HelixInitSeqNum(dataFile[i]);
            endSeqNum = HelixEndSeqNum(dataFile[i]);
            length = HelixLength(dataFile[i]);
            helixPoints.resize(length);
            for (int row = 0; row < length; row++)
            {
                helixPoints.resize(3);
            }
            HelixCoordExtract(helixPoints, dataFile, helixChainID, initSeqNum, endSeqNum);
            helixFound = true;
        }
        if (helixFound)
        {
            break;
        }
    }

}

void HelixCoordExtract(vector< vector<float> >& helixPoints, vector<string>& dataFile, string helixChainID, int initSeqNum, int endSeqNum)
{
    int j = 0;
    int resSeq;
    string altLocInd;

    for (int i = 0; i < dataFile.size(); i++)
    {
        if (RecordType(dataFile[i]) == "ATOM  ")
        {
            if (ChainID(dataFile[i]) == helixChainID)
            {
                resSeq = ResSeq(dataFile[i]);
                if (resSeq >= initSeqNum && resSeq <= endSeqNum)
                {
                    if (AtomName(dataFile[i]) == " CA ")
                    {
                        altLocInd = AltLocInd(dataFile[i]);
                        if (altLocInd == " " || altLocInd == "A")
                        {
                            helixPoints[j][0] = XCoord(dataFile[i]);
                            helixPoints[j][1] = YCoord(dataFile[i]);
                            helixPoints[j][2] = ZCoord(dataFile[i]);
                            j++;
                        }
                    }
                }
                if (resSeq > endSeqNum)
                {
                    break;
                }
            }
        }
    }
}

void HelixStartPoint(vector< vector<float> >& helixPoints, vector<float>& helixStartPoint)
{
    for (int i = 0; i < 3; i++)
    {
        helixStartPoint.push_back(CoordAvg(helixPoints, 0, i));
    }
}

void HelixEndPoint(vector< vector<float> >& helixPoints, vector<float>& helixEndPoint, int length)
{
    for (int i = 0; i < 3; i++)
    {
        helixEndPoint.push_back(CoordAvg(helixPoints, length - 4, i));
    }
}

void MoveDistance(vector<float>& moveDistance, vector<float>& point1, vector<float>& point2)
{
    for (int i = 0; i < 3; i++)
    {
        moveDistance.push_back(point1[i] - point2[i]);
    }
}


// MAtrix Functions
float** TranslationMatrix(float* move)
{
    float** matrix = new float*[N];
    for (int i = 0; i < N; i++)
    {
        matrix[i] = new float[N];
    }
    matrix[0][0] = 1; matrix[0][1] = 0; matrix[0][2] = 0; matrix[0][3] = move[0];
    matrix[1][0] = 0; matrix[1][1] = 1; matrix[1][2] = 0; matrix[1][3] = move[1];
    matrix[2][0] = 0; matrix[2][1] = 0; matrix[2][2] = 1; matrix[2][3] = move[2];
    matrix[3][0] = 0; matrix[3][0] = 0; matrix[3][0] = 0; matrix[3][0] = 1;
    return matrix;
}

float** XRotateMatrix(float thetaX)
{
    float** matrix = new float* [N];
    for (int i = 0; i < N; i++)
    {
        matrix[i] = new float[N];
    }
    matrix[0][0] = cos(thetaX); matrix[0][1] = -sin(thetaX); matrix[0][2] = 0; matrix[0][3] = 0;
    matrix[1][0] = sin(thetaX); matrix[1][1] = cos(thetaX); matrix[1][2] = 0; matrix[1][3] = 0;
    matrix[2][0] = 0; matrix[2][1] = 0; matrix[2][2] = 1; matrix[2][3] = 0;
    matrix[3][0] = 0; matrix[3][0] = 0; matrix[3][0] = 0; matrix[3][0] = 1;
    return matrix;
}

float** YRotateMatrix(float thetaY)
{
    float** matrix = new float* [N];
    for (int i = 0; i < N; i++)
    {
        matrix[i] = new float[N];
    }
    RyMatrix = { {cos(thetaY),0,sin(thetaY),0},{0,1,0,0},{-sin(thetaY),0,cos(thetaY),0},{0,0,0,1} };
}

float** ZRotateMatrix(float thetaZ)
{
    float** matrix = new float* [N];
    for (int i = 0; i < N; i++)
    {
        matrix[i] = new float[N];
    }
    RzMatrix = { {1,0,0,0},{0,cos(thetaZ),-sin(thetaZ),0},{0,sin(thetaZ),cos(thetaZ),0}, {0,0,0,1} };
}

float** TransformationMatrix(float TfMatrix[][N], float RxMatrix[][N], float RyMatrix[][N], float RzMatrix[][N], float ToMatrix[][N], float TransMatrix[][N])
{
    float** tempMatrix1 = new float* [N];
    for (int i = 0; i < N; i++)
    {
        matrix[i] = new float[N];
    }
    float** tempMatrix2 = new float* [N];
    for (int i = 0; i < N; i++)
    {
        matrix[i] = new float[N];
    }
    tempMatrix1 = MultiplyNxN(TfMatrix, RxMatrix);
    tempMatrix2 = MultiplyNxN(tempMatrix1, RyMatrix);
    tempMatrix1 = MultiplyNxN(tempMatrix2, RzMatrix);
    tempMatrix2 = MultiplyNxN(tempMatrix1, ToMatrix);
    return tempMatrix2;
}

float** MultiplyNxN(float** mat1, float** mat2)
{
    float** matrix = new float* [N];
    for (int i = 0; i < N; i++)
    {
        matrix[i] = new float[N];
    }
    int i, j, k;
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
            matrix[i][j] = 0;
            for (k = 0; k < N; k++)
                matrix[i][j] += mat1[i][k] * mat2[k][j];
        }
    }
}

void Translation(vector<string>& inputFile, vector<string>& outputFile, vector<string>& moveToOrigen, float thetaX, float thetaY, float thetaZ, float* moveToFinal)
{
    string record;
    float x, y, z;
    ostringstream xstr, ystr, zstr;



    for (int i = 0; i < inputFile.size(); i++)
    {
        xstr.width(8);
        ystr.width(8);
        zstr.width(8);
        record = inputFile[i];

        if (RecordType(record) == "ATOM  ")
        {
            x = XCoord(record);
            y = YCoord(record);
            z = ZCoord(record);

            Move(moveToOrigen, x, y, z);
            Rotate(thetaZ, x, y);
            Rotate(thetaY, x, z);
            Rotate(thetaX, y, z);
            Move(moveToFinal, x, y, z);

            xstr << fixed << setprecision(3) << x;
            ystr << fixed << setprecision(3) << y;
            zstr << fixed << setprecision(3) << z;

            record.replace(30, 8, xstr.str());
            record.replace(38, 8, ystr.str());
            record.replace(46, 8, zstr.str());

            xstr.str("");
            ystr.str("");
            zstr.str("");
        }
        outputFile.push_back(record);
    }
}

void Move(float* moveDist, float& x, float& y, float& z)
{
    x += moveDist[0];
    y += moveDist[1];
    z += moveDist[2];
}

void Rotate(float theta, float& coord1, float& coord2)
{
    float new1, new2;
    new1 = (coord1 * cos(theta)) - (coord2 * sin(theta));
    new2 = (coord2 * cos(theta)) + (coord1 * sin(theta));
    coord1 = new1;
    coord2 = new2;
}

string RecordType(string record)
{
    return record.substr(0, 6);
}

int HelixLength(string record)
{
    return stoi(record.substr(71, 5));
}

string HelixChainID(string record)
{
    return record.substr(19, 1);
}

int HelixInitSeqNum(string record)
{
    return stoi(record.substr(21, 4));
}

int HelixEndSeqNum(string record)
{
    return stoi(record.substr(33, 4));
}

string ChainID(string record)
{
    return record.substr(21, 1);
}

int ResSeq(string record)
{
    return stoi(record.substr(22, 4));
}

string AtomName(string record)
{
    return record.substr(12, 4);
}

string AltLocInd(string record)
{
    return record.substr(16, 1);
}

float XCoord(string record)
{
    return stof(record.substr(30, 8));
}

float YCoord(string record)
{
    return stof(record.substr(38, 8));
}

float ZCoord(string record)
{
    return stof(record.substr(46, 8));
}

float CoordAvg(vector< vector<float> >& helixPoints, int first, int pos)
{
    float coordAvg = 0;
    for (int i = first; i < first + 4; i++)
    {
        coordAvg = coordAvg + helixPoints[i][pos];
    }
    return coordAvg / 4;
}

float Theta(vector<float>& helixStartPoint, vector<float>& helixEndPoint, int opp, int adj)
{
    return atan((helixEndPoint[opp] - helixStartPoint[opp]) / (helixEndPoint[adj] - helixStartPoint[adj]));
}

void PrintData(vector<string>& dataFile)
{
    for (int i = 0; i < dataFile.size(); i++)
    {
        cout << dataFile[i] << endl;
    }
}

void SaveData(vector<string>& dataFile, string file)
{

    ofstream outData;
    outData.open(file);
    for (int i = 0; i < dataFile.size(); i++)
    {
        outData << dataFile[i] << endl;
    }
    outData.close();
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
