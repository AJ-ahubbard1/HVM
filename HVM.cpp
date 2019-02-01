//author: Joey Hubbard
//sent 1-31-19
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

bool gameOver;
bool firing;
bool firingNuke;
bool firingMG;
bool firingBurst;
bool Bonus;
bool levelUp;
bool nukeAdd;
bool mgAdd;
bool Boss;
int nukes;
int machineguns;
int score;
int streak;
int level;
int clip;
const int width = 20;
const int height = 20;
const char nuke_symbol = 'n';
const char mg_symbol = 'm';
const char border = 'X';
const char space = ' ';
const char bonus = '$';
const char martian = 'M';
const char human = 'H';
char up[] = " LEVEL UP!! ";
char nukeplus[] = "NUKE +1!";
char mgplus[] = "MACHINEGUN +1!";
int bonus_X, X, Y;
int boom_X, boom_Y;
const int bonus_Y = 0;
int num_Ms;
const int max_Ms = 10000;
int M_X[max_Ms];
int M_Y[max_Ms];
int num_bullets;
const int def_bullets = 3;
const int max_bullets = 60;
int bullet_X[max_bullets];
int bullet_Y[max_bullets];
int SPEED = 60000; 
const int OB = -10000;
enum eDirection { STOP = 0, LEFT, RIGHT};
eDirection dir;
int cycles;
bool unique[width-1] = {0};
int start_time, end_time;
const int MG_ROF = 2;
int Mspeed = 9;
const int bonus_speed = 4;
int spawnrate = 130;
int scale = 8;
int wave_size = 3;
const int bonus_rate = 300;
int levels[16] = 
{30,60,100,200,350,500,750,1000,1500,2000,2500,3000,4000,5000,7500,10000};
const int hot_streak =5;
const int burst_length = 8;
int sustain;
const int sustain_ = 20;
int sustain_nuke;
int sustain_mg;
char mothership[154] ="\
_____       _____\
|MMM|__   __|MMM|\
|MMMMMM|_|MMMMMM|\
|MMM|MMo|oMM|MMM|\
|MMM||MMMMM||MMM|\
|MMM| |MMM| |MMM|\
|MMM|       |MMM|\
|MMM|       |MMM|\
|MMM|       |MMM|";       

// Functions
void Setup();
void Draw();
void Input();
void Logic();
int kbhit(void);
void Machinegun();
void Nuke();
void Burst();
void Bullet();
void Invade();
void Boss_Battle();

int main(void)
{
    Setup();
    while (!gameOver)
    {
        Draw();
        Input();
        Logic();
    }
    return 0;
}

void Setup()
{
    srand(time(NULL));
	gameOver = false;
	firing = false;
    firingNuke = false;
    firingMG = false;
	firingBurst = false;
	Bonus = false;
    levelUp = false;
    nukeAdd = false;
    mgAdd = false;
    Boss = false;
    nukes = 1;
    machineguns = 1;
	score = streak = level = 0;
	num_Ms = num_bullets = 0;
    bonus_X = OB;
	X = width / 2;
	Y = height;
    boom_X=OB;
    boom_Y=OB;
	cycles = sustain = sustain_nuke = sustain_mg = 0;
	dir = STOP;
	for (int i = 0; i < max_Ms; i++)
	{
		M_X[i] = OB;
		M_Y[i] = OB;
	}
	for (int i = 0; i < max_bullets; i++)
	{
		bullet_X[i] = OB;
		bullet_Y[i] = OB;
	}
	start_time = time(0);
}

