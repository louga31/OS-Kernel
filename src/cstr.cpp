#include "cstr.h"

char to_StringOutput[128]; // Malloc this when possible
const char* to_string(uint64_t value) {
    uint8_t size;
    uint64_t sizeTest = value;
    while (sizeTest / 10 > 0)
    {
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0)
    {
        uint8_t remainder = value % 10;
        value /= 10;
        to_StringOutput[size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    to_StringOutput[size - index] = remainder + '0';
    to_StringOutput[size + 1] = '\0';
    return to_StringOutput;
}

const char* to_string(int64_t value) {
    uint8_t isNegative = 0;

    if (value < 0)
    {
        isNegative = 1;
        value *= -1;
        to_StringOutput[0] = '-';
    }
    
    
    uint8_t size;
    uint64_t sizeTest = value;
    while (sizeTest / 10 > 0)
    {
        sizeTest /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0)
    {
        uint8_t remainder = value % 10;
        value /= 10;
        to_StringOutput[isNegative + size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    to_StringOutput[isNegative + size - index] = remainder + '0';
    to_StringOutput[isNegative + size + 1] = '\0';
    return to_StringOutput;
}

const char* to_hstring(uint64_t value) {
    uint64_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 8 * 2 - 1 + 2;
    for (uint8_t i = 0; i < size / 2; i++)
    {
        ptr = ((uint8_t*) valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        to_StringOutput[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 'A' - 10 : '0');
        tmp = (*ptr & 0x0F);
        to_StringOutput[size - (i * 2)] = tmp + (tmp > 9 ? 'A' - 10 : '0');
    }
    to_StringOutput[0] = '0';
    to_StringOutput[1] = 'x';
    to_StringOutput[size + 1] = '\0';
    return to_StringOutput;
}

const char* to_hstring(uint32_t value) {
    uint32_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 4 * 2 - 1 + 2;
    for (uint8_t i = 0; i < size / 2; i++)
    {
        ptr = ((uint8_t*) valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        to_StringOutput[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 'A' - 10 : '0');
        tmp = (*ptr & 0x0F);
        to_StringOutput[size - (i * 2)] = tmp + (tmp > 9 ? 'A' - 10 : '0');
    }
    to_StringOutput[0] = '0';
    to_StringOutput[1] = 'x';
    to_StringOutput[size + 1] = '\0';
    return to_StringOutput;
}

const char* to_hstring(uint16_t value) {
    uint16_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 2 * 2 - 1 + 2;
    for (uint8_t i = 0; i < size / 2; i++)
    {
        ptr = ((uint8_t*) valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        to_StringOutput[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 'A' - 10 : '0');
        tmp = (*ptr & 0x0F);
        to_StringOutput[size - (i * 2)] = tmp + (tmp > 9 ? 'A' - 10 : '0');
    }
    to_StringOutput[0] = '0';
    to_StringOutput[1] = 'x';
    to_StringOutput[size + 1] = '\0';
    return to_StringOutput;
}

const char* to_hstring(uint8_t value) {
    uint8_t* valPtr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 1 * 2 - 1 + 2;
    for (uint8_t i = 0; i < size / 2; i++)
    {
        ptr = ((uint8_t*) valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        to_StringOutput[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 'A' - 10 : '0');
        tmp = (*ptr & 0x0F);
        to_StringOutput[size - (i * 2)] = tmp + (tmp > 9 ? 'A' - 10 : '0');
    }
    to_StringOutput[0] = '0';
    to_StringOutput[1] = 'x';
    to_StringOutput[size + 1] = '\0';
    return to_StringOutput;
}

char doubleto_StringOutput[128]; // Malloc this when possible
const char* to_string(double value, uint8_t digits) {
    if (digits > 20)
        digits = 20;
    

    char* intPtr = (char*)to_string((int64_t)value);
    char* doublePtr = doubleto_StringOutput;

    if (value < 0)
    {
        value *= -1;
    }
    
    while (*intPtr != 0)
    {
        *doublePtr = *intPtr;
        intPtr++;
        doublePtr++;
    }
    
    *doublePtr = '.';
    doublePtr++;

    double newValue = value - (int)value;

    for (uint8_t i = 0; i < digits; i++)
    {
        newValue *= 10;
        *doublePtr = (int)newValue + '0';
        newValue -= (int)newValue;
        doublePtr++;
    }

    *doublePtr = '\0';
    return doubleto_StringOutput;
}

char* itoa(int64_t value, char* result, int base) {
    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int64_t tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}