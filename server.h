#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED


int play(int);
void game(int);
void error(const char*);
void serialize_pointList(PointList*,int, int);

#endif // SERVER_H_INCLUDED
