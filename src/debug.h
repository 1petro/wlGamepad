#include <stdio.h>

#define RESET   "\x1B[0m"
#define RED     "\x1B[31m"
#define GREEN   "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE    "\x1B[34m"

#define DEBUG_LEVEL 4

#define WLGP_PRINT(level, format, ...) \
    do { \
        if (level <= DEBUG_LEVEL) { \
            switch(level) { \
		case 0: \
                    fprintf(stderr, RED "[FATAL_ERROR]: " format RESET, ##__VA_ARGS__); \
                    break; \
                case 1: \
                    fprintf(stderr, GREEN "[INFO]: " format RESET, ##__VA_ARGS__); \
                    break; \
                case 2: \
                    fprintf(stderr, YELLOW "[WARNING]: " format RESET, ##__VA_ARGS__); \
                    break; \
                case 3: \
                    fprintf(stderr, RED "[ERROR]: " format RESET, ##__VA_ARGS__); \
                    break; \
		case 4: \
                    fprintf(stderr, BLUE "[DEBUG]: " format RESET, ##__VA_ARGS__); \
                    break; \
                default: \
                    fprintf(stderr, "UNKNOWN: " format, ##__VA_ARGS__); \
            } \
        } \
    } while(0)
