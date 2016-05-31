#include "nbnl_db.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <ctime>
#include <vector>
#include <time.h>
using namespace std;
struct db_con_s {
	sqlite3 * db;
};
bool sortFunction (garage_stats_entry g1, garage_stats_entry g2)
 {
     return g1.actionTime < g2.actionTime;
 }
 
void sortEntries(vector<garage_stats_entry> entries)
{
    sort (entries.begin(), entries.end (), sortFunction);
}
db_con_t * db_connect(char * file_name)
{
	db_con_t * self = (db_con_t *)malloc(sizeof(db_con_t));
	int rc = 0;
	rc = sqlite3_open(file_name, &(self->db));
	if (rc != SQLITE_OK)
	{
		free(self);
		return NULL;
	}
	return self;
}
char*  time_to_string(time_t t, char * buffer)
{
	struct tm *gm;
	gm = localtime(&t);
	sprintf(buffer, "%d/%d/%d   %d.%d", gm->tm_year + 1900, gm->tm_mon + 1, gm->tm_mday, gm->tm_hour, gm->tm_min);
	return buffer;
}

void db_add_entry(db_con_t * self, char * type, time_t actionTime, char * called)
{
	sqlite3_stmt *stmt = NULL;
	int rc = 0;
	char * sql = "INSERT INTO garage_actions (Type, Time, Called) VALUES (?,?,?);";

	rc = sqlite3_prepare_v2(self->db, sql, strlen(sql) + 1, &stmt, NULL);
	if (SQLITE_OK != rc) {
		printf("Error prepare! %i\n", rc);
		return;
	}

	rc = sqlite3_bind_text(stmt, 1, type, strlen(type), SQLITE_STATIC);
	if (SQLITE_OK != rc) {
		printf("Error bind type! %i\n", rc);
		return;
	}

	rc = sqlite3_bind_int(stmt, 2, actionTime);
	if (SQLITE_OK != rc) {
		printf("Error bind actionTime! %i\n", rc);
		return;
	}

	rc = sqlite3_bind_text(stmt, 3, called, strlen(called), SQLITE_STATIC);
	if (SQLITE_OK != rc) {
		printf("Error bind called! %i\n", rc);
		return;
	}
	if (SQLITE_DONE != (rc = sqlite3_step(stmt))) {
		printf("Not inserted! %i\n", rc);
		return;
	}
	printf("Data inserted!\n");
	sqlite3_finalize(stmt);
}

void db_get_garage_stats(db_con_t * self, vector<garage_stats_entry>& refs)
{
	const char* statement = "SELECT * FROM garage_actions";
	sqlite3_stmt* stmt = NULL;

	int ret = sqlite3_prepare_v2(self->db,
		statement,
		strlen(statement) + 1,
		&stmt,
		NULL);
	ret = SQLITE_OK;

	while (1) {
		ret = sqlite3_step(stmt);
		if (SQLITE_DONE == ret)
		{  // we reached the end of the table
			break;
		}
		else if (SQLITE_ROW == ret) {
			// process current row
			int count = sqlite3_column_count(stmt);  // get number of values in row
			garage_stats_entry entry;
			memset(&entry, 0, sizeof(garage_stats_entry));
			for (int i = 0; i < count; i++) {  // iterate values in row
				const char * colName = sqlite3_column_name(stmt, i);
				//const char * colType = sqlite3_column_decltype(*stmt, i);
				const unsigned char * value = sqlite3_column_text(stmt, i);
				//printf("%10s [%7s] = %s\n", colName, colType, value);
				if (!strcmp(colName, "Id"))
					entry.id = atoi((const char*)value);
				else if (!strcmp(colName, "Type"))
					strcpy(entry.type, (const char*)value);
				else if (!strcmp(colName, "Time"))
					entry.actionTime = atoi((const char*)value);
				else if (!strcmp(colName, "Called"))
					strcpy(entry.called, (const char*)value);
			}
			refs.push_back(entry);
		}
	}
}
void db_close(db_con_t * self)
{
	int rc = sqlite3_close(self->db);
	free(self);
}
