#include "common.h"

int sid=0,con=0,movee=0;
int game_round=0,r=0;

void* server_close(void* arg)
{
    char name[20];
    
    pthread_mutex_t mutex =PTHREAD_MUTEX_INITIALIZER;
    
    snprintf(name, 15, "/game_signal%d",con);
    sem_t* sem = sem_open(name, O_CREAT, 0600, 0);
    err(sem == SEM_FAILED, "sem_open");
    
    r=game_round;
    
    while(movee!='q' && movee!='Q' && r!=-1)
    {
        pthread_mutex_lock(&mutex);
        
        r+=1;
        if(abs(r-game_round)>8)
        {
            sem_post(sem);    
            r=-1;
        }  
         
        pthread_mutex_unlock(&mutex);  
        usleep(800000);
    }
  
    sem_close(sem);
    return NULL;
}

void* game(void* arg)
{
    int i=0,j=0;
    char name[20];
    pthread_t th;
    
    initscr();
    timeout(800); 
    keypad(stdscr, TRUE);
    noecho();
    start_color();
    pthread_mutex_t mutex =PTHREAD_MUTEX_INITIALIZER;
    
    init_pair(1,COLOR_BLACK,COLOR_BLACK);
    init_pair(2,COLOR_BLACK,COLOR_YELLOW);  //treasures
    init_pair(3,COLOR_WHITE,COLOR_WHITE);   //background
    init_pair(4,COLOR_BLACK,COLOR_WHITE);   //default
    init_pair(5,COLOR_WHITE,COLOR_MAGENTA); //players
    init_pair(6,COLOR_RED,COLOR_WHITE);     //beast
    init_pair(7,COLOR_YELLOW,COLOR_GREEN);  //camp

    wbkgd(stdscr,COLOR_PAIR(3));
    
    snprintf(name, 15, "/game_signal%d",con);
    sem_t* sem = sem_open(name, O_CREAT, 0600, 0);
    err(sem == SEM_FAILED, "sem_open");
    
    snprintf(name, 15, "/game_signal2%d",con);
    sem_t* sem2 = sem_open(name, O_CREAT, 0600, 1);
    err(sem2 == SEM_FAILED, "sem_open");
        
    snprintf(name, 15, "/game_data%d",con);
    int fd = shm_open(name, O_CREAT | O_RDWR, 0600);
    err(fd == -1, "shm_open");
        
    struct data_t* data = (struct data_t*)mmap(NULL, sizeof(struct data_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    err(data == NULL, "mmap");


    while(movee!='Q' && movee!='q' && r!=-1)     
    {         
        sem_wait(sem);
            
        pthread_mutex_lock(&mutex);    
        sem_wait(&data->cs);
            
        if(data->direction!='Q' && data->direction!='q' && r!=-1) 
        {
            data->direction=' ';
                
            //WYSWIETLENIE MAPY
            for(i=0;i<5;++i)
            {
                for(j=0;j<5;++j)
                {
                    if(data->map[i][j]=='x')
                    {
                        attron(COLOR_PAIR(1));
                        mvprintw(i+1,j+1,"%c",data->map[i][j]);
                        attroff(COLOR_PAIR(1));
                    }
                    else if(data->map[i][j]=='t' || data->map[i][j]=='T' || data->map[i][j]=='c' || data->map[i][j]=='D')
                    {
                        attron(COLOR_PAIR(2));
                        mvprintw(i+1,j+1,"%c",data->map[i][j]);
                        attroff(COLOR_PAIR(2));
                    }
                    else if(data->map[i][j]>'0' && data->map[i][j]<'5')
                    {
                        attron(COLOR_PAIR(5));
                        mvprintw(i+1,j+1,"%c",data->map[i][j]);
                        attroff(COLOR_PAIR(5));
                    }
                    else if(data->map[i][j]=='A')
                    {
                        attron(COLOR_PAIR(7));
                        mvprintw(i+1,j+1,"%c",data->map[i][j]);
                        attroff(COLOR_PAIR(7));
                    }
                    else if(data->map[i][j]=='*')
                    {
                        attron(COLOR_PAIR(6));
                        mvprintw(i+1,j+1,"%c",data->map[i][j]);
                        attroff(COLOR_PAIR(6));
                    }
                    else
                    {
                        attron(COLOR_PAIR(4));
                        mvprintw(i+1,j+1,"%c",data->map[i][j]);
                        attroff(COLOR_PAIR(4));
                    }    
                }
                mvprintw(j,i,"\n");
            }
                
            attron(COLOR_PAIR(4));
                
            mvprintw(1,56,"Server's PID: %d",sid);
                
            if(data->camp_x==-1 && data->camp_y==-1)   
                mvprintw(2,57,"Campsite X/Y: unknown");
            else
                mvprintw(2,57,"Campsite X/Y: %u/%u  ",data->camp_x,data->camp_y);

            mvprintw(3,57,"Round number: %d",data->game_round);
                
            mvprintw(5,56,"Player");
            mvprintw(6,57,"Number:    %d",data->id);
            mvprintw(7,57,"Type:      HUMAN");
            mvprintw(8,57,"Curr X/Y:  %02u/%02u",data->x,data->y);
            mvprintw(9,57,"Deaths:    %02u",data->deaths);
                
            mvprintw(11,56,"Coins");
            mvprintw(12,57,"carried:   %05u",data->coins);
            mvprintw(13,57,"brought:   %05u",data->brought);
                
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
                
                
            movee=getch();
                
            switch(movee)
            {
                case KEY_UP:
                    data->direction='w';
                    break;
                case KEY_DOWN:
                    data->direction='s';
                    break;
                case KEY_RIGHT:
                data->direction='d';
                    break;
                case KEY_LEFT:
                    data->direction='a';
                    break;   
                case 'q':
                case 'Q':
                    data->direction='q';
                    break; 
                default:
                    break;
            } 
        }
            
        //SERWER ZAKONCZYL PRACE Q
        movee=data->direction;
        
        game_round=data->game_round;
        if(r==0)
            pthread_create(&th,NULL,server_close,NULL); 
                
        sem_post(&data->cs);
        pthread_mutex_unlock(&mutex);
        
        sem_post(sem2);  
    }
     
    pthread_join(th,NULL);
    
    munmap(data, sizeof(struct data_t));
    close(fd); 
    endwin();
    
    if(r==-1)
    {
        shm_unlink(name);
        snprintf(name, 15, "/game_signal%d",con);
        sem_unlink(name);
        snprintf(name, 15, "/game_signal2%d",con);
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
    int my=0,mx=0;
    pthread_t th;
    
    initscr();
    start_color();
    
    init_pair(1,COLOR_BLACK,COLOR_BLACK);
    init_pair(2,COLOR_BLACK,COLOR_YELLOW);  //treasures
    init_pair(3,COLOR_WHITE,COLOR_WHITE);   //background
    init_pair(4,COLOR_BLACK,COLOR_WHITE);   //default
    init_pair(5,COLOR_WHITE,COLOR_MAGENTA); //players
    init_pair(6,COLOR_RED,COLOR_WHITE);     //beast
    init_pair(7,COLOR_YELLOW,COLOR_GREEN);  //camp

    wbkgd(stdscr,COLOR_PAIR(3));
    
    
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
    
    
    //POLACZENIE
    sem_wait(sem_connect2);
    
    connect->id=getpid();
    strcpy(connect->type,"HUMAN");
        
    sem_post(sem_connect1);
    
    //OCZEKUJE NA SERWER
    sem_wait(sem_connect2);
    sem_post(sem_connect2);
    
    
    //GRA
    if(connect->number>=0)
    {       
        sid=connect->sid;
        con=connect->number;
        
        pthread_create(&th,NULL,game,NULL);   
        
        pthread_join(th,NULL);
    }
    else
    {
        getmaxyx(stdscr,my,mx);
        
        attron(COLOR_PAIR(6));
        mvprintw(5,5,"Przepraszamy, obecnie serwer jest pelny");
        attroff(COLOR_PAIR(6));
        refresh();
        usleep(10000000);
    }
            
    sem_close(sem_connect1);
    sem_close(sem_connect2);
    munmap(connect, sizeof(struct connection_t));
    close(fd0);
    endwin();
    
    return 0;    
}