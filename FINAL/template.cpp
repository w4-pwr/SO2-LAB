 #include <ncurses.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <list>

#define PASSAGNERS_NUM_THREADS  2
#define TRAINS_NUM_THREADS   3
#define BUSES_NUM_THREADS     3
#define NUM_THREADS     3
using namespace std;


bool repaint = true;


int horizontal_railway_y = 0;
int vertical_railway_x = 0;

int station_INT_X = 50;
int station_INT_Y = 15;
int station_INT_W = 40;
int station_INT_H = 30;

int station_EXT_X = 40;
int station_EXT_Y = 10;
int station_EXT_W = 60;
int station_EXT_H = 40;
//STRUCTS/////////////////////////////////////////////////////////////
struct passanger {
    char letter;
    bool validTicket;
    bool needBePainted;
    bool isTraveling;
    int X;
    int Y;
    int status; //0-left,1-right, 2-top, 3-down
    passanger() {
 		isTraveling = false;
    }
};


struct station {
    list<passanger *> passangerList;

};

struct locomotion {
    list<passanger *> passangerList;
    int capacity;
    bool leftDirection = true;
    int X;
    int Y;
    char sign;
    locomotion(){
        X = 1;
        Y = 0;
    }
};
//THREADS///////////////////////////////////////////////////////////


void drawVerticalRailway();

void initPassangerThread();

void drawPassangers();

pthread_t pasangersThreads[PASSAGNERS_NUM_THREADS];
pthread_t trainThreads [TRAINS_NUM_THREADS];
pthread_t busesThreads[BUSES_NUM_THREADS];


//MUTEXES
pthread_mutex_t platformMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t drawMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t passangerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t crossingMutex = PTHREAD_MUTEX_INITIALIZER;

// pthread_cond_t leftPlatformCond = PTHREAD_COND_INITIALIZER;
// pthread_cond_t rightPlatformCond = PTHREAD_COND_INITIALIZER;
// pthread_cond_t downPlatformCond = PTHREAD_COND_INITIALIZER;
// pthread_cond_t topPlatformCond = PTHREAD_COND_INITIALIZER;


pthread_cond_t leftToRightPassangerCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t rightToLeftPassangerCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t downToTopPassangerCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t topToDownPassangerCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t passangerCond = PTHREAD_COND_INITIALIZER;

pthread_cond_t crossingCond = PTHREAD_COND_INITIALIZER;

//RESOURCES
bool freeLeftPlaform = true;
bool freeRightPlaform = true;
bool freeTopPlaform = true;
bool freeDownPlaform = true;
bool crossingIsEmpty = true;







bool freePayDeskA = true;
bool freePayDeskB = true;

station *stationA = new station();
station *stationB = new station();

locomotion *trains;
locomotion *buses;
passanger* passangers;
//NCURSES PAINTINGS
//DEBUG print
int line = 0;

void printfText(char *text) {
    move(line++, 0);
    printw(text);
}

//NCURSES
int SCREEN_HEIGHT = 0;
int SCREEN_WIDTH = 0;



//x - to poziom
//y - to pion

int MAX_TRAIN_CAPACITY = 3;

bool finish = false;

void drawSquare(int x, int y, int width, int height) {
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

}


void drawTrains(){
	for(int i= 0; i < TRAINS_NUM_THREADS; i++){
		int x = trains[i].X;
		int y = trains[i].Y;
		move(horizontal_railway_y, x);
		printw( "%d", i );
	}
}

void drawBuses(){
	for(int i= 0; i < BUSES_NUM_THREADS; i++){
		int x = buses[i].X;
		int y = buses[i].Y;
		move(y, vertical_railway_x);
		printw( "%c", i + 'A' );
	}
}
void drawVerticalRailway() { //|
    
    
    move(station_INT_Y, vertical_railway_x );

    vline('=', station_INT_H+1);
}

void drawHorizontalRailway() {//-
    
    move(horizontal_railway_y, station_INT_X);
    hline('=', station_INT_W+1);
}


