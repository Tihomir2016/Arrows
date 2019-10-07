#include <iostream>
#include <ncurses.h>
#include <pthread.h>
#include <math.h>
#include <cstdlib>
#include <time.h>
#include "Enemy.h"
#include <vector>
#include "Coin.h"


using std::vector;

char inp = 0;
const int Board_x =120, Board_y =38;              //граница экрана
int position_x = 0, position_y = 0;              //начальная позиция игрока
const int winposx = Board_x-9, winposy = Board_y-3;     //выигрыш
int x, y, x2, y2; // рамки

vector<Enemy> Arrows;                              //стрелы
int enemyposx=Board_x, enemyposy=rand()%Board_y+3; // координаты врага
bool quit = false;
bool IsDead = false;    

vector<Coin> Coin_Item;                 //монеты
int coin_pos_x=rand()%(Board_x - 10);
int coin_pos_y=rand()%Board_y+3;
int coin_amount = 10, // кол-во монет, которое необходимо собрать для победы
coin_counter = coin_amount;   //кол-во оставшихся монет




void GAME_restart() //перезапуск игры 
{
        clear();
        
        //cброс к-т персонажа
	position_x = 0;
        position_y = 0;
        mvaddstr(round(position_y),   round(position_x), " # ");
        mvaddstr(round(position_y)+1, round(position_x), "=+=");
        mvaddstr(round(position_y)+2, round(position_x), "/ \\");
        
	Arrows.clear(); Coin_Item.clear(); //чистим стрелы и монеты
	Arrows.push_back(Enemy(enemyposy, enemyposx)); //добавляем стрелу
        IsDead = false;
        coin_counter = coin_amount;
        for (int i =0; i < coin_amount; i++)
        	Coin_Item.push_back( Coin(rand()%(Board_y-3)+3, rand()%(Board_x-10)) ); // появляются монеты
}

bool UpdateArrows();

static void *Input(void *none)  //потоковая функция ввода
{
	cbreak(); noecho();
	keypad(stdscr, TRUE);
	inp = 0;

	while (!quit)
	{
		/*
		inp = getch();
                if( getch() == 0) // ^@  
                    getch();
		
		//refresh();
		
               if(!IsDead)
               {
                    if (inp == 'w')
			position_y -= 1;
                    if (inp == 's')
			position_y += 1;
                    if (inp == 'a')
			position_x -= 1;
                    if (inp == 'd')
			position_x += 1;
                    if (inp == 'q')
			quit = true;
                    if(inp == 'r')
                        GAME_restart();
		    
                   
                }   
                
                else //урезаем выбор
                {
                    if (inp == 'q')
                        quit = true;
                    if(inp == 'r')
                        GAME_restart();
                    
                }

                //персонаж не может выйти за границы игрового поля
		if (position_x > Board_x-9) 
                    position_x = Board_x-9;
		if (position_x < 0) 
                    position_x = 0;
		if (position_y > Board_y-3)
                    position_y = Board_y-3;
		if (position_y < 0) 
                    position_y = 0;
                    */
                inp = getch();

               if(!IsDead)
               {if (inp == 'w')
			position_y -= 1;
		if (inp == 's')
			position_y += 1;
		if (inp == 'a')
			position_x -= 1;
		if (inp == 'd')
			position_x += 1;
		if (inp == 'q')
			quit = true;
                if(inp == 'r')
                GAME_restart();}
                
                else 
                {
                    if (inp == 'q')
                        quit = true;
                    if(inp == 'r')
                        GAME_restart();
                    
                }


		if (position_x > Board_x-9) 
                    position_x = Board_x-9;
		if (position_x < 0) 
                    position_x = 0;
		if (position_y > Board_y-3)
                    position_y = Board_y-3;
		if (position_y < 0) 
                    position_y = 0;
		
	}

	pthread_exit(NULL);
	return NULL;
}


bool UpdateArrows() // движение стрел, попадание в игрока
{
	for (int i = 0; i < Arrows.size(); i++)
	{
            mvaddstr(round(Arrows.at(i).enemyposy),round(Arrows.at(i).enemyposx),"<--");
            for (int j=0; j<3; j++)
            if(   (position_x +j==   Arrows.at(i).enemyposx && position_y == Arrows.at(i).enemyposy)   
        	||(position_x +j== Arrows.at(i).enemyposx && position_y+1== Arrows.at(i).enemyposy)
        	||(position_x +j== Arrows.at(i).enemyposx && position_y+2== Arrows.at(i).enemyposy))
    	    {   clear();
                mvprintw(round(Board_y / 2),round(Board_x/2),"YOU ARE DEAD!!!");
                mvprintw(round(Board_y / 2 +1),round(Board_x/2),"press \"q\" to exit");
                mvprintw(round(Board_y / 2 +2),round(Board_x/2),"press \"r\" to restart"); IsDead = true;
                return true;} 
        
        
        if(Arrows.at(i).enemyposx <= 0) // возвращение стрелы после прохождения пути
 	       {Arrows.at(i).enemyposx = Board_x; Arrows.at(i).enemyposy = rand()%(Board_y - 3)+3;}
        Arrows.at(i).enemyposx--;
	} 
	return false;
}

