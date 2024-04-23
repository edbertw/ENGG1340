#include <fstream>
#include <iostream>
#include <string>
#include <ncurses.h>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
using namespace std; 

struct User { // Make user struct. Refers to the structure of user, pass, and score on user.txt file.
    string username;
    string password;
    int highestScore;
};

User currentUser; // Make variable for currentUser, will be updated during gamerun.

// PUT THE OTHER FUNCTIONS BROSSSSS. Change the function or variable name if needed, this is the main algorithm though.
void Setup() {
  
}

void Draw(){
  
}

void Input() {
  
}

void Logic(){
  
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
