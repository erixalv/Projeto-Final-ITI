#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

// Compressão LZW
vector<int> compressLZW(const string& input) {
    unordered_map<string, int> dictionary;
    for (int i = 0; i < 256; i++)
        dictionary[string(1, char(i))] = i;

    string w;
    vector<int> result;
    int dictSize = 256;

    for (char c : input) {
        string wc = w + c;
        if (dictionary.count(wc))
            w = wc;
        else {
            result.push_back(dictionary[w]);
            dictionary[wc] = dictSize++;
            w = string(1, c);
        }
    }

    if (!w.empty())
        result.push_back(dictionary[w]);

    return result;
}

// Descompressão LZW
string decompressLZW(const vector<int>& compressed) {
    unordered_map<int, string> dictionary;
    for (int i = 0; i < 256; i++)
        dictionary[i] = string(1, char(i));

    int dictSize = 256;
    string w = dictionary[compressed[0]];
    string result = w;

    for (size_t i = 1; i < compressed.size(); i++) {
        int k = compressed[i];
        string entry;

        if (dictionary.count(k))
            entry = dictionary[k];
        else if (k == dictSize)
            entry = w + w[0];
        else
            throw "Erro na descompressão";

        result += entry;
        dictionary[dictSize++] = w + entry[0];
        w = entry;
    }

    return result;
}

// Salvar vetor de inteiros como binário
void saveCompressed(const vector<int>& data, const string& filename) {
    ofstream out(filename, ios::binary);
    for (int code : data) {
        out.write(reinterpret_cast<const char*>(&code), sizeof(int));
    }
    out.close();
}

// Carregar vetor de inteiros de um arquivo binário
vector<int> loadCompressed(const string& filename) {
    ifstream in(filename, ios::binary);
    vector<int> data;
    int code;
    while (in.read(reinterpret_cast<char*>(&code), sizeof(int))) {
        data.push_back(code);
    }
    in.close();
    return data;
}

int main() {
    string inputFilename = "input.txt";
    string compressedFilename = "comprimido.bin";
    string decompressedFilename = "descomprimido.txt";

    // Ler o conteúdo do arquivo de entrada
    ifstream inFile(inputFilename);
    if (!inFile) {
        cerr << "Erro ao abrir " << inputFilename << endl;
        return 1;
    }

    string input((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();

    // Compressão
    vector<int> compressed = compressLZW(input);
    saveCompressed(compressed, compressedFilename);
    cout << "Arquivo comprimido salvo em: " << compressedFilename << endl;

    // Descompressão
    vector<int> loadedCompressed = loadCompressed(compressedFilename);
    string decompressed = decompressLZW(loadedCompressed);

    ofstream outFile(decompressedFilename);
    outFile << decompressed;
    outFile.close();
    cout << "Arquivo descomprimido salvo em: " << decompressedFilename << endl;

    return 0;
}