void Draw()
{
    system("clear");
    // Header
    cout << "  ";
    for(int i = 0; i < nukes; i++)
        cout << nuke_symbol;
    cout << "\tH VERSUS M   ";
    for(int i = 0; i < machineguns; i++)
        cout << mg_symbol;
    cout << " ";
    if(firingMG)
        cout << clip;
    cout << endl;
    
    cout << "  jl=LR space=FIRE nm=$ ";
    cout << endl;
    
    cout << "  SCORE  "; 
    if(firingBurst) 
        cout << " BURST    ";
    else 
        cout << " STREAK   ";
    cout <<   "LEVEL" << endl;
    
    cout << "  " << score;
    if(firingBurst)
       cout << "\t  MODE";
    else
       cout << "\t  " << streak;
    cout << "\t   " << level + 1 << endl;    
	
	// Grid
    cout << "  "; 
    for (int i = 0; i <= width + 1; i++)
    {   
        if(levelUp & i > 4 && i < 17)
            cout << up[i-5];
        else  
            cout << border;
    }
    cout << endl;			
    int index = 0;
    for (int i = 0; i <= height; i++)
    {   
        cout << "  "; 
        for (int j = 0; j <= width; j++)
        {
            bool print = false;
            bool doubletap = false;
            if(nukeAdd && i == 0 && j > 5 && j < 13)
            {
                cout << nukeplus[j-6];
                print = true;
            }              
            if(mgAdd && i == 0 && j > 2 && j < 17)
            {
                cout << mgplus[j-3];
                print = true;              
            } 
            
            if (Boss && i > 0 && i < 10 && j > 1 && j < 19)
            {
                cout << mothership[index];
                index++;
                print = true;
            }

            if (j == 0 || j == width)
            {   
            	cout << border;
            }
            if ( j == bonus_X && i == bonus_Y)
            {   
                cout << bonus;
                print = true;
            }
            if ( j == boom_X && i == boom_Y)
            {   
                cout << "@";
                print = true;
            }
            bool duplicate = false;
            for(int k = 0; k < num_Ms; k++)
            {
                if(i == M_Y[k] && j == M_X[k])
                {      
                    if(!duplicate)
                    {
                        cout << martian;
                        duplicate = true;
                        print = true;
                    }
                }
            } 
            for (int m = 0; m < max_bullets; m++)        
            {
                if((nukeAdd || mgAdd) && i > 0)
                {
                    if (i == bullet_Y[m] && j == bullet_X[m])
                    {     
                        if(!doubletap)
                        {      
                            cout << ".";
                            doubletap = true;
                        }
                        print = true; 
                    }
                }
                if (!nukeAdd && !mgAdd)
                {
                    if (i == bullet_Y[m] && j == bullet_X[m])
                    {     
                        if(!doubletap)
                        {      
                            cout << ".";
                            doubletap = true;
                        }
                        print = true; 
                    }
                }
            }
            if (j == X && i == Y)
            {    
                cout << human;
                print = true;
            }
            if (!print)
                cout << " ";
        }
    cout << endl;
    }        
    cout << "  "; 
    for (int i = 0; i <= width + 1; i++)
    {
        cout << border;
    }
    cout << endl;
    usleep(SPEED);
}

