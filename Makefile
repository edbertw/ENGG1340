.FLAGS = -pedantic-errors -std=c++11

Snake_Game : Snake_Game.cpp Main.h
	g++ $(FLAGS) $^ -o $@ -lncurses

menu : MenuTest.cpp Menu.h
	g++ $(FLAGS) $^ -o $@ -lncurses

login : LoginInfo.cpp Login.h 
	g++ $(FLAGS) $^ -o $@ -lncurses

Run : login
	./login

clean :
	rm -f login menu Snake_Game

.PHONY : clean

