#include "SnakeGame.h"
#include <queue>
#include <algorithm>
void SnakeGame::BuildGridFromSnake()
{
    for (int r = 0; r < gridHeight; ++r)
        for (int c = 0; c < gridWidth; ++c)
            grid[r][c].Reset();

    for (int i = 0; i < (int)snake.size() - 1; i++)
    {
        int x = snake[i].x;
        int y = snake[i].y;

        grid[y][x].blocked = true;
    }
}

std::vector<Node*> SnakeGame::GetNeighbours(Node* node)
{
    std::vector<Node*> result;
    int row = node->row;
    int col = node->col;

    if (row > 0)               result.push_back(&grid[row - 1][col]);
    if (row < gridHeight - 1)  result.push_back(&grid[row + 1][col]);
    if (col > 0)               result.push_back(&grid[row][col - 1]);
    if (col < gridWidth - 1)   result.push_back(&grid[row][col + 1]);

    return result;
}

bool SnakeGame::ComputeAIPath()
{
    aStarPath.clear();
    aiPathIndex = 0;

    BuildGridFromSnake();

    auto Heuristic = [](Node* a, Node* b)
        {
            return (float)(abs(a->col - b->col) + abs(a->row - b->row));
        };

    auto CompareStar = [](Node* a, Node* b)
        {
            if (a->fCost == b->fCost)
                return a->hCost > b->hCost;
            return a->fCost > b->fCost;
        };

    Node* startNode = &grid[snake[0].y][snake[0].x];
    Node* endNode = &grid[food.y][food.x];
    startNode->gCost = 0.0f;
    startNode->hCost = Heuristic(startNode, endNode);
    startNode->fCost = startNode->hCost;
    startNode->parent = nullptr;

    std::priority_queue<Node*, std::vector<Node*>, decltype(CompareStar)> openSet(CompareStar);

    startNode->gCost = 0.0f;
    startNode->hCost = Heuristic(startNode, endNode);
    startNode->fCost = startNode->hCost;

    openSet.push(startNode);

    while (!openSet.empty())
    {
        Node* current = openSet.top();
        openSet.pop();

        if (current->visited)
            continue;

        current->visited = true;

        if (current == endNode)
        {
            Node* p = endNode;
            while (p != startNode)
            {
                aStarPath.push_back(p);
                p = p->parent;
            }
            std::reverse(aStarPath.begin(), aStarPath.end());
            return true;
        }

        for (Node* n : GetNeighbours(current))
        {
            if (n->blocked || n->visited) continue;

            float newCost = current->gCost + 1.0f;
            if (newCost < n->gCost)
            {
                n->gCost = newCost;
                n->hCost = Heuristic(n, endNode);
                n->fCost = n->gCost + n->hCost;
                n->parent = current;
                openSet.push(n);
            }
        }
    }

    return false;
}