#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include <vector>
#include <string>
#include <utility>

using namespace std;

// Snake movement declaration
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };

//Structure for User
struct User {
    string username;
    string password;
    int highestScore;
};

// Declare all the headers of the functions in the game

void updateHighestScore(const string& filename);
bool isValidFruitPosition(int x, int y, const vector<pair<int, int>>& snake, const vector<pair<int, int>>& blocks);
void spawnFruit();
bool isValidBlockPosition(int x, int y, const vector<pair<int, int>>& blocks, const vector<pair<int, int>>& snake, int snakeX, int snakeY, int fruitX, int fruitY);
void spawnBlock(const vector<pair<int, int>>& snake, int snakeX, int snakeY, int fruitX, int fruitY);
void InitColors();
void GameOver();
void Setup();
void Draw();
void Input();
void Logic();

#endif
