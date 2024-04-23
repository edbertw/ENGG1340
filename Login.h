#ifndef LOGIN_H
#define LOGIN_H

#include <string>

//Structure declaration for user
struct User {
  string username;
  string password;
  int highestScore;
};

//Headers of functions in main LoginInfo.cpp function
void PrintCenter(const char * format, ...);
void PrintCenterInput(const char * prompt, string &input);
bool registerUser(const string &filename);
bool loginUser(const string &filename, User &currentUser);
void displayMenuPage();

#endif