void drawStationAndRailways() {
    drawSquare(station_INT_X, station_INT_Y, station_INT_W, station_INT_H); //draw internal rect
    drawSquare(station_EXT_X, station_EXT_Y, station_EXT_W, station_EXT_H); //draw external rect

    drawHorizontalRailway();
    drawVerticalRailway();
 	move(0,0);
}

void drawCheckout(){

}

void drawPassangers() {
    for(int i = 0; i<PASSAGNERS_NUM_THREADS;i++){
    	if(passangers[i].needBePainted){
        	move(passangers[i].Y, passangers[i].X);
        	printw( "%c", i + 'a');
        	//printw("X");
    	}
    }
}

void initNcurses() {
    initscr();
    getmaxyx(stdscr, SCREEN_HEIGHT, SCREEN_WIDTH);
}

void quitNcurses() {
    getch();
    endwin();
}

void *drawThread(void *arg){

    while(!finish){
        pthread_mutex_lock(&drawMutex);
        erase();
        drawStationAndRailways();
        drawCheckout();
        drawTrains();
        drawBuses();
        drawPassangers();
        refresh();
        
        pthread_mutex_unlock(&drawMutex);
    }
}


/////////////////////////////////////////////////////////////////////////////////

void * trainThreadFunction(void *tid) {
   locomotion* currentLocomotion =  (locomotion*) tid;

    while (!finish) {
        usleep(300*1000);

        bool rideFromRightToLeft = currentLocomotion->leftDirection;
        if (rideFromRightToLeft) {

            if(currentLocomotion->X == vertical_railway_x + 1 ){
        		pthread_mutex_lock(&crossingMutex);
        		while(!crossingIsEmpty){
        			pthread_cond_wait(&crossingCond, &crossingMutex);
        		}
                crossingIsEmpty = false;//zajmij przejazd
        		pthread_mutex_unlock(&crossingMutex);
        	}

        	if(currentLocomotion->X == vertical_railway_x - 1 ){
				pthread_mutex_lock(&crossingMutex);
        		crossingIsEmpty = true;
        		pthread_cond_broadcast(&crossingCond);
	        	pthread_mutex_unlock(&crossingMutex);	
        	}


            if (currentLocomotion->X == station_INT_X + 1) {
             	//wysadz pasazerow:
      			std::list<passanger*>::const_iterator it;
            	for(list<passanger*>::iterator it= currentLocomotion->passangerList.begin(); it!= currentLocomotion->passangerList.end(); ++it){
           				//(*it)->status = 0;
           				(*it)->X = station_INT_X-1;
           				(*it)->Y = horizontal_railway_y;
           				(*it)->isTraveling = false;
           				(*it)->needBePainted = true;
           		}
           		currentLocomotion->passangerList.clear();
           		pthread_cond_broadcast(&passangerCond);

            	currentLocomotion->leftDirection = false;
				
				//odjazd          
	            for(int i = 0; i<PASSAGNERS_NUM_THREADS; i++){
           			if(!passangers[i].needBePainted &&  !passangers[i].isTraveling && passangers[i].status == 1){
           				passangers[i].isTraveling = true;
           				currentLocomotion->passangerList.push_back(&passangers[i]);
           			}

           		}
	            
           }	
            currentLocomotion->X--;

        } else {

        	if(currentLocomotion->X == vertical_railway_x -1 ){
        		pthread_mutex_lock(&crossingMutex);
        		while(!crossingIsEmpty){
        			pthread_cond_wait(&crossingCond, &crossingMutex);
        		}
                crossingIsEmpty = false;//zajmij przejazd
        		pthread_mutex_unlock(&crossingMutex);
        	}
        	if(currentLocomotion->X == vertical_railway_x + 1 ){
				pthread_mutex_lock(&crossingMutex);
        		crossingIsEmpty = true;
        		pthread_cond_broadcast(&crossingCond);
	            pthread_mutex_unlock(&crossingMutex);	
        	}

            // if (currentLocomotion->X == station_INT_X + station_INT_W - 1 ) {
            //     //jestemy przy prawym dworcu
            //     currentLocomotion->leftDirection = true;
            // }

            if (currentLocomotion->X == station_INT_X + station_INT_W - 1) {
            	//wysadz pasazerow:

				std::list<passanger*>::const_iterator it;
            	for(list<passanger*>::iterator it= currentLocomotion->passangerList.begin(); it!= currentLocomotion->passangerList.end(); ++it){
           				//(*it)->status = 1;
           				(*it)->X = station_INT_X + station_INT_W + 1 ;
           				(*it)->Y = horizontal_railway_y;
           				(*it)->isTraveling = false;
           				(*it)->needBePainted = true;
           		}
           		

           		currentLocomotion->passangerList.clear();
           		pthread_cond_broadcast(&passangerCond);
    			

            	currentLocomotion->leftDirection = true;
				
				//wez pasazerow	            
	            for(int i = 0; i<PASSAGNERS_NUM_THREADS; i++){
           			if(!passangers[i].needBePainted && !passangers[i].isTraveling && passangers[i].status == 0){
           				passangers[i].isTraveling = true;
           				currentLocomotion->passangerList.push_back(&passangers[i]);
           			}
           		}
    //         	pthread_mutex_lock(&platformMutex);
	   //          //freeLeftPlaform = true;
	   //          pthread_cond_broadcast(&leftToPlatformCond);
	   //          pthread_mutex_unlock(&platformMutex);
				// //currentLocomotion->X++;
	            
            }
            currentLocomotion->X++;
        }

    }
    pthread_exit(NULL);
}

