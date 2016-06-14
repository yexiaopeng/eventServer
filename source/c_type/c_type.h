//
// Created by whyxp on 16-6-14.
//

#ifndef EVENTSERVER_C_TYPE_H
#define EVENTSERVER_C_TYPE_H


#include <stdint.h>




typedef signed char     int8_t;
typedef short int      int16_t;
typedef int            int32_t;

#define INT8_T  int8_t
#define INT16_T int16_t
#define INT16    int16_t
#define INT32_T int32_t




# if __WORDSIZE == 64
typedef long int                int64_t;
#define INT64_T                 int64_t
# else
__extension__
typedef long long int           int64_t;
#define INT64_T                 int64_t
# endif
#endif

/* Unsigned.  */
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;

#define UINT8_T                 uint8_t
#define UINT16_T                uint16_t
#define UINT16                   uint16_t

#ifndef __uint32_t_defined
typedef unsigned int            uint32_t;
#define UINT32_T                 uint32_t
# define __uint32_t_defined
#endif
#if __WORDSIZE == 64
typedef unsigned long int       uint64_t;
#define UINT64_T                 uint64_t

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;
#else
__extension__
typedef unsigned long long int  uint64_t;

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;


#endif //EVENTSERVER_C_TYPE_H
