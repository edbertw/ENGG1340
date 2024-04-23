#include <ncurses.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <cstring>
using namespace std;

string filename="users.txt";
struct User {
    string username;
    string password;
    int highestScore;
};

User currentUser;
void displayMenuPage();

//The PrintCenter function prints the text to be displayed at center of screen
void PrintCenter(const char* format, ...) { //The input parameters are a format string to specify the desired format and text of the output
    // Outputs are in the form of the string to be displayed
    int screenWidth, screenHeight;
    getmaxyx(stdscr, screenHeight, screenWidth); // Get the screen size

    va_list args;
    va_start(args, format);

    // We need to calculate the length of the formatted string.
    // vsnprintf with nullptr and 0 as buffer and size will return the required length for the formatted string.
    int len = vsnprintf(nullptr, 0, format, args) + 1; // +1 for '\0'
    va_end(args);

    // Allocate memory for the formatted string
    char* str = new char[len];

    // Format the string
    va_start(args, format);
    vsnprintf(str, len, format, args);
    va_end(args);

    // print the formatted string at the center of the screen
    mvprintw(screenHeight / 2, (screenWidth - strlen(str)) / 2, "%s", str);

    // Clean up memory
    delete[] str;
}

// Make user's input to be displayed on the centre of the screen as they type
void PrintCenterInput(const char* prompt, string& input) {
    echo(); // Turn on echoing 
    char buffer[256];
    PrintCenter(prompt); // Display Prompt
    getnstr(buffer, 255); // Read input
    noecho(); // Turn off echoing
    input = buffer;
}

// The function registers username and password and stores it in users.txt
bool registerUser(const string& filename) { //Takes the name of the file as input parameter and returns if successful registration
    bool userExists = false;
    string username, password;

    do {
        clear();
        PrintCenterInput("Enter username (or type 'exit' to cancel): ", username);
        if (username=="exit") {
	    return false;
	} else {
	    endwin();
	    clear();
        }
        // Check if the user already exists
        ifstream fin(filename);
        string checkUsername, dummyPassword;
        int dummyScore;
        userExists = false;

        while (fin >> checkUsername >> dummyPassword >> dummyScore) {
            if (checkUsername == username) {
                userExists = true;
                break;
            }
        }
        fin.close();

        if (userExists) {
            PrintCenter("User already exists. Please try a different username.\n");
            getch(); // Wait for user input
	    endwin();
	    clear();
	    return false;
        }

    } while (userExists);

    PrintCenterInput("Enter password: ", password);
    endwin();
    clear();

    User user = {username, password, 0};

    // Append the new user to the file
    ofstream fout(filename, ios::app);
    if (fout.is_open()) {
        fout << user.username << " " << user.password << " " << user.highestScore << "\n";
        fout.close();
        return true;
    }
    getch(); // Wait for user input
}

// Check if username exist and password is correct
bool loginUser(const string& filename, User& currentUser) {
    clear();
    int attempts = 3;
    string username, password;
    bool wrong = false;
    bool exist = false;
    User user;

    vector<User> users;
    ifstream fin(filename);
    while (fin >> user.username >> user.password >> user.highestScore) {
        users.push_back(user);
    }
    fin.close();

    while ( attempts > 0 ) {
        PrintCenterInput("Enter username: ", username);
        endwin();
	clear();
        for ( const auto& usr : users ) {
            if (usr.username == username) {
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

        PrintCenterInput("Enter password: ", password);
        endwin();
        clear();

        if (user.password == password) {
            currentUser = user;
            return true;
        } else {
            wrong = true;
        }

        if (wrong) {
            --attempts; 
            if (attempts > 0) {
                PrintCenter(("Wrong Password. " + std::to_string(attempts) + " attempts left.\n").c_str());
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

// This function displays our menu page on the center (with colors) and takes no input parameters
void displayMenuPage() {
    int maxWidth, maxHeight;
    getmaxyx(stdscr, maxHeight, maxWidth); // Get the size of the terminal window

    clear(); // Clear the screen using NCurses

    // Use NCurses attributes for color if desired
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Green text on black background

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

    int startY = (maxHeight - lines.size() - 4) / 2; // Center the content vertically
    int startX = 0;

    attron(COLOR_PAIR(1)); // Turn on green text color
    for (size_t i = 0; i < lines.size(); ++i) {
        startX = (maxWidth - lines[i].length()) / 2; // Center the content horizontally
        mvprintw(startY + i, startX, "%s", lines[i].c_str());
    }
    attroff(COLOR_PAIR(1)); // Turn off green text color

    // Display menu options
    startX = (maxWidth - 20) / 2; // "20" is the approximate length of the longest menu string
    mvprintw(startY + lines.size() + 1, startX, "1. Login");
    mvprintw(startY + lines.size() + 2, startX, "2. Registration");
    mvprintw(startY + lines.size() + 3, startX, "3. Quit");
    //mvprintw(startY + lines.size() + 1, startX - 3, "->"); // Arrow to highlight option

    refresh(); // Refresh the screen to display changes
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int choice = 1;
    bool running = true;
    int maxWidth, maxHeight;
    getmaxyx(stdscr, maxHeight, maxWidth);
    int menuStartX = (maxWidth - 20) / 2; 

    displayMenuPage();

    // Highlight the first option initially
    mvprintw((maxHeight / 2) + 3, menuStartX - 3, "->");

    while (running) {
        int ch = getch();
        switch (ch) {
            case KEY_DOWN:
                mvprintw((maxHeight / 2) + 2 + choice, menuStartX - 3, "  ");
                choice = (choice % 3) + 1; // Loop back around to the first option
                mvprintw((maxHeight / 2) + 2 + choice, menuStartX - 3, "->");
                break;
            case KEY_UP:
                mvprintw((maxHeight / 2) + 2 + choice, menuStartX - 3, "  ");
                choice = (choice == 1) ? 3 : choice - 1; // Loop back around to the last option
                mvprintw((maxHeight / 2) + 2 + choice, menuStartX - 3, "->");
                break;
            case '\n':
                if (choice == 1) {
                    if (!loginUser(filename, currentUser)) {
                        PrintCenter("Please try again or register.\n");
			getch();
			displayMenuPage();
			mvprintw((maxHeight / 2) + 3, menuStartX - 3, "->");
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
                } else if (choice == 2) {
                    if (!registerUser(filename)) {
		                displayMenuPage();
			            mvprintw((maxHeight / 2) + 4, menuStartX - 3, "->");
			            initscr();
                        cbreak();
                        noecho();
                        keypad(stdscr, TRUE);
		            } else {
		                PrintCenter("Registration Successful! Welcome, %s!\n", currentUser.username.c_str());
			            getch();
			            displayMenuPage();
                        mvprintw((maxHeight / 2) + 4, menuStartX - 3, "->");
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
