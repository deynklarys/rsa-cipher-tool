#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "../bignum.h"

// Includes the appropriate header file to use operating system-specific functions. This is useful for functions like clearing the screen, moving the cursor, and getting the terminal size.
#ifdef _WIN32
	#include <windows.h>
#else
	#include <sys/ioctl.h>
	#include <unistd.h>
#endif

// Function prototypes
	// Function to get the terminal size. This function will set the width and height variables to the width and height of the terminal, respectively. The function will use operating system-specific functions to get the terminal size.
	void getTerminalSize();
	// Function to clear the screen. This function will clear the screen using operating system-specific functions.
	void clearScreen();
	// Function to sleep the program. This function will pause the program for a specified number of milliseconds using operating system-specific functions.
	void sleepProgram(int milliseconds);
	// Function to move the cursor. This function will move the cursor to the specified x and y coordinates using operating system-specific functions.
	void moveCursor(int x, int y);
	// Function to clear lines. This function will clear the specified lines starting from the startLine to the endLine using the specified width.
	void clearLines(int startLine, int endLine, int width);
	// Function to display a loading bar.
	void loadingBar(int progress);
	// Function to wait for the user to input "DONE" to continue.
	void waitForDone();
	// Function to get the user's confirmation. This function will get the user's confirmation by asking the user to input 'Y' or 'N' and return the input.
	char getConfirm();
	// Function to display a message with the specified width and height and return the number of lines displayed.
	int displayMessage(char *message[], int count);
	// Function to display the About message. Separate function was made because it starts at a different y position.
	void displayAbout(char *message[], int count, int offsestY);
	// Function to generate the public and private keys for the RSA algorithm.
	void generateKeys(int i);
	// Function to encrypt the text using the RSA algorithm.
	void encryptText();
	// Function to decrypt the text using the RSA algorithm.
	void decryptText();
	// Function to display information about the program.
	void aboutProgram();

// Global variables
char confirm;
int offsetY, width, height, adjustedHeight;

char fileName[100];
FILE *encryptionFile;
FILE *decryptionFile;
const int PROGRESS_BAR_LENGTH = 50;

int main() {
	clearScreen();

	getTerminalSize();
	printf("Width: %d\nHeight: %d\n", width, height);

	int i, userInput;

	do {
		char *optionsArr[5] = {"1) Generate Keys", "2) Encrypt Text", "3) Decrypt Text", "4) About", "5) Exit program"};
		for ( i = 0; i < 5; i++) {
			moveCursor((width - 14) / 2, adjustedHeight + i);
			printf("%s\n", optionsArr[i]);
		}

		moveCursor((width - 10)/ 2, adjustedHeight + i);
		printf("Enter number: ");
		scanf("%d", &userInput);
		// Consume any extra characters in the input buffer to prevent an infinite loop
		while (getchar() != '\n');

		switch (userInput) {
			case 1:
				generateKeys(i);
				break;
			case 2:
				encryptText();
				break;
			case 3:
				decryptText();
				break;
			case 4:
				aboutProgram();
				break;
			case 5:
				clearScreen();
				moveCursor((width - 20) / 2, adjustedHeight + 5);
				printf("Exiting program...\n");
				sleepProgram(500);
				break;
			default:
				clearScreen();
				moveCursor((width - 15) / 2, adjustedHeight + 1);
				printf("Invalid input.");
				moveCursor((width - 39) / 2, adjustedHeight + 2);
				printf("Please enter a number between 1 and 5.");
				sleepProgram(1000);
				clearScreen();
				break;
		}
	} while (userInput != 5);

	return 0;
}

void getTerminalSize() {
	#ifdef _WIN32
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	#else
		struct winsize size;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
		width = size.ws_col;
		height = size.ws_row;
	#endif
		// Since the horizontal positioning of the outputs can be adjusted by incrementing the y-axis, this integer variable stores value of the height divided by 3 as all the outputs starts to be displayed at this value. This can't be applied to the x-axis as the outputs are displayed at the center of the screen. The vertical positioning is depedent on the length of the string to be displayed.
	  adjustedHeight = height / 3;
}

void clearScreen() {
	#ifdef _WIN32
		system("cls");
	#else
		printf("\033[2J");
		printf("\033[H");
	#endif
}

void sleepProgram(int milliseconds) {
	#ifdef _WIN32
		Sleep(milliseconds);
	#else
		usleep(milliseconds * 1000);
	#endif
}

void moveCursor(int x, int y) {
	#ifdef _WIN32
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD pos = {x, y};
		SetConsoleCursorPosition(hConsole, pos);
	#else
		printf("\033[%d;%dH", y, x);
	#endif
}

