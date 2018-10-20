#include <fstream>
#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>
#include <set>
#include "HuffmanTree.h"

void HuffmanTree::encode(std::istream &in, std::ostream &out) {
    if (in.peek() == EOF) {
        return;
    }

    std::vector<int> count(256);
    while (in.peek() != EOF) {
        char c;
        in.read(&c, sizeof(char));
        count[c + 128]++;
    }

    std::shared_ptr<Node> root = buildTree(count);
    std::vector<bool> encoded_letter[256];
    encodeTree(root, encoded_letter, std::vector<bool>(0));

    std::vector<bool> detoured_tree;
    detour(root, detoured_tree);
    detoured_tree.pop_back();

    auto size_Tree = static_cast<uint16_t>(detoured_tree.size());
    out.write((char *) &size_Tree, sizeof(uint16_t));
    pushBuffer(out, detoured_tree, true);

    std::vector<char> detoured_letters;
    detour(root, detoured_letters);
    auto size_letters = static_cast<uint16_t>(detoured_letters.size());
    out.write((char *) &size_letters, sizeof(uint16_t));
    for (char c: detoured_letters) {
        out.write(&c, sizeof(char));
    }

    uint32_t len = getLen(root, count, 0);  // count of ignore bites
    out.write((char *) &len, sizeof(uint32_t));

    in.seekg(0, std::ios::beg);
    std::vector<bool> buffer;
    while (in.peek() != EOF) {
        char c;
        in.read(&c, sizeof(char));
        buffer.insert(buffer.end(), encoded_letter[c + 128].begin(), encoded_letter[c + 128].end());
        pushBuffer(out, buffer, false);
    }
    pushBuffer(out, buffer, true);
}

void HuffmanTree::decode(std::istream &in, std::ostream &out) {
    if (in.peek() == EOF) {
        return;
    }

    uint16_t size_Tree;
    in.read((char *) &size_Tree, sizeof(uint16_t));
    if (size_Tree <= 0 || size_Tree > 1024) {
        throw std::exception();
    }
    std::vector<bool> detoured_tree;
    for (int i = 0; i < (size_Tree + 7) / 8; i++) {
        uint8_t detoured;
        in.read((char *) &detoured, sizeof(uint8_t));
        for (int j = 0; j < 8; j++) {
            detoured_tree.push_back(bool(detoured & (1 << 7)));
            detoured <<= 1;
        }
    }
    if (detoured_tree.size() > size_Tree + static_cast<uint32_t >(8)) {
        throw std::exception();
    }
    while (detoured_tree.size() != size_Tree) {
        detoured_tree.pop_back();
    }

    uint16_t countLetters;
    in.read((char *) &countLetters, sizeof(uint16_t));
    if (countLetters < 0 || countLetters > 256) {
        throw std::exception();
    }

    std::vector<char> letters(countLetters);
    for (int i = 0; i < countLetters; i++) {
        in.read(&letters[i], sizeof(char));
    }
    std::shared_ptr<Node> root = std::make_shared<Node>();
    int dpos = 0;
    int lpos = 0;
    buildTree(root, detoured_tree, dpos, letters, lpos);

    uint32_t numberOfBites;
    in.read((char *) &numberOfBites, sizeof(uint32_t));
    int ignored = (8 - numberOfBites % 8) % 8;

    std::shared_ptr<Node> curr = root;
    uint32_t readCount = 0;
    while (in.peek() != EOF) {
        uint8_t byte;
        in.read((char *) &byte, sizeof(uint8_t));
        readCount += 8;
        if (readCount >= numberOfBites + 8) {
            throw std::exception();
        }
        int ignore = 0;
        if (in.peek() == EOF) {
            ignore = ignored;
        }
        for (int j = 0; j < 8 - ignore; j++) {
            bool x = bool(byte & (1 << 7));
            if (x == 0) {
                if (curr->l == nullptr) {
                    throw std::exception();
                }
                curr = curr->l;
            } else {
                if (curr->r == nullptr) {
                    throw std::exception();
                }
                curr = curr->r;
            }
            if (curr->l == nullptr) {
                out.write(&(curr->c), sizeof(char));
                curr = root;
            }
            byte <<= 1;
        }
    }
}

