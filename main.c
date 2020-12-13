#include "common.h"

char end=0;
int game_round=0;
int clients=0;

struct dropped_t
{
    unsigned int x;
    unsigned int y;
    unsigned int value;
};
struct dropped_t dropped[200]={0};

struct players_t
{
    unsigned int id;
    unsigned int pid;
    unsigned int y;
    unsigned int x;
    unsigned int camp_y;
    unsigned int camp_x;
    unsigned int coins;
    unsigned int brought;
    int slowed;
    unsigned int deaths;
    int game_round;
    char type[6];
};
struct players_t players[4]=
    {
        {0,0,1,1,0,0,0,0,0,0,0,"-----"},
        {1,0,1,1,0,0,0,0,0,0,0,"-----"},
        {2,0,1,1,0,0,0,0,0,0,0,"-----"},
        {3,0,1,1,0,0,0,0,0,0,0,"-----"}
    };

char map[][54]=
{
    {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"},
    {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"},
    {"xxc  x       x   #####       x         xcccccccx   xx"},
    {"xx x xxx xxx xxxxxxxxxxx xxx x xxxxxxx xxx xxxxx t xx"},
    {"xx x   x x x           x x x   x     x     x       xx"},
    {"xx xxx x x xxx###xxxxx x x xxxxx xxxxxxxxxxxxx xxx xx"},
    {"xx xTx   x           x x x ##  x      cx       x x xx"},
    {"xx x xxxxx xxx xxxxxxx x x x xxx xxx xxx xxx x x x xx"},
    {"xx x         x x       x x x     x   x   x x x   x xx"},
    {"xx x xxxxxxx xxx xxxxxxx xxxxx xxx xxx xxx x xxx x xx"},
    {"xx xTx     x   x   x     x   x   x         x x x x xx"},
    {"xx xxx xxx xxx xxx xxx xxx x xxx xxxxxxxxxxx x x x xx"},
    {"xx x   x       x x  cx  A  x x   x x       x x   x xx"},
    {"xx x xxxxxx#xx x xxx xxx xxx xxx x x xxxxx x x xxx xx"},
    {"xx x    #x   x x   x   x   x   x   x x     x x x   xx"},
    {"xx x x ##x xxx xxx xxx xxxxxxx xxx x xxx xxx x x xxxx"},
    {"xx x x## x    #  x   x x  ###  x   x   x     x x x xx"},
    {"xx x x#  xxxxxxx x x x x xx#xxxx xxxxx xxxxxxx x x xx"},
    {"xx x x#      x   x x x   x     x   x x       ##x   xx"},
    {"xx x xxxxxxx x xxx xxxxxxx xxxxxxx x xxxxx x ##xxx xx"},
    {"xx x#      x x     x     x       x   x   x x ##  x xx"},
    {"xx x xxxxx x xxxxxxx x xxx xxxxx xxx x x xxx#xxxxx xx"},
    {"xx###x     x         x  ###x## x     x x   x###### xx"},
    {"xxcxxx xxxxxxxxxxxxxxxxxxxxx#x xxxxxxx xxx x#    # xx"},
    {"xxcccx                 ######x##         x    ##   xx"},
    {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"},
    {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"}
};
    
char map_orginal[][54]=
{
    {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"},
    {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"},
    {"xxc  x       x   #####       x         xcccccccx   xx"},
    {"xx x xxx xxx xxxxxxxxxxx xxx x xxxxxxx xxx xxxxx t xx"},
    {"xx x   x x x           x x x   x     x     x       xx"},
    {"xx xxx x x xxx###xxxxx x x xxxxx xxxxxxxxxxxxx xxx xx"},
    {"xx xTx   x           x x x ##  x      cx       x x xx"},
    {"xx x xxxxx xxx xxxxxxx x x x xxx xxx xxx xxx x x x xx"},
    {"xx x         x x       x x x     x   x   x x x   x xx"},
    {"xx x xxxxxxx xxx xxxxxxx xxxxx xxx xxx xxx x xxx x xx"},
    {"xx xTx     x   x   x     x   x   x         x x x x xx"},
    {"xx xxx xxx xxx xxx xxx xxx x xxx xxxxxxxxxxx x x x xx"},
    {"xx x   x       x x  cx  A  x x   x x       x x   x xx"},
    {"xx x xxxxxx#xx x xxx xxx xxx xxx x x xxxxx x x xxx xx"},
    {"xx x    #x   x x   x   x   x   x   x x     x x x   xx"},
    {"xx x x ##x xxx xxx xxx xxxxxxx xxx x xxx xxx x x xxxx"},
    {"xx x x## x    #  x   x x  ###  x   x   x     x x x xx"},
    {"xx x x#  xxxxxxx x x x x xx#xxxx xxxxx xxxxxxx x x xx"},
    {"xx x x#      x   x x x   x     x   x x       ##x   xx"},
    {"xx x xxxxxxx x xxx xxxxxxx xxxxxxx x xxxxx x ##xxx xx"},
    {"xx x#      x x     x     x       x   x   x x ##  x xx"},
    {"xx x xxxxx x xxxxxxx x xxx xxxxx xxx x x xxx#xxxxx xx"},
    {"xx###x     x         x  ###x## x     x x   x###### xx"},
    {"xxcxxx xxxxxxxxxxxxxxxxxxxxx#x xxxxxxx xxx x#    # xx"},
    {"xxcccx                 ######x##         x    ##   xx"},
    {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"},
    {"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"}
};
    
void* round_counter(void* arg)
{
    int i=0;
    char name[15];
    
    pthread_mutex_t mutex =PTHREAD_MUTEX_INITIALIZER;
    
    while(end!='q' && end!='Q')
    {
        pthread_mutex_lock(&mutex);
    
        //SPRAWDZENIE CZY KTORYS KLIENT PRZESTAL ODPOWIADAC
        for(i=0;i<clients;++i)
        {
           if(abs(game_round-players[i].game_round)>8 && players[i].pid!=0)
           {
                snprintf(name, 15, "/game_signal2%d",players[i].id);
                sem_t* sem2 = sem_open(name, O_CREAT, 0600, 1);
                err(sem2 == SEM_FAILED, "sem_open");
                
                players[i].game_round=-1;
                sem_post(sem2);  
                
                sem_close(sem2);
           }
        }
        game_round+=1;
    
        pthread_mutex_unlock(&mutex);
        usleep(800000);
    }
        
    return NULL;
}
   
void* beast(void *arg)
{
    int i=0,j=0,x=0,y=0,move=0,good_move=0,x0=0,y0=0,p=0,slowed=0;
    
    srand(time(NULL));
    pthread_mutex_t mutex =PTHREAD_MUTEX_INITIALIZER;
    
    do
    {
        x=2+rand()%47;
        y=2+rand()%21;
    }
    while(map[y][x]!=' ');    
    map[y][x]='*';
    

    while(end!='q' && end!='Q')
    {
        pthread_mutex_lock(&mutex);
        
        if(slowed)
            slowed=0;
        else
        {
            map[y][x]=map_orginal[y][x];
            
            //SZUKANIE GRACZY
            x0=x;
            y0=y;
            //RUCH D
            if(map[y][x+1]!='x' && x0==x && y0==y)
            {
                if( (map[y][x+1]>'0' && map[y][x+1]<'5') || (map[y-1][x+1]>'0' && map[y-1][x+1]<'5') || (map[y+1][x+1]>'0' && map[y+1][x+1]<'5') || (map[y][x+2]>'0' && map[y][x+2]<'5') )
                    x++;
                else if(map[y-1][x+1]!='x')
                {
                    if(map[y-1][x+2]>'0' && map[y-1][x+2]<'5')
                        x++;
                    else if(map[y-1][x+2]!='x' && map[y-2][x+2]>'0' && map[y-2][x+2]<'5')
                        x++;
                }
                else if(map[y+1][x+1]!='x')
                {
                    if(map[y+1][x+2]>'0' && map[y+1][x+2]<'5')
                        x++;
                    else if(map[y+1][x+2]!='x' && map[y+2][x+2]>'0' && map[y+2][x+2]<'5')
                        x++;
                }
            }
            //RUCH S
            if(map[y+1][x]!='x' && x0==x && y0==y)
            {
                if( (map[y+1][x]>'0' && map[y+1][x]<'5') || (map[y+1][x+1]>'0' && map[y+1][x+1]<'5') || (map[y+1][x-1]>'0' && map[y+1][x-1]<'5') || (map[y+2][x]>'0' && map[y+2][x]<'5') )
                    y++;
                else if(map[y+1][x+1]!='x')
                {
                    if(map[y+2][x+1]>'0' && map[y+2][x+1]<'5')
                        y++;
                    else if(map[y+2][x+1]!='x' && map[y+2][x+2]>'0' && map[y+2][x+2]<'5')
                        y++;
                }  
                else if(map[y+1][x-1]!='x')
                {
                    if(map[y+2][x-1]>'0' && map[y+2][x-1]<'5')
                        y++;
                    else if(map[y+2][x-1]!='x' && map[y+2][x-2]>'0' && map[y+2][x-2]<'5')
                        y++;
                }
            }
            //RUCH A
            if(map[y][x-1]!='x' && x0==x && y0==y)
            {
                if( (map[y][x-1]>'0' && map[y][x-1]<'5') || (map[y-1][x-1]>'0' && map[y-1][x-1]<'5') || (map[y+1][x-1]>'0' && map[y+1][x-1]<'5') || (map[y][x-2]>'0' && map[y][x-2]<'5') )
                    x--;
                else if(map[y-1][x-1]!='x')
                {
                    if(map[y-1][x-2]>'0' && map[y-1][x-2]<'5')
                        x--;
                    else if(map[y-1][x-2]!='x' && map[y-2][x-2]>'0' && map[y-2][x-2]<'5')
                        x--;
                }
                else if(map[y+1][x-1]!='x')
                {
                    if(map[y+1][x-2]>'0' && map[y+1][x-2]<'5')
                        x--;
                    else if(map[y+1][x-2]!='x' && map[y+2][x-2]>'0' && map[y+2][x-2]<'5')
                        x--;
                }
            }
            //RUCH W
            if(map[y-1][x]!='x' && x0==x && y0==y)
            {
                if( (map[y-1][x]>'0' && map[y-1][x]<'5') || (map[y-1][x+1]>'0' && map[y-1][x+1]<'5') || (map[y-1][x-1]>'0' && map[y-1][x-1]<'5') || (map[y-2][x]>'0' && map[y-2][x]<'5') )
                    y--;
                else if(map[y-1][x+1]!='x')
                {
                    if(map[y-2][x+1]>'0' && map[y-2][x+1]<'5')
                        y--;
                    else if(map[y-2][x+1]!='x' && map[y-2][x+2]>'0' && map[y-2][x+2]<'5')
                        y--;
                }  
                else if(map[y-1][x-1]!='x')
                {
                    if(map[y-2][x-1]>'0' && map[y-2][x-1]<'5')
                        y--;
                    else if(map[y-2][x-1]!='x' && map[y-2][x-2]>'0' && map[y-2][x-2]<'5')
                        y--;
                }
            }
            
            
            if((x0==x && y0==y) || map_orginal[y][x]=='A' || map_orginal[y][x]=='x')
            {
                x=x0;
                y=y0;
                good_move=0;
                while(!good_move)
                {
                    move=rand()%4;
                    
                    switch(move)
                    {
                        case 0:
                            if(map[y-1][x]!='x' && map_orginal[y-1][x]!='A')
                            {
                                y--;
                                good_move=1;
                            }   
                            break;
                        case 1:
                            if(map[y+1][x]!='x' && map_orginal[y+1][x]!='A')
                            {
                                y++;
                                good_move=1;
                            }
                            break;
                        case 2:
                            if(map[y][x+1]!='x' && map_orginal[y][x+1]!='A')
                            {
                                x++;
                                good_move=1;
                            }
                            break;
                        case 3:
                            if(map[y][x-1]!='x' && map_orginal[y][x-1]!='A')
                            {
                                x--;
                                good_move=1;
                            }
                            break; 
                        default:
                            break;
                    }
                } 
            }
            
            if(map[y][x]>'0' && map[y][x]<'5')
            {
                p=map[y][x]-'0'-1;
                
                //BESTIA NIE MOZE ZABIC GRACZA W MIEJSCU JEGO SPAWNU
                if(players[p].camp_x==x && players[p].camp_y==y)
                {
                    x=x0;
                    y=y0;
                }
                else
                {
                    if(players[p].coins>0)
                    {
                        map[y][x]='D';
                        map_orginal[y][x]='D';
                    
                        for(i=0;dropped[i].x!=0;++i);
                        dropped[i].y=y;
                        dropped[i].x=x;
                        dropped[i].value=players[p].coins;
                    }
                    else
                        map[y][x]=map_orginal[y][x];
                    
                        
                    players[p].y=players[p].camp_y;
                    players[p].x=players[p].camp_x;
                    players[p].coins=0;
                    players[p].deaths++;    
                }
            }
            map[y][x]='*';
            
            if(map_orginal[y][x]=='#')
                slowed=1;
        }
        
        for(i=1;i<26;++i)
        {
            for(j=1;j<52;++j)
            {
                if(map[i][j]=='x')
                {
                    attron(COLOR_PAIR(1));
                    mvprintw(i,j,"%c",map[i][j]);
                    attroff(COLOR_PAIR(1));
                }
                else if(map[i][j]=='t' || map[i][j]=='T' || map[i][j]=='c' || map[i][j]=='D')
                {
                    attron(COLOR_PAIR(2));
                    mvprintw(i,j,"%c",map[i][j]);
                    attroff(COLOR_PAIR(2));
                }
                else if(map[i][j]>'0' && map[i][j]<'5')
                {
                    attron(COLOR_PAIR(5));
                    mvprintw(i,j,"%c",map[i][j]);
                    attroff(COLOR_PAIR(5));
                }
                else if(map[i][j]=='A')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(i,j,"%c",map[i][j]);
                    attroff(COLOR_PAIR(7));
                }
                else if(map[i][j]=='*')
                {
                    attron(COLOR_PAIR(6));
                    mvprintw(i,j,"%c",map[i][j]);
                    attroff(COLOR_PAIR(6));
                }
                else
                {
                    attron(COLOR_PAIR(4));
                    mvprintw(i,j,"%c",map[i][j]);
                    attroff(COLOR_PAIR(4));
                }    
            }
            mvprintw(j,i,"\n");
        }      
        refresh();
        
        pthread_mutex_unlock(&mutex);
        usleep(800000);
    }
        
    return NULL;
} 

void* control(void* arg)
{
    int x=0,y=0,i=0,j=0,beasts=0;
    pthread_t b[50];
    
    initscr();
    noecho();
    start_color();

    init_pair(1,COLOR_BLACK,COLOR_BLACK);
    init_pair(2,COLOR_BLACK,COLOR_YELLOW);  //treasures
    init_pair(3,COLOR_WHITE,COLOR_WHITE);   //background
    init_pair(4,COLOR_BLACK,COLOR_WHITE);   //default
    init_pair(5,COLOR_WHITE,COLOR_MAGENTA); //players
    init_pair(6,COLOR_RED,COLOR_WHITE);     //beast
    init_pair(7,COLOR_YELLOW,COLOR_GREEN);     //camp
    
    pthread_mutex_t mutex =PTHREAD_MUTEX_INITIALIZER;
    
    
    while(end!='q' && end!='Q')
    {
        pthread_mutex_lock(&mutex);
        
        end=getch();
        
        switch(end)
        {
            case 'b':
            case 'B':
                if(beasts<50)
                    pthread_create(&b[beasts],NULL,beast,NULL);
                break;
            case 'c':
            case 't':
            case 'T':
                do
                {
                    x=2+rand()%48;
                    y=2+rand()%22;
                }
                while(map[y][x]!=' ');
                
                map_orginal[y][x]=end;
                map[y][x]=end;
                break;
            default:
            break;
        }
        
        for(i=1;i<26;++i)
        {
            for(j=1;j<52;++j)
            {
                if(map[i][j]=='x')
                {
                    attron(COLOR_PAIR(1));
                    mvprintw(i,j,"%c",map[i][j]);
                    attroff(COLOR_PAIR(1));
                }
                else if(map[i][j]=='t' || map[i][j]=='T' || map[i][j]=='c' || map[i][j]=='D')
                {
                    attron(COLOR_PAIR(2));
                    mvprintw(i,j,"%c",map[i][j]);
                    attroff(COLOR_PAIR(2));
                }
                else if(map[i][j]>'0' && map[i][j]<'5')
                {
                    attron(COLOR_PAIR(5));
                    mvprintw(i,j,"%c",map[i][j]);
                    attroff(COLOR_PAIR(5));
                }
                else if(map[i][j]=='A')
                {
                    attron(COLOR_PAIR(7));
                    mvprintw(i,j,"%c",map[i][j]);
                    attroff(COLOR_PAIR(7));
                }
                else if(map[i][j]=='*')
                {
                    attron(COLOR_PAIR(6));
                    mvprintw(i,j,"%c",map[i][j]);
                    attroff(COLOR_PAIR(6));
                }
                else
                {
                    attron(COLOR_PAIR(4));
                    mvprintw(i,j,"%c",map[i][j]);
                    attroff(COLOR_PAIR(4));
                }    
            }
            mvprintw(j,i,"\n");
        }      
        refresh();
        
        pthread_mutex_unlock(&mutex);
    }
    
    for(i=0;i<beasts;++i)
        pthread_join(b[i],NULL);
    
    sem_t* sem_connect1 = sem_open("/connection_signal1", O_CREAT, 0600, 0);
    err(sem_connect1 == SEM_FAILED, "sem_open");
    
    sem_post(sem_connect1);
    
    sem_close(sem_connect1);
    endwin();
    
    return NULL;  
}

void* game(void* arg)
{
    int i=0,j=0,ii=0,jj=0,br=0,p2=0;
    char name[15],values[4][6];
    
    struct players_t *player=(struct players_t*)arg;
    pthread_mutex_t mutex =PTHREAD_MUTEX_INITIALIZER;
    
    initscr();
    start_color();

    init_pair(1,COLOR_BLACK,COLOR_BLACK);
    init_pair(2,COLOR_BLACK,COLOR_YELLOW);  //treasures
    init_pair(3,COLOR_WHITE,COLOR_WHITE);   //background
    init_pair(4,COLOR_BLACK,COLOR_WHITE);   //default
    init_pair(5,COLOR_WHITE,COLOR_MAGENTA); //players
    init_pair(6,COLOR_RED,COLOR_WHITE);     //beast
    init_pair(7,COLOR_YELLOW,COLOR_GREEN);     //camp
    
    wbkgd(stdscr,COLOR_PAIR(3));
    
    
    snprintf(name, 15, "/game_signal%d",player->id);
    sem_t* sem = sem_open(name, O_CREAT, 0600, 0);
    err(sem == SEM_FAILED, "sem_open");
    
    snprintf(name, 15, "/game_signal2%d",player->id);
    sem_t* sem2 = sem_open(name, O_CREAT, 0600, 1);
    err(sem2 == SEM_FAILED, "sem_open");

    snprintf(name, 15, "/game_data%d",player->id);
    int fd = shm_open(name, O_CREAT | O_RDWR, 0600);
    err(fd == -1, "shm_open");
        
    ftruncate(fd, sizeof(struct data_t));
    struct data_t* data = (struct data_t*)mmap(NULL, sizeof(struct data_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    err(data == NULL, "mmap");
    
    sem_init(&data->cs, 1, 1); 
    
    data->camp_x=-1;
    data->camp_y=-1;
    data->id=player->id+1;

    while(end!='q' && end!='Q' && !br && player->game_round!=-1)
    {
        sem_wait(sem2);
        
        pthread_mutex_lock(&mutex);
        
        if(player->game_round!=-1)
        {
            sem_wait(&data->cs);
        
            if(data->direction!='Q' && data->direction!='q' && end!='q' && end!='Q')
            {      
                if(data->deaths==player->deaths)
                    map[player->y][player->x]=map_orginal[player->y][player->x]; 
                
               
                //RUCH
                switch(data->direction)
                {
                    case 'w':
                        if(map[player->y-1][player->x]!='x')
                        {
                            if(player->slowed)
                                player->slowed=-1;
                            else
                                player->y--;
                        }
                        break;
                    case 's':
                        if(map[player->y+1][player->x]!='x')
                        {
                            if(player->slowed)
                                player->slowed=-1;
                            else
                                player->y++;
                        }
                        break;
                    case 'd':
                        if(map[player->y][player->x+1]!='x')
                        {
                            if(player->slowed)
                                player->slowed=-1;
                            else
                                player->x++;
                        }    
                        break;
                    case 'a':
                        if(map[player->y][player->x-1]!='x')
                        {
                            if(player->slowed)
                                player->slowed=-1;
                            else
                                player->x--;
                        }    
                        break;    
                    default:
                        break;
                }
                
                //WEJSCIE NA BESTIE
                if(map[player->y][player->x]=='*')
                {
                    if(player->coins>0)
                    {
                        map[player->y][player->x]='D';
                        map_orginal[player->y][player->x]='D';
                        
                        for(i=0;dropped[i].x!=0;++i);
                        dropped[i].y=player->y;
                        dropped[i].x=player->x;
                        dropped[i].value=player->coins;
                    }
                    
                    player->y=player->camp_y;
                    player->x=player->camp_x;
                    player->coins=0;
                    player->deaths++;
                }
                
                //WEJSCIE NA INNEGO GRACZA
                if(map[player->y][player->x]>'0' && map[player->y][player->x]<'5' && map_orginal[player->y][player->x]!='A')
                {
                    p2=map[player->y][player->x]-'0'-1;
                    
                    if(player->coins+players[p2].coins>0)
                    {
                        map[player->y][player->x]='D';
                        map_orginal[player->y][player->x]='D';
                    
                        for(i=0;dropped[i].x!=0;++i);
                        dropped[i].y=player->y;
                        dropped[i].x=player->x;
                        dropped[i].value=player->coins+players[p2].coins;
                    }
                    else
                        map[player->y][player->x]=map_orginal[player->y][player->x];
                    
                    player->y=player->camp_y;
                    player->x=player->camp_x;
                    player->coins=0;
                    player->deaths++;
                    
                    players[p2].y=players[p2].camp_y;
                    players[p2].x=player[p2].camp_x;
                    players[p2].coins=0;
                    players[p2].deaths++;    
                }
                else
                    map[player->y][player->x]=(player->id+1)+'0';
                    
                //REAKCAJ NA OBIEKTY    
                switch(map_orginal[player->y][player->x])
                {
                    case 'c':
                        player->coins+=1;
                        map_orginal[player->y][player->x]=' ';
                        break;
                    case 't':
                        player->coins+=10;
                        map_orginal[player->y][player->x]=' ';
                        break;
                    case 'T':
                        player->coins+=50;
                        map_orginal[player->y][player->x]=' ';
                        break;
                    case 'D':
                        for(i=0;dropped[i].y!=player->y && dropped[i].x!=player->x;++i);
                        player->coins+=dropped[i].value;
                        dropped[i].x=0;
                        map_orginal[player->y][player->x]=' ';
                        break;
                    case '#':
                        player->slowed++;
                        break;
                    case 'A':
                        player->brought+=player->coins;
                        player->coins=0;
                        break;
                    default:
                        break;
                }
                
                
                //PRZEKAZYWANIE MAPY
                for(i=player->y-2,ii=0; ii<5 ;++i,++ii)
                {
                    for(j=player->x-2,jj=0; jj<5 ;++j,++jj)
                    {
                        data->map[ii][jj]=map[i][j];
                        if(map[i][j]=='A')
                        {
                            data->camp_y=i-1;
                            data->camp_x=j-1;
                        }
                    }
                }
                
                data->x=player->x-1;
                data->y=player->y-1;
                data->coins=player->coins;
                data->brought=player->brought;
                data->deaths=player->deaths;
                data->game_round=game_round;
                player->game_round=game_round;
            
                    
                //WYSWIETLENIE MAPY PO ZMIENIANIE
                for(i=1;i<26;++i)
                {
                    for(j=1;j<52;++j)
                    {
                        if(map[i][j]=='x')
                        {
                            attron(COLOR_PAIR(1));
                            mvprintw(i,j,"%c",map[i][j]);
                            attroff(COLOR_PAIR(1));
                        }
                        else if(map[i][j]=='t' || map[i][j]=='T' || map[i][j]=='c' || map[i][j]=='D')
                        {
                            attron(COLOR_PAIR(2));
                            mvprintw(i,j,"%c",map[i][j]);
                            attroff(COLOR_PAIR(2));
                        }
                        else if(map[i][j]>'0' && map[i][j]<'5')
                        {
                            attron(COLOR_PAIR(5));
                            mvprintw(i,j,"%c",map[i][j]);
                            attroff(COLOR_PAIR(5));
                        }
                        else if(map[i][j]=='A')
                        {
                            attron(COLOR_PAIR(7));
                            mvprintw(i,j,"%c",map[i][j]);
                            attroff(COLOR_PAIR(7));
                        }
                        else if(map[i][j]=='*')
                        {
                            attron(COLOR_PAIR(6));
                            mvprintw(i,j,"%c",map[i][j]);
                            attroff(COLOR_PAIR(6));
                        }
                        else
                        {
                            attron(COLOR_PAIR(4));
                            mvprintw(i,j,"%c",map[i][j]);
                            attroff(COLOR_PAIR(4));
                        }    
                    }
                    mvprintw(j,i,"\n");
                }
                    
                attron(COLOR_PAIR(4));
            
                mvprintw(1,56,"Server's PID: %d",getpid());
                mvprintw(2,57,"Campsite X/Y: 23/11");
                mvprintw(3,57,"Round number: %d",game_round);
                
                mvprintw(5,56,"Parameters:   Player1   Player2   Player3   Player4");
                mvprintw(6,57,"PID          %04u      %04u      %04u      %04u",players[0].pid,players[1].pid,players[2].pid,players[3].pid);
                mvprintw(7,57,"Type         %s     %s     %s     %s",players[0].type,players[1].type,players[2].type,players[3].type);
                
                mvprintw(8,57,"Curr X/Y     %02u/%02u     %02u/%02u     %02u/%02u     %02u/%02u",players[0].x-1,players[0].y-1,players[1].x-1,players[1].y-1,players[2].x-1,players[2].y-1,players[3].x-1,players[3].y-1);
                mvprintw(9,57,"Deaths       %02u        %02u        %02u        %02u",players[0].deaths,players[1].deaths,players[2].deaths,players[3].deaths);
                
                mvprintw(11,56,"Coins");
                mvprintw(12,57,"carried      %05u     %05u     %05u     %05u",players[0].coins,players[1].coins,players[2].coins,players[3].coins);
                mvprintw(13,57,"brought      %05u     %05u     %05u     %05u",players[0].brought,players[1].brought,players[2].brought,players[3].brought);
        
                mvprintw(16,56,"Legend:");
                
                attron(COLOR_PAIR(5));
                mvprintw(17,57,"1234");
                attroff(COLOR_PAIR(5));
                attron(COLOR_PAIR(4));
                mvprintw(17,62,"- players");
                attroff(COLOR_PAIR(4));
                
                attron(COLOR_PAIR(1));
                mvprintw(18,57,"x");
                attroff(COLOR_PAIR(1));
                attron(COLOR_PAIR(4));
                mvprintw(18,62,"- wall");

                mvprintw(19,57,"#");
                mvprintw(19,62,"- bushes (slow down)");

                attron(COLOR_PAIR(6));
                mvprintw(20,57,"*");
                attroff(COLOR_PAIR(6));
                attron(COLOR_PAIR(4));
                mvprintw(20,62,"- wild beast");
                
                attron(COLOR_PAIR(2));
                mvprintw(21,57,"c");
                mvprintw(21,87,"D");
                attroff(COLOR_PAIR(2));
                attron(COLOR_PAIR(4));
                mvprintw(21,62,"- one coin");
                mvprintw(21,90,"- dropped treasure");
                
                attron(COLOR_PAIR(2));
                mvprintw(22,57,"t");
                attroff(COLOR_PAIR(2));
                attron(COLOR_PAIR(4));
                mvprintw(22,62,"- treasure (10 coins)");
                
                attron(COLOR_PAIR(2));
                mvprintw(23,57,"T");
                attroff(COLOR_PAIR(2));
                attron(COLOR_PAIR(4));
                mvprintw(23,62,"- large treasure (50 coins)");
                
                attron(COLOR_PAIR(7));
                mvprintw(24,57,"A");
                attroff(COLOR_PAIR(7));
                attron(COLOR_PAIR(4));
                mvprintw(24,62,"- campsite");

                refresh();
            }
            else if(end!='q' && end!='Q')
                br=1;
            else if(end=='q' || end=='Q')
                data->direction='q';
                
            sem_post(&data->cs); 
        }
        else
            player->pid=0;
            
        pthread_mutex_unlock(&mutex);
           
        sem_post(sem);    
    }
    
    map[player->y][player->x]=map_orginal[player->y][player->x]; 
    
    if(br)
    {
        sem_post(sem2);   
        sem_wait(sem);  
    }
    
    if(br || player->game_round==-1)
    {
        clients--;
        player->pid=0;
        player->x=1;
        player->y=1;
        player->coins=0;
        player->brought=0;
        player->slowed=0;
        player->deaths=0;
        strcpy(player->type,"-----");
    } 

    munmap(data, sizeof(struct data_t));
    close(fd);
    shm_unlink(name);
    endwin();
    
    if(player->game_round==-1)
    {
        snprintf(name, 15, "/game_signal%d",player->id);
        sem_unlink(name);
        snprintf(name, 15, "/game_signal2%d",player->id);
        sem_unlink(name);
    }
    else
    {
        sem_close(sem);
        sem_close(sem2);
    }
    
    return NULL;
}


int main()
{
    int i=0,j=0,x=0,y=0;
    pthread_t th1,th2,th[4];
    
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    start_color();
    srand(time(NULL));

    init_pair(1,COLOR_BLACK,COLOR_BLACK);
    init_pair(2,COLOR_BLACK,COLOR_YELLOW);  //treasures
    init_pair(3,COLOR_WHITE,COLOR_WHITE);   //background
    init_pair(4,COLOR_BLACK,COLOR_WHITE);   //default
    init_pair(5,COLOR_WHITE,COLOR_MAGENTA); //players
    init_pair(6,COLOR_RED,COLOR_WHITE);     //beast
    init_pair(7,COLOR_YELLOW,COLOR_GREEN);  //camp
    
    wbkgd(stdscr,COLOR_PAIR(3));
    
    for(i=1;i<26;++i)
    {
        for(j=1;j<52;++j)
        {
            if(map[i][j]=='x')
            {
                attron(COLOR_PAIR(1));
                mvprintw(i,j,"%c",map[i][j]);
                attroff(COLOR_PAIR(1));
            }
            else if(map[i][j]=='t' || map[i][j]=='T' || map[i][j]=='c' || map[i][j]=='D')
            {
                attron(COLOR_PAIR(2));
                mvprintw(i,j,"%c",map[i][j]);
                attroff(COLOR_PAIR(2));
            }
            else if(map[i][j]>'0' && map[i][j]<'5')
            {
                attron(COLOR_PAIR(5));
                mvprintw(i,j,"%c",map[i][j]);
                attroff(COLOR_PAIR(5));
            }
            else if(map[i][j]=='A')
            {
                attron(COLOR_PAIR(7));
                mvprintw(i,j,"%c",map[i][j]);
                attroff(COLOR_PAIR(7));
            }
            else if(map[i][j]=='*')
            {
                attron(COLOR_PAIR(6));
                mvprintw(i,j,"%c",map[i][j]);
                attroff(COLOR_PAIR(6));
            }
            else
            {
                attron(COLOR_PAIR(4));
                mvprintw(i,j,"%c",map[i][j]);
                attroff(COLOR_PAIR(4));
            }    
        }
        mvprintw(j,i,"\n");
    }

    
    attron(COLOR_PAIR(4));
        
    mvprintw(1,56,"Server's PID: %d",getpid());
    mvprintw(2,57,"Campsite X/Y: 23/11");
    mvprintw(3,57,"Round number: %d",game_round);
    
    mvprintw(5,56,"Parameters:   Player1   Player2   Player3   Player4");
    mvprintw(6,57,"PID          %04u      %04u      %04u      %04u",players[0].pid,players[1].pid,players[2].pid,players[3].pid);
    mvprintw(7,57,"Type         %s     %s     %s     %s",players[0].type,players[1].type,players[2].type,players[3].type);
    
    mvprintw(8,57,"Curr X/Y     %02u/%02u     %02u/%02u     %02u/%02u     %02u/%02u",players[0].x-1,players[0].y-1,players[1].x-1,players[1].y-1,players[2].x-1,players[2].y-1,players[3].x-1,players[3].y-1);
    mvprintw(9,57,"Deaths       %02u        %02u        %02u        %02u",players[0].deaths,players[1].deaths,players[2].deaths,players[3].deaths);
            
    mvprintw(11,56,"Coins");
    mvprintw(12,57,"carried      %05u     %05u     %05u     %05u",players[0].coins,players[1].coins,players[2].coins,players[3].coins);
    mvprintw(13,57,"brought      %05u     %05u     %05u     %05u",players[0].brought,players[1].brought,players[2].brought,players[3].brought);
    
    
    mvprintw(16,56,"Legend:");
    
    attron(COLOR_PAIR(5));
    mvprintw(17,57,"1234");
    attroff(COLOR_PAIR(5));
    attron(COLOR_PAIR(4));
    mvprintw(17,62,"- players");
    attroff(COLOR_PAIR(4));
    
    attron(COLOR_PAIR(1));
    mvprintw(18,57,"x");
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(4));
    mvprintw(18,62,"- wall");

    mvprintw(19,57,"#");
    mvprintw(19,62,"- bushes (slow down)");

    attron(COLOR_PAIR(6));
    mvprintw(20,57,"*");
    attroff(COLOR_PAIR(6));
    attron(COLOR_PAIR(4));
    mvprintw(20,62,"- wild beast");
    
    attron(COLOR_PAIR(2));
    mvprintw(21,57,"c");
    mvprintw(21,87,"D");
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(4));
    mvprintw(21,62,"- one coin");
    mvprintw(21,90,"- dropped treasure");
    
    attron(COLOR_PAIR(2));
    mvprintw(22,57,"t");
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(4));
    mvprintw(22,62,"- treasure (10 coins)");
    
    attron(COLOR_PAIR(2));
    mvprintw(23,57,"T");
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(4));
    mvprintw(23,62,"- large treasure (50 coins)");
    
    attron(COLOR_PAIR(7));
    mvprintw(24,57,"A");
    attroff(COLOR_PAIR(7));
    attron(COLOR_PAIR(4));
    mvprintw(24,62,"- campsite");

    refresh();
    
    
    //WATEK STERUJACY SERWEREM I GRA
    pthread_create(&th1,NULL,control,NULL);
    //WATEK LICZACY TURY
    pthread_create(&th2,NULL,round_counter,NULL);
    
    
    //DOLACZENIE DO GRY
    sem_t* sem_connect1 = sem_open("/connection_signal1", O_CREAT, 0600, 0);
    err(sem_connect1 == SEM_FAILED, "sem_open");
    
    sem_t* sem_connect2 = sem_open("/connection_signal2", O_CREAT, 0600, 1);
    err(sem_connect2 == SEM_FAILED, "sem_open");
    
    int fd0 = shm_open("/connection_data", O_CREAT | O_RDWR, 0600);
    err(fd0 == -1, "shm_open");
    
    ftruncate(fd0, sizeof(struct connection_t));
    struct connection_t* connect= (struct connection_t*)mmap(NULL, sizeof(struct connection_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd0, 0);
    err(connect == NULL, "mmap");
    
    sem_init(&connect->cs, 1, 1); 
    
    
    while(end!='q' && end!='Q')
    {
        sem_wait(sem_connect1);
        
        if(end!='q' && end!='Q')
        {
            if(clients<4)
            {
                for(i=0;players[i].pid!=0;++i);
            
                connect->number=i;
                connect->sid=getpid();
                players[i].pid=connect->id;
                
                do
                {
                    x=2+rand()%48;
                    y=2+rand()%22;
                }
                while(map[y][x]!=' ');
                
                players[i].y=y;
                players[i].x=x;
                players[i].camp_y=y;
                players[i].camp_x=x;
                strcpy(players[i].type,connect->type);
                players[i].game_round=game_round;
                
                pthread_create(&th[i],NULL,game,&players[i]);
                
                clients++;
            }
            else
                connect->number=-1;
    
            sem_post(sem_connect2);
        } 
    }
        
    munmap(connect, sizeof(struct connection_t));
    close(fd0);
    shm_unlink("/connection_data");
    sem_close(sem_connect1);
    sem_close(sem_connect2);
    
    pthread_join(th1,NULL);
    pthread_join(th2,NULL);
    endwin();
    
    return 0;
}