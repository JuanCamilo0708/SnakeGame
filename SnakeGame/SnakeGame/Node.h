#pragma once
#include <float.h>
struct Node
{
    int row = 0;
    int col = 0;

    bool blocked = false;
    bool visited = false;

    float gCost = FLT_MAX;
    float hCost = 0.0f;
    float fCost = 0.0f;

    Node* parent = nullptr;

    void Reset()
    {
        blocked = false;
        visited = false;
        gCost = 1e9f;
        hCost = 0.0f;
        fCost = 0.0f;
        parent = nullptr;
    }
};