void clearLines(int startLine, int endLine, int width) {
	for (int i = startLine; i <= endLine; i++) {
		moveCursor(0, i);
		for (int j = 0; j < width; j++) {
			printf(" ");
		}
	}
}

void loadingBar(int percentDone) { 
	int numChar = percentDone * PROGRESS_BAR_LENGTH / 100;
	int numSpace = PROGRESS_BAR_LENGTH - numChar;
  int start = (width - PROGRESS_BAR_LENGTH) / 2;

	char a = 177, b = 219;

  moveCursor(start, (height * 3) / 2);
	printf("[");
	for (int i = 0; i < numChar; i++) {
		printf("%c", a);
	}
	for (int i = 0; i < numSpace; i++) {
		printf("%c", b);
	}
	printf("] %d%% Done", percentDone);
	fflush(stdout);
}

void waitForDone() {
	char done[100];
	do {
		clearLines(height - 2, height - 2, width);
		moveCursor((width - 21)/ 2, height - 2);
		printf("Enter DONE to back: ");
		// Get the user's input from the standard input stream	
		fgets(done, sizeof(done), stdin); 

		// Remove the newline character at the end of the input
		if (done[strlen(done) - 1] == '\n') {
			done[strlen(done) - 1] = '\0';
		}

		// If the input was just a newline character, continue with the next iteration
		if (strlen(done) == 0) {
			continue;
		}

		// Convert the user's input to lowercase. This allows the user to enter "done" or "DONE" to exit the loop in any case.
		for(int i = 0; done[i]; i++){
			done[i] = tolower(done[i]);
		}

		moveCursor((width - 21)/ 2, height - 2);

	} while (strcmp(done, "done") != 0);
}

char getConfirm(int width, int adjustedHeight, int offsetY) {
	char confirm;
	confirm = getchar();
	if (confirm == '\n') {
		confirm = getchar();
	}
	if (confirm != 'Y' && confirm != 'y' && confirm != 'N' && confirm != 'n') {
		moveCursor((width - 37) / 2, adjustedHeight + offsetY);
		printf("Invalid input. Please enter Y or N.");
		moveCursor((width - 37) / 2, adjustedHeight + offsetY);
	}

	// Consume any extra characters in the input buffer
	while (getchar() != '\n');
	
	return confirm;
}

int displayMessage(char *message[], int count) {
	int i;
	for ( i = 0; i < count; i++) {
		moveCursor((width - strlen(message[i])) / 2, adjustedHeight + i);
		printf("%s\n", message[i]);
	}

	return i;
}

void displayAbout(char *message[], int count, int offsetY) {
	for (int i = 0; i < count; i++) {
		moveCursor((width - strlen(message[i])) / 2, height / 4 + i + offsetY);
		printf("%s\n", message[i]);
	}
}

void generateKeys(int i) {
	clearScreen();
	char *msgGenerate[] = {"You are about to generate an RSA key", "this option. This will display a private key", "and public key of the generated private key."};
	int countGenerate = sizeof(msgGenerate) / sizeof(msgGenerate[0]);
	offsetY = displayMessage(msgGenerate, countGenerate);

	do{
		clearLines(adjustedHeight + offsetY, adjustedHeight + offsetY + 1, width);
		moveCursor((width - 37)/ 2, adjustedHeight + offsetY);
		printf("Do you agree to save a copy of your\n");
		moveCursor((width - 32)/ 2, adjustedHeight + offsetY + 1);
		printf("private and public keys? [Y/N] ");
		confirm = getConfirm(width, adjustedHeight, offsetY + 1);

	} while (confirm != 'Y' && confirm != 'y' && confirm != 'N' && confirm != 'n');

	clearScreen();	
	if (confirm == 'Y' || confirm == 'y') {

		Bignum keys;
		initBignum(&keys);
		unsigned long long int primeLength;

		moveCursor((width - 30)/ 2, adjustedHeight);
		printf("Enter the length of the prime");
		moveCursor((width - 30)/ 2, adjustedHeight + 1);
		printf("number to be generated: ");
		scanf("%llu", &primeLength);

		clearScreen();
		clock_t start = clock();
		
		// At this point, function calls can be made to generate the keys. For now, the program will display a message that the keys are generated.

		generatePrimeBignum(&keys, primeLength);

		clock_t end = clock();
		double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;


		printf("\n");

		clearScreen();
		
		moveCursor((width - 22)/ 2, adjustedHeight);
		printf("Your prime number is:");
		moveCursor((width - primeLength)/ 2, adjustedHeight + 1);
		printBignum(&keys);

		moveCursor((width - 36)/ 2, adjustedHeight + 2);
		printf("Function executed in: %.2f seconds\n", cpu_time_used);

		moveCursor((width - 30)/ 2, adjustedHeight * 3 - 2);
		printf("Keys generated successfully!\n");

	} else {
		moveCursor((width - 25)/ 2, adjustedHeight);
		printf("Keys generation failed!\n");

	}

	waitForDone();
	freeAllBignums();
	clearScreen();
}