void initTrainThreads() {

    for (int i = 0; i < TRAINS_NUM_THREADS; i++) {

        int rc = pthread_create(&trainThreads[i], NULL, trainThreadFunction, &trains[i]);

        if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
    }
}

void * busThreadFunction(void *tid) {
   locomotion* currentLocomotion =  (locomotion*) tid;

    while (!finish) {
        usleep(300*1000);

        bool rideFromTopToDown = currentLocomotion->leftDirection;
        if (rideFromTopToDown) {
        	if(currentLocomotion->Y == horizontal_railway_y - 1 ){
        		pthread_mutex_lock(&crossingMutex);
        		while(!crossingIsEmpty){
        			pthread_cond_wait(&crossingCond, &crossingMutex);
        		}
        		
                crossingIsEmpty = false;//zajmij przejazd
                pthread_mutex_unlock(&crossingMutex);
        	}

        	if(currentLocomotion->Y == horizontal_railway_y + 1 ){
        		pthread_mutex_lock(&crossingMutex);
        		crossingIsEmpty = true;
        		pthread_cond_broadcast(&crossingCond);
				pthread_mutex_unlock(&crossingMutex);
        	}
			
            if (currentLocomotion->Y == station_INT_Y + station_INT_H - 1   ) {
                
                //jestesmy przy dolnym dworcu

            	//wysadz pasazerow:

				std::list<passanger*>::const_iterator it;
            	for(list<passanger*>::iterator it= currentLocomotion->passangerList.begin(); it!= currentLocomotion->passangerList.end(); ++it){
           				//(*it)->status = 3;
           				(*it)->X = vertical_railway_x ;
           				(*it)->Y = station_INT_Y + station_INT_H + 1;
           				(*it)->isTraveling = false;
           				(*it)->needBePainted = true;
           		}
           		
           		currentLocomotion->passangerList.clear();
           		pthread_cond_broadcast(&passangerCond);
    			

            	currentLocomotion->leftDirection = false;
				
				//wez pasazerow	            
	            for(int i = 0; i<PASSAGNERS_NUM_THREADS; i++){
           			if(!passangers[i].needBePainted && !passangers[i].isTraveling && passangers[i].status == 2){
           				passangers[i].isTraveling = true;
           				currentLocomotion->passangerList.push_back(&passangers[i]);
           			}
           		}

            }
            currentLocomotion->Y++; 
        } else {// jedziemy z dolu na gore

            if(currentLocomotion->Y == horizontal_railway_y + 1 ){
        		pthread_mutex_lock(&crossingMutex);
        		while(!crossingIsEmpty){
        			pthread_cond_wait(&crossingCond, &crossingMutex);
        		}
                crossingIsEmpty = false;//zajmij przejazd
        		pthread_mutex_unlock(&crossingMutex);
        	}

        	if(currentLocomotion->Y == horizontal_railway_y - 1 ){
				pthread_mutex_lock(&crossingMutex);
        		crossingIsEmpty = true;
        		pthread_cond_broadcast(&crossingCond);
				pthread_mutex_unlock(&crossingMutex);	
        	}

             if (currentLocomotion->Y == station_INT_Y + 1   ) {
                
                //jestesmy przy gornym dworcu

            	//wysadz pasazerow:

				std::list<passanger*>::const_iterator it;
            	for(list<passanger*>::iterator it= currentLocomotion->passangerList.begin(); it!= currentLocomotion->passangerList.end(); ++it){
           				//(*it)->status = 2;
           				(*it)->X = vertical_railway_x ;
           				(*it)->Y = station_INT_Y - 1;
           				(*it)->isTraveling = false;
           				(*it)->needBePainted = true;
           		}
           		
           		currentLocomotion->passangerList.clear();
           		pthread_cond_broadcast(&passangerCond);
    			

            	currentLocomotion->leftDirection = true;
				
				//wez pasazerow	            
	            for(int i = 0; i<PASSAGNERS_NUM_THREADS; i++){
           			if(!passangers[i].needBePainted && !passangers[i].isTraveling && passangers[i].status == 3){
           				passangers[i].isTraveling = true;
           				currentLocomotion->passangerList.push_back(&passangers[i]);
           			}
           		}

            }

            currentLocomotion->Y--; 

        }
    }
}

