
#include <cstdlib>
#include <iostream>
#include "bot3.h"

using namespace std;

static loc target;
static loc savedTar;
static loc previousTar; //keep destination location

//store dm locations
static loc dm;

//easily changable distance of search radius
static const int XSEARCHRAD = 2;       //this is to guarantee a search area of 5x7
static const int YSEARCHRAD = 4;

static bool cornerStart = false;
static bool moveUp = false;
static bool dmFound = false;
static bool resumeTar = false;

static int upC(0), downC(0), leftC(0), rightC(0);

void init_robot(int w, int h, loc robot, ostream &log) {
  log << "Start." << endl;
  target = robot;
  savedTar = robot;
  previousTar = robot;
}

//check the map and find exactly one diamond
void check_diamond(int w, int h, const vector< vector<tile> > map){
  loc temp;
  if(dmFound == false){
    for(int i = 0; i < w; i++){
      for(int j = 0; j < h; j++){
        if(map[i][j] == 1){
          temp.x = i;
          temp.y = j;
          dm = temp;
          dmFound = true;  //prevents robot from losing current diamond target
        }
      }
    }
  }
}

void change_target_from_mine(loc &targetToChange, const vector< vector<tile> > map){
  if(map[targetToChange.x][targetToChange.y] == 3){
    targetToChange.y = targetToChange.y - 1;
  }
}

void direction_counter_reset(){
  upC = 0;
  downC = 0;
  leftC = 0;
  rightC = 0;
}

