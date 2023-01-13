/*------------------
$Log: mifith.C,v $
Revision 1.3  2014/03/11 01:38:21  ykato
deleted DOS support since it is not used.
deleted hidden function since it is no longer used.
use keypad.

Revision 1.2  2014/03/11 00:38:24  ykato
just delete 0x0d character ...

Revision 1.1  2014/03/10 23:35:31  ykato
Initial revision
  
   ana okochi game
   falling into the hole

   man is falling into the hole
   you have to move man to very deep to the hole

--------*/

#include <stdio.h>
#include "conio4linux.h"
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>

#define FTIME 	clock_gettime

#define MIGI        77
#define HIDARI      75

#define PASSCODE   0xc9


int     MANPOS    = 15;
int     CENTER    = 19;
int     BOTTOM    = 24;
int     MIGIHASHI = 38;


//static struct termios initial_settings, new_settings;
static int      hole_width1=10;
static int      hole_width2=12;
static int      num_holes=1;

static int      *earth1=NULL, *earth2=NULL;
static int      surface;
static int      debug_show=0;   /* debug use only */
static int      debug_mode=0;   /* debug use only */


void initialize_earth()
{
    int i;
    //for ( i=0; i<BOTTOM; i++) earth1[i]=0;
    //for ( i=0; i<BOTTOM; i++) earth2[i]=0;
    earth1[BOTTOM]=CENTER-RANDOM(20)+13;
    if (num_holes==2) earth2[BOTTOM]=earth1[BOTTOM];
    clear();
    surface=BOTTOM;
}




void init_keyboard()
{
    noecho();
    timeout(1);
    curs_set(0);

    keypad(stdscr, TRUE);
}

void close_keyboard()
{
    curs_set(1);
    echo();
}




void    overlay_copy(char   *target,char   *source, int  position, int hole_width)
{
    int         i=0;
    while(*source) {
        if ((i>=position)&&(i<position+hole_width)) {
            *target=' ';
        }
        else *target=*source;
        target++;
        source++;
        i++;
    }
}



int     timer_break(int cycle_time)
{
    struct      timespec current_time;
    static struct timespec  old_time;
    static int	first=1; 
    int		t_diff;

    if (first) {
	first=0;
	FTIME(CLOCK_REALTIME, &old_time);
    }

    FTIME(CLOCK_REALTIME,&current_time);
//    t_diff = (int) (1000.0 * (current_time.time - old_time.time)
//	    + (current_time.millitm - old_time.millitm) );
    t_diff = (int) ( ( (long)( current_time.tv_nsec - old_time.tv_nsec))/1000000L);
    if (t_diff > cycle_time) {
	old_time = current_time;
	return 1;
    }
    return 0;

}

void    disp_score(int score, int high_score)
{
   static WINDOW *score_window=NULL;
   if (score_window==NULL){
       score_window=newwin(5,20,2,2);
       //box(score_window,ACS_VLINE,ACS_HLINE);
       box(score_window,'|','-');
   }
   mvwprintw(score_window,1,1,"Your Score:%6d",score);
   mvwprintw(score_window,3,1,"High Score:%6d",high_score);
   refresh();
   touchwin(score_window);
   wrefresh(score_window);
}

void   show_value(const char * name, int value)
{

   static WINDOW *debug_window=NULL;
   if (debug_window==NULL){
       debug_window=newwin(3,25,2,COLS-27);
       //box(debug_window,ACS_VLINE,ACS_HLINE);
       box(debug_window,'|','-');
   }
   mvwprintw(debug_window,1,1,"%s =>:%4d",name,value);
   refresh();
   touchwin(debug_window);
   wrefresh(debug_window);
}




