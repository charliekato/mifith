#ifndef CONIO4LINUX_H__
#define CONIO4LINUX_H__
#include <curses.h>
#include <unistd.h>
//#include <termios.h>
#include <stdlib.h>

inline
void gotoxy(int x, int y) {  move(y,x); }

inline
void cputs(char *string) { printw("%s",string); }


#define RANDOM(N)      ( (int) (( (double)rand()/RAND_MAX)*(N) ) )
#undef random
#define random RANDOM


#undef cprintf
#define cprintf printw

#undef clrscr
#define clrscr clear


#endif
