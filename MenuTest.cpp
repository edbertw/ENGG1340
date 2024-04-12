#include <ncurses.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>
using namespace std;

string filename="users.txt";
struct User {
    string username;
    string password;
    int highestScore;
};

User currentUser;
