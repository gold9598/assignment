//-----------------------------------------------------------
// 
// SWE2007: Software Experiment II (Fall 2017)
//
// Skeleton code for PA #3
// October 11, 2017
//
// Jong-Won Park
// Embedded Software Laboratory
// Sungkyunkwan University
//
//-----------------------------------------------------------

#include "string_sw.h"

char *strcpy (char *dst, const char *src)
{
	char *t = dst;
	while(*src)
		*t++ = *src++;
	*t = '\0';
	return dst;
}

char *strcat (char *dst, const char *src)
{
	char *t = dst-1;
	while(*++t);
    strcpy(t, src);
	return dst;
}

size_t strlen (const char *str)
{
	size_t r = -1;
	while(str[++r]);
	return r;
}

int strcmp(const char *lhs, const char *rhs)
{
	do {
		if (*lhs < *rhs)
			return -1;
		else if (*lhs > *rhs)
			return 1;
	} while (*lhs++ && *rhs++);
	return 0;
}

char *strrchr (const char *str, int ch)
{
	char *s = (char *) --str;
	while(*++str);
	do {
		if (*str == ch)
			return (char *) str;
	} while (s < --str);
	
	return NULL;
}
