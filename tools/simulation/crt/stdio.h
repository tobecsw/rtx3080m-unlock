#pragma once
#include <stddef.h>
#include <stdarg.h>
typedef struct {char *_ptr;int _cnt;char *_base;int _flag;int _file;int _charbuf;int _bufsiz;char *_tmpfname;} FILE;
__declspec(dllimport) FILE *__iob_func(void);
#define stdin (&__iob_func()[0])
#define stdout (&__iob_func()[1])
#define stderr (&__iob_func()[2])
#define EOF (-1)
#define SEEK_SET 0
#define SEEK_END 2
#define _IONBF 4
__declspec(dllimport) int printf(const char *,...);
__declspec(dllimport) int fprintf(FILE *,const char *,...);
__declspec(dllimport) int puts(const char *);
__declspec(dllimport) int _snprintf(char *,size_t,const char *,...);
#define snprintf _snprintf
__declspec(dllimport) int vsnprintf(char *,size_t,const char *,va_list);
__declspec(dllimport) FILE *fopen(const char *,const char *);
__declspec(dllimport) int fclose(FILE *);
__declspec(dllimport) int fseek(FILE *,long,int);
__declspec(dllimport) long ftell(FILE *);
__declspec(dllimport) size_t fread(void *,size_t,size_t,FILE *);
__declspec(dllimport) int fgetc(FILE *);
__declspec(dllimport) int setvbuf(FILE *,char *,int,size_t);
