#ifndef HUFFMAN_HUFFMANTREE_H
#define HUFFMAN_HUFFMANTREE_H

#include <iosfwd>
#include <memory>
#include <vector>
#include <iostream>

class HuffmanTree {
public:
    static void encode(std::istream &in, std::ostream &out);
    static void decode(std::istream &in, std::ostream &out);
private:
    struct Node{
        char c;
        int w;
        std::shared_ptr<Node> l, r;
        Node() : l(nullptr), r(nullptr) {};
        Node(char c, int weight) : c(c), w(weight), l(nullptr), r(nullptr) {};

        bool operator<(const Node& q) const
        {
            return w < q.w;
        }
    };
    struct Comparator {
        bool operator()(const std::shared_ptr<Node> &q, const std::shared_ptr<Node> &w) const {
            return q->w < w->w;
        }
    };


    static uint32_t getLen(const std::shared_ptr<Node> &q, std::vector <int> &count, uint32_t depth);
    static void detour(std::shared_ptr<Node> q, std::vector <bool> &cur);
    static void detour(std::shared_ptr<Node> q, std::vector <char> &cur);
    static void pushBuffer(std::ostream &out, std::vector <bool> &v, bool flag);
    static std::shared_ptr<Node> buildTree(std::vector<int> &count);
    static void buildTree(std::shared_ptr<Node> &q, std::vector<bool> &detour, int &dpos, std::vector<char> &letters, int &lpos);
    static void encodeTree(std::shared_ptr<Node> q, std::vector<bool> encodedLetters[], std::vector<bool> cur);
};


#endif //HUFFMAN_HUFFMANTREE_H