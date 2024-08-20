// Wojciech Kieloch 2024

#include "vEB.h"

#include <cmath>
#include <stdexcept>

vEB::Node *vEB::constructTree(u64 uniSize)
{
    Node *newNode = new Node();
    newNode->uniSize = uniSize;
    if (uniSize == 2)
        return newNode;
    const u64 noOfClusters = upperSqrt(uniSize);
    const u64 childSize = lowerSqrt(uniSize);
    newNode->clusters = new Node *[noOfClusters];
    newNode->summary = constructTree(noOfClusters);
    for (u64 i = 0; i != noOfClusters; i++)
        newNode->clusters[i] = constructTree(childSize);
    return newNode;
}

void vEB::destroyTree(Node *currentNode)
{
    if (currentNode == nullptr)
        return;
    if (currentNode->uniSize == 2)
    {
        delete currentNode;
        return;
    }

    const u64 noOfClusters = upperSqrt(currentNode->uniSize);
    for (u64 i = 0; i != noOfClusters; i++)
        destroyTree(currentNode->clusters[i]);
    destroyTree(currentNode->summary);
    delete currentNode;
}

u64 vEB::high(u64 val, u64 uniSize)
{
    return std::floor(val / lowerSqrt(uniSize));
}

u64 vEB::low(u64 val, u64 uniSize)
{
    return val % lowerSqrt(uniSize);
}

bool vEB::findRec(const Node *currentNode, u64 orgVal) const
{
    if (!(currentNode->min.has_value())) // no value in the tree;
        return false;

    if (currentNode->min.value() == orgVal || currentNode->max.value() == orgVal)
        return true;

    if (currentNode->uniSize == 2)
        return false;

    const u64 uniSize = currentNode->uniSize;
    const u64 specificCell = high(orgVal, uniSize);
    return findRec(currentNode->clusters[specificCell], low(orgVal, uniSize));
}

void vEB::insertRec(Node *currentNode, u64 val)
{
    if (!(currentNode->min.has_value()))
    {
        currentNode->min = val;
        currentNode->max = val;
        return;
    }

    if (val < currentNode->min.value())
    {
        const u64 temp = currentNode->min.value();
        currentNode->min = val;
        val = temp;
    }

    if (currentNode->uniSize > 2)
    {
        const u64 highVal = high(val, currentNode->uniSize);
        const u64 lowVal = low(val, currentNode->uniSize);

        if (currentNode->clusters[highVal]->min.has_value()) // is not empty
        {
            insertRec(currentNode->clusters[highVal], lowVal);
        }
        else // has no value
        {
            insertRec(currentNode->summary, highVal);
            currentNode->clusters[highVal]->min = lowVal;
            currentNode->clusters[highVal]->max = lowVal;
        }
    }

    if (val > currentNode->max.value())
    {
        currentNode->max = val;
    }
}

std::optional<u64> vEB::successorRec(const Node *currentNode, u64 orgVal) const
{
    // works iff provied value is present in the set.
    if (currentNode->uniSize == 2)
    {
        if (orgVal == 0 && currentNode->max.has_value() && currentNode->max.value() == 1)
            return 1;
        else
            return std::optional<u64>();
    }

    if (currentNode->min.has_value() && orgVal < currentNode->min.value())
        return currentNode->min.value();

    const u64 highVal = high(orgVal, currentNode->uniSize);
    const u64 lowVal = low(orgVal, currentNode->uniSize);
    const std::optional<u64> maxLow = currentNode->clusters[highVal]->max;

    if (maxLow.has_value() && lowVal < maxLow.value())
    {
        const std::optional<u64> offset = successorRec(currentNode->clusters[highVal], lowVal);
        return highVal * lowerSqrt(currentNode->uniSize) + offset.value();
    }
    else
    {
        const std::optional<u64> nextClusterIdx = successorRec(currentNode->summary, highVal);
        if (nextClusterIdx.has_value())
        {
            const u64 offset = currentNode->clusters[nextClusterIdx.value()]->min.value();
            return nextClusterIdx.value() * lowerSqrt(currentNode->uniSize) + offset;
        }
        else
            return std::optional<u64>();
    }
}

