//library
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ncurses.h>// colour library
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <random>
#include <cstring>
#include <algorithm>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <time.h> 

using namespace std;

//variabless

class crate {
    public:
        int colour;
        int column;
        int row;
        bool freespace;

        void draw() {
            attrset(COLOR_PAIR(colour));
            mvprintw(-2*row+15,7*column+4,"┏━┓");
            mvprintw(-2*row+16,7*column+4,"┗━┛");
        }
};
class crate coordinates[7][6];//7 column, 6 rows 
int columnsize[7];

class player {

    private:
        int arm;
        int maxarm;
        int y;
    public:
        int x;
        int button;
             // standby, grabbing, placeing, lowering, raising
        enum status {
            standby,
            raising,
            lowering
        };
        status status;
        class crate holding;


        void move() {

            if (button != x || status == standby) {
                status = standby;

                if (arm > 0) {
                    arm--;
                } else if (button < x) {
                    x--;
                } else if (button > x) {
                    x++;
                }
                
            }

            if (button == x) {
                if (status != raising) {
                    status = lowering;
                }
                y = columnsize[x];
                if (holding.freespace) {// not holding anything
                    if (coordinates[x][y].freespace == true) {
                        maxarm = -2*y+16;
                        // mvprintw(12,60,"1");
                    }
                    if ((coordinates[x][y].freespace == false) && (y == 6)) {
                        maxarm = 4;
                        // mvprintw(12,60,"2");
                    }
                } else if (!holding.freespace) {// holding a crate
                    if ((coordinates[x][y].freespace == true) && (y == 0)) {
                        maxarm = 14;
                        // mvprintw(12,60,"3");
                    }
                    if (coordinates[x][y].freespace == true) {
                        maxarm = -2*y+13;
                        // mvprintw(12,60,"4");
                    }
                }
            
                if ((status == lowering) && (arm < maxarm)) {
                    arm++;
                }
                if (arm == maxarm) {
                    status = raising;
                    // y = (maxarm-15)/-2;
                    // y = 2;

                    if (holding.freespace) { // grabbing
                        coordinates[x][y].freespace = true;
                        holding = coordinates[x][y];
                        columnsize[x]--;
                        // coordinates[x][y].colour = 0;
                    } else if (!holding.freespace) { // placing
                        coordinates[x][y] = holding;
                        holding.freespace = true;
                        columnsize[x]++;
                    }
                }
                if ((status == raising) && (arm > 0)) {
                    arm--;
                }

            }
        }

        void draw () {
            attrset(COLOR_PAIR(7));
            mvprintw(0,7*x+5,"┳");
            if (arm) {
                for (int i = 1; i <= arm; i++) {
                    mvprintw(i,7*x+5,"┃");
                }
            }
            mvprintw(arm+1,7*x+3,"┏━┻━┓");
            if (holding.freespace && holding.colour != 0) {
                attrset(COLOR_PAIR(holding.colour));
                mvprintw(arm+2,7*x+4,"┏━┓");
                mvprintw(arm+3,7*x+4,"┗━┛");
            }
        }

};


int ch;
int currentscore = 0;
bool gameover = false;

player player;

// different colours
//COLOR_YELLOW, COLOR_CYAN, COLOR_BLUE, COLOR_WHITE, COLOR_RED, COLOR_GREEN, COLOR_MAGENTA

//functions

