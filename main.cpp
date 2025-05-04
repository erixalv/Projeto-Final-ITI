#include "LZWCompressor.h"
#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>

// Classe auxiliar para estender o LZWCompressor para classificação
class LZWClassifier : public LZWCompressor {
public:
    // Gera um dicionário baseado num corpus de texto
    static std::unordered_map<std::string, int> generateDictionary(const std::string& input) {
        std::unordered_map<std::string, int> dictionary;
        // Inicializa o dicionário com caracteres ASCII
        for (int i = 0; i < 256; i++) {
            dictionary[std::string(1, char(i))] = i;
        }

        std::string w;
        int dictSize = 256;

        for (char c : input) {
            std::string wc = w + c;
            if (dictionary.find(wc) != dictionary.end()) {
                w = wc;
            } else {
                dictionary[wc] = dictSize++;
                w = std::string(1, c);
            }
        }

        return dictionary;
    }

    // Comprime texto usando um dicionário pré-gerado
    static std::vector<int> compressWithDictionary(const std::string& input, 
                                                  const std::unordered_map<std::string, int>& baseDict) {
        // Cria uma cópia do dicionário base
        auto dictionary = baseDict;
        std::string w;
        std::vector<int> result;
        int dictSize = dictionary.size();

        for (char c : input) {
            std::string wc = w + c;
            if (dictionary.find(wc) != dictionary.end()) {
                w = wc;
            } else {
                // Se w está no dicionário, adicionamos seu código ao resultado
                if (dictionary.find(w) != dictionary.end()) {
                    result.push_back(dictionary.at(w));
                }
                dictionary[wc] = dictSize++;
                w = std::string(1, c);
            }
        }

        // Não esqueça do último código
        if (!w.empty() && dictionary.find(w) != dictionary.end()) {
            result.push_back(dictionary.at(w));
        }

        return result;
    }
};

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
    
        std::vector<int> compressed = LZWClassifier::compressWithDictionary(text, dict);
        if (compressed.size() < bestSize) {
            bestSize = compressed.size();
            bestGenre = genre;
        }
        
        // Exibir tamanho da compressão para cada gênero (opcional para diagnóstico)
        std::cout << "Tamanho comprimido para " << genre << ": " << compressed.size() << std::endl;
    }    
    return bestGenre;
}

int main() {
    // Arquivos de treino por gênero
    std::vector<std::string> romanceFiles = {"romance/romance1.txt", "romance/romance2.txt", "romance/romance3.txt", "romance/romance4.txt", "romance/romance5.txt"};
    std::vector<std::string> fantasiaFiles = {"fantasia/fantasia1.txt", "fantasia/fantasia2.txt"};
    std::vector<std::string> terrorFiles = {"terror/1.txt", "terror/2.txt", "terror/3.txt", "terror/4.txt", "terror/5.txt"};

    // Treinamento
    std::cout << "Carregando corpora de treinamento..." << std::endl;
    std::string romanceCorpus = loadText(romanceFiles);
    std::string fantasiaCorpus = loadText(fantasiaFiles);
    std::string terrorCorpus = loadText(terrorFiles);

    std::cout << "Tamanho do corpus de Romance: " << romanceCorpus.size() << " bytes" << std::endl;
    std::cout << "Tamanho do corpus de Fantasia: " << fantasiaCorpus.size() << " bytes" << std::endl;
    std::cout << "Tamanho do corpus de Terror: " << terrorCorpus.size() << " bytes" << std::endl;

    std::cout << "Gerando dicionários..." << std::endl;
    std::unordered_map<std::string, int> romanceDict = LZWClassifier::generateDictionary(romanceCorpus);
    std::unordered_map<std::string, int> fantasiaDict = LZWClassifier::generateDictionary(fantasiaCorpus);
    std::unordered_map<std::string, int> terrorDict = LZWClassifier::generateDictionary(terrorCorpus); 

    std::cout << "Tamanho do dicionário de Romance: " << romanceDict.size() << " entradas" << std::endl;
    std::cout << "Tamanho do dicionário de Fantasia: " << fantasiaDict.size() << " entradas" << std::endl;
    std::cout << "Tamanho do dicionário de Terror: " << terrorDict.size() << " entradas" << std::endl;

    std::map<std::string, std::unordered_map<std::string, int>> genreDicts = {
        {"Romance", romanceDict},
        {"Fantasia", fantasiaDict},
        {"Terror", terrorDict}
    };

    // Arquivos de teste
    std::vector<std::string> testFiles = {"teste1.txt", "teste2.txt", "teste3.txt", "teste4.txt", "teste5.txt", "teste6.txt", "teste7.txt", "teste8.txt", "teste9.txt"};

    std::cout << "\n===== CLASSIFICAÇÃO DE TEXTOS =====\n" << std::endl;
    for (const auto& testFile : testFiles) {
        std::string testText = loadText({testFile});
        if (testText.empty()) {
            std::cout << "Arquivo " << testFile << " está vazio ou não pôde ser lido. Pulando..." << std::endl;
            continue;
        }
        
        std::cout << "Classificando " << testFile << " (" << testText.size() << " bytes)..." << std::endl;
        std::string genre = classifyText(testText, genreDicts);
        std::cout << "Resultado: Arquivo " << testFile << " classificado como: " << genre << "\n" << std::endl;
    }

    return 0;
}