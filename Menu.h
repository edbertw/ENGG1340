#ifndef MENU_H
#define MENU_H

#include <string>

//Structure Definition for User
struct User {
  string username;
  string password;
  int highestScore;
};

//define headers of all functions 
void displayMenuPage();
void leaderboard(const string &filename);

#endif
