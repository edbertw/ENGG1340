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
