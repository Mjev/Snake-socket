#include <ncurses.h>
#include "frontend.h"
#include <stdlib.h>

void display_points(PointList* snake, const chtype symbol) {
  while(snake) {
    mvaddch(snake->y, snake->x, symbol);
    snake = snake->next;
  }
}


PointList* create_cell(int x, int y) {
  PointList* cell = (PointList*) malloc(sizeof(PointList));
  cell->x = x;
  cell->y = y;
  cell->next = NULL;
  return cell;
}



enum Direction get_next_move(enum Direction previous) {
  int ch = getch();
  switch (ch) {
    case KEY_LEFT:
      if (previous != RIGHT) return LEFT;
    case KEY_RIGHT:
      if (previous != LEFT) return RIGHT;
    case KEY_DOWN:
      if (previous != UP) return DOWN;
    case KEY_UP:
      if (previous != DOWN) return UP;
    default:
      return previous;
  }
}
