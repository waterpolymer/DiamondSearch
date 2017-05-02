
#include "state.h"

void sample(state &s, int &x, int &y) {
  x = rand() % s.w;
  y = rand() % s.h;
}

void check_visibility(state &s) {
  int radius = 5;
  for(int i = s.x-radius; i <= s.x+radius; ++i) {
    for(int j = s.y-radius; j <= s.y+radius; ++j) {
      
      int dist2 = (i-s.x)*(i-s.x) + (j-s.y)*(j-s.y);

      if (i >= 0 && i < s.w && j >= 0 && j < s.h && dist2 < radius*radius)
        s.explored[i][j] = true;

    }
  }
}

bool square_is_empty(state &s, int x, int y) {
  for (int i = x-1; i <= x+1; ++i) {
    for (int j = y-1; j <= y+1; ++j) {
      if (i >= 0 && i < s.w && j >= 0 && j < s.h && s.map[i][j] != EMPTY)
        return false;
    }
  }
  return true;
}

void init(state &s, int width, int height, int diamonds, int bombs) {
  s.w = width;
  s.h = height;
  
  // initialize the map
  s.map.resize(width);
  s.explored.resize(width);
  for(int i = 0; i<width; ++i) {
    s.map[i].resize(height);
    s.explored[i].resize(height);
    for(int j = 0; j<height; ++j) {
      s.map[i][j] = EMPTY;
      s.explored[i][j] = false;
    }
  }
  
  // add bombs
  int b = bombs;
  while(b > 0) {
    int x, y;
    sample(s, x, y);
    if (square_is_empty(s,x,y)) {
      s.map[x][y] = BOMB;
      b--;
    }
  }

  // add diamonds
  int n = diamonds;
  while(n > 0) {
    int x, y;
    sample(s, x, y);
    if (s.map[x][y] == EMPTY) {
      s.map[x][y] = DIAMOND;
      n--;
    }
  }
  s.initial_diamonds = diamonds;
  s.time = 0;
  s.score = 0;
  
  s.play = true;

  // add player
  while(true) {
    int x, y;
    sample(s, x, y);
    if (s.map[x][y] == EMPTY) {
      s.x = x;
      s.y = y;
      s.alive = true;
      break;
    }
  }
  // explore the tiles around the player
  check_visibility(s);

}

void perform_action(state &s, action a) {

  s.time++;
  
  int dx = 0, dy = 0;
  switch (a) {
    case UP: dy += 1; break;
    case DOWN: dy -= 1; break;
    case RIGHT: dx += 1; break;
    case LEFT: dx -= 1; break;
  }
  
  int nx = s.x + dx;
  int ny = s.y + dy;
  if (nx >= 0 && nx < s.w && ny >= 0 && ny < s.h) {
    s.x = nx;
    s.y = ny;

    if (s.map[nx][ny] == DIAMOND) {
      s.map[nx][ny] = EMPTY;
      s.score++;
    }
    
    if (s.map[nx][ny] == BOMB) {
      s.map[nx][ny] = EMPTY;
      s.alive = false;
    }

    check_visibility(s); // explore the map
  }
}

void update(state &s, std::ostream &gamelog) { 
  // don't do anything if the game has finished
  if (s.score >= s.initial_diamonds || !s.alive) return;

  // do robot's logic
  std::vector<std::vector<tile> > m;
  m.resize(s.w);
  for(int i = 0; i<s.w; ++i) {
    m[i].resize(s.h);
    for(int j = 0; j<s.h; ++j) {
      if(s.explored[i][j]) 
        m[i][j] = s.map[i][j];
      else
        m[i][j] = UNEXPLORED;
    }
  }
  loc pl = {s.x, s.y};
  action a = choose_next_step(s.w, s.h, pl, m, gamelog);
  perform_action(s, a);
}

