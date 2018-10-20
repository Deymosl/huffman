cmake_minimum_required(VERSION 3.10)
project(Huffman)

set(CMAKE_CXX_STANDARD 17)

add_executable(Huffman
        test/huffmanTest.cpp
        lib/HuffmanTree.cpp
        lib/HuffmanTree.h
        test/gtest-all.cc
        test/gtest.h
        test/gtest_main.cc
        console/main.cpp
     )



target_link_libraries(Huffman -lpthread)