void Input()
{
    if (kbhit())
    {
        switch (getchar())
        {
            case 'w': 
                firing = true;
                break;
            case ' ': 
                firing = true;
                break;
            case 'a': 
                dir = LEFT;
                break;
            case 'j': 
                dir = LEFT;
                break;
            case 'q': 
                dir = LEFT;
                break;
            case 'd': 
                dir = RIGHT;
                break;
            case 'l': 
                dir = RIGHT;
                break;
            case 'p': 
                dir = RIGHT;
                break;
        	case 'm':
                clip=50;
                firingMG=true; 
                break;
        	case 'n':
                firingNuke = true;
                break;
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////
void Logic()
{
    switch (dir)
    {
        case LEFT:
            X--;
            break;
        case RIGHT:
            X++;
            break;
    }
    dir = STOP;	
    boom_X=OB;
    boom_Y=OB;
	// Keep H in bounds
    if ( X >= width)
        X = width - 1;
    if (X <= 0)
        X = 0;
    //bullet motion
    for(int i = 0; i < max_bullets; i++)
        bullet_Y[i]--;
    //enemy motion
    if(num_Ms > 0 && cycles % Mspeed == 0)
    {
        for (int i = 0; i < max_Ms; i++)
        {
            M_Y[i]++;
        }
    }
    //bonus motion
    if(cycles % bonus_speed == 0)
    {
            bonus_X++;
    }
    //spawns bonus
    if(cycles % bonus_rate == 0)
    {
        bonus_X = 1;
        Bonus = true;
    }
    //bonus out of bounds
    if(bonus_X >= width)
    {
        bonus_X=OB;
        Bonus = false;
    }
    //resets streak when bullet misses
    for(int i = 0; i < max_bullets; i++)
    {
        if(bullet_Y[i] == 0)
        {
            streak=0;
        }
    }
    //enables burst mode 
    if(streak >= hot_streak)
    {
        streak = 0;
        firingBurst = true;
        end_time = time(0) + burst_length;
    }
    //disables burst mode after b_length
    if (time(0) == end_time)
        firingBurst = false;
    //level up(enemy speeds up, wave size inc, 
    if(score >= levels[level])
    {
        level++;
        spawnrate -= scale;
        if((level+1)%8==0)
            Mspeed -=1;
        levelUp = true;
        if ( (level+1)%3 == 0)
            wave_size++;
    }
/*
    if (level >= 7)
    { Boss = true; }
*/
    if(firing)
        Bullet();
    if(firingNuke && nukes > 0)
        Nuke();
    else
        firingNuke = false;
    if(firingMG && machineguns > 0)
        Machinegun();
    else
        firingMG = false;
    
    if (cycles % spawnrate == 0 && !Boss)
    {
    	for(int i = 0; i < wave_size; i++)
        	Invade();
    }
/*
    if (Boss)
    {
        bool ongrid = false;
        for (int i = 0; i < num_Ms; i++)
        {
            if (M_Y[i] >= 0)
            {
                ongrid = true;
            }
            if (!ongrid)
            {
                Boss_Battle();
            }
        }
    }
*/
    for(int i = 0; i < num_Ms; i++)
        if(M_Y[i] >= height)
        {    
            gameOver = true;
            system ("clear");
            cout << "score: " << score << "\t level: " << level + 1 << endl;
            cout << "Invader #" << i+1 << " Touched down on Earth\n";
            cout << "(" << M_X[i] << "," << M_Y[i] << ")" << endl;
            cout << "Total time: " << time(0) - start_time << " seconds." << endl;
            cout << "Cycles: " << cycles << endl;
        }
    for (int i = 0; i < max_bullets; i++)
    {
        if(bullet_Y[i] == bonus_Y && bullet_X[i] == bonus_X)
        {
            score +=100;
            bullet_Y[i] = OB;
            bonus_X = OB;
            cout << "\a";
            if(cycles%2==0)
            {
                nukes++;
                nukeAdd = true;        
            }
            else
            {    
                machineguns++;
                mgAdd = true;
            }
            if(nukes > 3)
                nukes = 3;
            if(machineguns > 3)
                machineguns = 3;
            Bonus = false;
        }
    }
    for(int i = 0; i < num_Ms; i++)
    {
        for (int j = 0; j < max_bullets; j++)
            if (bullet_Y[j] == M_Y[i] && bullet_X[j] == M_X[i])
            {
                score += 10; 
                boom_X=M_X[i];
                boom_Y=M_Y[i];

                M_Y[i] = OB;
                bullet_Y[j] = OB;
                cout << "\a";
                streak++;
            }
    }
    if (levelUp)
    {
        sustain++;
        if(sustain == sustain_)
        {
            levelUp = false;
            sustain = 0;
        }
    }
    if (nukeAdd)
    {
        sustain_nuke++;
        if(sustain_nuke == sustain_)
        {
            nukeAdd = false;
            sustain_nuke = 0;
        }
    }
    if (mgAdd)
    {
        sustain_mg++;
        if(sustain_mg == sustain_)
        {
            mgAdd = false;
            sustain_mg = 0;
        }
    }
    cycles++;
}
/////////////////////////////////////////////////////////////////////////////////////
void Machinegun()
{
	if(cycles % MG_ROF == 0)
	{
		bullet_X[num_bullets % max_bullets] = X;
		bullet_Y[num_bullets % max_bullets] = Y-1;
		num_bullets++;
		clip--;
	}
	if(clip <= 0)
	{
		firingMG = false;
		machineguns--;
	}
}
void Nuke()
{
	for (int i = 0; i < width; i++)
    {
		bullet_X[num_bullets % max_bullets] = i;
		bullet_Y[num_bullets % max_bullets] = Y-1;
		num_bullets++;
		bullet_X[num_bullets % max_bullets] = i;
		bullet_Y[num_bullets % max_bullets] = Y-2;
		num_bullets++;
	}	
	nukes--;
    firingNuke = false;
}
void Burst()
{   
    //Middle bullet    
    bullet_X[num_bullets % max_bullets] = X;
    bullet_Y[num_bullets % max_bullets] = Y-2;
    num_bullets++;
    bullet_X[num_bullets % max_bullets] = X;
    bullet_Y[num_bullets % max_bullets] = Y-1;
    num_bullets++;
   
    //Right bullet
    if((X+1) >= width);
    else 
    {
        bullet_X[num_bullets % max_bullets] = X+1;
		bullet_Y[num_bullets % max_bullets] = Y-1;
      	num_bullets++;
    }
    //Left bullet
    bullet_X[num_bullets % max_bullets] = X-1;
    bullet_Y[num_bullets % max_bullets] = Y-1;
    num_bullets++;
}
void Bullet()
{       
    if(firingBurst)
        Burst();
    else
    {
        bullet_X[num_bullets % def_bullets] = X;
        bullet_Y[num_bullets % def_bullets] = Y-1;
        num_bullets++;
    }
    firing = false;
}
void Invade()
{   
    int xvalue;
    do{
        xvalue = rand() % (width);
    }while(unique[xvalue] == true);
    
    unique[xvalue] = true;     
    M_X[num_Ms % max_Ms] = xvalue;
    M_Y[num_Ms % max_Ms] = (rand() % 2) +1;
    num_Ms++;
    if (num_Ms % (width-1) == 0)
        for (int i = 0; i < width-1; i++)
        {
            unique[i] = false;
        }
}

void Boss_Battle()
{


/*    
   87654321012345678       
 -2 ___         ___       
 -1|20M\__   __/21M|       
  0|16M18M\_/19M17M|    Underscore is boss center           
  1|12M\14o|o15/13M|      
  2|M9M|\M11MM/|10M|      
  3|M6M| \M8M/ |M7M|      
  4|M4M|       |M5M|      
  5|M2M|       |M3M|      
  6|M0M|       |M1M|      
   ___         ___
  |012\__   __/345|  
  |890456\_/789123|
  |678\23o|o67/901|
  |789|\43455/|012|
  |890| \456/ |123| 
  |234|       |567| 
  |678|       |901| 
  |012|       |345| 
 ___         ___
|MMM\__   __/MMM|  
|MMMMMM\_/MMMMMM|
|MMM\MMo|oMM/MMM|
|MMM|\MMMMM/|MMM|
|MMM| \MMM/ |MMM|
|MMM|       |MMM|
|MMM|       |MMM|
|MMM|       |MMM|       
*/
}


int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}
