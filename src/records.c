#include <genesis.h>
#include "states.h"
#include "records.h"

#define RECORDS_TITLE "BEST PLAYERS"

static void padding(char*, char, int, bool);
static void load_records(t_record*);
static void save_records(t_record*);

void records_create()
{
    t_record records[NUM_RECORDS] = {0};
    
    load_records(records);
    VDP_drawText(RECORDS_TITLE, 20 - strlen(RECORDS_TITLE)/2 , 2);

    for (int position = 1; position <= NUM_RECORDS; position++) 
    {
        char name[RECORD_NAME_SIZE+1] = {0};
        char score[7] = {0};
        char pos[4] = {0};

        if (records[position-1].score)
        {
            sprintf(score, "%lu", records[position-1].score);
            memcpy(name, records[position-1].name, RECORD_NAME_SIZE);
        }

        padding(name, '.', RECORD_NAME_SIZE, FALSE);
        padding(score, '.', 6, TRUE);

        sprintf(pos, "%02d.", position);

        VDP_drawText(pos,   9,  4 + 2*position);
        VDP_drawText(name,  13,  4 + 2*position);
        VDP_drawText(score, 24, 4 + 2*position);
    }
}

void records_joystick(u16 joy, u16 changed, u16 state)
{
    if (state & BUTTON_C)
    {
        change_state(STATE_MENU);
    }
}

void load_records(t_record* records)
{
    int addr = RECORD_BASE_ADDR;
    u16 check = 0; 

    SRAM_enableRO();
    check = SRAM_readWord(addr);
    addr += sizeof(u16);
    if (check == RECORD_SIGNATURE)
    {
        for (int idx = 0; idx < NUM_RECORDS; idx++)
        {
            for (int char_idx = 0; char_idx < RECORD_NAME_SIZE; char_idx++)
            {
                records[idx].name[char_idx] = SRAM_readByte(addr);
                addr += sizeof(char);
            }
            records[idx].score = SRAM_readLong(addr);
            addr += sizeof(long);
        }
    }
    SRAM_disable();
}

void insert_record(char* name, long score)
{
    t_record record = {0};
    t_record records[NUM_RECORDS] = {0};
    t_record new_records[NUM_RECORDS] = {0};

    record.score = score;
    strcpy(record.name, name);

    load_records(records);

    memcpy(new_records, records, sizeof(records));
    for (int position = 0; position < NUM_RECORDS; position++)
    {
        if (records[position].score < record.score)
        {
            memcpy(&new_records[position + 1], &records[position], sizeof(t_record) * (NUM_RECORDS - (position + 1)));
            memcpy(new_records[position].name, record.name, RECORD_NAME_SIZE);
            new_records[position].score = record.score;
            save_records(new_records);
            break;
        }
    }
}

bool is_record(long score)
{
    t_record records[NUM_RECORDS] = {0};
    
    load_records(records);
    return score > 0 && records[NUM_RECORDS-1].score < score;
}

void save_records(t_record* records)
{
    int addr = RECORD_BASE_ADDR;
    SRAM_enable();
    SRAM_writeWord(addr, RECORD_SIGNATURE);
    addr += sizeof(u16);
    
    for (int idx = 0; idx < NUM_RECORDS; idx++)
    {
        for (int char_idx = 0; char_idx < RECORD_NAME_SIZE; char_idx++)
        {
            SRAM_writeByte(addr, records[idx].name[char_idx]);
            addr += sizeof(char);
        }
        SRAM_writeLong(addr, records[idx].score);
        addr += sizeof(long);
    }
    SRAM_disable();
}

void padding(char *str, char padding_char, int total_width, bool is_left) {
    int length = strlen(str);
    int count = total_width - length;

    if (count <= 0) return;

    if (is_left) {
        char temp[strlen(str) + 1];
        strcpy(temp, str);
        memset(str, padding_char, count);
        memcpy(str + count, temp, length + 1);
    } else {
        memset(str + length, padding_char, count);
        str[total_width] = '\0';
    }
}