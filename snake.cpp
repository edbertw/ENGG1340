#include <ncurses.h>
#include <cstdlib>
#include <vector>
#include <ctime>
#include <chrono>
#include <thread>
#include <algorithm>
//not updated file -Edbert
using namespace std;

enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction dir;

const int width = 40;
const int height = 20;
int x, y, fruitX, fruitY, score;
bool gameOver;
vector<pair<int, int>> tail;

void InitColors() {
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK); // Red on black text
    init_pair(2, COLOR_CYAN, COLOR_BLACK); // Cyan on black text
    // Define more colors as needed
}

void GameOver() {
    clear(); // Clear the screen

    const char* digits[] = {
    // 0
    R"(
  _
 / _ \
| | | |
| | | |
| |_| |
 \_/
 )",
    // 1
    R"(
 __
/_ |
 | |
 | |
 | |
 |_|
)",
    // 2
    R"(
 _
|__ \
   ) |
  / /
 / /_
|__|
)",
    // 3
    R"(
 __
|_ \
  __) |
 |__ <
 _) |
|__/
)",
    // 4
    R"(
 _  _
| || |
| || |_
|__   _|
   | |
   |_|
)",
    // 5
    R"(
 __
| _|
| |__
|_ \
 _) |
|__/

)",
    // 6
    R"(
   __
  / /
 / /_
| '_ \
| (_) |
 \_/

)",
    // 7
    R"(
 ___
|_  |
   / /
  / /
 /_/

)",
    // 8
    R"(
  _
 / _ \
| (_) |
 > _ <
|()_|
 \_/

)",
    // 9
    R"(
  _
 / _ \
| (_) |
 \__, |
   / /
  /_/
)"
};


    const char* gameOverText =
    R"(
  __                         ___
 / _|                       / _ \
| |  _  _ _ _ _ __   _  | |  | |_   __ _ _
| | |_ |/ ` | ' ` _ \ / _ \ | |  | \ \ / / _ \ '__|
| |_| | (| | | | | | |  _/ | || |\ V /  _/ |
 \__|\,|| || ||\_|  \_/  \/ \_||
    )";

    mvprintw(1, 1, "%s", gameOverText);


    int scorePosX = (width / 2) - 6; // Assuming "Score: XXXX" is about 12 characters wide
    int scorePosY = 10; // Place the score below the "GAME OVER" text, adjust as needed

    // Print the score
    mvprintw(scorePosY, scorePosX, "Score: %d", score);

    // Move below the score to print the final message
    mvprintw(scorePosY + 2, scorePosX - 3, "Press 'q' to quit."); // Center the message
    refresh(); // Make sure the last message is shown

    // Wait for 'q' to be pressed before exiting
    nodelay(stdscr, FALSE); // Turn off non-blocking input to wait for a
}

void Setup() {
    initscr(); // Start curses mode
    clear(); // Clear the screen
    noecho(); // Don't echo any keypresses
    cbreak(); // Disable line buffering
    curs_set(0); // Hide the cursor

    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    tail.clear();

    keypad(stdscr, TRUE); // Enable keyboard mapping
    nodelay(stdscr, TRUE); // Make getch non-blocking
    timeout(100); // Set a delay for reading input to simulate game speed
}

void Draw() {
    clear(); // Clear the screen
    for (int i = 0; i < width + 2; i++)
        mvprintw(0, i, "#");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0)
                mvprintw(i + 1, 0, "#");
            if (i == y && j == x)
                mvprintw(i + 1, j + 1, "O");
            else if (i == fruitY && j == fruitX)
                mvprintw(i + 1, j + 1, "F");
            else {
                bool print = false;
                for (auto t : tail) {
                    if (t.first == j && t.second == i) {
                        mvprintw(i + 1, j + 1, "o");
                        print = true;
                    }
                }
                if (!print) mvprintw(i + 1, j + 1, " ");
            }
            if (j == width - 1)
                mvprintw(i + 1, width + 1, "#");
        }
    }
    for (int i = 0; i < width + 2; i++)
        mvprintw(height + 1, i, "#");
    mvprintw(height + 3, 0, "Score: %d", score);
    refresh(); // Print it on to the real screen
}

void Input() {
    int c = getch();
    switch(c) {
    case KEY_LEFT:
        if (dir != RIGHT) dir = LEFT;
        break;
    case KEY_RIGHT:
        if (dir != LEFT) dir = RIGHT;
        break;
    case KEY_UP:
        if (dir != DOWN) dir = UP;
        break;
    case KEY_DOWN:
        if (dir != UP) dir = DOWN;
        break;
    case 'q':
        gameOver = true;
        break;
    }
}

void Logic() {
    int prevX = x;
    int prevY = y;
    int prev2X, prev2Y;
    switch (dir) {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }
    // Wrap snake position around the screen
    if (x<0 || x>= width || y<0 || y>= height) {
        gameOver=true;
        }

    // Check collision with the tail
    for (auto t : tail) {
        if (t.first == x && t.second == y) {
            gameOver = true;
        }
    }

    // Check fruit collection
    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        tail.push_back(make_pair(x, y)); // Add a new segment to the tail at the current position
    } else if (!tail.empty()) {
        // Move tail segments forward
        prev2X = tail[0].first;
        prev2Y = tail[0].second;
        tail[0].first = prevX;
        tail[0].second = prevY;
        for (size_t i = 1; i < tail.size(); i++) {
            prevX = prev2X;
            prevY = prev2Y;
            prev2X = tail[i].first;
            prev2Y = tail[i].second;
            tail[i].first = prevX;
            tail[i].second = prevY;
        }
    }
}


int main() {
    srand(time(0)); // Seed the random number generator
    Setup();
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        // Control the speed of the game
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Sleep for 100 milliseconds
    }

    GameOver();

    while (true) {
        int ch = getch();
        if (ch == 'q') {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Check for input every 100 milliseconds
    }

    endwin(); // End curses mode
    return 0;
}
