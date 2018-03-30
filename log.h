#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include <stdio.h>

#define log(MSG) printf(MSG "\n")
#define logf(MSG, ARGS...) printf(MSG "\n", ARGS)

#endif // LOG_H_INCLUDED