void    disp_man(int   *oldx, int     *x, char  *man, int manidx)
{
    if (*oldx < *x) {
        gotoxy(*oldx,MANPOS);
        cputs((char *)" ");
        gotoxy(*oldx,MANPOS+1);
        cputs((char *)" ");
        gotoxy(*oldx,MANPOS+2);
        cputs((char *)" ");
    }
    if(*oldx > *x) {

        gotoxy(*oldx+2,MANPOS);
        cputs((char *)" ");
        gotoxy(*oldx+2,MANPOS+1);
        cputs((char *)" ");
        gotoxy(*oldx+2,MANPOS+2);
        cputs((char *)" ");

    }
    gotoxy(*x,MANPOS);
    //cprintf("%s",man);
    addch(man[0]);
    //addch(man[1]);
    if (manidx%2)
//    addch(ACS_DIAMOND);
    addch('*');
    else
//    addch(ACS_DEGREE);
    addch('@');
    addch(man[2]);

    man+=4;
    gotoxy(*x,MANPOS+1);
    cprintf("%s",man);
    man+=4;

    gotoxy(*x,MANPOS+2);
    cprintf("%s",man);

    *oldx=*x;
    gotoxy(1,1);
}




int     nextman(int     i)
{
    i++;
    if (i==6) i=0;
    return i;
}


int     inc(int         i)         /* move man to right */
{
    if (i==MIGIHASHI) return i;
    i++; return i;
}


int     dec(int         i)         /* move man to left */
{
    if (i==1) return i;
    i--; return i;
}

int     next_holex1(int         i)
{
    int         r;
    static int  center=50;
    static int  migi_counter=0;
    static int  hidari_counter=0;


    r=random(10);
    if (( r<3)&&(center==50) ) center=10;
    if (( r>7)&&(center==50) ) center=90;

    if (debug_show==1) show_value("center1",center);

    r=random(100);
    if (r<center) {
        hidari_counter++;
        migi_counter=0;
        if (i>1) i--;
        else { i++; center=20;}
        if (hidari_counter>10) {
            hidari_counter=0;
            center=20;
        }
                return i;
    }
    else {
        migi_counter++;
        hidari_counter=0;
        if (i< MIGIHASHI-2) i++;
        else { i--; center=80;}
        if (migi_counter>10) {
            migi_counter=0;
            center=80;
        }
        return i;
    }
    if ((r>48)&&(r<52) )
        center=50;
}

int     next_holex2(int         i)
{
    int         r=random(100);
    static int  center=50;
    if ( r<10) center=10;
    if ( r>90) center=90;
    if (debug_show==2) show_value("center2",center);
    if (r<center) {
        if (i>1) i--;
        else { i++; center=20;}
                return i;
    }
    else {
        if (i< MIGIHASHI-2) i++;
        else { i--; center=80;}
        return i;
    }
//    if ((r>15)&&(r<85)&&(i>7)&&(i<MIGIHASHI-7))
//      center=50;
}



/*
int     go_up(int       s)
{
    if (s>0) s--;
    return s;
}
*/

void clear_stage_message(int stage)
{
    WINDOW    *clear_stage_message_window;
    clear_stage_message_window = newwin(5,25,  3, COLS/2 -14);
    //box(clear_stage_message_window, ACS_VLINE, ACS_HLINE);
    box(clear_stage_message_window, '|', '-');
    mvwprintw(clear_stage_message_window,1,1,"OK!!");
    mvwprintw(clear_stage_message_window,3,1,"you cleared stage %d",stage);

    wrefresh(clear_stage_message_window);
    sleep(2);
    wclear(clear_stage_message_window);
//    box(clear_stage_message_window, ACS_VLINE, ACS_HLINE);
    box(clear_stage_message_window, '|', '-');
    mvwprintw(clear_stage_message_window,1,1,"You will now enter");
    mvwprintw(clear_stage_message_window,3,1,"New stage, stage-%d",stage+1);

    wrefresh(clear_stage_message_window);
    sleep(2);
    delwin(clear_stage_message_window);
}


