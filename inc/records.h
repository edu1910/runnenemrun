#ifndef _RECORDS_H_
#define _RECORDS_H_

#include <genesis.h>

#define NUM_RECORDS 10
#define RECORD_NAME_SIZE 11
#define RECORD_BASE_ADDR 0
#define RECORD_SIGNATURE 0xCAFE

typedef struct
{
    char name[RECORD_NAME_SIZE];
    long score;
} t_record;

void insert_record(char*, long);
bool is_record(long);

#endif // _RECORDS_H_
