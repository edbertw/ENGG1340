#include <ncurses.h>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std; 

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction dir;

const int width = 40;
const int height = 20;
int x, y, fruitX, fruitY, score, sleepDuration=100;
bool gameOver, growTail=false, restart=false;
vector<pair<int, int>> tail;
vector<pair<int, int>> blocks;

struct User {
    string username;
    string password;
    int highestScore;
};

User currentUser;