u64 vEB::upperSqrt(u64 x)
{
    return std::pow(2, std::ceil(std::log2(x) / 2));
}

u64 vEB::lowerSqrt(u64 x)
{
    return std::pow(2, std::floor(std::log2(x) / 2));
}

std::optional<u64> vEB::predecessorRec(const Node *currentNode, u64 val) const
{
    if (currentNode->uniSize == 2)
    {
        if (val == 1 && currentNode->min.has_value() && currentNode->min.value() == 0) // why not check this.
            return 0;
        else
            return std::optional<u64>();
    }

    if (currentNode->max.has_value() && currentNode->max.value() < val)
        return currentNode->max.value();

    const u64 highVal = high(val, currentNode->uniSize);
    const u64 lowVal = low(val, currentNode->uniSize);
    const std::optional<u64> minLow = currentNode->clusters[highVal]->min;

    if (minLow.has_value() && minLow.value() < lowVal)
    {
        const std::optional<u64> offset = predecessorRec(currentNode->clusters[highVal], lowVal);
        return highVal * lowerSqrt(currentNode->uniSize) + offset.value();
    }
    else
    {
        const std::optional<u64> prevClusterIdx = predecessorRec(currentNode->summary, highVal);
        if (prevClusterIdx.has_value())
        {
            const u64 offset = currentNode->clusters[prevClusterIdx.value()]->max.value();
            return prevClusterIdx.value() * lowerSqrt(currentNode->uniSize) + offset;
        }
        else
        {
            if (currentNode->min.has_value() && currentNode->min.value() < val)
                return currentNode->min.value();
            else
                return std::optional<u64>();
        }
    }
}

void vEB::removeRec(Node *currentNode, u64 val)
{
    if (currentNode->max == currentNode->min) // has one element
        currentNode->max = currentNode->min = std::optional<u64>();
    else if (currentNode->uniSize == 2) // has two elemetns
    {
        if (val == 0)
            currentNode->min = 1;
        else
            currentNode->min = 0;
        currentNode->max = currentNode->min;
    }
    else
    {
        if (currentNode->min.value() == val)
        {
            const u64 nextMinIdx = currentNode->summary->min.value();
            const u64 nextMinVal = nextMinIdx * lowerSqrt(currentNode->uniSize) + currentNode->clusters[nextMinIdx]->min.value();
            currentNode->min = nextMinVal;
            val = nextMinVal;
        }

        const u64 highVal = high(val, currentNode->uniSize);
        const u64 lowVal = low(val, currentNode->uniSize);
        removeRec(currentNode->clusters[highVal], lowVal);
        if (!currentNode->clusters[highVal]->min.has_value()) // does not have elements now
        {
            removeRec(currentNode->summary, highVal);
            if (val == currentNode->max.value())
            {
                const std::optional<u64> predMaxIdx = currentNode->summary->max;
                if (predMaxIdx.has_value())
                {
                    const u64 newMaxValue = predMaxIdx.value() * lowerSqrt(currentNode->uniSize) + currentNode->clusters[predMaxIdx.value()]->max.value();
                    currentNode->max = newMaxValue;
                }
                else
                    currentNode->max = currentNode->min;
            }
        }
        else if (val == currentNode->max.value())
        {
            const u64 newMaxValue = highVal * lowerSqrt(currentNode->uniSize) + currentNode->clusters[highVal]->max.value();
            currentNode->max = newMaxValue;
        }
    }
}

bool vEB::inputIsValid(u64 x) const
{
    return root->uniSize > x;
}

void vEB::checkInput(u64 x) const noexcept(false) 
{
    if(!inputIsValid(x))
        throw std::runtime_error("Input out of range of the structure.");
}