void encryptText() {
	char* publicKEY = malloc(1000000000 * sizeof(char));
		
	clearScreen();
	char* msgEncrypt[] = {"Encryption includes the message to be encrypted and", "the public key of the recipient. The txt file of the", "message must be in the same folder of the C program."};
	int countEncrypt = sizeof(msgEncrypt) / sizeof(msgEncrypt[0]);
	offsetY = displayMessage(msgEncrypt, countEncrypt);

	do {
		clearLines(adjustedHeight + offsetY, adjustedHeight + offsetY, width);
		moveCursor((width - 60)/ 2, adjustedHeight + offsetY);
		printf("Is the txt file in the same folder of the C program? [Y/N] ");
		confirm = getConfirm(width, adjustedHeight, offsetY + 1);

	} while (confirm != 'Y' && confirm != 'y' && confirm != 'N' && confirm != 'n');

	if (confirm == 'Y' || confirm == 'y') {
		do {
			clearLines(adjustedHeight + offsetY + 1, adjustedHeight + offsetY + 2, width);
			moveCursor((width - 47)/ 2, adjustedHeight + offsetY + 1);
			printf("Do you have the recipient's public key? [Y/N] ");
			confirm = getConfirm(width, adjustedHeight, offsetY + 2);

		} while (confirm != 'Y' && confirm != 'y' && confirm != 'N' && confirm != 'n');

		if (confirm == 'Y' || confirm == 'y') {
			clearScreen();

			do {
				clearLines(adjustedHeight, adjustedHeight, width);
				moveCursor((width - 38)/ 2, adjustedHeight);
				printf("Enter file name: ");
				fgets(fileName, sizeof(fileName), stdin);

				// Remove the newline character at the end of the filename
				if (fileName[strlen(fileName) - 1] == '\n') {
					fileName[strlen(fileName) - 1] = '\0';
				}
		
				// Try to open the file for reading and appending. If the file does not exist, it will be created.
				encryptionFile = fopen(fileName, "a+");
		
				if (encryptionFile == NULL) {
					// If the file could not be opened, print an error message
					moveCursor((width - 38)/ 2, adjustedHeight + offsetY + 3);
					printf("Could not open file. Please try again.\n");
				}
			} while (encryptionFile == NULL);

			if (publicKEY == NULL) {
					printf("Failed to allocate memory for publicKEY\n");
					exit(1);
			}
			moveCursor((width - 38)/ 2, adjustedHeight + 1);
			printf("Enter public key: ");
			scanf("%s", publicKEY);

			clearScreen();

			clock_t start = clock();

			// At this point, function calls can be made to ENCRYPT the MESSAGE. For now, the program will display a message that the MESSAGE IS ENCRYPTED.

			fprintf(encryptionFile, "Encrypted message here\n");
			fprintf(encryptionFile, "%s", publicKEY);
			printf("Public key: %s\n", publicKEY);
			
			clock_t end = clock();
			double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

			moveCursor((width - 30)/ 2, adjustedHeight);
			printf("Message encryption success!\n");
			moveCursor((width - 30)/ 2, adjustedHeight + 1);
			printf("Function executed in: %f seconds\n", cpu_time_used);
			moveCursor((width - 30)/ 2, adjustedHeight + 2);
			printf("Encrypted ____ characters in __ seconds\n");

			fclose(encryptionFile);
		} else {
			clearScreen();
			moveCursor((width - 25)/ 2, adjustedHeight);
			printf("Message encryption failed!\n");
		}
	}
	else {
			clearScreen();
			moveCursor((width - 25)/ 2, adjustedHeight);
			printf("Message encryption failed!\n");
	}

	waitForDone();
	clearScreen();

	free(publicKEY);
}