action choose_next_step(int w, int h, loc robot, const vector< vector<tile> > map, ostream &log) 
{
  change_target_from_mine(savedTar, map);
  change_target_from_mine(previousTar, map);
  //log << "savedTarA (" << savedTar.x << "," << savedTar.y << "); ";

  //start at bottom left corner and then start to explore
  if(cornerStart == false && dmFound == false && resumeTar == false
     && robot.x == XSEARCHRAD && robot.y == YSEARCHRAD){
    cornerStart = true;
    log << "flag 5 ";
  }else if(cornerStart == false && dmFound == false && resumeTar == false
     && robot.x == XSEARCHRAD && robot.y == YSEARCHRAD - 1
     && map[XSEARCHRAD][YSEARCHRAD] == 3){
    cornerStart = true;
  }else if (cornerStart == false && dmFound == false && resumeTar == false
     && map[XSEARCHRAD][YSEARCHRAD] != 3){
    savedTar.x = XSEARCHRAD;
    savedTar.y = YSEARCHRAD;
    log << "flag 6 ";
  }

  check_diamond(w, h, map);  //check if there is a diamond visable on the map

  if(dmFound == false && cornerStart == true && resumeTar == false
     && robot.x == savedTar.x && robot.y == savedTar.y){
    if(moveUp == true){
      if(h - robot.y > YSEARCHRAD*2){
        savedTar.y = savedTar.y + YSEARCHRAD*2;
	log << "flag 8 " << savedTar.y << " ";
      }else if(h - robot.y > YSEARCHRAD + 1){  //if there is no diamond
        savedTar.y = savedTar.y + YSEARCHRAD + 1;  //and no targets, move up
	log << "flag 9 " << savedTar.y << " ";
      }
      moveUp = false;
    log << "flag 10 ";
    }else if(robot.x <= XSEARCHRAD){
      savedTar.x = w - 1 - XSEARCHRAD;  //if you don't need to move up, explore right
      moveUp = true;
    log << "flag 11 ";
    }else if(robot.x >= w - 1 - XSEARCHRAD){
      savedTar.x = XSEARCHRAD;  //if you don't need to move up or right, explore left
      moveUp = true;
    log << "flag 12 ";
    }
  }

  //if you find a diamond, save the robot's current loc and destination if you haven't
  //and set the new target to the dimond

  if(dmFound == true && dm.x == robot.x && dm.y == robot.y){
     dmFound = false;
     log << "flag 13 ";
  }else if(dmFound == true){
    if(resumeTar == false){
      previousTar = savedTar;
      resumeTar = true;
      log << "flag 14 ";
    }
    savedTar = dm;
    log << "flag 15 ";
  }
  
  //after you find a diamond, go back into position
  if(dmFound == false){
    if(map[previousTar.x][previousTar.y] == 3){
      if(previousTar.y <= 2){
        previousTar.y++;
        log << "flag 16 ";
      }else {
        previousTar.y--; 
        log << "flag 17 ";
      }
    }
    if(resumeTar == true && previousTar.x == robot.x && previousTar.y == robot.y){
       resumeTar = false;
       log << "flag 18 ";
    }else if(resumeTar == true && robot.y != previousTar.y){
       savedTar.y = previousTar.y;
       log << "flag 19 ";
    }else if(resumeTar == true && robot.x != previousTar.x){
       savedTar.x = previousTar.x;
       log << "flag 20 ";
    }
  }



  if(robot.x < savedTar.x && map[robot.x+1][robot.y] == 3){
      direction_counter_reset(); //in case while it's moving, it sees another mine
      rightC = 2;
      if(robot.y == h-1){
        return DOWN;  //in case the robot detects mine at ceiling
      }
      if(robot.x == w-1 && robot.y < savedTar.y){
        rightC = 1; //in case the robot detects mine at wall
        return UP;
      }
      if(robot.x == w-1 && robot.y > savedTar.y){
        rightC = 1;
        return DOWN;
      }
      log << "flag 25 ";
      return UP;
  }
  if(robot.x > savedTar.x && map[robot.x-1][robot.y] == 3){
      direction_counter_reset();
      leftC = 2;
      if(robot.y == 0){
        return UP;  //in case the robot detects mine at floor
      }
      if(robot.x == 1 && robot.y < savedTar.y){
        leftC = 1;  //in case the robot detects mine at wall
        return UP;
      }
      if(robot.x == 1 && robot.y > savedTar.y){
        leftC = 1;
        return DOWN;
      }
      log << "flag 26 ";
      return DOWN;
  }
  if(robot.y < savedTar.y && map[robot.x][robot.y+1] == 3){
      direction_counter_reset();
      upC = 2;
      if(robot.x == 0){
        return RIGHT;
      }
      if(robot.y == h-2 && robot.x < savedTar.x){
        upC = 1;
        return RIGHT;   //in case the robot detects mine at the ceiling of the map
      }
      if(robot.y == h-2 && robot.x > savedTar.x){
        upC = 1;
        return LEFT;
      }
      log << "flag 27 ";
      return LEFT;
  }
  if(robot.y > savedTar.y && map[robot.x][robot.y-1] == 3){
      direction_counter_reset();
      downC = 2;	
      if(robot.x == w-1){
        return LEFT;
      }
      if(robot.y == 1  && robot.x < savedTar.x){
        downC = 1;
        return RIGHT;  //in case the robot detects at the floor of the map
      }
      if(robot.y == 1 && robot.x > savedTar.x){
        downC = 1;
        return LEFT;
      }
      log <<"flag 28 ";
      return RIGHT;
  }

  target = savedTar;

  log << "savedTar (" << savedTar.x << "," << savedTar.y << "); ";
  //log << "map: " << map[savedTar.x][savedTar.y] << " "; 
  log << "map: " << map[robot.x][robot.y-1] << " "; 

  //if there is a command to avoid a mine, do it first before executing moving in the direction
  //of the target
  if(upC > 0 && map[robot.x][robot.y+1] != 3){
    upC--;
    log<< "flag 1 ";
    return UP;
  }
  if(downC > 0 && map[robot.x][robot.y-1] != 3){
    downC--;
    log << "flag 2 ";
    return DOWN;
  }
  if(leftC > 0 && map[robot.x-1][robot.y] != 3){
    leftC--;
    log << "flag 3 ";
    return LEFT;
  }
  if(rightC > 0 && map[robot.x+1][robot.y] != 3){
    rightC--;
    log << "flag 4 ";
    return RIGHT;
  }

  if(robot.x < target.x && map[robot.x+1][robot.y] != 3){
    log << "flag 29 ";
    return RIGHT;
  }
  if(robot.x > target.x && map[robot.x-1][robot.y] != 3){
    log << "flag 30 ";
    return LEFT;
  }
  if(robot.y < target.y && map[robot.x][robot.y+1] != 3){
    log << "flag 31 ";
    return UP;
  }
  if(robot.y > target.y && map[robot.x][robot.y-1] != 3){
    log << "flag 32 ";
    return DOWN;
  }

  //log << "End Prog ";
  //return UP;
}

//small fixes to make: lag at corners & lag when seeing two diamonds
//map: 2 = unexplored, 1 = diamond, 0 = explored, 3 = mine

/* problem list:

cornerStart error
moving to corners error
something unknown? maybe another corner case
*/



