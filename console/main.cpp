#include <iostream>
#include <fstream>
#include <vector>
#include "../lib/HuffmanTree.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cout << "Usage: <input_file> <output_file> <e|d>\n";
        return 0;
    }
    char const *input = argv[1];
    char const *output = argv[2];
    std::string mode = argv[3];
    std::ifstream in(input, std::ifstream::binary);
    std::ofstream out(output, std::ofstream::binary);
    if (mode == "e") {
        HuffmanTree::encode(in, out);
    } else if (mode == "d") {
        try {
            HuffmanTree::decode(in, out);
        } catch (std::exception e) {
            std::cerr << "corrupted file \n";
        }
    } else std::cout << "Usage: <input_file> <output_file> <e|d>\n";
}
