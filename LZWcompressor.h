#ifndef LZW_COMPRESSOR_H
#define LZW_COMPRESSOR_H

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

class LZWCompressor {
public:
    static std::unordered_map<std::string, int> generateDictionary(const std::string& input) {
        std::unordered_map<std::string, int> dictionary;
        for (int i = 0; i < 256; i++)
            dictionary[std::string(1, char(i))] = i;

        std::string w;
        int dictSize = 256;

        for (char c : input) {
            std::string wc = w + c;
            if (dictionary.count(wc))
                w = wc;
            else {
                dictionary[wc] = dictSize++;
                w = std::string(1, c);
            }
        }

        return dictionary;
    }

    static std::vector<int> compressWithDictionary(const std::string& input, const std::unordered_map<std::string, int>& baseDict) {
        auto dictionary = baseDict;
        std::string w;
        std::vector<int> result;
        int dictSize = dictionary.size();

        for (char c : input) {
            std::string wc = w + c;
            if (dictionary.count(wc))
                w = wc;
            else {
                result.push_back(dictionary.at(w));
                dictionary[wc] = dictSize++;
                w = std::string(1, c);
            }
        }

        if (!w.empty())
            result.push_back(dictionary.at(w));

        return result;
    }

    static std::string decompress(const std::vector<int>& compressed) {
        std::unordered_map<int, std::string> dictionary;
        for (int i = 0; i < 256; i++)
            dictionary[i] = std::string(1, char(i));

        int dictSize = 256;
        std::string w = dictionary[compressed[0]];
        std::string result = w;

        for (size_t i = 1; i < compressed.size(); i++) {
            int k = compressed[i];
            std::string entry;

            if (dictionary.count(k))
                entry = dictionary[k];
            else if (k == dictSize)
                entry = w + w[0];
            else
                throw std::runtime_error("Erro na descompressao");

            result += entry;
            dictionary[dictSize++] = w + entry[0];
            w = entry;
        }

        return result;
    }
};

#endif // LZW_COMPRESSOR_H