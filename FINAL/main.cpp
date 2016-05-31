#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <ncurses.h>
#include <list>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>
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
    //1 peron prawy
    //0 w autobusie
    int status;
};

struct bus {
	// -1 jedzie w lewo
	// 1 jedzie w prawo
	int direction;
	int capacity;
	int passangers;
	// lisst<passanger> passangers;

	//-1 - lewy
    // 1 - prawy
	int onStation;
	int x;
	int y;
	int endurance;
	// bool onPlatform;
};

struct place {
	int status;
};



struct mechanic {
    int x;
    int y;
    //-1 - lewy
    // 1 - prawy
    int onStation;
};

//    __________  _   _____________   _    _____    ____  _______    ____  __    ___________
//   / ____/ __ \/ | / / ___/_  __/  | |  / /   |  / __ \/  _/   |  / __ )/ /   / ____/ ___/
//  / /   / / / /  |/ /\__ \ / /     | | / / /| | / /_/ // // /| | / __  / /   / __/  \__ \
// / /___/ /_/ / /|  /___/ // /      | |/ / ___ |/ _, _// // ___ |/ /_/ / /___/ /___ ___/ /
// \____/\____/_/ |_//____//_/       |___/_/  |_/_/ |_/___/_/  |_/_____/_____/_____//____/

#define PASSAGNERS_COUNT 0
#define BOARD_HEIGHT 50
#define BOARD_WIDTH 50
#define MECHANIC_COUNT 2
#define BUS_COUNT 2

int BUS_SPEED = 40;
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

struct place board[BOARD_WIDTH][BOARD_HEIGHT];
struct bus busArray[BUS_COUNT];
struct passanger passangers[PASSAGNERS_COUNT];
struct mechanic mechanics[MECHANIC_COUNT];

bool finish = false;

//     ____  ________  ______  _________    ____  _____
//    / __ \/_  __/ / / / __ \/ ____/   |  / __ \/ ___/
//   / /_/ / / / / /_/ / /_/ / __/ / /| | / / / /\__ \
//  / ____/ / / / __  / _, _/ /___/ ___ |/ /_/ /___/ /
// /_/     /_/ /_/ /_/_/ |_/_____/_/  |_/_____//____/


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
pthread_cond_t platformCond = PTHREAD_COND_INITIALIZER;

//     ____  ____  ___ _       _______   ________
//    / __ \/ __ \/   | |     / /  _/ | / / ____/
//   / / / / /_/ / /| | | /| / // //  |/ / / __
//  / /_/ / _, _/ ___ | |/ |/ // // /|  / /_/ /
// /_____/_/ |_/_/  |_|__/|__/___/_/ |_/\____/
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

void drawPassangers() {
    for(int i = 0; i < PASSAGNERS_COUNT; i++){
        move(passangers[i].y, passangers[i].x);
        if(passangers[i].status != 0){
        	printw("X");	
        }
    }
}

void drawMechanics() {
    for(int i = 0; i < MECHANIC_COUNT; i++){
        move(mechanics[i].y, mechanics[i].x);
        printw("R");
        mvprintw(i, 40, "Mechanic %d - direction: %d", i,  busArray[i].onStation);	
    }
}

void drawBus(){
	int i;
	for(i = 0; i < BUS_COUNT; i++) {
		int x = busArray[i].x;
		int y = busArray[i].y;
		move(railway_y,x);
		printw("@");

		mvprintw(i, 0, "Bus %d - direction: %d", i,  busArray[i].direction);	
		mvprintw(i+1, 0, "Bus %d - onStation: %d", i,  busArray[i].onStation);	
		mvprintw(i+2, 0, "Bus %d - endurance: %d", i,  busArray[i].endurance);	

	}
}

void drawRailway() {
    move(railway_y, railway_start_x);
    int distanceX = abs(railway_start_x - railway_end_x)+1;
    hline('=', distanceX);
}

