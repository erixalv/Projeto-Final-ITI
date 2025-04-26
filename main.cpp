#include "LZWCompressor.h"
#include <fstream>
#include <iostream>
#include <map>

std::string loadText(const std::vector<std::string>& filenames) {
    std::string result;
    for (const auto& file : filenames) {
        std::ifstream in(file);
        if (!in) {
            std::cerr << "Erro ao abrir " << file << std::endl;
            continue;
        }
        result += std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        in.close();
    }
    return result;
}

std::string classifyText(const std::string& text,
                         const std::map<std::string, std::unordered_map<std::string, int>>& genreDicts) {
    std::string bestGenre;
    size_t bestSize = SIZE_MAX;

    for (const auto& pair : genreDicts) {
        const std::string& genre = pair.first;
        const auto& dict = pair.second;
    
        std::vector<int> compressed = LZWCompressor::compressWithDictionary(text, dict);
        if (compressed.size() < bestSize) {
            bestSize = compressed.size();
            bestGenre = genre;
        }
    }    
    return bestGenre;
}

int main() {
    // Arquivos de treino por gênero
    std::vector<std::string> romanceFiles = {"romance/romance1.txt", "romance/romance2.txt", "romance/romance3.txt", "romance/romance4.txt", "romance/romance5.txt"};
    std::vector<std::string> fantasiaFiles = {"fantasia/fantasia1.txt", "fantasia/fantasia2.txt"};

    // Treinamento
    std::string romanceCorpus = loadText(romanceFiles);
    std::string fantasiaCorpus = loadText(fantasiaFiles);

    std::unordered_map<std::string, int> romanceDict = LZWCompressor::generateDictionary(romanceCorpus);
    std::unordered_map<std::string, int> fantasiaDict = LZWCompressor::generateDictionary(fantasiaCorpus);

    std::map<std::string, std::unordered_map<std::string, int>> genreDicts = {
        {"Romance", romanceDict},
        {"Fantasia", fantasiaDict}
    };

    // Arquivos de teste
    std::vector<std::string> testFiles = {"teste1.txt", "teste2.txt"};

    for (const auto& testFile : testFiles) {
        std::string testText = loadText({testFile});
        std::string genre = classifyText(testText, genreDicts);
        std::cout << "Arquivo " << testFile << " classificado como: " << genre << std::endl;
    }

    return 0;
}
