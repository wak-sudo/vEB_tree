// Wojciech Kieloch 2024

#include "vEB.h"

#include <stdexcept>
#include <cmath>


// Maximal count of the structure is 2^powerOfTwo
vEB::vEB(u64 powerOfTwo)
{
    if (powerOfTwo == 0)
        throw std::runtime_error("Invalid tree size: power lesser than 1.");
    root = constructTree(std::pow(2, powerOfTwo));
}

vEB::~vEB()
{
    destroyTree(root);
}

void vEB::remove(u64 val)
{
    checkInput(val);
    if (find(val))
    {
        removeRec(root, val);
        size--;
    }
}

bool vEB::find(u64 val) const
{
    checkInput(val);
    return findRec(root, val);
}

std::optional<u64> vEB::min() const
{
    return root->min;
}

std::optional<u64> vEB::max() const
{
    return root->max;
}

void vEB::insert(u64 val)
{
    checkInput(val);

    if (!find(val))
    {
        size++;
        insertRec(root, val);
    }
}

u64 vEB::getSize() const
{
    return size;
}

std::vector<u64> vEB::getArray() const
{
    const std::optional<u64> treeMin = root->min;
    if (!treeMin.has_value())
        return std::vector<u64>();

    std::vector<u64> res;
    res.push_back(treeMin.value());
    std::optional<u64> currentVal = successorRec(root, treeMin.value());
    while (currentVal.has_value())
    {
        res.push_back(currentVal.value());
        currentVal = successorRec(root, currentVal.value());
    }
    return res;
}