void decryptText() {
	char* privateKEY = malloc(1000000000 * sizeof(char));

	clearScreen();
	char* msgDecrypt[] = {"Decryption includes the message to be decrypted and", "the private key of the recipient. The txt file of the", "message must be in the same folder of the C program."};
	int countDecrypt = sizeof(msgDecrypt) / sizeof(msgDecrypt[0]);
	offsetY = displayMessage(msgDecrypt, countDecrypt);

	do {
		clearLines(adjustedHeight + offsetY, adjustedHeight + offsetY, width);
		moveCursor((width - 60)/ 2, adjustedHeight + offsetY);
		printf("Is the txt file in the same folder of the C program? [Y/N]");
		confirm = getConfirm(width, adjustedHeight, offsetY);

	} while (confirm != 'Y' && confirm != 'y' && confirm != 'N' && confirm != 'n');

	if (confirm == 'Y' || confirm == 'y') {
		do {
			clearLines(adjustedHeight + offsetY + 1, adjustedHeight + offsetY + 2, width);
			moveCursor((width - 38)/ 2, adjustedHeight + offsetY + 1);
			printf("Do you have your private key? [Y/N] ");
			confirm = getConfirm(width, adjustedHeight, offsetY + 1);

		} while (confirm != 'Y' && confirm != 'y' && confirm != 'N' && confirm != 'n');

		if (confirm == 'Y' || confirm == 'y') {
			clearScreen();

			do {
				clearLines(adjustedHeight, adjustedHeight, width);
				moveCursor((width - 38)/ 2, adjustedHeight);
				printf("Enter file name: ");
				fgets(fileName, sizeof(fileName), stdin);

				// Remove the newline character at the end of the filename
				if (fileName[strlen(fileName) - 1] == '\n') {
					fileName[strlen(fileName) - 1] = '\0';
				}
		
				// Try to open the file for reading and appending. If the file does not exist, it will be created.
				decryptionFile = fopen(fileName, "a+");
		
				if (decryptionFile == NULL) {
					// If the file could not be opened, print an error message
					moveCursor((width - 38)/ 2, adjustedHeight + offsetY + 3);
					printf("Could not open file. Please try again.\n");
				}
			} while (decryptionFile == NULL);

			if (privateKEY == NULL) {
					printf("Failed to allocate memory for privateKEY\n");
					exit(1);
			}
			moveCursor((width - 38)/ 2, adjustedHeight + 1);
			printf("Enter private key: ");
			scanf("%s", privateKEY);

			clearScreen();

			clock_t start = clock();

			// At this point, function calls can be made to DECRYPT the MESSAGE. For now, the program will display a message that the MESSAGE IS DECRYPTED.

			fprintf(decryptionFile, "Decrypted message here\n");
			fprintf(decryptionFile, "%s", privateKEY);
			printf("Private key: %s\n", privateKEY);
			
			clock_t end = clock();
			double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

			moveCursor((width - 30)/ 2, adjustedHeight);
			printf("Message encryption success!\n");
			moveCursor((width - 30)/ 2, adjustedHeight + 1);
			printf("Function executed in: %f seconds\n", cpu_time_used);
			moveCursor((width - 30)/ 2, adjustedHeight + 2);
			printf("Decrypted ____ characters in __ seconds\n");

			fclose(decryptionFile);
		} else {
			clearScreen();
			moveCursor((width - 25)/ 2, adjustedHeight);
			printf("Message encryption failed!\n");
		}
	}
	else {
			clearScreen();
			moveCursor((width - 25)/ 2, adjustedHeight);
			printf("Message encryption failed!\n");
	}

	waitForDone();
	clearScreen();

	free(privateKEY);
}

void aboutProgram() {
	clearScreen();
	moveCursor((width - 45) / 2, height / 4);
	printf("RSA Cipher Tool with Custom Bignum Library");

	char *about[] = {"The RSA Cipher Tool program generates, encrypts, and decrypts messages using the RSA", "algorithm. The program uses the RSA (Rivest, Shamir, Adleman) Algorithm to generate public", "and private keys needed to encrypt and decrypt messages and a custom bignum library to", "handle large numbers numbers for the RSA encryption and decryption processes."};
	int aboutCount = sizeof(about) / sizeof(about[0]);
	displayAbout(about, aboutCount, 2);

	char* about2[] = {"The user will use the program to encrypt a message in a file, where it will then return the", "encrypted/secret message. Then, they can now send it to the recipient of the message safely,", "where they can also use the same program to decrypt the message and reveal the actual message."};
	int about2Count = sizeof(about2) / sizeof(about2[0]);
	displayAbout(about2, about2Count, 7);

	char* about3[] = {"This is a programming project for Computer Programming 2 during the Academic Year", "2023-2024 at the Bicol University College of Science. The program is developed by the", "following students of Bicol University College of Science:"};
	int about3Count = sizeof(about3) / sizeof(about3[0]);
	displayAbout(about3, about3Count, 11);

	char* members[] = { "Michael Xavier Canonizado", "Deanne Clarice Bea", "Simon Narvaez", "Marc Jordan Campopos"};
	int membersCount = sizeof(members) / sizeof(members[0]);
	displayAbout(members, membersCount, 14);

	waitForDone();
	clearScreen();
}
