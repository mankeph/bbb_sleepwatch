#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

/* Sensor numbers and table names:
 * Light level: 0, lvals
 * Noise level: 1, mvals
 * Temperature: 2, tvals
 * Humidity:    3, hvals
 * Air quality: 4, gvals
 */

#define LSENS 0
#define NSENS 1
#define TSENS 2
#define HSENS 3
#define GSENS 4

#define LTAB lvals
#define NTAB mvals
#define TTAB tvals
#define HTAB hvals
#define GTAB gvals

// can hold a line from the sqlite3 database
typedef struct db_line{
  long mnr;
  char mtimestamp[20];
  int value;
} LINE; 

// struct for multiple lines from the database
typedef struct db_table{
  long linecount;
  LINE* lines;
} TABLE;

typedef struct str_stmnt{
  int length;
  char* stmnt;
} STMT;

typedef sqlite3 DBO;

char* db_file;
long mnr_cnts[5];

int init_db(char* file);
int build_new_db(void);
int insert_db(char sensnr, int* value);
TABLE* query_db(char sensnr, char* s_time, char* e_time);
TABLE* tail_db(char sensnr, int nr);
void exit_db(void);

int init_sql(char* file, DBO* db);
void exit_sql(DBO* db);
int exec_sql(STMT* cstmt, DBO* db);
TABLE* exec_sql_ret(STMT* cstmt, DBO* db);
STMT* build_insert_stmt(char sensnr, int* value);
STMT* build_query_tail_stmt(char sensnr, int nr);
STMT* build_query_time_stmt(char sensnr, char* from, char* to);
char* get_tablename(char sensnr);