void init_soil(char *p, char sc)
{

//  char        soil[]="лллллллллллллллллллллллллллллллллллллллл";
    for (int i=0;i<COLS-1;i++) {
        *p++=  sc;
    }
    *p=0;
}

int main(void)
{


    int          ch;
    long         high_score=0;

    char        man[6][3][4] =  {
                   {  " /",
                      "/|_",
                      "/  "},
                   {  "\\_",
                      "_| ",
                      "  \\"},
                   {  "\\/",
                      " | ",
                      "/ \\"},
                   {  "__",
                      " | ",
                      "/ \\"} ,
                   {  "  ",
                      "/|\\",
                      "/ \\"},
                   {  "__",
                      "_| ",
                      "  \\"}
                } ;


    int         y;

//  textmode(BW40);
//  textattr(23);

    initscr();

    struct      timespec current_time;


    init_keyboard();


 start:
    hole_width1 = 10;
    num_holes = 1;
    debug_show=0;
    FTIME(CLOCK_REALTIME,&current_time);
    debug_mode=0;

    CENTER    = COLS/2 ;
    MANPOS    = LINES -8;
    BOTTOM    = LINES-1;
    MIGIHASHI = COLS-3;
//  char        soil[]="лллллллллллллллллллллллллллллллллллллллл";
                      /*0123456789012345678901234567890123456789*/
    char        *soil = (char *)malloc(sizeof(char)*COLS);
    init_soil(soil,'$');
    int		cycle_time=300;
    char        *work  = (char *)malloc(sizeof(char)*COLS);
    char        *work2 = (char *)malloc(sizeof(char)*COLS);
    int         s;


    int         x=CENTER;     // location of the man
    int         oldx=CENTER;
    int         manidx=0;

    int         timer=0;
    long        score=0;
    char        buf[5];
    char        *p=buf;
    int         c;
    int         n;
    int         quick_mode=0;
    int         stage=1;
    int         code=0;



    if (earth1) free(earth1);
    if (earth2) free(earth2);
    earth1= (int *) malloc (sizeof (int) * (BOTTOM+1));
    earth2= (int *) malloc (sizeof (int) * (BOTTOM+1));
    initialize_earth();

//  randomize();
    srand((int) (current_time.tv_nsec/1000000L));


//    clrscr();
    disp_man(&oldx,&x,man[manidx][0],manidx);
    disp_score(score,high_score);


    while((ch=getch() ) != '\x18' ) {    /* = ctrl-X */
	switch (ch) {
	    case 'l' :
	    case KEY_RIGHT :
		x=inc(x);
		disp_man(&oldx, &x, man[manidx][0],manidx);
		refresh();
		break;
	    case 'h' :
	    case KEY_LEFT :
		x=dec(x);
		disp_man(&oldx,&x,man[manidx][0],manidx);
		refresh();
		break;
	    default :
		break;
	}


        if (timer_break(cycle_time)){
            manidx=nextman(manidx);
            for (s=surface; s<BOTTOM; s++ ) {
                earth1[s]=earth1[s+1];
                overlay_copy(work,soil,earth1[s],hole_width1);
                gotoxy(1,s);
                if (num_holes==2) {
                    earth2[s]=earth2[s+1];
                    overlay_copy(work2,work,earth2[s],hole_width2);
                    cprintf("%s",work2);
                }
                else
                    cprintf("%s",work);
            }
            earth1[s]=next_holex1(earth1[s]);
            if (num_holes==2) {
                earth2[s]=next_holex2(earth2[s]);
            }
            if (surface>1) surface--;


            disp_man(&oldx,&x,man[manidx][0],manidx);
            refresh();
            if ( y > surface) {
                if ( quick_mode) score +=10;
                else score++;

                if (score==100) {
                    cycle_time=200;
                }
                if (score==200) {
                    cycle_time=180;
                }
                if (score==220) cycle_time=150;
                if (score==230) cycle_time=120;
                if (score==240) cycle_time=100;
                if (score==300) cycle_time= 80;
                if (score==350) cycle_time=50;

                if (score==400) {
                    cycle_time=200;
                    hole_width1--; /*9*/
                    num_holes=2;
                    init_soil(soil,'#');
                    initialize_earth();
                    clear_stage_message(stage++);
                }
                if (score==500) {
                    cycle_time=150;
                    hole_width2--;  /*11*/
                }
                if (score==600) {
                    cycle_time=130;
                    hole_width2--; /* 10 */
                }
                if (score==700) {
                    cycle_time=120;
                    hole_width2--; /* 9 */
                }

                if (score==720) cycle_time=100;
                if (score==740) cycle_time=90;
                if (score==750) cycle_time=80;
                if (score==760) cycle_time=70;
                if (score==770) cycle_time=60;

                if (score==800) {
                    clear_stage_message(stage++);
                    init_soil(soil,'@');
                    hole_width1=8;
                    hole_width2=8;
                    cycle_time = 130;
                }
                if (score==900) {
                    cycle_time=200;
                    hole_width2--; /* 8 */
                }
                if (score==1100) {
                    cycle_time=190;
                    hole_width2--; /*  7 */
                    hole_width1=10;
                }
                if (score==1200)  {
                    clear_stage_message(stage++);
                    hole_width1=7;
                    cycle_time=180;
                }
                if (score==1300) cycle_time=170;
                if (score==1400) cycle_time=160;
                if (score==1500) cycle_time=150;
                if (score==1600) cycle_time=140;
                if (score==1700) cycle_time=130;
                if (score==1800) cycle_time=120;
                if (score==1900) cycle_time=110;
                if (score==2000) cycle_time=100;
                if (score==2100) cycle_time= 90;
                if (score==2200) cycle_time= 80;
                if (score==2300) cycle_time= 70;
                if (score==2400) cycle_time= 1;
                disp_score(score,high_score);
            }
        } /* of if timer */
        ////////////////////////////////////////////
        //  CHECK if the man touches the ground.....
        ////////////////////////////////////////////
        if (debug_mode) continue;
        for(y=MANPOS;y<MANPOS+3;y++) {
            if ( y > surface) {

                if ( ( x > earth1[y] ) && ( x < earth1[y]+  hole_width1-1) ) continue;

                if (num_holes==2) {
                    if ( ( x > earth2[y] ) && ( x < earth2[y]+  hole_width2-1) ) continue;

                    if ( ( earth2[y] >= earth1[y]) && (earth2[y] <= earth1[y]+hole_width1) ) {
                        if ( ( x>earth1[y] ) && (x < earth2[y] + hole_width2-1) ) continue;
                    }
                    if ( ( earth1[y] >= earth2[y]) && (earth1[y] <= earth2[y]+hole_width2) ) {
                        if ( ( x>earth2[y] ) && (x < earth1[y] + hole_width1-1) ) continue;
                    }
                }

                goto game_over;
            }
        }
    }  /* of while(1) */
  game_over:
    WINDOW    *game_over_window;
    game_over_window = newwin(5,23,  LINES/2 - 2, COLS/2 -11);
//  box(game_over_window, ACS_VLINE, ACS_HLINE);
    box(game_over_window, '|'      , '-'      );
    if (high_score<score) high_score=score;
    //gotoxy(10,10);
    //cprintf("GAME OVER\n");
    mvwprintw(game_over_window,1,1,"GAME OVER");
    //gotoxy(10,12);
    //cprintf("will you replay?(Y/N)");
    mvwprintw(game_over_window,3,1,"Will you replay?(Y/N)");
    wrefresh(game_over_window);
  repeat:
    ch=getch();
    if (ch=='y') goto start;
    if (ch=='Y') goto start;

    if (ch!='n') goto repeat;

    close_keyboard();
    endwin();
}
