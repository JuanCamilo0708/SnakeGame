#include "SnakeGame.h"

SnakeGame::SnakeGame()
{
	screenWidth = 500;
	screenHeight = 800;
	cellSize = 25;
	topBarHeight = 50;
	bottomBarHeight = 50;
	gameHeight = screenHeight - (topBarHeight + bottomBarHeight);
	gridWidth = screenWidth / cellSize;
	gridHeight = gameHeight / cellSize;

	grid.resize(gridHeight, std::vector<Node>(gridWidth));
	for (int r = 0; r < gridHeight; ++r)
		for (int c = 0; c < gridWidth; ++c)
		{
			grid[r][c].row = r;
			grid[r][c].col = c;
		}
}

void SnakeGame::Main()
{
	InitWindow(screenWidth, screenHeight, "Snake Game - Raylib");
	SetTargetFPS(60);

	Start();

	while (!WindowShouldClose())
	{
		Interaction();
		Update();

		BeginDrawing();
		ClearBackground(BLACK);
		Draw();
		EndDrawing();
	}

	CloseWindow();
}

void SnakeGame::Start()
{
	ResetSnake();
	SpawnFood();
	state = GameState::Playing;
	autoPilotCharges = 0;

	applesInARow = 0;
	autoPilot = false;
	autoPilotTimer = 0.0f;
}

void SnakeGame::ResetSnake()
{
	snake.clear();
	int midX = gridWidth / 2;
	int midY = gridHeight / 2;

	snake.clear();
	snake.push_back({ midX, midY });
	snake.push_back({ midX - 1, midY });
	snake.push_back({ midX - 2, midY });

	dir = RIGHT;
	nextDir = RIGHT;
	canChangeDir = true;

	moveTimer = 0;
	aiPathIndex = 0;
	aStarPath.clear();
}

void SnakeGame::SpawnFood()
{
	while (true)
	{
		int x = GetRandomValue(0, gridWidth - 1);
		int y = GetRandomValue(0, gridHeight - 1);

		bool onSnake = false;

		for (auto& s : snake)
		{
			if (s.x == x && s.y == y)
			{
				onSnake = true;
				break;
			}
		}

		if (!onSnake)
		{
			food.x = x;
			food.y = y;
			return;
		}
	}
}

void SnakeGame::Interaction()
{
	if (state == GameState::GameOver)
	{
		if (IsKeyPressed(KEY_ENTER))
			Start();
		return;
	}

	if (!autoPilot && autoPilotCharges > 0 && IsKeyPressed(KEY_SPACE))
	{
		autoPilot = true;
		autoPilotTimer = autoPilotDuration;
		autoPilotCharges--;

		ComputeAIPath();
		aiPathIndex = 0;
	}

	if (autoPilot)
		return;

	if (!canChangeDir)
		return;

	Direction candidate = nextDir;

	if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
		candidate = UP;
	if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
		candidate = DOWN;
	if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
		candidate = LEFT;
	if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
		candidate = RIGHT;

	bool opposite =
		(dir == UP && candidate == DOWN) ||
		(dir == DOWN && candidate == UP) ||
		(dir == LEFT && candidate == RIGHT) ||
		(dir == RIGHT && candidate == LEFT);

	if (!opposite && candidate != dir)
	{
		nextDir = candidate;
		canChangeDir = false;
	}
}