std::shared_ptr<HuffmanTree::Node> HuffmanTree::buildTree(std::vector<int> &count) {
    std::vector<std::pair<int, char> > w;
    for (size_t i = 0; i < count.size(); i++) {
        if (count[i] != 0) {
            w.push_back(std::make_pair(count[i], (char) (i - 128)));
        }
    }
    std::sort(w.begin(), w.end());

    if (w.size() == 1) {
        std::shared_ptr<Node> list = std::make_shared<Node>(w[0].second, w[0].first);
        std::shared_ptr<Node> root = std::make_shared<Node>();
        root->l = list;
        return root;
    }

    std::multiset<std::shared_ptr<Node>, Comparator> s;
    for (auto x: w) {
        s.insert(std::make_shared<Node>(x.second, x.first));
    }
    while (s.size() > 1) {
        std::shared_ptr<Node> a = *s.begin();
        s.erase(s.begin());
        std::shared_ptr<Node> b = *s.begin();
        s.erase(s.begin());
        std::shared_ptr<Node> tmp = std::make_shared<Node>();
        tmp->w = a->w + b->w;
        tmp->l = a;
        tmp->r = b;
        s.insert(tmp);
    }

    return *s.begin();
}

void HuffmanTree::encodeTree(std::shared_ptr<Node> q, std::vector<bool> encLetters[], std::vector<bool> curr) {
    if (q->l == nullptr && q->r == nullptr) {
        encLetters[q->c + 128] = curr;
        return;
    }
    if (q->l != nullptr) {
        curr.push_back(false);
        encodeTree(q->l, encLetters, curr);
        curr.pop_back();
    }
    if (q->r != nullptr) {
        curr.push_back(true);
        encodeTree(q->r, encLetters, curr);
        curr.pop_back();
    }
}

void HuffmanTree::pushBuffer(std::ostream &out, std::vector<bool> &v, bool flag) {
    for (int i = 0; i < static_cast<int>((v.size() / 8) * 8); i += 8) {
        char tmp = 0;
        for (int j = i; j < i + 8; j++) {
            tmp <<= 1;
            tmp += v[j];
        }
        out.write(&tmp, sizeof(char));
    }

    if (flag) {
        if (v.size() % 8 != 0) {
            unsigned char tmp = 0;
            for (auto i = static_cast<int>((v.size() / 8) * 8); i < static_cast<int>(v.size()); i++) {
                tmp <<= 1;
                tmp += v[i];
            }
            tmp <<= (8 - v.size() % 8);
            out.write((char *) &tmp, sizeof(char));
        }
    } else {
        std::vector<bool> tmp;
        for (auto i = static_cast<int>((v.size() / 8) * 8); i < static_cast<int>(v.size()); i++) {
            tmp.push_back(v[i]);
        }
        v = tmp;
    }
}

void HuffmanTree::detour(std::shared_ptr<Node> q, std::vector<bool> &curr) {
    if (q->l != nullptr) {
        curr.push_back(false);
        detour(q->l, curr);
    }
    if (q->r != nullptr) {
        curr.push_back(false);
        detour(q->r, curr);
    }
    curr.push_back(true);
}

void HuffmanTree::detour(std::shared_ptr<Node> q, std::vector<char> &curr) {
    if (q->l == nullptr && q->r == nullptr) {
        curr.push_back(q->c);
    }
    if (q->l != nullptr) {
        detour(q->l, curr);
    }
    if (q->r != nullptr) {
        detour(q->r, curr);
    }
}

uint32_t HuffmanTree::getLen(const std::shared_ptr<Node> &q, std::vector<int> &count, uint32_t deep) {
    if (q->l == nullptr && q->r == nullptr) {
        return count[q->c + 128] * deep;
    }
    uint32_t sum = 0;
    if (q->l != nullptr) {
        sum += getLen(q->l, count, deep + 1);
    }
    if (q->r != nullptr) {
        sum += getLen(q->r, count, deep + 1);
    }
    return sum;
}

void
HuffmanTree::buildTree(std::shared_ptr<Node> &q, std::vector<bool> &detoured, int &dpos, std::vector<char> &letters,
                       int &lpos) {
    if (detoured[dpos] == 1) {
        if (q->l == nullptr && q->r == nullptr) {
            q->c = letters[lpos++];
        }
    } else {
        q->l = std::make_shared<Node>();
        buildTree(q->l, detoured, ++dpos, letters, lpos);
        if (static_cast<size_t>(dpos) < detoured.size() && detoured[dpos] == 0) {
            q->r = std::make_shared<Node>();
            buildTree(q->r, detoured, ++dpos, letters, lpos);
        }
    }
    dpos++;
}