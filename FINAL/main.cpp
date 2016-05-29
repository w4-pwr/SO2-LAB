#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <ncurses.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>
using namespace std;
struct bus {
	//-1 jedzie w lewo
	// 1 jedzie w prawo
	int direction;
	int capacity;
	int x;
	int y;
	int endurance;
};

struct place {
	int status;
};

struct passanger {
    char letter;
    bool validTicket;
    int x;
    int y;
    int status;
};

struct mechanic {
    int x;
    int y;
    int station;
};


#define PASSAGNERS_COUNT 5
#define BOARD_HEIGHT 50
#define BOARD_WIDTH 50
#define MECHANIC_COUNT 2
#define BUS_COUNT 1

int railway_y = 0;

int stationA_X = 10;
int stationA_Y = 10;
int stationA_W = 20;
int stationA_H = 10;

int stationB_X = 60;
int stationB_Y = 10;
int stationB_W = 20;
int stationB_H = 10;

struct place board[BOARD_WIDTH][BOARD_HEIGHT];
struct bus busArray[BUS_COUNT];
struct passanger passangers[PASSAGNERS_COUNT];
struct mechanic mechanics[MECHANIC_COUNT];

bool finish = false;


pthread_t passangerThread[PASSAGNERS_COUNT];
pthread_t mechanicThread[MECHANIC_COUNT];
pthread_t busThread[BUS_COUNT];
pthread_t drawThread;
pthread_t updateThread;

pthread_mutex_t platformMutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t enduranceMutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t drawMutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mechanicMutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t repairMutex  = PTHREAD_MUTEX_INITIALIZER;


pthread_cond_t repairCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t mechanicCond = PTHREAD_COND_INITIALIZER;

bool freeLeftPlaform = true;
bool freeRightPlaform = true;

void cleanBoard()
{
	int i,j;
	for(i = 0; i < BOARD_WIDTH; i++) {
		for(j = 0; j < BOARD_HEIGHT; j++) {
			board[i][j].status = 0;
		}
	}
}

void drawStation(int x, int y, int width, int height, char *name) {
    //poziome linie
    move(y, x);
    hline('-', width);
    move(y + height, x);
    hline('-', width);

    //pionowe linie
    move(y, x);
    vline('|', height + 1);
    move(y, x + width);
    vline('|', height + 1);

    move(y - 1, x);

    printw(name);
}

int railwayY;


void drawPassangers() {
    for(int i = 0; i < PASSAGNERS_COUNT; i++){
        move(passangers[i].y, passangers[i].x);
        printw("X");
    }
}

void drawMechanics() {
    for(int i = 0; i < PASSAGNERS_COUNT; i++){
        move(mechanics[i].y, mechanics[i].x);
        printw("R");
    }
}

void drawBus(){
	int i;
	for(i = 0; i < BUS_COUNT; i++) {
		int x = busArray[i].x;
		int y = busArray[i].y;
		move(railwayY,x);

		int capacity = busArray[i].capacity;
		int j;
		for(j = 0; j < capacity; j++){
			printw("@");
		}

		mvprintw(i, 0, "Bus %d - endurance: %d", i,  busArray[i].endurance);	


	}
}

void drawRailway() {
    int endX = stationB_X;
	int startX = stationA_X+stationA_W;
 	int railwayHeight = (stationA_H/2)+stationA_Y;

	railwayY = railwayHeight;
    move(railwayHeight, startX);
    int distanceX = abs(startX - endX)+1;
    hline('=', distanceX);
}

void draw() {
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	pthread_mutex_lock(&drawMutex);

	drawStation(stationA_X, stationA_Y, stationA_W, stationA_H, "Station A");
    drawStation(stationB_X, stationB_Y, stationB_W, stationB_H, "Station B");
    drawPassangers();
    drawMechanics();
	drawRailway();
	drawBus();

	move(0,0);
	refresh();

	pthread_mutex_unlock(&drawMutex);
}

void busUpdate(void *arg) {
	struct bus* mBus = (struct bus*)arg;

	int i;
	for(i = 0; i < BUS_COUNT; i++) {
		int x = busArray[i].x;
		int y = busArray[i].y;
		int capacity = busArray[i].capacity;
		int endurance = busArray[i].endurance;

		//sprawdzenie kierunku jazdy
		bool shouldGoToLeft = x + capacity-1 == stationB_X;
		bool shouldGoToRight= x == (stationA_W + stationA_X);
		if(shouldGoToLeft){
			busArray[i].direction = -1;
			busArray[i].endurance--;
		} else if (shouldGoToRight){
			busArray[i].direction = 1;
			busArray[i].endurance--;
		}

		if (busArray[i].endurance == 0){
			pthread_cond_broadcast(&mechanicCond);
			pthread_cond_wait(&repairCond, &repairMutex);
		
		}



		//jazda wg kierunku
			if(busArray[i].direction == 1){
				x++;
			} else if (busArray[i].direction == -1) {
				x--;
			} 
			busArray[i].x = x;
	}

}

