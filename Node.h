#pragma once

#include <windows.h>

#define NAME_NOCHINA 0X0001
#define NAME_SPECIFY 0X0080
#define NAME_MAXSIZE 0X0200
#define SUFF_MAXSIZE 0X0200
#define NODE_MINSIZE 0X0008

typedef struct _NODE {
    UINT    next;
    BYTE    mark;
    BYTE    cite;
    USHORT  length;
    char    str[1];

    bool equal(char* str, USHORT length, BYTE mark)
    {
        if (length != this->length || mark != this->mark)
        {
            return false;
        }
        char* beg = this->str, * end = beg + length;
        while (beg != end)
        {
            if (*beg++ != *str++)
            {
                return false;
            }
        }
        return true;
    }
}NODE, * PNODE;