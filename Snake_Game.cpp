#include "Main.h"
#include <fstream>
#include <iostream>
#include <string>
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <vector>
using namespace std; 

const int width = 40;
const int height = 20;
int x, y, fX, fY, score, sleepDuration=100;
bool gameOver, growTail=false, restart=false;
vector<pair<int, int>> tail;
vector<pair<int, int>> blocks;

struct User { // Make user struct. Refers to the structure of user, pass, and score on user.txt file.
    string username;
    string password;
    int highestScore;
};

User currentUser; // Make variable for currentUser, will be updated during gamerun.

void InitColors() {
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK); // Red on black text
    init_pair(2, COLOR_CYAN, COLOR_BLACK); // Cyan on black text
    init_pair(3, COLOR_GREEN, COLOR_BLACK); // Green on black text
}

bool isValidFruitPosition(int x, int y, const vector<pair<int, int>>&snake, const vector<pair<int, int>>& blocks) {

    for (const auto& segment : snake) {
        if (fX == segment.first && fY == segment.second) {
            return false; // Checks if fruit overlaps with the snake. If overlap, invalid positon.
        }
    }

    for (const auto& block : blocks) {
        // Check all 4 coordinates of the 2x2 block
        for (int bx = 0; bx < 2; ++bx) {
            for (int by = 0; by < 2; ++by) {
                if (x == block.first + bx && y == block.second + by) {
                    return false; // Check if the fruit's position intersects with a block. If overlap, invalid position.
                }
            }
        }
    }
    return true; // No intersection with any blocks, nor with the snake
} // Checks if fruit will intersect with block or snake. 
// If yes, the position is invalid, and false is returned. If no, the position is valid, and true is returned.

void spawnFruit() {
    do {
        fX = rand() % width;
        fY = rand() % height;
    } while (!isValidFruitPosition(fX, fY, tail, blocks)); 
} // Function that spawns fruit. Keeps generating fruit coordinates until one is valid.

bool isValidBlockPosition(int x, int y, const vector<pair<int, int>>& blocks, const vector<pair<int, int>>& snake, int snakeX, int snakeY, int fX, int fY) {
    
    for (const auto& block : blocks) {
        if ((x == block.first || x == block.first + 1) && 
            (y == block.second || y == block.second + 1)) {
            return false; // Checks if new block would intersect with an existing block. If overlap, invalid position.
        }
    }

    if ((x == snakeX || x + 1 == snakeX) && (y == snakeY || y + 1 == snakeY)) {
        return false; // Checks if new block would intersect with snake head. If overlap, invalid position.
    }
    for (const auto& segment : snake) {
        if ((x == segment.first || x + 1 == segment.first) && 
            (y == segment.second || y + 1 == segment.second)) {
            return false; // Checks if new block would intersect with snake's body. If overlap, invalid position.
        }
    }

    for (int fx = fX; fx <= fX + 1; fx++) {
        for (int fy = fY; fy <= fY + 1; fy++) {
            if (x == fx && y == fy) {
                return false; // Checks if new block would intersect with a fruit's position. If overlap, invalid position.
            }
        }
    }

    return true; // Valid position, no intersections
} // Checks if new block will intersect with existing block, fruit or snake.
// If yes, the position is invalid, and false is returned. If no, the position is valid, and true is returned.

void spawnBlock(const vector<pair<int, int>>& snake, int snakeX, int snakeY, int fX, int fY) {
    int newX, newY;
    do {
        newX = rand() % (width - 2); // Check if there's space for the block
        newY = rand() % (height - 2);
    } while (!isValidBlockPosition(newX, newY, blocks, snake, snakeX, snakeY, fX, fY)); 

    blocks.push_back(make_pair(newX, newY)); // Add the new block
} // Function that spawns block. Keeps generating block coordinates until one is valid.

void Setup() {
    initscr(); 
    clear(); 
    noecho(); 
    cbreak(); 
    curs_set(0); 
    // These functions above starts curse mode and does all the things to make the UI clean, such as clearing screen, hifing cursore, disabling line buffering, etc.

    gameOver = false;
    restart = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fX = rand() % width;
    fY = rand() % height;
    score = 0;
    tail.clear();
    blocks.clear();

    keypad(stdscr, TRUE); // Enable user to use arrow keys
    nodelay(stdscr, TRUE); // Make getch() non-blocking
    timeout(100); 
} // function that sets up the cursed environment

