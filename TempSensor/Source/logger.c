#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOG_FILE_NAME "error.log"
#define LOG_SESSION_START 2000
#define LOG_SESSION_END 2001

void Log(int code, const char *msg)
{
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  char timestr[20];
  strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", t);
  printf("%s [%04d] %s\n", timestr, code, msg);
  FILE *logf = fopen(LOG_FILE_NAME, "a");
  if (!logf)
    return;
  fprintf(logf, "%s [%04d] %s\n", timestr, code, msg);
  fclose(logf);
}

void LogSessionStart()
{
  Log(LOG_SESSION_START, "Session started");
}

void LogSessionEnd()
{
  Log(LOG_SESSION_END, "Session ended");
}