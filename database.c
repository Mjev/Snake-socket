#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>


char* filename = "tmp/result.txt";
int k = 1;

static int callback(void *data, int argc, char **argv, char **azColName){
   fprintf(stderr, "%s: ", (const char*)data);
   FILE *fp;

   fp = fopen(filename,"a");

   for(int i = 0; i<argc; i++) {
      fprintf(fp,"%i. %s - %s\n",k, argv[i] ? argv[i] : "NULL", argv[i+1] ? argv[i+1] : "NULL");
      k++;
      i++;
      //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NUiiLL");
   }
   fprintf(fp,"\n");
   fclose(fp);

   return 0;
}

int insert(char* user_name, int score)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char sql[256];

    char score_str[6];
    sprintf(score_str, "%d", score);

    rc = sqlite3_open("snake.db", &db);

    if(rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }


    char* str1 = "INSERT INTO highscores (user_name,score) VALUES ('";
    char* str3 = "','";
    char* str5 = "');";

    snprintf(sql, sizeof sql, "%s%s%s%s%s", str1, user_name, str3, score_str,str5);

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if(rc != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Records created successfully\n");
    }

    sqlite3_close(db);
}

char* get_highscore()
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char* sql;
    char* buffer;


    rc = sqlite3_open("snake.db", &db);

    if(rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    } else {
        fprintf(stderr, "Opened database successfully\n");
    }


    sql = "SELECT user_name, score FROM highscores ORDER BY score DESC LIMIT 10;";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

    if(rc != SQLITE_OK){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Operation done successfully\n");
    }

    sqlite3_close(db);

    buffer = parse_file();

    return buffer;
}

char* parse_file()
{
    char* buffer;
    FILE *fp;
    int length;
    fp = fopen(filename,"r");

    if(fp)
    {
        fseek(fp,0,SEEK_END);
        length = ftell(fp);
        fseek(fp,0,SEEK_SET);
        buffer = malloc(length);
        if(buffer) {
            fread(buffer,1,length,fp);
        }

        fclose(fp);
        fclose(fopen(filename, "w")); // clear file
        buffer[length] = '\0';

        k = 1;
        return buffer;
    }

}
