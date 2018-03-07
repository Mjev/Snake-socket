#ifndef FRONTEND_H_INCLUDED
#define FRONTEND_H_INCLUDED
#include <ncurses.h>

enum Direction { UP = 0, DOWN, LEFT, RIGHT };
enum Status { SUCCESS, FAILURE };
struct PointList {
  int x;
  int y;
  struct PointList* next;
};

typedef struct PointList PointList;


void free_pointList(PointList*);
PointList* create_cell(int,int);
enum Direction get_next_move(enum Direction previous);
void display_points(PointList* snake, const chtype symbol);


#endif // FRONTEND_H_INCLUDED