void SnakeGame::Update()
{
	if (state == GameState::GameOver)
		return;
	if (autoPilot)
	{
		autoPilotTimer -= GetFrameTime();
		if (autoPilotTimer <= 0.0f)
		{
			autoPilot = false;
		}
	}

	moveTimer += GetFrameTime();
	if (moveTimer < moveDelay) return;
	moveTimer = 0;


	if (autoPilot)
	{

		if (aiPathIndex >= aStarPath.size())
		{
			ComputeAIPath();
			aiPathIndex = 0;
		}

		if (!aStarPath.empty() && aiPathIndex < aStarPath.size())
		{
			Node* next = aStarPath[aiPathIndex];
			int dx = next->col - snake[0].x;
			int dy = next->row - snake[0].y;

			if (dx == 1)  nextDir = RIGHT;
			if (dx == -1) nextDir = LEFT;
			if (dy == 1)  nextDir = DOWN;
			if (dy == -1) nextDir = UP;

			aiPathIndex++;
		}
		else
		{
			// Fallback safe movement
			auto safe = [&](Direction d)
				{
					int nx = snake[0].x;
					int ny = snake[0].y;

					if (d == UP)    ny--;
					if (d == DOWN)  ny++;
					if (d == LEFT)  nx--;
					if (d == RIGHT) nx++;

					if (nx < 0 || nx >= gridWidth || ny < 0 || ny >= gridHeight)
						return false;

					for (auto& s : snake)
						if (s.x == nx && s.y == ny)
							return false;

					return true;
				};

			if (safe(dir)) nextDir = dir;
			else if (safe(RIGHT)) nextDir = RIGHT;
			else if (safe(LEFT)) nextDir = LEFT;
			else if (safe(UP)) nextDir = UP;
			else if (safe(DOWN)) nextDir = DOWN;
		}
	}


	dir = nextDir;
	canChangeDir = true;


	for (int i = (int)snake.size() - 1; i > 0; i--)
		snake[i] = snake[i - 1];


	if (dir == UP) snake[0].y--;
	if (dir == DOWN) snake[0].y++;
	if (dir == LEFT) snake[0].x--;
	if (dir == RIGHT) snake[0].x++;


	if (snake[0].x < 0 || snake[0].x >= gridWidth ||
		snake[0].y < 0 || snake[0].y >= gridHeight)
	{
		state = GameState::GameOver;
		return;
	}

	if (snake[0].x == food.x && snake[0].y == food.y)
	{
		snake.push_back(snake.back());
		SpawnFood();

		applesInARow++;

		if (applesInARow >= 10)
		{
			applesInARow = 0;
			if (autoPilotCharges < autoPilotMaxCharges)
				autoPilotCharges++;
		}
	}

	for (int i = 1; i < (int)snake.size(); i++)
	{
		if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
		{
			state = GameState::GameOver;
			return;
		}
	}
}

void SnakeGame::Draw()
{
	int gameTop = topBarHeight;
	int gameBottom = screenHeight - bottomBarHeight;

	DrawRectangle(0, 0, screenWidth, topBarHeight, Fade(BLACK, 0.5f));

	DrawText(TextFormat("Score: %i", (int)snake.size() - 3),
		20, 15, 20, WHITE);

	if (autoPilot)
	{
		DrawText(TextFormat("AUTO: %.1f", autoPilotTimer),
			screenWidth - 150, 15, 20, YELLOW);
	}
	else
	{
		DrawText("AUTO: OFF",
			screenWidth - 150, 15, 20, GRAY);
	}

	DrawRectangleLines(0, gameTop, screenWidth, gameBottom - gameTop, DARKGRAY);


	DrawRectangle(food.x * cellSize,
		food.y * cellSize + gameTop,
		cellSize, cellSize, RED);

	for (auto& s : snake)
	{
		if (s.x < 0 || s.x >= gridWidth ||
			s.y < 0 || s.y >= gridHeight)
			continue;
		DrawRectangle(s.x * cellSize,
			s.y * cellSize + gameTop,
			cellSize, cellSize, GREEN);
	}

	DrawRectangle(0, screenHeight - bottomBarHeight,
		screenWidth, bottomBarHeight,
		Fade(BLACK, 0.5f));

	DrawText(TextFormat("Fruits in a row: %i", applesInARow),
		20, screenHeight - bottomBarHeight + 15,
		20, WHITE);
	int slotSize = 30;
	int spacing = 10;
	int startX = screenWidth - (slotSize * 3 + spacing * 2) - 20;
	int y = screenHeight - bottomBarHeight + (bottomBarHeight - slotSize) / 2;

	for (int i = 0; i < 3; i++)
	{
		Color col = (i < autoPilotCharges) ? YELLOW : DARKGRAY;

		DrawRectangle(startX + i * (slotSize + spacing),
			y,
			slotSize,
			slotSize,
			col);

		DrawRectangleLines(startX + i * (slotSize + spacing),
			y,
			slotSize,
			slotSize,
			BLACK);
	}
	if (state == GameState::GameOver)
	{
		DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));

		DrawText("GAME OVER",
			screenWidth / 2 - MeasureText("GAME OVER", 40) / 2,
			screenHeight / 2 -  100,
			40, RED);
		DrawText(TextFormat("Score: %i", (int)snake.size() - 3),
			screenWidth / 2 - MeasureText(TextFormat("Score: %i", (int)snake.size() - 3), 40) / 2,
			screenHeight / 2 - 60,
			40, RED);
		DrawText("Press ENTER to restart",
			screenWidth / 2 - MeasureText("Press ENTER to restart", 20) / 2,
			screenHeight / 2 + 10,
			20, WHITE);
	}
}