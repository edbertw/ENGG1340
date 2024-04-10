Team members:
Howard Anggawijaya (3036259035),
Noah Alexander Lie (3036259047),
Alexander Briant Arsa Pratama (3036184096),
Edbert Widjaja (3036240503),
Sebastian Nicholas Kusuma (3036191300)

WELCOME TO OUR GAME, SNAKE !!
When you first start the game, you will be given 3 options to Login, Register and check Leaderboard. Since you are probably a new player, you must register your name and password, then you will be sent to the game page where you can start playing.

Your goal is to collect as many tokens as possible. Each token collected grants you 10 points. Throughout the game, there will be many randomly generated blocks and tokens. Avoiding the outer-walls and the randomly spawning blocks will allow you to survive the game. As you collect tokens, your snake will increase in size. Be careful not to hit your own snake's body, as doing so will immediately result in your defeat. Also, notice that your snake will keep on moving by itself and it is your duty to control its direction.

In the game of Snake, the player uses the arrow keys to move a "snake" around the board. As the snake finds food, it eats the food, and thereby grows larger. The game ends when the snake either moves off the screen or moves into itself. The goal is to make the snake as large as possible before that happens.

LIST OF FEATURES :
- Randomly generating tokens and blocks = Generating random game situations as the position of blocks and tokens vary for every game.
- Leaderboard and Storing Usernames, Passwords, Highscore = We implemented File I/O to keep track of changing highscores as well as initial input of username and password.
- Automated moving and increasing size snake = Implemented STL Container (vectors) to allow snake to increase in size by pushing to vector.
- Block Generation = Vectors to store multiple element blocks that randomly appear on the map.

LIST OF HEADERS & LIBRARIES USED:
- ncurses.h = creates our game's user interface and allows automated moving and continuous input of arrow keys
- cstdlib = random generation of blocks and tokens
- vector = STL container to store blocks and snake
- ctime/chrono = Timer function for duration of screens 
- thread = Handle changing game logic for every user input
- algorithm = Sorting leaderboard entries
- cstring = Character arrays for length measurement.
- fstream = file I/O manipulations for storing login details and high scores
- iostream = I/O operations
- sstream = string input for menu and interface
- string = string text and manipulations
  
