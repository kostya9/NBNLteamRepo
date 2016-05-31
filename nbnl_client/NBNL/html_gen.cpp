#include "html_gen.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#define HTML_BUFF_LEN 8192
void html_gen(char * fileName, vector<garage_stats_entry> entries)
{
	FILE * f = fopen(fileName, "w+");
	char html_buf[HTML_BUFF_LEN] = "";
	char table_buff[HTML_BUFF_LEN] = "";
	strcat(table_buff, "<tr><th>ID</th><th>Type</th><th>ActionTime</th><th>Called</th></tr>");
	int count = entries.size();
	for (int i = 0; i < count; i++)
	{
		char entry_buf[HTML_BUFF_LEN];
		char time_str[1000];
		time_to_string(entries[i].actionTime, time_str);
		sprintf(entry_buf, "<tr><td>%i</td><td>%s</td><td>%s</td><td>%s</td></tr>", entries[i].id, entries[i].type, time_str, entries[i].called);
		strcat(table_buff, entry_buf);
	}
	sprintf(html_buf, "<html><title>GARAGE STATS</title><body><table>%s</table></body></html>", table_buff);
	fwrite(html_buf, sizeof(char), strlen(html_buf), f);
	fclose(f);
}

void html_file_open(char * fileName)
{
	char path[1000];
	wchar_t NPath[1000];
	DWORD a = GetCurrentDirectory(MAX_PATH, NPath);
	wcscat(NPath, L"\\");
	wchar_t text_w[100];
	mbstowcs(text_w, fileName, strlen(fileName) + 1);//Plus null
	LPWSTR ptr = text_w;
	wcscat(NPath, ptr);
	LPWSTR open = L"open";
	ShellExecute(NULL, open, NPath, NULL, NULL, SW_SHOWNORMAL);
}
