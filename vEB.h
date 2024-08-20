// Wojciech Kieloch 2024

#pragma once

#include <optional>
#include <vector>

typedef unsigned long u64;

class vEB
{
    struct Node
    {
        Node **clusters = nullptr;
        std::optional<u64> min, max; // empty by default
        Node *summary = nullptr;
        u64 uniSize = 0;
    };

    Node *constructTree(u64 uniSizeArg);
    void destroyTree(Node *currentNode);

    void insertRec(Node *currentNode, u64 val);
    void removeRec(Node *currentNode, u64 val);
    bool findRec(const Node *currentNode, u64 val) const;
    std::optional<u64> successorRec(const Node *currentNode, u64 val) const;
    std::optional<u64> predecessorRec(const Node *currentNode, u64 val) const;

    bool inputIsValid(u64 x) const;
    void checkInput(u64 x) const noexcept(false);

    static u64 high(u64 val, u64 uniSize);
    static u64 low(u64 val, u64 uniSize);
    static u64 upperSqrt(u64 x);
    static u64 lowerSqrt(u64 x);

    Node *root = nullptr;
    u64 size = 0;

public:
    vEB(u64 powerOfTwo);
    ~vEB();
    void insert(u64 val);
    bool find(u64 val) const;
    void remove(u64 val);
    u64 getSize() const;

    std::optional<u64> min() const;
    std::optional<u64> max() const;

    std::vector<u64> getArray() const;
};