void passangerUpdate() {
	

}

void mechanicUpdate(){
	pthread_mutex_lock(&mechanicMutex);
	bool repairNeeded = false;
	int i;
	for(i = 0; i < BUS_COUNT; i++) {

		if(busArray[i].endurance == 0){
			repairNeeded = true;
			break;
		}
	}

	if(!repairNeeded){
		pthread_cond_wait(&mechanicCond, &mechanicMutex);
	}

	for(i = 0; i < 4; i++){
		usleep(80*1000);
		mechanics[0].y++;
		refresh();
	}

	for(i = 0; i < BUS_COUNT; i++) {
		busArray[i].endurance = 1;
	}
	pthread_cond_broadcast(&repairCond);
	pthread_mutex_unlock(&mechanicMutex);
}

void *drawThreadWrapper(void *arg) {
	while(finish != true){
		clear();
		draw();
	}
	return NULL;
}

void *busThreadWrapper(void *arg) {
	struct bus* mBus = (struct bus*)arg;
	while(finish != true){

		pthread_mutex_lock(&repairMutex);
		usleep(80*1000);
		busUpdate(mBus);
		pthread_mutex_unlock(&repairMutex);
	}
	return NULL;
}

void *passangerThreadWrapper(void *arg) {
	while(finish != true){
		usleep(80*1000);
		passangerUpdate();
	}
	return NULL;
}

void *mechanicThreadWrapper(void *arg) {
	while(finish != true){
		usleep(5000*1000);
		mechanicUpdate();
	}
	return NULL;
}

void setup()
{
	int i;
	for(i = 0; i < BUS_COUNT; i++) {
		busArray[i].x = rand()%10 + (stationA_W+ stationA_X);
		busArray[i].y = railwayY;
		busArray[i].endurance = 1;
		busArray[i].capacity = rand()%8 + 2;

		bool foo = rand()%2;
		if(foo){
			busArray[i].direction = 1;
		} else{
			busArray[i].direction = -1;
		}
	}

	for(i = 0; i < PASSAGNERS_COUNT; i++){
		passangers[i].x = rand()%10 + stationA_X + 2;
		passangers[i].y = rand()%5 + stationA_Y + 2;
	}

	mechanics[0].x = stationA_X+stationA_W-1;
	mechanics[0].y = stationA_Y+1;
	mechanics[0].station = 1;

	mechanics[1].x = stationB_X+1;
	mechanics[1].y = stationB_Y+1;
	mechanics[1].station = 2;
}

int main(void) {
	initscr();
	curs_set(0);
	srand(time(NULL));
	int i;

	setup();

	//START WATKOW - DRAW
	pthread_create(&drawThread, NULL, drawThreadWrapper, NULL);
	
	for(i = 0; i < MECHANIC_COUNT; i++) {
		pthread_create(&mechanicThread[i], NULL, mechanicThreadWrapper, &mechanics[i]);
	}

	for(i = 0; i < BUS_COUNT; i++) {
		pthread_create(&busThread[i], NULL, busThreadWrapper, &busArray[i]);
	}

	for(i = 0; i < PASSAGNERS_COUNT; i++) {
		pthread_create(&passangerThread[i], NULL, passangerThreadWrapper, &passangers[i]);
	}




	//ZAKONCZENIE WATKOW
	pthread_join(drawThread, NULL);
	
	for (i = 0; i < MECHANIC_COUNT; i++) {
        pthread_join(mechanicThread[i], NULL);
	}

	for (i = 0; i < BUS_COUNT; i++) {
        pthread_join(busThread[i], NULL);
	}

	for (i = 0; i < PASSAGNERS_COUNT; i++) {
        pthread_join(passangerThread[i], NULL);
	}



	getch();
	endwin();

	pthread_cond_destroy(&repairCond);		
	pthread_mutex_destroy(&enduranceMutex);
	pthread_mutex_destroy(&platformMutex);
	pthread_mutex_destroy(&drawMutex);


	return EXIT_SUCCESS;
}