int UpdateCoins()
{
    for(int i=0; i < coin_counter; i++)
    {
        mvaddstr(Coin_Item.at(i).coin_pos_y, Coin_Item.at(i).coin_pos_x, " @ ");
        mvprintw(Board_y+2,1, "need to collect:%d",coin_amount);
        mvprintw(Board_y+3,1, "collected:%d",coin_counter);
    }
    mvprintw(Board_y+2,1, "need to collect:%d",coin_amount);
        mvprintw(Board_y+3,1, "collected:%d",coin_counter);
    for (int j=0; j < Coin_Item.size();j++) // собирание монет if true - монетка удаляется из векторa, иначе просто возвращается значение счетчика
    {
        if(   (position_x == Coin_Item.at(j).coin_pos_x && position_y == Coin_Item.at(j).coin_pos_y)   
        	||(position_x == Coin_Item.at(j).coin_pos_x && position_y+1== Coin_Item.at(j).coin_pos_y)
        	||(position_x == Coin_Item.at(j).coin_pos_x && position_y+2== Coin_Item.at(j).coin_pos_y)
                ||(position_x == Coin_Item.at(j).coin_pos_x && position_y == Coin_Item.at(j).coin_pos_y)   
        	||(position_x+1 == Coin_Item.at(j).coin_pos_x && position_y+1== Coin_Item.at(j).coin_pos_y)
        	||(position_x+1 == Coin_Item.at(j).coin_pos_x && position_y+2== Coin_Item.at(j).coin_pos_y)
                ||(position_x-1 == Coin_Item.at(j).coin_pos_x && position_y+1== Coin_Item.at(j).coin_pos_y)
        	||(position_x-1 == Coin_Item.at(j).coin_pos_x && position_y+2== Coin_Item.at(j).coin_pos_y)
        )
            {  
               std::cout << '\a';
               Coin_Item.erase(Coin_Item.begin()+j);
               coin_counter--; 
               return coin_counter;
            }   
    } 
    return coin_counter;
}
int main()
{
	srand(time(0));
	if (initscr() == NULL)
	{
		std::cout << "can't load ncurses.";
		return 1;
	} 
	printw("Press S to start\n");
	cbreak(); noecho(); raw();
	keypad(stdscr, TRUE);
        while(getch()!='s') 
        {
            clear();
            printw("Press S to start\n");
            
        }
	
	
	pthread_t inputthread;
	void *retval;
	pthread_create(&inputthread, NULL, Input, NULL);
        
    	Arrows.push_back(Enemy(Board_x,rand()%(Board_y-3)+3 ));//появляется первая стрела
   	 for (int i =0; i < coin_amount; i++)
        	Coin_Item.push_back(Coin(rand()%(Board_y-3)+3,rand()%(Board_x-10))); // появляются монетки

	 while (!quit)
	{
		
                napms(45);
	
		clear();

                //границы игрового поля
		
                y=Board_y;
                for (x = 0; x < Board_x-2; x++)
                    mvaddstr(y,x,"=");
                x2=Board_x-6;
                for (y2 = 0; y2 <Board_y; y2++)
                    mvaddstr(y2,x2,"||");

		
                coin_counter = UpdateCoins();   // обновление coin_counter
		
		

                if( !(UpdateArrows()))
                {
			if(coin_counter==0 && ( position_x == winposx && position_y ==winposy ))  // выигрыш
                	{
				
				mvprintw(Board_y / 2,Board_x/2, "YOU ARE WINNER!!!");
                    		mvprintw(round(Board_y / 2 +1),round(Board_x/2),"press \"q\" to exit");
                    		mvprintw(round(Board_y / 2 +2),round(Board_x/2),"press \"r\" to restart with more coins");
                                mvprintw(round(Board_y / 2 +3),round(Board_x/2),"game ends after 20 coins");
	                        while(inp != 'q' || inp != 'r')
                    		{
                        		if (inp == 'q')
                        		break;
                        		if(inp == 'r')
                    			{
                    	    			clear();
						Coin_Item.clear();
                        			if ( coin_amount < 16)
                                                {
                                                    coin_amount +=5;
                                                    GAME_restart();
                                                    clear();break;
                                                } 
                                                else 
                                                    clear();
                                                    quit=true;
                        			break;
                    			}
                    		}goto END; //при прохождении игры 
                    	}
                }   
                
                

		for(int j=0;Arrows.size() < 35  &&  j < Arrows.size(); j++)   // появление стрел
            		if(Arrows.at(j).enemyposx == 30)
            			Arrows.push_back(Enemy(Board_x,rand()%(Board_y-3)+3 ));
                
                //персонаж
		mvaddstr(round(position_y),   round(position_x), " # ");
		mvaddstr(round(position_y)+1, round(position_x), "=+=");
		mvaddstr(round(position_y)+2, round(position_x), "/ \\");
                
                //дверь
                mvaddstr(round(winposy),   round(winposx), "###");
                mvaddstr(round(winposy)+1, round(winposx), "###");
                mvaddstr(round(winposy)+2, round(winposx), "###");
                END:;
                refresh();
                
                

		
	
            
                
        } 
	clear();
        

	void* result;
	pthread_join(inputthread,&result);

	endwin();

	pthread_exit(NULL);
        

	return 0;

}
