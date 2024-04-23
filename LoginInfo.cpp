#include "Login.h"
#include <ncurses.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <cstring>
using namespace std;

string filename = "users.txt";

User currentUser;
void displayMenuPage();

//Prints the text to be displayed at center of screen
void PrintCenter(const char* format, ...) { 
    int sW, sH;
    getmaxyx(stdscr, sH, sW); 

    va_list args;
    va_start(args, format);

    int len = vsnprintf(nullptr, 0, format, args) + 1; 
    va_end(args);

    char* str = new char[len];

    va_start(args, format);
    vsnprintf(str, len, format, args);
    va_end(args);

    mvprintw(sH / 2, (sW - strlen(str)) / 2, "%s", str);
    delete[] str;
}

// Make user's input to be displayed on the centre of the screen as they type
void PrintCenterInput(const char* prompt, string& input) {
    echo(); 
    char buffer[256];
    PrintCenter(prompt); 
    getnstr(buffer, 255); 
    noecho(); 
    input = buffer;
}

// Registers username and password and stores it in users.txt
bool registerUser(const string& filename) { 
    bool uexist = false;
    string uname, pass;

    do {
        clear();
        PrintCenterInput("Enter username (or type 'exit' to cancel): ", uname);
        if ( uname == "exit" ) {
            return false;
        } 
        else {
	        endwin();
	        clear();
        }

        ifstream fin(filename);
        string cuname, dpass;
        int dscore;
        uexist = false;

        while (fin >> cuname >> dpass >> dscore) {
            if (cuname == uname) {
                uexist = true;
                break;
            }
        }
        fin.close();

        if ( uexist ) {
            PrintCenter("User already exists. Please try a different username.\n");
            getch(); 
	    endwin();
	    clear();
	    return false; }

    } while ( uexist );

    PrintCenterInput("Enter password: ", pass);
    endwin();
    clear();

    User user = {uname, pass, 0};

    ofstream fout(filename, ios::app);
    if (fout.is_open()) {
        fout << user.username << " " << user.password << " " << user.highestScore << "\n";
        fout.close();
        return true;
    }
    getch(); 
}

bool loginUser(const string& filename, User& currentUser) {
    clear();
    int att = 3;
    string uname, pass;
    bool wrong = false;
    bool exist = false;
    User user;

    vector<User> users;
    ifstream fin(filename);
    while (fin >> user.username >> user.password >> user.highestScore) {
        users.push_back(user);
    }
    fin.close();

    while ( att > 0 ) {
        PrintCenterInput("Enter username: ", uname);
        endwin();
	    clear();
        for ( const auto& usr : users ) {
            if (usr.username == uname) {
                exist = true;
                user = usr; 
                break; 
            }
        }

        if ( !exist ) {
            PrintCenter("User does not exist.\n");
            getch();
            return false;
        }

        PrintCenterInput("Enter password: ", pass);
        endwin();
        clear();

        if (user.password == pass) {
            currentUser = user;
            return true;
        } else {
            wrong = true;
        }

        if ( wrong ) {
            --att; 
            if (att > 0) {
                PrintCenter(("Wrong Password. " + std::to_string(att) + " attempts left.\n").c_str());
                getch();
                endwin();
                clear();
            } else {
                PrintCenter("Login failed. No attempts left.\n");
                getch();
                endwin();
                clear();
                return false; 
            }
        }
    }
    return false;
}

// Displays menu page on the center
void displayMenuPage() {
    int maxW, maxH;
    getmaxyx(stdscr, maxH, maxW); 
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

    int sY = (maxH - lines.size() - 4) / 2; 
    int sX = 0;

    attron(COLOR_PAIR(1)); 
    for (size_t i = 0; i < lines.size(); ++i) {
        startX = (maxW - lines[i].length()) / 2; 
        mvprintw(sY + i, startX, "%s", lines[i].c_str());
    }
    attroff(COLOR_PAIR(1)); 

    sX = (maxW - 20) / 2; 
    mvprintw(sY + lines.size() + 1, sX, "1. Login");
    mvprintw(sY + lines.size() + 2, sX, "2. Registration");
    mvprintw(sY + lines.size() + 3, sX, "3. Quit");

    refresh(); 
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int choice = 1;
    bool running = true;
    int maxW, maxH;
    getmaxyx(stdscr, maxH, maxW);
    int menuStartX = (maxW - 20) / 2; 

    displayMenuPage();

    mvprintw((maxH / 2) + 3, menuStartX - 3, "->");

    while ( running ) {
        int ch = getch();
        switch (ch) {
            case KEY_DOWN:
                mvprintw((maxH / 2) + 2 + choice, menuStartX - 3, "  ");
                choice = (choice % 3) + 1; 
                mvprintw((maxH / 2) + 2 + choice, menuStartX - 3, "->");
                break;
            case KEY_UP:
                mvprintw((maxH / 2) + 2 + choice, menuStartX - 3, "  ");
                choice = (choice == 1) ? 3 : choice - 1; 
                mvprintw((maxH / 2) + 2 + choice, menuStartX - 3, "->");
                break;
            case '\n':
                if (choice == 1) {
                    if (!loginUser(filename, currentUser)) {
                        PrintCenter("Please try again or register.\n");
			getch();
			displayMenuPage();
			mvprintw((maxH / 2) + 3, menuStartX - 3, "->");
                        initscr();
                        cbreak();
                        noecho();
                        keypad(stdscr, TRUE);
                    } else {
                        PrintCenter("Login successful. Welcome, %s!\n", currentUser.username.c_str());
                        getch();
                        endwin();
                        system(("./menu " + currentUser.username).c_str());
                        return 0;
                    }
                } else if ( choice == 2 ) {
                    if ( !registerUser(filename) ) {
		                displayMenuPage();
			            mvprintw((maxH / 2) + 4, menuStartX - 3, "->");
			            initscr();
                        cbreak();
                        noecho();
                        keypad(stdscr, TRUE);
		            } else {
		                PrintCenter("Registration Successful! Welcome, %s!\n", currentUser.username.c_str());
			            getch();
			            displayMenuPage();
                        mvprintw((maxH / 2) + 4, menuStartX - 3, "->");
                        initscr();
                        cbreak();
                        noecho();
                        keypad(stdscr, TRUE);
		            }
                } else if ( choice == 3 ) {
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