void Draw(){
    clear(); 
    InitColors();

    
    int maxWidth, maxHeight;
    getmaxyx(stdscr, maxHeight, maxWidth);

    // Calculate starting positions for centered board
    int startCol = (maxWidth - width - 2) / 2; // -2 is for the left and right walls
    int startRow = (maxHeight - height - 2) / 2; // -2 is for the top and bottom walls

    // Draw top wall
    for (int i = 0; i < width + 2; i++)
        mvprintw(startRow, startCol + i, "#");
 for (int i = 0; i < height; i++) {
        mvprintw(startRow + i + 1, startCol, "#"); // Draw Left wall
        for (int j = 0; j < width; j++) {
            if (i == y && j == x) {
	        attron(COLOR_PAIR(3));
                mvprintw(startRow + i + 1, startCol + j + 1, "O"); // Snake head
		attroff(COLOR_PAIR(3));
            } else if (i == fY && j == fX) {
	        attron(COLOR_PAIR(1));
                mvprintw(startRow + i + 1, startCol + j + 1, "F"); // Fruit
                attroff(COLOR_PAIR(2));
	    } else {
                bool print = false;
                for (auto t : tail) {
                    if (t.first == j && t.second == i) {
		        attron(COLOR_PAIR(3));
                        mvprintw(startRow + i + 1, startCol + j + 1, "o"); // Snake tail
                        attroff(COLOR_PAIR(3));
			print = true;
                        break;
                    }
                }
                for (auto& block : blocks) {
                    for (int bx=0;bx<2;bx++) {
		        for (int by=0;by<2;by++) {
			    if (block.first+bx==j && block.second+by==i) {
			        attron(COLOR_PAIR(2));
			        mvprintw(startRow+i+1, startCol+j+1, "#");
				attroff(COLOR_PAIR(2));
				print=true;
			    }
			}
			if (print) break;
		    }
		if (print) break;
	        }
	    }
	}
	
        mvprintw(startRow + i + 1, startCol + width + 1, "#"); // Draw Right wall
    }

    // Draw bottom wall
    for (int i = 0; i < width + 2; i++)
        mvprintw(startRow + height + 1, startCol + i, "#");

    // Score text printed below bottom wall, centred.
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);
    mvprintw(startRow + height + 3, (maxWidth - strlen(scoreText)) / 2, "%s", scoreText);

    char highScoreText[50];
    sprintf(highScoreText, "High Score: %d", currentUser.highestScore);

    // Highscore text printed below score text
    mvprintw(startRow +height +5, (maxWidth - strlen(highScoreText)) / 2, "%s", highScoreText);


    refresh(); // Print it on to the real screen
}

