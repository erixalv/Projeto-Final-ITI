#include "LZWCompressor.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

// Função para ler o conteúdo de um arquivo de texto
std::string readTextFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile)
        throw std::runtime_error("Erro ao abrir o arquivo de entrada: " + filename);

    std::string content((std::istreambuf_iterator<char>(inFile)),
                         std::istreambuf_iterator<char>());
    return content;
}

// Função para escrever um vetor de inteiros em arquivo binário
void writeBinaryFile(const std::string& filename, const std::vector<int>& data) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile)
        throw std::runtime_error("Erro ao abrir o arquivo binário para escrita: " + filename);

    for (int val : data)
        outFile.write(reinterpret_cast<const char*>(&val), sizeof(int));
}

// Função para ler um vetor de inteiros de um arquivo binário
std::vector<int> readBinaryFile(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile)
        throw std::runtime_error("Erro ao abrir o arquivo binário para leitura: " + filename);

    std::vector<int> data;
    int val;
    while (inFile.read(reinterpret_cast<char*>(&val), sizeof(int)))
        data.push_back(val);

    return data;
}

// Função para escrever o texto descomprimido em um arquivo
void writeTextFile(const std::string& filename, const std::string& text) {
    std::ofstream outFile(filename);
    if (!outFile)
        throw std::runtime_error("Erro ao escrever no arquivo de saída: " + filename);
    outFile << text;
}

int main() {
    try {
        // Passo 1: Ler texto original
        std::string inputText = readTextFile("arquivo_teste_comp.txt");
        std::cout << "Tamanho do arquivo original: " << inputText.size() << " bytes\n";

        // Passo 2: Comprimir o texto
        auto compressed = LZWCompressor::compress(inputText);
        std::cout << "Tamanho comprimido: " << compressed.size() * sizeof(int) << " bytes\n";
        
        // Taxa de compressão
        float compressionRatio = static_cast<float>(compressed.size() * sizeof(int)) / inputText.size();
        std::cout << "Taxa de compressão: " << compressionRatio * 100 << "%\n";

        // Passo 3: Salvar dados comprimidos em arquivo binário
        writeBinaryFile("teste_comprimido.bin", compressed);

        // Passo 4: Ler dados comprimidos do arquivo binário
        auto readCompressed = readBinaryFile("teste_comprimido.bin");
        
        // Verificar se os dados lidos correspondem aos dados comprimidos
        if (compressed == readCompressed) {
            std::cout << "Verificação de leitura do arquivo binário: OK\n";
        } else {
            std::cout << "ERRO: Os dados lidos do arquivo binário não correspondem aos dados comprimidos\n";
        }

        // Passo 5: Descomprimir
        std::string decompressed = LZWCompressor::decompress(readCompressed);
        
        // Verificar se o texto descomprimido corresponde ao texto original
        if (inputText == decompressed) {
            std::cout << "Verificação de descompressão: OK\n";
        } else {
            std::cout << "ERRO: O texto descomprimido não corresponde ao texto original\n";
        }

        // Passo 6: Salvar texto descomprimido em arquivo
        writeTextFile("teste_descomprimido.txt", decompressed);

        std::cout << "Compressão e descompressão concluídas com sucesso.\n";
    } catch (const std::exception& ex) {
        std::cerr << "Erro: " << ex.what() << std::endl;
    }

    return 0;
}