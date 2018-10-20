cmake_minimum_required(VERSION 3.10)
project(huffman)

set(CMAKE_CXX_STANDARD 17)

add_executable(Huffman ../lib/HuffmanTree.cpp ../lib/HuffmanTree.h ../console/main.cpp)