void initBusesThreads(){
	for (int i = 0; i < BUSES_NUM_THREADS; i++) {

        int rc = pthread_create(&busesThreads[i], NULL, busThreadFunction, &buses[i]);

        if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
    }
}

void * passangerThreadFunction(void *tid) {
    passanger* currentPassanger = (passanger*) tid;
	int status = currentPassanger->status;

	int tm = 100000 + rand() % 800000;

    while(!finish){
        switch(status){//0-left,1-right, 2-top, 3-down
            case 0: //left 
            	
                if(currentPassanger->Y >= station_INT_Y ){
                    currentPassanger->Y--;
               	}else{
                    if(currentPassanger->X == vertical_railway_x){
                    	currentPassanger->needBePainted = false;
	                    pthread_mutex_lock(&passangerMutex);
	                    currentPassanger->status = 3;
		        		while(!currentPassanger->needBePainted){
	                    	pthread_cond_wait(&passangerCond, &passangerMutex);
		        		}
		        		break;
		        		pthread_mutex_unlock(&passangerMutex);

                    	//currentPassanger->status = 1;

                    }else{
                        currentPassanger->X++;
                    }

               }
               break;
            case 1:
                if(currentPassanger->Y <= station_INT_Y + station_INT_H){
                    currentPassanger->Y++;
                 }else{
                    if(currentPassanger->X == vertical_railway_x){
                        pthread_mutex_lock(&passangerMutex);
                        currentPassanger->needBePainted = false;
                        currentPassanger->status = 2;
		        		while(!currentPassanger->needBePainted){
                         	pthread_cond_wait(&passangerCond, &passangerMutex);
		        		}
		        		break;
		        		pthread_mutex_unlock(&passangerMutex);

                    }else{
                        currentPassanger->X--;
                    }
                }
               break;
            case 2:
                if(currentPassanger->X < station_INT_X + station_INT_W + 1 ){
                    currentPassanger->X++;
                 }else{

                    if(currentPassanger->Y == horizontal_railway_y){
                        pthread_mutex_lock(&passangerMutex);
		        		currentPassanger->needBePainted = false;
		        		currentPassanger->status = 0;
		        		while(!currentPassanger->needBePainted ){
		        			pthread_cond_wait(&passangerCond, &passangerMutex);
		        			
		        		}
		        		break;
		        		pthread_mutex_unlock(&passangerMutex);

                    }else{
                        currentPassanger->Y++;
                    }
                }
               break;
            case 3:
                if(currentPassanger->X > station_INT_X - 1 ){
                    currentPassanger->X--;
                 }else{
                    
                    if(currentPassanger->Y == horizontal_railway_y){
                   		pthread_mutex_lock(&passangerMutex);
                   		currentPassanger->needBePainted = false;
                   		currentPassanger->status = 1;
                   		while(!currentPassanger->needBePainted){
		        			pthread_cond_wait(&passangerCond, &passangerMutex);
                   		}
                   		break;
		        		pthread_mutex_unlock(&passangerMutex);
                    }else{
                        currentPassanger->Y--;
                    }

                }
               break;
            }
            //refresh();
            usleep(tm);
    }
}

