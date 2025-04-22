#ifndef SISTEMA_H
#define SISTEMA_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <queue>
using namespace std;

struct HuffmanNode {
    int symbol;             // Valor de intensidad (0..M) o -1 si es nodo interno
    unsigned long freq;     // Frecuencia de aparición
    HuffmanNode* left;      // Hijo izquierdo
    HuffmanNode* right;     // Hijo derecho

    HuffmanNode(int s, unsigned long f);
};
struct CompareNode {
    bool operator()(HuffmanNode* a, HuffmanNode* b) const;
};
class ImageProcessingSystem {
private:
    //  Atributos privados
    vector<vector<int>> imageData;
    int width, height, maxPixelValue;
    string imageFilename;
    string volume;
    vector<vector<vector<int>>> volumeData;

    //  Métodos privados
    void loadImage(string filename);
    void loadVolume(string param);
    void infoImage();
    void infoVolume();
    void projection2D(string param);
    void encodeImage(string param);
    void decodeFile(string param);
    void segmentImage(string param);
    void buildCodes(HuffmanNode* node,const string& prefix,vector<string>& codes) const;
    void handleCommand(string command);
    void showHelp(string cmd);

public:
    void start();  // Único método público
};

#endif

