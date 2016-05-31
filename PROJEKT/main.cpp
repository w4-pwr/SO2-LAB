#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <ncurses.h>
#include <list>
#include <time.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <mach/mach_time.h>
#include <sys/time.h>
using namespace std;

//    _______________  __  ___________________
//   / ___/_  __/ __ \/ / / / ____/_  __/ ___/
//   \__ \ / / / /_/ / / / / /     / /  \__ \
//  ___/ // / / _, _/ /_/ / /___  / /  ___/ /
// /____//_/ /_/ |_|\____/\____/ /_/  /____/
struct passanger {
    char letter;
    bool validTicket;
    int x;
    int y;
    //-1 peron lewy
    // 1 peron prawy
    // 0 w autobusie
    int direction;
    int onStation;
};

struct bus {
	// -1 jedzie w lewo
	// 1 jedzie w prawo
	int direction;
	int passangers;
	
	//-1 - lewy
    // 1 - prawy
	int onStation;
	bool onPlatform;
	int x;
	int y;
	int endurance;
};

struct mechanic {
    int x;
    int y;

    //-1 - lewy
    // 1 - prawy
    int onStation;
};


//                    _       __    __
//  _   ______ ______(_)___ _/ /_  / /__  _____
// | | / / __ `/ ___/ / __ `/ __ \/ / _ \/ ___/
// | |/ / /_/ / /  / / /_/ / /_/ / /  __(__  )
// |___/\__,_/_/  /_/\__,_/_.___/_/\___/____/
#define PASSAGNERS_COUNT 2
#define BOARD_HEIGHT 50
#define BOARD_WIDTH 50
#define MECHANIC_COUNT 2
#define BUS_COUNT 3

struct bus busArray[BUS_COUNT];
struct passanger passangers[PASSAGNERS_COUNT];
struct mechanic mechanics[MECHANIC_COUNT];
struct timespec ts;

int BUS_SPEED = 20;
int PASSAGNER_SPEED = 80;
int MECHANIC_SPEED = 60;

int railway_y = 0;
int railway_start_x;
int railway_end_x;

int stationA_X = 5;
int stationA_Y = 5;
int stationA_W = 20;
int stationA_H = 10;

int stationB_X = 100;
int stationB_Y = 5;
int stationB_W = 20;
int stationB_H = 10;

int offset = 25;
bool finish = false;
bool debug = false;


pthread_t passangerThread[PASSAGNERS_COUNT];
pthread_t mechanicThread[MECHANIC_COUNT];
pthread_t busThread[BUS_COUNT];
pthread_t drawThread;
pthread_t updateThread;

pthread_mutex_t drawMutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mechanicMutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t busMutex  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t passangerMutex  = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t mechanicCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t busCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t passangerCond = PTHREAD_COND_INITIALIZER;

//     ____  ____  ___ _       _______   ________
//    / __ \/ __ \/   | |     / /  _/ | / / ____/
//   / / / / /_/ / /| | | /| / // //  |/ / / __
//  / /_/ / _, _/ ___ | |/ |/ // // /|  / /_/ /
// /_____/_/ |_/_/  |_|__/|__/___/_/ |_/\____/
void drawStation(int x, int y, int width, int height, char *name) {
    //poziome linie
    attrset(COLOR_PAIR(2));

    move(y, x);
    hline(' ', width);
    move(y + height, x);
    hline(' ', width);

    //pionowe linie
    move(y, x);
    vline(' ', height + 1);
    move(y, x + width);
    vline(' ', height + 1);
    move(y - 1, x);
    attroff(COLOR_PAIR(2));
    printw(name);

}

void drawPassangers() {
    for(int i = 0; i < PASSAGNERS_COUNT; i++){
        if(passangers[i].onStation != 0){
        	move(passangers[i].y, passangers[i].x);
        	printw("X");
        }
        if(debug){
			mvprintw(offset + i, 0, "Passanger %d - x: %d | y: %d | direction: %d | validTicket: %d | onStation: %d", 
			i,  passangers[i].x, passangers[i].y, passangers[i].direction, passangers[i].validTicket, passangers[i].onStation);
		}
    }
}

void drawMechanics() {
    for(int i = 0; i < MECHANIC_COUNT; i++){
        move(mechanics[i].y, mechanics[i].x);
        printw("R");
        if(debug){
        	mvprintw(offset + i, 100, "Mechanic %d - direction: %d", i,  busArray[i].onStation);	
    	}
    }
}