void initPassangerThread() {

    for (int i = 0; i < TRAINS_NUM_THREADS; i++) {

        int rc = pthread_create(&pasangersThreads[i], NULL, passangerThreadFunction, &passangers[i]);

        if (rc) {
            cout << "Error:unable to create thread," << rc << endl;
            exit(-1);
        }
        usleep(100000);
    }
}

void initDrawThread(){
    int rc;

	repaint = true;
	pthread_t drawingThread;
	rc = pthread_create(&drawingThread, NULL, drawThread, NULL);

    if (rc) {
        cout << "Error:unable to create draw thread," << rc << endl;
        exit(-1);
    }
}

int main() {
    initNcurses();
    srand( time( NULL ) );

    horizontal_railway_y = (station_INT_H/2)+station_INT_Y;    
    vertical_railway_x = (station_INT_W/2)+station_INT_X;
    
     trains = new locomotion[TRAINS_NUM_THREADS];
     for(int i =0;i<TRAINS_NUM_THREADS;i++){
	     trains[i].X = rand()%(station_INT_W -1 )  + station_INT_X;
	     trains[i].Y = horizontal_railway_y;
	     if(rand()%2>1)
        	 trains[i].leftDirection = true;
     	 else
     		trains[i].leftDirection = false;
        
     }

     buses = new locomotion[BUSES_NUM_THREADS];
     for(int i =0;i<TRAINS_NUM_THREADS;i++){
         buses[i].X = vertical_railway_x;
         buses[i].Y = rand()%(station_INT_H - 1 ) + station_INT_Y;
         
    }

    passangers = new passanger[PASSAGNERS_NUM_THREADS];
    for(int i = 0; i<PASSAGNERS_NUM_THREADS; i++){
       
        passangers[i].validTicket = false;
        passangers[i].needBePainted = true;
        passangers[i].status = rand()%4;
        switch(passangers[i].status){
            case 0:
                passangers[i].X =  station_INT_X - 1 ;// zeby nie isc po scianie
                passangers[i].Y =  horizontal_railway_y;
                break;
            case 1:
                passangers[i].X =  station_INT_X + station_INT_W + 1 ; // zeby nie isc po scianie
                passangers[i].Y =  horizontal_railway_y;
                break;
            case 2:
                passangers[i].X = vertical_railway_x ;
                passangers[i].Y = station_INT_Y - 1;
                break;
            case 3:
                passangers[i].X =  vertical_railway_x ;
                passangers[i].Y = station_INT_Y + station_INT_H + 1;
                break;
        }
    }
   

	initDrawThread();
    initTrainThreads();
    initBusesThreads();
    initPassangerThread();
	string c;
	cin>>c;
	if(c=="q")
		finish = true;	


    for (int i=0; i<TRAINS_NUM_THREADS; i++) {
        pthread_join(trainThreads[i], NULL);
    }
    for (int i=0; i<BUSES_NUM_THREADS; i++) {
        pthread_join(trainThreads[i], NULL);
    }
    for (int i=0; i<PASSAGNERS_NUM_THREADS; i++) {
        pthread_join(pasangersThreads[i], NULL);
    }
   	
   	
    quitNcurses();

    return 0;
}


2