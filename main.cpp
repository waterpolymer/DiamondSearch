
#include <unistd.h>
#include <stdio.h>
#include <curses.h>
#include <locale.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <iostream>

#include <stdlib.h>

#include "state.h"
#include "output.h"

volatile sig_atomic_t input_ready;
volatile sig_atomic_t time_to_redraw;
 
void on_timer(int signum);   /* handler for alarm */
int update_from_input(state &s, std::ostream &gamelog);

/* Run the game */
void run (state &s) {

  buf buf;
  std::ostream gamelog (&buf);
  
  output(s, buf.data);
  
  {
    struct loc pl = {s.x, s.y};
    init_robot(s.w, s.h, pl, gamelog);
  }

  int k = 0;
  int finished = 0;
  while( !finished ) {
    if (time_to_redraw) {
      time_to_redraw = 0;
      k++;
      if (k>=10) { 
        
        k=0;
        // run robot's logic and update the state
        if (s.play) {
          update(s, gamelog);
        }

        output(s, buf.data);
      }
    }
    finished = update_from_input(s, gamelog);
    pause(); // sleep until woken up by SIGALRM
  }
}


/* Main function. Mostly boilerplate code of setting up the terminal. */
int main(int argc, char* argv[]){

  /* Setup signals */
  struct sigaction newhandler;            /* new settings         */
  sigset_t         blocked;               /* set of blocked sigs  */
  newhandler.sa_flags = SA_RESTART;       /* options     */
  sigemptyset(&blocked);                  /* clear all bits       */
  newhandler.sa_mask = blocked;           /* store blockmask      */
  newhandler.sa_handler = on_timer;      /* handler function     */
  if ( sigaction(SIGALRM, &newhandler, NULL) == -1 )
    perror("sigaction");
  

  /* Initialize ncurses output and colors */
  setlocale(LC_ALL, "");
  initscr();     /* initialize the library and screen */
  cbreak();      /* put terminal into non-blocking input mode */
  noecho();      /* turn off echo */
  start_color();
  curs_set(0);   /* hide the cursor */

  use_default_colors();
  init_pair(0, COLOR_WHITE, COLOR_BLACK);
  init_pair(1, COLOR_WHITE, COLOR_BLACK);
  init_pair(2, COLOR_BLACK, COLOR_BLACK);
  init_pair(3, COLOR_RED, COLOR_BLACK);
  init_pair(4, COLOR_GREEN, COLOR_BLACK);
  init_pair(5, COLOR_BLUE, COLOR_BLACK);
  init_pair(6, COLOR_YELLOW, COLOR_BLACK);
  init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(8, COLOR_CYAN, COLOR_BLACK);

  color_set(0, NULL);
  assume_default_colors(COLOR_WHITE, COLOR_BLACK);

  attrset(A_NORMAL | COLOR_PAIR(0));
  refresh();
  clear();
 
  /* Non-blocking input */
  int fd_flags = fcntl(0, F_GETFL);
  fcntl(0, F_SETFL, (fd_flags|O_NONBLOCK));

  /* Start the real time interval timer */
  struct itimerval it;
  it.it_value.tv_sec = 0;
  it.it_value.tv_usec = 10000;
  it.it_interval.tv_sec = 0;
  it.it_interval.tv_usec = 10000;
  setitimer(ITIMER_REAL, &it, NULL);
  
  refresh();        
  input_ready = 0;
  time_to_redraw = 1;

  /* Init the game */
  if (argc > 1) {
    /* read the first command line argument as an integer seed */
    srand(atoi(argv[1]));
  }
  else {
    srand(time(NULL));
  }
  
  state st;
  init(st, 40, 20, 10, 20);
  
  /* Run the game */
  
  run(st);
  
  /* Restore the teminal state */
  echo();
  curs_set(1);
  clear();
  endwin();

  return 0;
}

/* SIGIO signal handler (keyboard input) */
int update_from_input(state &s, std::ostream &gamelog)
{
    int c;
    char buf[1];
    int finished=0;

    while ( fread(buf, 1, 1, stdin) == 1 ) {
      c = buf[0];

      switch(c){
        case 'q': case 'Q':
          finished = 1;
          break;
        case 'f': case 'F':
          for(int i = 0; i<500; ++i){ 
            update(s, gamelog);
          }
          break;
        case 's': case 'S':
          s.play = false;
          update(s, gamelog);
          break;
        case 'p': case 'P':
          s.play = !s.play;
          break;
        default:;
      }

    }                
    return finished;
}

/* SIGALRM signal handler (our timer) */
void on_timer(int signum)
{
  time_to_redraw = 1;
}
