
#ifndef BOT_H
#define BOT_H

#include <vector>
#include <iostream>

using namespace std;

/* Each tile of the map can be either Empty, or contain a Diamond or Bomb, or remains Unexplored */
enum tile {EMPTY, DIAMOND, UNEXPLORED, BOMB};

/* Four actions available to the robot */
enum action {LEFT, RIGHT, UP, DOWN};

/* Location */
struct loc {
  int x;
  int y;
};

/* choose_next_step(w, h, robot, map, log) 
 * 
 *    Returns the next action to perform: UP, RIGHT, DOWN, or LEFT
 *
 * Arguments:
 * w and h: width and height of the map,
 * robot: location of the robot (Example: robot.x and robot.y are its x and y coordinates),
 * map: a 2D map of the area. Each element map[x][y] can be either of {EMPTY, DIAMOND, UNEXPLORED}
 */
action choose_next_step(int w, int h, loc robot, const vector< vector<tile> > map, ostream &log);


/* init_robot (w, h, robot, log) 
 * 
 * This funciton runs before the robot starts moving. Use it to initialize 
 * global variables if you need them.
 */
void init_robot(int w, int h, loc robot, ostream &log);


#endif
