#include <cassert>
#include <iostream>
#include <fstream>
#include "gtest.h"
#include "../lib/HuffmanTree.h"

bool compareFiles(const std::string &q1, const std::string &q2) {
    std::ifstream file1(q1, std::ifstream::binary);
    std::ifstream file2(q2, std::ifstream::binary);

    if (file1.fail() || file2.fail()) {
        return false;
    }

    if (file1.tellg() != file2.tellg()) {
        return false;
    }

    file1.seekg(0, std::ifstream::beg);
    file2.seekg(0, std::ifstream::beg);
    return std::equal(std::istreambuf_iterator<char>(file1.rdbuf()),
                      std::istreambuf_iterator<char>(),
                      std::istreambuf_iterator<char>(file2.rdbuf()));
}

void encode(const std::string &in, const std::string &out) {
    std::ifstream input(in, std::ifstream::binary);
    std::ofstream output(out, std::ofstream::binary);
    HuffmanTree::encode(input, output);
    input.close();
}

void decode(const std::string &in, const std::string &out) {
    std::ifstream input(in, std::ifstream::binary);
    std::ofstream output(out, std::ofstream::binary);

    HuffmanTree::decode(input, output);
}

bool encode_decode(const std::string &in) {
    encode(in, "encodedFile");
    decode("encodedFile", "decodedFile");

    return compareFiles(in, "decodedFile");

}

TEST(correctness, eightQ) {
    std::stringstream input("QQQQQQQQ");
    std::stringstream encode;
    std::stringstream decode;
    HuffmanTree::encode(input, encode);
    HuffmanTree::decode(encode, decode);

    EXPECT_EQ(input.str(), decode.str());
}


TEST(correctness, aaaaaaa) {
    std::stringstream input("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    std::stringstream encode;
    std::stringstream decode;
    HuffmanTree::encode(input, encode);
    HuffmanTree::decode(encode, decode);

    EXPECT_EQ(input.str(), decode.str());
}

TEST(correctness, corrupted) {
    std::stringstream input("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
    std::stringstream encode;
    std::stringstream decode;
    HuffmanTree::encode(input, encode);
    char c = 'a';
    encode.write(&c, sizeof(char));
    EXPECT_ANY_THROW(HuffmanTree::decode(encode, decode));
}

TEST(correctness, empty_file) {
    EXPECT_TRUE(encode_decode("../test/empty"));
}

TEST(correctness, jpg) {
    EXPECT_TRUE(encode_decode("../test/fff.jpg"));
}

TEST(correctness, randomString) {
    unsigned int count_of_strings = 1000;
    for (unsigned int i = 0; i < count_of_strings; i++) {
        std::string tmp;
        for (unsigned int j = 0; j < 5000; j++) {
            tmp += char(rand() % 256 - 128);
        }
        std::stringstream input(tmp);
        std::stringstream encode;
        std::stringstream decode;
        HuffmanTree::encode(input, encode);
        HuffmanTree::decode(encode, decode);

        EXPECT_EQ(input.str(), decode.str());
    }
}