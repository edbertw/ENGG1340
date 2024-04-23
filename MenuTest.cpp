#include <ncurses.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include "Menu.h"
using namespace std;

string filename="users.txt";

User currentUser;
void displayMenuPage();

//Creates leaderboard structure and relevant information (Username, HighScore)
void leaderboard(const string& filename) { //Takes the string name of file as input and prints leaderboard as output
    // Clear the screen and prepare for NCurses display
    clear();
    
    vector<User> users;
    ifstream fin(filename);
    string uname, pwd;
    int hscore;
    while (fin >>uname >>pwd >>hscore) {
        users.emplace_back(User{uname, pwd, hscore});
    }
    fin.close();
    sort(users.begin(), users.end(), [](const User& a, const User& b) {
        return a.highestScore > b.highestScore;
    });
    int maxWidth, maxHeight;
    getmaxyx(stdscr, maxHeight, maxWidth);
    int startY = (maxHeight-users.size()-6)/2; 
    int startX = (maxWidth-20)/2; 
    mvprintw(startY, startX, "Leaderboard:");
    mvprintw(startY +1, startX, "Username\t\tScore");
    mvprintw(startY +2, startX, "-------------------------");

    for (size_t i = 0; i < users.size(); ++i) {
        mvprintw(startY+i+3, startX, "%s\t\t%d", users[i].username.c_str(), users[i].highestScore);
    }
    
    mvprintw(startY+users.size()+4, startX, "Press 'm' to return to the menu");

    refresh(); 

    while (true) {
        int ch = getch();
        if (ch == 'm' || ch == 'M') {
	    endwin();
	    displayMenuPage();
	    break;
        }
    }
}
// displays Menu Details on a new screen by clearing and displaying
void displayMenuPage() {
    int maxWidth, maxHeight;
    getmaxyx(stdscr, maxHeight, maxWidth); 

    clear(); 

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); 

    vector<string> lines = {
        "=============================================",
        "                      __                  ",
        "  ______ ____ _____  |  | __ ____   ______",
        "  /  ___//    \\__  \ |  |/ // __ \ /  ___/",
        " \___ \|   |  \/ __ \|    <\  ___/ \___ \ ",
        "/____  >___|  (____  /__|_ \\___  >____  >",
        "     \/     \/     \/     \/    \/     \/ ",
        "============================================="
    };
    int startY = (maxHeight-lines.size()-4) /2;
    int startX = 0;
    
    attron(COLOR_PAIR(1)); 
    for (size_t i = 0; i < lines.size(); ++i) {
        startX = (maxWidth-lines[i].length()) / 2; 
        mvprintw(startY + i, startX, "%s", lines[i].c_str());
    }
    attroff(COLOR_PAIR(1)); 
    startX = (maxWidth - 20) / 2; 
    mvprintw(startY + lines.size()+1, startX, "1. Start Game");
    mvprintw(startY + lines.size()+2, startX, "2. Leaderboard");
    mvprintw(startY + lines.size()+3, startX, "3. Quit");

    refresh(); 
}

// Provide the main user interface for the game and allows user to input their choice to proceed to playing the game.
int main(int argc, char* argv[]) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int choice = 1;
    bool running = true;
    int maxWidth, maxHeight;
    getmaxyx(stdscr, maxHeight, maxWidth);
    int menuStartX = (maxWidth - 20) / 2;

    string username = argv[1];

    displayMenuPage();

    mvprintw((maxHeight/2)+3, menuStartX - 3, "->");

    while (running) {
        int ch = getch();
        switch (ch) {
            case KEY_DOWN:
                mvprintw((maxHeight/2) + 2 + choice, menuStartX-3, "  ");
                choice = (choice%3) + 1;
                mvprintw((maxHeight/2) + 2 + choice, menuStartX -3, "->");
                break;
            case KEY_UP:
                mvprintw((maxHeight/2) + 2 + choice, menuStartX-3, "  ");
                choice = (choice == 1) ? 3 : choice - 1; 
                mvprintw((maxHeight/2) + 2 + choice, menuStartX-3, "->");
                break;
            case '\n':
                if (choice == 1) {
                    endwin();
		    system(("./Snake_Game "+username).c_str());
		    initscr();
		    cbreak();
		    noecho();
		    keypad(stdscr, TRUE);
		    refresh();
		} else if (choice == 2) {
		    leaderboard(filename);
		    mvprintw((maxHeight / 2) + 4, menuStartX - 3, "->");
		    initscr();
            cbreak();
            noecho();
            keypad(stdscr, TRUE);
		    refresh();
                } else if (choice == 3) {
                    running = false;
		    endwin();
		    exit(0);
                }
                break;
	    }
        }
    clear();
    refresh();
    endwin();
    return 0;
}
