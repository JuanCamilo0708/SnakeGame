#pragma once
#include "Node.h"
#include <raylib.h>
#include <vector>
#include <stdio.h>
#include <string>


class SnakeGame
{
public:
    SnakeGame();
    void Main();         // Raylib window + loop
    void Start();        // Initialize game state
    void Interaction();  // Handle player input
    void Update();       // Game logic
    void Draw();         // Rendering

private:
    struct Segment { int x, y; };

    int cellSize;
    int gridWidth;
    int gridHeight;

    int screenWidth;
    int screenHeight;
    int topBarHeight;
    int bottomBarHeight;
    int gameHeight;

    std::vector<Segment> snake;
    Segment food;

    enum Direction { UP, DOWN, LEFT, RIGHT };
    Direction dir;
    Direction nextDir;
    bool canChangeDir = true;

    enum class GameState { Playing, GameOver };
    GameState state = GameState::Playing;

    float moveTimer = 0;
    float moveDelay = 0.15f;

    int autoPilotCharges = 0;
    int autoPilotMaxCharges = 3;
    bool autoPilot = false;
    int applesInARow = 0;
    float autoPilotTimer = 0.0f;
    float autoPilotDuration = 10.0f;

    std::vector<std::vector<Node>> grid;
    std::vector<Node*> aStarPath;
    int aiPathIndex = 0;

    void ResetSnake();
    void SpawnFood();

    void BuildGridFromSnake();
    std::vector<Node*> GetNeighbours(Node* node);
    bool ComputeAIPath();
};