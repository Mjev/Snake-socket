#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED
static int callback(void*,int,char**,char**);
int insert(char*,int);
char* get_highscore();
char* parse_file();

#endif // DATABASE_H_INCLUDED