void inventoryspawn() {
    // srand (time(0));
    int randomcrate = rand() % 100;
    int randomcoliumn = rand() % 7;
    string craterarity[100] = {"white", "pink", "pink", "red", "red", "red", "yellow", "yellow", "yellow", "yellow", "green", "green", "green", "green", "green", "green", "green", "green", "cyan", "cyan", "cyan", "cyan", "cyan", "cyan", "cyan", "cyan", "cyan", "cyan", "blue", "blue", "blue", "blue", "blue", "blue", "blue", "blue", "blue", "blue", "blue", "blue", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing", "nothing"};
    if (!(craterarity[randomcrate] == "nothing") && !(player.x == randomcoliumn)) {
        for (int i = 0; i < 6; i++) {
            if (coordinates[randomcoliumn][i].freespace == true) {
                crate newcrate;
                if (craterarity[randomcrate] == "blue") {
                    newcrate.colour = 1;//blue
                } else if (craterarity[randomcrate] == "cyan") {
                    newcrate.colour = 2;//cyan
                } else if (craterarity[randomcrate] == "green") {
                    newcrate.colour = 3;//green
                } else if (craterarity[randomcrate] == "yellow") {
                    newcrate.colour = 4;//yellow
                } else if (craterarity[randomcrate] == "red") {
                    newcrate.colour = 5;//red
                } else if (craterarity[randomcrate] == "pink") {
                    newcrate.colour = 6;//pink
                } else if (craterarity[randomcrate] == "white") {
                    newcrate.colour = 7;//white
                }
                newcrate.column = randomcoliumn;
                newcrate.row = i;
                newcrate.freespace = false;
                coordinates[randomcoliumn][i] = newcrate;
                columnsize[randomcoliumn]++;
                break;
            }
        }
    }
}

void inventorydraw () {
    for (int c = 0; c < 7; c++) {
        for (int r = 0; r < 6; r++) {
            if (coordinates[c][r].colour != 0 && coordinates[c][r].freespace == false) {
                coordinates[c][r].draw();
            }
        }
    }
}

string readLine(string str, int n) {
    // returns the nth line of a string
    stringstream f(str);
    string s;
    
    for (int i = 0; i < n; i++) {
        getline(f, s);
    }

    getline(f,s);
    return s; 
}

void draw() {
    // print the first of the screen line to cover up shape

    const string screenstring = "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓    ┏━━━━━score━━━━━┓\n┃                                                  ┃    ┃ current: 0    ┃\n┃                                                  ┃    ┃ high: 0       ┃\n┃                                                  ┃    ┗━━━━━━━━━━━━━━━┛\n┃                                                  ┃    ┏━━━━━items━━━━━┓\n┃                                                  ┃    ┃               ┃\n┃                                                  ┃    ┃     T * |     ┃\n┃                                                  ┃    ┗━━━━━━━━━━━━━━━┛\n┃                                                  ┃\n┃                                                  ┃\n┃                                                  ┃\n┃                                                  ┃\n┃                                                  ┃\n┃                                                  ┃\n┃                                                  ┃\n┃                                                  ┃\n┃                                                  ┃\n┗━━┗━1━┛━━┗━2━┛━━┗━3━┛━━┗━4━┛━━┗━5━┛━━┗━6━┛━━┗━7━┛━┛\n";
    for (int i = 0; i < 18; i++) {
        mvprintw(i,0,readLine(screenstring, i).c_str());
    }
    inventorydraw();
    player.draw();

    attrset(COLOR_PAIR(7)); // delete this line for a cool effect :)
}

void game() {
	switch(ch) {
        case '1':
            player.button = 0;
            break;
        case '2': 
            player.button = 1;
            break;
        case '3': 
            player.button = 2;
            break;
        case '4':
            player.button = 3;
            break;
        case '5':
            player.button = 4;
            break;
        case '6':
            player.button = 5;
            break;
        case '7':
            player.button = 6;
            break;
        case KEY_MOUSE: 
            MEVENT event;
            if (getmouse(&event) == OK) {
                if (event.x >= 0 && event.x <= 8) {
                    player.button = 0;
                }
                if (event.x >= 9 && event.x <= 15) {
                    player.button = 1;
                }
                if (event.x >= 16 && event.x <= 22) {
                    player.button = 2;
                }
                if (event.x >= 23 && event.x <= 29) {
                    player.button = 3;
                }
                if (event.x >= 30 && event.x <= 36) {
                    player.button = 4;
                }
                if (event.x >= 37 && event.x <= 43) {
                    player.button = 5;
                }
                if (event.x >= 44) {
                    player.button = 6;
                }
            }

            break; 
        default: 
            break;
    }

    player.move();
    inventoryspawn();
}

//main game loop

int main()
{
    //state the defult settings
    setlocale(LC_CTYPE, "");
    initscr();
    noecho();
    keypad(stdscr, true);
    nodelay(stdscr, true);
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
    curs_set(0);
	start_color();
    use_default_colors();

    init_pair(1, COLOR_BLUE, -1);
    init_pair(2, COLOR_CYAN, -1);
    init_pair(3, COLOR_GREEN, -1);
    init_pair(4, COLOR_YELLOW, -1);
    init_pair(5, COLOR_RED, -1);
    init_pair(6, COLOR_MAGENTA, -1);
    init_pair(7, COLOR_WHITE, -1);
    init_pair(8, COLOR_BLACK, -1);

    player.status = player::standby;
    // player.status = player::lowering;
    // player.status = "grabbing";
    player.holding.freespace = true;
    for (int y = 0; y < 7; y++) {
        for (int x = 0; x < 6; x++) {
            coordinates[y][x].freespace = true;
        }
    }


    //clear the screen and refreash it
    clear();
    refresh();
    // the main game stuff starts here

	while ((ch = getch()) != 'q' && ch != 'Q') {
        draw();
        game();
        refresh();
        usleep(100000);
    }
    //and ends here
    refresh();
    //exit the game
    endwin();
    return 0;
}