void malenRegistrierkasse(){
	move(stationA_Y+1, stationA_X+1);
	attrset(COLOR_PAIR(4));
    printw("        KASY       ");
    move(stationA_Y+1, stationB_X+1);
    printw("        KASY       ");
    attroff(COLOR_PAIR(4));

}

void drawBus(){
	int i;
	for(i = 0; i < BUS_COUNT; i++) {
		int x = busArray[i].x;
		int y = busArray[i].y;
		move(railway_y,x);
		printw("@");

		mvprintw(railway_y+1,x, "%d", busArray[i].endurance);
		mvprintw(railway_y-1,x, "%d", busArray[i].passangers);
		if(debug){
			mvprintw(offset + i + MECHANIC_COUNT + 2, 0, 
			"Bus %d - direction: %d | endurance: %d | onStation: %d | passangers: %d | onPlatform: %d",
			i,  busArray[i].direction, busArray[i].endurance,  busArray[i].onStation, busArray[i].passangers, busArray[i].onPlatform);
		}
	}
}

void drawRailway() {
    move(railway_y, railway_start_x);
    int distanceX = abs(railway_start_x - railway_end_x)+1;
    attrset(COLOR_PAIR(1));
    hline(' ', distanceX);
    attrset(COLOR_PAIR(1));
}

void *drawThreadWrapper(void *arg) {
	while(finish != true){
		pthread_mutex_lock(&drawMutex);

		erase();
		start_color();
		init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(2, COLOR_WHITE, COLOR_GREEN);
	init_pair(3, COLOR_WHITE, COLOR_CYAN);
	init_pair(4, COLOR_RED,COLOR_BLUE);
	init_pair(6,COLOR_YELLOW,COLOR_YELLOW);
	init_pair(7,COLOR_RED,COLOR_RED);
	mvprintw(0,0, "Jakub Pomykala 209897");
		drawStation(stationA_X, stationA_Y, stationA_W, stationA_H, "Stacja A");
	    drawStation(stationB_X, stationB_Y, stationB_W, stationB_H, "Stacja B");
	    malenRegistrierkasse();
	    drawPassangers();
	    drawMechanics();
		drawRailway();
		drawBus();
		refresh();



		pthread_mutex_unlock(&drawMutex);
	}
	pthread_exit(NULL);
}

//     ____  __  _______    ________  ______  _________    ____
//    / __ )/ / / / ___/   /_  __/ / / / __ \/ ____/   |  / __ \
//   / __  / / / /\__ \     / / / /_/ / /_/ / __/ / /| | / / / /
//  / /_/ / /_/ /___/ /    / / / __  / _, _/ /___/ ___ |/ /_/ /
// /_____/\____//____/    /_/ /_/ /_/_/ |_/_____/_/  |_/_____/
void busUpdate(void *arg) {
	struct bus* mBus = (struct bus*)arg;

	int i;
	int x = mBus->x;
	int y = mBus->y;

	//sprawdzenie kierunku jazdy
	bool shouldGoToLeft = x == railway_end_x;
	bool shouldGoToRight = x == railway_start_x;
	if(shouldGoToLeft){
		mBus->onStation = 1;
		mBus->direction = -1;
		mBus->endurance--;
		mBus->onPlatform = true;
	} else if (shouldGoToRight){
		mBus->onStation = -1;
		mBus->direction = 1;
		mBus->endurance--;
		mBus->onPlatform = true;
	} else{
		mBus->onStation = 0;
		mBus->onPlatform = false;
	}



	bool waitForPassangers = mBus->onStation != 0 && mBus->onPlatform;
	if (waitForPassangers) {
		pthread_cond_broadcast(&passangerCond);
		pthread_cond_wait(&busCond, &busMutex);		
	}

	bool needRepair = mBus->endurance == 0;
	if (needRepair){
		pthread_cond_broadcast(&mechanicCond);
		pthread_cond_wait(&busCond, &busMutex);	
	}

	//jazda wg kierunku
	if(mBus->direction == 1){
		x++;
	} else if (mBus->direction == -1) {
		x--;
	} 
	mBus->x = x;
}

