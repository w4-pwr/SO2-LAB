/*
 ============================================================================
 Name        : Projekt Systemy Operacyjne 2
 Author      : Piotr Maćkowiak 184776 Poniedziałek 13:15
 Version     : 56.05
 Copyright   : Your copyright notice
 Description : Pthreads in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pthread.h"
#include <ncurses.h>
#include <errno.h>


struct statek {
	int team;
	//int amunicja;
	int x1,x2,x3;
	int y1,y2,y3;
	int zycie;
};

struct pole{
	int status; // 0 ,1 ,2
};


#define rozmiar 35
#define rozmiarx 160
#define ile 20
#define HP 15


struct pole plansza[rozmiarx][rozmiar];
struct statek statki[ile];
bool koniec =false;
int ilosc[2];

pthread_t watek[ile];
pthread_t rysowacz;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t health  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rysowanie  = PTHREAD_MUTEX_INITIALIZER;

void czysc ()
{
	int i,j;

	for(i=0;i<rozmiarx;i++)
	{
		for(j=0;j<rozmiar;j++)
		{
			plansza[i][j].status=0;
		}
	}
}

void pisz ()
{


	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLUE);
	init_pair(2, COLOR_WHITE, COLOR_GREEN);
	init_pair(3, COLOR_WHITE, COLOR_CYAN);
	init_pair(4, COLOR_RED,COLOR_BLUE);
	init_pair(6,COLOR_YELLOW,COLOR_YELLOW);
	init_pair(7,COLOR_RED,COLOR_RED);
	//usleep(400*10000);
	int i,j;
	//pthread_mutex_lock(&rysowanie);
	for(i=0;i<rozmiar;i++)
	{
		for(j=0;j<rozmiarx;j++)
		{
			char a;





			if(plansza[j][i].status==1)
			{		attrset(COLOR_PAIR(2));
					a='1';
					move(i+1, j+1);
				    addch(a);
				    attroff(COLOR_PAIR(2));

			};
			if(plansza[j][i].status==0)
			{
				attrset(COLOR_PAIR(1));
				a=' ';
				move(i+1, j+1);
				addch(a);
				attroff(COLOR_PAIR(1));


			};
			if(plansza[j][i].status==3)
					{
							attrset(COLOR_PAIR(4));
							a='X';
							move(i+1, j+1);
						    addch(a);
						    attroff(COLOR_PAIR(4));

					};
			if(plansza[j][i].status==4)
						{
						attrset(COLOR_PAIR(6));
						a=' ';
						move(i+1, j+1);
						addch(a);
						attroff(COLOR_PAIR(6));

					};
			if(plansza[j][i].status==5)
			{
					attrset(COLOR_PAIR(7));
					a=' ';
					move(i+1, j+1);
					addch(a);
					attroff(COLOR_PAIR(7));

			};
			if(plansza[j][i].status==2)
			{
				attrset(COLOR_PAIR(3));
				a='2';
			  move(i+1, j+1);
			    addch(a);
			    attroff(COLOR_PAIR(3));

			}
		}

	}
	attroff(COLOR_PAIR(1));

	 refresh();
	 //pthread_mutex_unlock(&rysowanie);


}

void ustaw (struct statek* okret,int druzyna,int index)
{
	ilosc[0]=ile/2;
	ilosc[1]=ile/2;

	okret->team=druzyna;
	okret->zycie=HP;

	bool a =false;

	do{
		a=false;
	okret->x1=rand()%rozmiarx;
	okret->y1=rand()%rozmiar;
	int x1=okret->x1;
	int y1=okret->y1;
	if((y1-2>0) && (plansza[x1][y1].status==0) && (plansza[x1][y1-1].status==0) && (plansza[x1][y1-2].status==0))
	{
		plansza[x1][y1].status=okret->team;
		plansza[x1][y1-1].status=okret->team;
		plansza[x1][y1-2].status=okret->team;
		okret->x2=x1;
		okret->y2=y1-1;
		okret->x3=x1;
		okret->y3=y1-2;

		a=true;
	}
	}while(a==false);

}

void ostrzal (struct statek* atakujacy, struct statek* atakowany)
{

			int x2[2];
			int y2[2];



			x2[0] = atakujacy->x2;
			y2[0] = atakujacy->y2;

	pthread_mutex_lock(&mutex);
	if(atakowany != NULL)
	{



		x2[1] = atakowany->x2;
		y2[1] = atakowany->y2;

		plansza[x2[0]][y2[0]].status=4;
		plansza[x2[1]][y2[1]].status=5;



	}
	else
	{
		plansza[x2[0]][y2[0]].status=4;
	}
	pthread_mutex_unlock(&mutex);
	//pisz();
	}




void niszcz (struct statek* okret)
{
	int x1 = okret->x1;
	int x2 = okret->x2;
	int x3 = okret->x3;
	int y1 = okret->y1;
	int y2 = okret->y2;
	int y3 = okret->y3;

	pthread_mutex_lock(&mutex);
	plansza[x1][y1].status=3;
	plansza[x2][y2].status=3;
	plansza[x3][y3].status=3;
	pthread_mutex_unlock(&mutex);
	}

void* rysunek (void* arg){
	while(koniec!=true)
	pisz();
}

void* akcja (void* arg){
	    struct statek* okret = (struct statek*)arg;
		int t=okret->team;
		int to;
		if(t==1)to=1;
		else to=0;
		while((okret->zycie>0) && (ilosc[to]>=1))
		{

			usleep(300*1000);
			int co = rand()%2;

			if(co == 1)//ruch
			{
							int x1 = okret->x1;
							int x2 = okret->x2;
							int x3 = okret->x3;
							int y1 = okret->y1;
							int y2 = okret->y2;
							int y3 = okret->y3;

				int kierunek = rand()%4; //1-gora 2- prawo 3- dol 0- lewo
				if(kierunek==0)
				{
							pthread_mutex_lock(&mutex);
					        if(x1-1>0)
					        if((plansza[x1-1][y1].status==0) && (plansza[x2-1][y2].status==0) && (plansza[x3-1][y3].status==0))
					        {
					        	plansza[x1-1][y1].status=okret->team;
					        	plansza[x2-1][y2].status=okret->team;
					        	plansza[x3-1][y3].status=okret->team;

					        	plansza[x1][y1].status=0;
					        	plansza[x2][y2].status=0;
					        	plansza[x3][y3].status=0;

					        	okret->x1=x1-1;
					        	okret->x2=x2-1;
					        	okret->x3=x3-1;
					        	okret->y1=y1;
					        	okret->y2=y2;
					        	okret->y3=y3;


					        }
					pthread_mutex_unlock(&mutex);

				}
				if(kierunek==1)
				{


					pthread_mutex_lock(&mutex);
					if((y1+1<rozmiar) && (plansza[x1][y1+1].status==0))
					{
						plansza[x1][y1].status=0;
						plansza[x2][y2].status=0;
						plansza[x3][y3].status=0;

						plansza[x1][y1+1].status=okret->team;
						plansza[x2][y2+1].status=okret->team;
						plansza[x3][y3+1].status=okret->team;



						okret->y1=y1+1;
						okret->y2=y2+1;
						okret->y3=y3+1;
						okret->x1=x1;
						okret->x2=x2;
						okret->x3=x3;

					}
					pthread_mutex_unlock(&mutex);

				}
				if(kierunek==2)
				{


					pthread_mutex_lock(&mutex);
			        if((x1+1<rozmiarx) && (plansza[x1+1][y1].status==0) && (plansza[x2+1][y2].status==0) && (plansza[x3+1][y3].status==0))
					{
						plansza[x1+1][y1].status=okret->team;
						plansza[x2+1][y2].status=okret->team;
						plansza[x3+1][y3].status=okret->team;

						plansza[x1][y1].status=0;
						plansza[x2][y2].status=0;
						plansza[x3][y3].status=0;
						okret->x1=x1+1;
						okret->x2=x2+1;
						okret->x3=x3+1;
						okret->y1=y1;
						okret->y2=y2;
						okret->y3=y3;

					}
					pthread_mutex_unlock(&mutex);


				}
				if(kierunek==3)
				{


					pthread_mutex_lock(&mutex);
					if((y3-1>0) && (plansza[x3][y3-1].status==0))
					{
						plansza[x1][y1].status=0;
					    plansza[x2][y2].status=0;
						plansza[x3][y3].status=0;

						plansza[x1][y1-1].status=okret->team;
					    plansza[x2][y2-1].status=okret->team;
						plansza[x3][y3-1].status=okret->team;



					     okret->y1=y1-1;
						 okret->y2=y2-1;
						 okret->y3=y3-1;
						 okret->x1=x1;
						 okret->x2=x2;
					     okret->x3=x3;

					 }
					pthread_mutex_unlock(&mutex);


				}

			}
			if(co==0)//strzelanie
			{
				usleep(300*1000);
				int k = rand()% ile;
				if(statki[k].team!=okret->team)
				{

				if(statki[k].zycie>0)
				{

				statki[k].zycie--;

				ostrzal(okret,&statki[k]);

				}
				else
				{
					ostrzal(okret,NULL);

				}

				}
				else
				{
					ostrzal(okret,NULL);

				}

			}
		}
		if(okret->zycie==0)
						{
						pthread_mutex_lock(&health);
						ilosc[t-1]--;
						pthread_mutex_unlock(&health);
						niszcz(okret);
						if(ilosc[t-1]==0)
						{

							koniec=true;

						}

						}
		return NULL;
}



int main(void) {

	initscr();
	curs_set(0);
	srand(time(NULL));
	int i;
	for(i=0;i<ile;i++)
	{
		int team = i%2+1;
		ustaw(&statki[i],team,i);
	}
	//printf("\n\n\n");

	 pthread_create(&rysowacz, NULL, rysunek, NULL);
//	akcja(&statki[1]);
	for(i=0;i<ile;i++)
	{
		pthread_create(&watek[i], NULL, akcja, &statki[i]);

	}
	pthread_join(rysowacz, NULL);
	for (i=0; i < ile; i++)
	{
        pthread_join(watek[i], NULL);

	}

	endwin();
	for (i=0; i < ile; i++)
	{
		printf("%d. HP= %d Drużyna= %d\n ",i,statki[i].zycie,statki[i].team);
	}

	pthread_mutex_destroy(&mutex);
	pthread_mutex_destroy(&mutex2);
	pthread_mutex_destroy(&health);
	pthread_mutex_destroy(&rysowanie);
	return EXIT_SUCCESS;
}