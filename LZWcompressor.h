#ifndef LZW_COMPRESSOR_H
#define LZW_COMPRESSOR_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

class LZWCompressor {
public:
    // Comprime uma string usando o algoritmo LZW
    static std::vector<int> compress(const std::string& input) {
        std::unordered_map<std::string, int> dictionary;
        // Inicializa o dicionário com caracteres ASCII
        for (int i = 0; i < 256; i++) {
            dictionary[std::string(1, char(i))] = i;
        }

        std::string w;
        std::vector<int> result;
        int dictSize = 256;

        for (char c : input) {
            std::string wc = w + c;
            if (dictionary.find(wc) != dictionary.end()) {
                w = wc;
            } else {
                result.push_back(dictionary[w]);
                dictionary[wc] = dictSize++;
                w = std::string(1, c);
            }
        }

        // Não se esqueça do último código
        if (!w.empty()) {
            result.push_back(dictionary[w]);
        }

        return result;
    }

    // Descomprime uma sequência de códigos usando o algoritmo LZW
    static std::string decompress(const std::vector<int>& compressed) {
        // Caso de entrada vazia
        if (compressed.empty()) {
            return "";
        }

        std::unordered_map<int, std::string> dictionary;
        // Inicializa o dicionário com caracteres ASCII
        for (int i = 0; i < 256; i++) {
            dictionary[i] = std::string(1, char(i));
        }

        int dictSize = 256;
        int oldCode = compressed[0];
        std::string result = dictionary[oldCode];
        std::string entry;
        
        for (size_t i = 1; i < compressed.size(); i++) {
            int currentCode = compressed[i];
            
            // Caso especial: código ainda não existe no dicionário
            if (currentCode == dictSize) {
                entry = dictionary[oldCode] + dictionary[oldCode][0];
            } 
            // Caso normal: código já existe no dicionário
            else if (dictionary.find(currentCode) != dictionary.end()) {
                entry = dictionary[currentCode];
            } 
            // Caso de erro: código não existe e não é o próximo a ser adicionado
            else {
                throw std::runtime_error("Código inválido encontrado durante a descompressão");
            }
            
            result += entry;
            
            // Adiciona nova sequência ao dicionário
            dictionary[dictSize++] = dictionary[oldCode] + entry[0];
            oldCode = currentCode;
        }

        return result;
    }
};

#endif // LZW_COMPRESSOR_H