void *busThreadWrapper(void *arg) {
	struct bus* mBus = (struct bus*)arg;
	while(finish != true){
		pthread_mutex_lock(&busMutex);
		busUpdate(mBus);
		usleep(BUS_SPEED*1000);
		pthread_mutex_unlock(&busMutex);
	}
	pthread_exit(NULL);
}

//     ____  ___   __________ ___    _   __________________     ________  ______  _________    ____
//    / __ \/   | / ___/ ___//   |  / | / / ____/ ____/ __ \   /_  __/ / / / __ \/ ____/   |  / __ \
//   / /_/ / /| | \__ \\__ \/ /| | /  |/ / / __/ __/ / /_/ /    / / / /_/ / /_/ / __/ / /| | / / / /
//  / ____/ ___ |___/ /__/ / ___ |/ /|  / /_/ / /___/ _, _/    / / / __  / _, _/ /___/ ___ |/ /_/ /
// /_/   /_/  |_/____/____/_/  |_/_/ |_/\____/_____/_/ |_|    /_/ /_/ /_/_/ |_/_____/_/  |_/_____/
void passangerUpdate(void *arg) {
	struct passanger* mPassanger = (struct passanger*)arg;


	bool shouldGoBuyTicket, shouldGoToBus;
	bool buyingTicket = mPassanger->y == stationA_Y+3;
	bool waitingForBus = mPassanger->validTicket && mPassanger->y == railway_y-1; 

	if(mPassanger->validTicket){
		shouldGoBuyTicket = false;
		shouldGoToBus = true;
	} else{
		shouldGoBuyTicket = true;
		shouldGoToBus = false;
	}

	if(shouldGoBuyTicket){
		mPassanger->direction = -1;
	} else if (shouldGoToBus){
		mPassanger->direction = 1;
	}

	if(buyingTicket){
		mPassanger->validTicket = true;
		mPassanger->x++;
	}
	usleep(20*1000);
	//wsiądź do autobusu
	int i;
	bool shouldGetOut = mPassanger->onStation != 0 && mPassanger->validTicket;
	bool shouldGetIn = mPassanger->onStation == 0;
	if(shouldGetOut){
		for(i = 0; i < BUS_COUNT; i++) {
			bool onSameStation = busArray[i].onStation == mPassanger->onStation;
			if(onSameStation && busArray[i].onPlatform && waitingForBus){
				busArray[i].passangers++;
				mPassanger->onStation = 0;
				pthread_cond_broadcast(&busCond);
				break;
			}
		}
	} else if(shouldGetIn) { //wysiadz do autobusu
		for(i = 0; i < BUS_COUNT; i++) {
			if(busArray[i].onPlatform) {
				if(busArray[i].passangers == 0){
					break;
				}
				busArray[i].passangers--;
				mPassanger->onStation = busArray[i].onStation;
				mPassanger->validTicket = false;
				if(mPassanger->onStation == -1) {
					mPassanger->x = railway_start_x-1;
				} else if(mPassanger->x = 1) {
					mPassanger->x = railway_end_x-1;
				}

				mPassanger->direction = 1;
				pthread_cond_broadcast(&busCond);
				break;
			}
		}
	}
		pthread_cond_broadcast(&busCond);


	//czekaj na platformie na odjazd
	waitingForBus = mPassanger->validTicket && mPassanger->y == railway_y-1; 
	if(waitingForBus) {
		mPassanger->direction = 0;
		pthread_cond_wait(&passangerCond, &passangerMutex);
	}

	//zmiana pozycji
	if(mPassanger->direction == -1){
		mPassanger->y--;
	} else if(mPassanger->direction == 1){
		mPassanger->y++;
	}
}

void *passangerThreadWrapper(void *arg) {
	struct passanger* mPassanger = (struct passanger*)arg;
	while(finish != true){
		pthread_mutex_lock(&passangerMutex);
		passangerUpdate(mPassanger);
		usleep(PASSAGNER_SPEED*1000);
		pthread_mutex_unlock(&passangerMutex);
	}
	pthread_exit(NULL);
}