void *drawThreadWrapper(void *arg) {
	while(finish != true){
		pthread_mutex_lock(&drawMutex);

		erase();
		drawStation(stationA_X, stationA_Y, stationA_W, stationA_H, "Stacja A");
	    drawStation(stationB_X, stationB_Y, stationB_W, stationB_H, "Stacja B");
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
	} else if (shouldGoToRight){
		mBus->onStation = -1;
		mBus->direction = 1;
		mBus->endurance--;
	} else{
		mBus->onStation = 0;
	}

	if (mBus->endurance == 0){
		pthread_cond_broadcast(&mechanicCond);
		pthread_cond_wait(&repairCond, &repairMutex);	
	}

	// if(mBus->onStation == 1 || mBus->onStation == -1) {
	// 	pthread_cond_wait(&platformCond, &platformMutex);	
	// }

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

		pthread_mutex_lock(&repairMutex);
		usleep(40*1000);
		busUpdate(mBus);
		pthread_mutex_unlock(&repairMutex);
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

	if(mPassanger->x == stationA_X){
		mPassanger->validTicket = true;
	}

	int i;
	for(i = 0; i< BUS_COUNT; i++) {
		if(busArray[i].onStation != 0) {
			busArray[i].passangers = 1;
			mPassanger->validTicket = false;
			mPassanger->status = 0;
			busArray[i].onStation = 0;
			break;
		}
	}
	// if(busOnPlatform){
	// 	mPassanger->validTicket = false;
	// 	mPassanger->status = 0;
	// }

	// if(mPassanger->x == stationA_X+stationA_W){
	// 	mPassanger->validTicket = false;
	// 	mPassanger->status = 0;
	// }

	if(mPassanger->validTicket){
		if(mPassanger->x < stationA_X + stationA_W-1){
			mPassanger->x++;
		}
	} else{
		mPassanger->x--;
	}
}

void *passangerThreadWrapper(void *arg) {
	struct passanger* mPassanger = (struct passanger*)arg;
	while(finish != true){
		pthread_mutex_lock(&platformMutex);
		usleep(80*1000);
		passangerUpdate(mPassanger);
		pthread_mutex_unlock(&platformMutex);
	}
	return NULL;
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
		usleep(MECHANIC_SPEED*1000);
		mMechanic->x -= mMechanic->onStation;
	}

	for(i = 0; i < BUS_COUNT; i++) {
		if(busArray[i].onStation == -1 || busArray[i].onStation == 1){
			usleep(120*1000);
			busArray[i].endurance = 1;
		}
	}

	for(i = 0; i < step; i++){
		usleep(MECHANIC_SPEED*1000);
		mMechanic->x += mMechanic->onStation;
	}
	pthread_cond_broadcast(&repairCond);
}

void *mechanicThreadWrapper(void *arg) {
	struct mechanic* mMechanic = (struct mechanic*)arg;
	while(finish != true){
		pthread_mutex_lock(&mechanicMutex);
		usleep(MECHANIC_SPEED*1000);
		mechanicUpdate(mMechanic);
		pthread_mutex_unlock(&mechanicMutex);
	}
	return NULL;
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
 	railway_y = (stationA_H/2)+stationA_Y;

	int i;
	for(i = 0; i < BUS_COUNT; i++) {
		busArray[i].x = rand()%30 + (stationA_W+ stationA_X);
		busArray[i].y = railway_y;
		busArray[i].endurance = 1;
		busArray[i].capacity = 2;
		busArray[i].passangers = 0;
		busArray[i].onStation = 0;

		bool foo = i%2;
		if(foo){
			busArray[i].direction = 1;
		} else{
			busArray[i].direction = -1;
		}
	}

	for(i = 0; i < PASSAGNERS_COUNT; i++){
		passangers[i].x = rand()%10 + stationA_X + 2;
		passangers[i].y = rand()%5 + stationA_Y + 2;
		passangers[i].validTicket = false;
		passangers[i].status = -1;
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