#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

void game(int);
int play(int);
void error(const char*);
PointList* deserialize_pointList(PointList*,int,int);
void free_pointList(PointList*);

#endif // CLIENT_H_INCLUDED
