#ifndef BIGNUM_H
#define BIGNUM_H

#include <limits.h>

#define MAX_INT_LENGTH 10000
#define MAX_VALUE_OF_LONG_LONG_INT LLONG_MAX

typedef enum {
    positive = 0,
    negative = 1
} BIGNUM_SIGN;

typedef struct {
    // Integer will be in reverse for easier arithmetic
    int digits[MAX_INT_LENGTH];
    // Number of digits in integer
    int length;
    // Sign will be 0 (positive) or 1 (negative), following the binary sign bit convention.
    BIGNUM_SIGN sign;
} Bignum;

int getLengthOfInteger(long long int num);
Bignum initBignum();
void setBignum(Bignum *numStruct, char numStr[], BIGNUM_SIGN sign);
void intToBignum(Bignum *numStruct, unsigned long long int integer, BIGNUM_SIGN sign);
long long int bignumToInt(Bignum *num);

void copyBignum(Bignum *result, Bignum *num);
void printBignum(Bignum *num);
void printBignumCenter(Bignum *num, unsigned int requiredWidth);
void trimBignum(Bignum *num);
int isGreaterThanBignum(Bignum *num1, Bignum *num2);
int isLessThanBignum(Bignum *num1, Bignum *num2);
int isEqualToBignum(Bignum *num1, Bignum *num2);
int isBignumZero(Bignum *num);

void addBignum(Bignum *result, Bignum *num1, Bignum *num2);
void subtractBignum(Bignum *result, Bignum *num1, Bignum *num2);

#endif /* BIGNUM_H */