#include <stdio.h>
#include <string.h>
#include "src/bignum.h"

void generateKeys(Bignum *ePublic, Bignum *dPrivate, Bignum *nPublic);
void encryptMessage(FILE *inputFilePtr, FILE *outputFilePtr, Bignum *ePublic, Bignum *nPublic);
void decryptMessage(FILE *inputFilePtr, FILE *outputFilePtr, Bignum *dPublic, Bignum *nPublic);

void generateKeys(Bignum *ePublic, Bignum *dPrivate, Bignum *nPublic) {
    Bignum one;
    initBignum(&one);
    setBignum(&one, "1", positive);

    Bignum pPrimePrivate, qPrimePrivate;
    initBignum(&pPrimePrivate);
    initBignum(&qPrimePrivate);

    Bignum phiOfNPrivate, pPrimePrivateMinusOne, qPrimePrivateMinusOne;
    initBignum(&phiOfNPrivate);
    initBignum(&pPrimePrivateMinusOne);
    initBignum(&qPrimePrivateMinusOne);

    // Generate p and q primes
    setBignum(&pPrimePrivate, "11", positive);
    setBignum(&qPrimePrivate, "13", positive);

    // Get n:
    // n = p * q
    multiplyBignum(nPublic, &pPrimePrivate, &qPrimePrivate);

    // Get phi of n:
    // phi of n = (p - 1) * (q - 1)
    subtractBignum(&pPrimePrivateMinusOne, &pPrimePrivate, &one);
    subtractBignum(&qPrimePrivateMinusOne, &qPrimePrivate, &one);
    multiplyBignum(&phiOfNPrivate, &pPrimePrivateMinusOne, &qPrimePrivateMinusOne);

    // Generate e (public key):
    // 2 < e < phi of n
    setBignum(ePublic, "7",positive);
    
    // Get d (private key):
    // (e * d)mod(n) = 1
    modularInverseBignum(dPrivate, ePublic, &phiOfNPrivate);

    printf("\np: ");
    printBignum(&pPrimePrivate);
    printf("\nq: ");
    printBignum(&qPrimePrivate);
    printf("\nnPublic: ");
    printBignum(nPublic);
    printf("\nphiOfN: ");
    printBignum(&phiOfNPrivate);
    printf("\ne: ");
    printBignum(ePublic);
    printf("\nd: ");
    printBignum(dPrivate);

    freeBignum(&one);
    freeBignum(&pPrimePrivate);
    freeBignum(&qPrimePrivate);
    freeBignum(&phiOfNPrivate);
    freeBignum(&pPrimePrivateMinusOne);
    freeBignum(&qPrimePrivateMinusOne);
}

int main(void) {
    Bignum nPublic, ePublic, dPrivate;
    initBignum(&nPublic);
    initBignum(&ePublic);
    initBignum(&dPrivate);

    generateKeys(&ePublic, &dPrivate, &nPublic);

    FILE *inputFilePtr, *outputFilePtr;
    char inputFilename[] = "plaintext.txt";
    char outputFilename[] = "encrypted.txt";
    
    inputFilePtr = fopen(inputFilename, "r");
    outputFilePtr = fopen(outputFilename, "w");

    if (inputFilePtr == NULL) {
        printf("Error opening input %s...\n", inputFilename);
        return 1;
    }
    if (outputFilePtr == NULL) {
        printf("Error opening output %s...\n", outputFilename);
        return 2;
    }

    printf("\n\n");
    encryptMessage(inputFilePtr, outputFilePtr, &ePublic, &nPublic);
    
    fclose(inputFilePtr);
    fclose(outputFilePtr);

    freeAllBignums();

    return 0;
}


void encryptMessage(FILE *inputFilePtr, FILE *outputFilePtr, Bignum *ePublic, Bignum *nPublic) {
    char character;

    Bignum encryptedChar, plainChar;
    initBignum(&encryptedChar);
    initBignum(&plainChar);

    while ((character = fgetc(inputFilePtr)) != EOF) {
        intToBignum(&plainChar, character, positive);

        modularExponentiationBignum(&encryptedChar, &plainChar, ePublic, nPublic);

        printBignum(&encryptedChar);
        printf(",");

        for (unsigned long long int i = encryptedChar.length - 1; i > 0; i--) {
            fprintf(outputFilePtr, "%d", encryptedChar.digits[i]);
        }
        fprintf(outputFilePtr, "%d/", encryptedChar.digits[0]);

        resetBignum(&encryptedChar);
        resetBignum(&plainChar);
    };

    freeBignum(&encryptedChar);
    freeBignum(&plainChar);
}

void decryptMessage(FILE *inputFilePtr, FILE *outputFilePtr, Bignum *dPublic, Bignum *nPublic) {
    Bignum decryptedChar, encryptedTextChar;
    initBignum(&decryptedChar);
    initBignum(&encryptedTextChar);

    // intToBignum(&encryptedTextChar, encryptedText[i], positive);

    // modularExponentiationBignum(&decryptedChar, &encryptedTextChar, dPublic, nPublic);

    // printBignum(&decryptedChar);
    // printf(",");

    resetBignum(&decryptedChar);
    resetBignum(&encryptedTextChar);

    freeBignum(&decryptedChar);
    freeBignum(&encryptedTextChar);
}