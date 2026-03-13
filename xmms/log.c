#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"

void xmms_log(const char *format, ...)
{
    static FILE *log_file = NULL;
    static int initialized = 0;
    
    if (!initialized)
    {
        /* Truncate the log file on fresh start */
        log_file = fopen("xmms-log.txt", "w");
        initialized = 1;
    }
    else if (!log_file)
    {
        log_file = fopen("xmms-log.txt", "a");
    }

    time_t now;
    time(&now);
    struct tm *tm_info = localtime(&now);
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", tm_info);

    va_list args;
    
    /* Print to stderr */
    fprintf(stderr, "[XMMS %s] ", time_str);
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");

    /* Print to file */
    if (log_file)
    {
        fprintf(log_file, "[%s] ", time_str);
        va_list args2;
        va_start(args2, format);
        vfprintf(log_file, format, args2);
        va_end(args2);
        fprintf(log_file, "\n");
        fflush(log_file);
    }
}