void Input() {
  int c = getch(); // Movement using getch, always waiting for input at any time. 
    switch(c) { // Conditional movements based on key pressed (self-explanatory)
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
    // Check collision with wall
    if (x<0 || x>= width || y<0 || y>= height) {
    	gameOver=true;
	}
    // Check collision with snake's own tail
    for (auto t : tail) {
        if (t.first == x && t.second == y) {
            gameOver = true;
        }
    }

    //Check for collision with blocks
    for (auto& block : blocks) {
        for (int i=0;i<2;i++) {
	    for (int j=0; j<2; j++) {
	        if (x==block.first+i && y==block.second+j) {
		    gameOver=true;
		}
	    }
	}
    }
    // fruit collection
    if (x == fX && y == fY) {
        score += 10; 
        spawnFruit();  // If fruit collected, score added, new fruit and new block spawned
	    spawnBlock(tail, x, y, fX, fY);
        growTail = true; // Then add a new segment to the tail at the current position
	    sleepDuration = max(10, sleepDuration-7);
    }

    if (growTail) {
        pair<int,int> newSegment = make_pair(prevX, prevY);
	    tail.insert(tail.begin(), newSegment);
	    growTail=false;
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


void GameOver(){
    clear(); // Clear the screen
    int maxWidth, maxHeight;
    getmaxyx(stdscr, maxHeight, maxWidth); // Finds the maximum width and height of the user's terminal window


    const char* gameOverText[] = {
    
"  _____                         ____                 ",
" / ____|                       / __ `                ",
"| |  __  __ _ _ __ ___   ___  | |  | |_   _____ _ __ ",
"| | |_ |/ _` | '_ ` _ ` / _ ` | |  | ` ` / / _ ` '__|",
"| |__| | (_| | | | | | |  __/ | |__| |` V /  __/ |   ",
" `_____|`__,_|_| |_| |_|`___|  `____/  `_/ `___|_|   ",
    
    };

    int gameOverTextLines = sizeof(gameOverText) / sizeof(gameOverText[0]);
    int longestLineLength = 0;
    for (int i = 0; i < gameOverTextLines; ++i) {
        longestLineLength = max(longestLineLength, static_cast<int>(strlen(gameOverText[i])));
    }
    InitColors();
    int startY = (maxHeight - gameOverTextLines - 14) / 2;
    for (int i = 0; i < gameOverTextLines; ++i) { // 8 is the number of lines in gameOverText
        int startX = (maxWidth - strlen(gameOverText[i])) / 2; // Centering horizontally
        if (i==0 || i == 1 || i == 2 || i == 3) { // Lines that contain "Game"
            attron(COLOR_PAIR(1)); // Red
            mvprintw(startY + i, startX, "%s", gameOverText[i]);
            attroff(COLOR_PAIR(1));
        } else if (i == 4 || i == 5) { // Lines that contain "Over"
            attron(COLOR_PAIR(2)); // Blue, assuming init_pair(2, COLOR_BLUE, COLOR_BLACK) in InitColors()
            mvprintw(startY + i, startX, "%s", gameOverText[i]);
            attroff(COLOR_PAIR(2));
        } else { // For lines that don't specifically contain "Game" or "Over" text
            mvprintw(startY + i, startX, "%s", gameOverText[i]);
        }
    }
    
    char scoreText[20];
    sprintf(scoreText, "Score: %d", score);

    // Print the score centered
    mvprintw(maxHeight / 2, (maxWidth - strlen(scoreText)) / 2, "%s", scoreText);

    char highScoreText[50];
    sprintf(highScoreText, "High Score: %d", currentUser.highestScore);
    mvprintw(maxHeight/2+2, (maxWidth - strlen(highScoreText))/2, "%s", highScoreText);

    // Print the restart or quit text at the center
    const char* restartQuitText = "Press 'r' to restart or 'q' to quit.";
    mvprintw((maxHeight / 2) + 4, (maxWidth - strlen(restartQuitText)) / 2, "%s", restartQuitText);

    refresh(); // Refresh to show text

    // Wait for 'q' to be pressed before exiting
    nodelay(stdscr, FALSE);
    // Now, wait for the user input for 'r' or 'q'
    char ch;
    do {
        ch = getch(); // Wait for user input
        if (ch == 'q') {
            gameOver = true; 
            restart = false; 
            break;
        } else if (ch == 'r') {
            gameOver = false; 
            restart = true; 
            break;
        }
    } while(true);

    clear(); // Optionally clear the screen before restarting or exiting
} // Game Over function that displays user's score, highscore, and prompts the user to either quit the game or restart the game

void updateHighestScore(){
    vector<User> users;
    bool found = false;
    ifstream fileIn(filename);
    User user;
    while (fileIn >> user.username >> user.password>>user.highestScore) { // Updates the highest score of the current use.
        if (user.username == currentUser.username) {
            found = true;
            user.highestScore = max(user.highestScore, currentUser.highestScore); // Update if only the current score is higher
            currentUser.highestScore = score; 
        }
        users.push_back(user);
    }
    fileIn.close();

    if (!found) { // In case the user wasn't found, add new user
        users.push_back(currentUser);
    }

    // Rewrite the file with updated scores
    ofstream fileOut(filename, ios::trunc | ios::out);
    for (const auto& u : users) {
        fileOut << u.username <<" "<< u.password <<" "<< u.highestScore << endl;
    }
    fileOut.close(); 
}

int main(int argc, char*argv[]) {
    string filename = "users.txt"; // This is the file for username and scores
    User user; // Make a variable user type User
    ifstream fileIn(filename);

    while (fileIn >> user.username >> user.password >> user.highestScore) { // Reads every line from users.txt, its arranged this way.
        if (user.username==argv[1]) { // Updates currentUser if it finds the correct player from the user.txt
	        currentUser.username=user.username; 
	        currentUser.password=user.password; 
	        currentUser.highestScore=user.highestScore; 
	    break; 
        }
    }

    srand(time(0)); // Make it really random by using current time.

    // Starts THE GAME
    do {
    	Setup(); // Reset game setting for ncurses
    
    	while (!gameOver) { // This is the main loop where the game is running.
        	Draw(); // Update the display each loop
        	Input(); // Takes input from user keyboard if it is inputted
        	Logic(); // Updates the game whether it has reached collision and controls the movement

		if (dir != STOP) {
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration)); // Delay for 100 miliseconds, gets faster as the game goes though.
    		}
    	} 

    	if (score >currentUser.highestScore) { 
        	currentUser.highestScore = score; // Gameplay loop already ended, this updates the score of the user.
		updateHighestScore("users.txt"); // Call the function to update Highest Score on users.txt
    	}
    	GameOver(); // Goes to the game over screen.

    	if (restart) {
	    	sleepDuration=100; // Resets the sleep duration to amount at start if restart is chosen
        	continue;
    	} else {
        	break // Finisihes the game if quit is chosen
    	};
    }
    while (true); // Game ended :(

    endwin(); // End curses mode
    return 0;
}