//     __  _________________  _____    _   ____________   ________  ______  _________    ____
//    /  |/  / ____/ ____/ / / /   |  / | / /  _/ ____/  /_  __/ / / / __ \/ ____/   |  / __ \
//   / /|_/ / __/ / /   / /_/ / /| | /  |/ // // /        / / / /_/ / /_/ / __/ / /| | / / / /
//  / /  / / /___/ /___/ __  / ___ |/ /|  // // /___     / / / __  / _, _/ /___/ ___ |/ /_/ /
// /_/  /_/_____/\____/_/ /_/_/  |_/_/ |_/___/\____/    /_/ /_/ /_/_/ |_/_____/_/  |_/_____/
void mechanicUpdate(void *arg){
	struct mechanic* mMechanic = (struct mechanic*)arg;
	bool repairNeeded = false;
	int i;
	int activeMechanic;
	for(i = 0; i < BUS_COUNT; i++) {
		if(busArray[i].endurance == 0){
			repairNeeded = true;
			activeMechanic = busArray[i].onStation;
			break;
		}
	}

	mMechanic->y = railway_y;

	if(activeMechanic != mMechanic->onStation){
		return;
	}

	if(!repairNeeded){
		pthread_cond_wait(&mechanicCond, &mechanicMutex);
	}

	int step = 6;
	for(i = 0; i < step; i++){
		usleep(10*1000);
		mMechanic->x -= mMechanic->onStation;
	}

	for(i = 0; i < BUS_COUNT; i++) {
		if(busArray[i].onStation != 0){
			usleep(50*1000);
			busArray[i].endurance = 3;
			pthread_cond_broadcast(&busCond);
		}
	}

	for(i = 0; i < step; i++){
		usleep(MECHANIC_SPEED*1000);
		mMechanic->x += mMechanic->onStation;
	}	
}

void *mechanicThreadWrapper(void *arg) {
	struct mechanic* mMechanic = (struct mechanic*)arg;
	while(finish != true){
		pthread_mutex_lock(&mechanicMutex);
		mechanicUpdate(mMechanic);
		usleep(MECHANIC_SPEED*1000);
		pthread_mutex_unlock(&mechanicMutex);
	}
	pthread_exit(NULL);
}

//    _____ ______________  ______
//   / ___// ____/_  __/ / / / __ \
//   \__ \/ __/   / / / / / / /_/ /
//  ___/ / /___  / / / /_/ / ____/
// /____/_____/ /_/  \____/_/
void setup()
{
	railway_end_x = stationB_X + stationB_W - 8;
	railway_start_x = stationA_X + 8;
 	railway_y = (stationA_H/2)+stationA_Y+2;

	int i;
	for(i = 0; i < BUS_COUNT; i++) {
		busArray[i].x = (stationA_X + stationA_W) + (i * 18);
		busArray[i].y = railway_y;
		busArray[i].endurance = 3;
		busArray[i].passangers = 1;
		busArray[i].onStation = 0;
		bool randomDirection = i%2;
		if(randomDirection){
			busArray[i].direction = 1;
		} else{
			busArray[i].direction = -1;
		}
	}

	for(i = 0; i < PASSAGNERS_COUNT; i++){
		passangers[i].y = stationA_Y + rand()%5 + 5;
		passangers[i].validTicket = false;
		bool randomDirection = i%2;
		if(randomDirection){
			passangers[i].onStation  = 1;
			passangers[i].x = railway_end_x;
		} else{
			passangers[i].onStation  = -1;
			passangers[i].x = railway_start_x;
		}
	}

	mechanics[0].x = stationA_X + 1;
	mechanics[0].y = railway_y;
	mechanics[0].onStation = -1;

	mechanics[1].x = stationB_X+stationB_W-1;
	mechanics[1].y = railway_y;
	mechanics[1].onStation = 1;
}

//     __  ______    _____   __
//    /  |/  /   |  /  _/ | / /
//   / /|_/ / /| |  / //  |/ /
//  / /  / / ___ |_/ // /|  /
// /_/  /_/_/  |_/___/_/ |_/
int main(void) {
	initscr();
	curs_set(0);
	srand(time(NULL));
	setup();

	//START WATKOW - DRAW
	pthread_create(&drawThread, NULL, drawThreadWrapper, NULL);
	int i;
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
	pthread_mutex_destroy(&drawMutex);
	pthread_mutex_destroy(&mechanicMutex);
	pthread_mutex_destroy(&busMutex);
	pthread_mutex_destroy(&passangerMutex);

	pthread_cond_destroy(&mechanicCond);
	pthread_cond_destroy(&busCond);
	return EXIT_SUCCESS;
}