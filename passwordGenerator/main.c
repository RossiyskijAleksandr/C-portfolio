#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct setting {
    size_t length;
    int includeSymbols;
    int includeNumbers;
    int includeLowerCase;
    int includeUpperCase;
    int toBuffer;
    int autoSaveSettings;
}* Settings;

int readString();
void printMenu();
void generatePassword(Settings, char*);
void printSettings(Settings);
void saveSettings(Settings);
Settings readSettingsFromFile(Settings);
char* changeSettings(Settings, char*); 
int mygetline(char*, size_t);

int main() {
    int result;
    Settings setting = calloc(1, sizeof(*setting));
    if(!setting) {
        fprintf(stderr, "Cannot allocate memory %s, %d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    srand(time(NULL));
    readSettingsFromFile(setting);
    char* password = calloc(setting->length + 1, sizeof(char));
    if(!password) {
        fprintf(stderr, "Cannot allocate memory %s, %d\n", __FILE__, __LINE__);
        free(setting);
        exit(EXIT_FAILURE);
    }
    while(1) {
        printMenu();
        printf("Enter an option: ");
        int ch;
        while((ch = getchar()) != '\n' && ch != EOF)
            result = ch;
        if(result == '5')
            break;
        switch(result) {
            case '1':
                generatePassword(setting, password);
                printf("Password: %s\n", password);
                break;
            case '2':
                printSettings(setting);
                break;
            case '3':
                password = changeSettings(setting, password);
                break;
            case '4':
                saveSettings(setting);
                break;
            default:
                printf("Unknow option");
        }
    }
    free(password);
    free(setting);
    return 0;
}

void printMenu() {
    printf("\t******************************\n");
    printf("\t1. Generate Password\n");
    printf("\t2. Check Settings\n");
    printf("\t3. Change Settings\n");
    printf("\t4. Save Settings\n");
    printf("\t5. Exit\n");
}

void generatePassword(Settings setting, char* password) {
    char* numbers = "0123456789";
    char* lowerAlphabet = "abcdefghijklmnopqrstuvwxyz";
    char* upperAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char* symbols = "!@#$%%^&*()_-+={[}]|\\:;\"'<,>.?";
    size_t i = 0;
    for(; i < setting->length;) {
        int decision = rand() / ((RAND_MAX + 1u) / 4);
        if(decision == 0 && setting->includeNumbers)
            password[i++] = numbers[rand() / ((RAND_MAX + 1u) / strlen(numbers))];
        else if(decision == 1 && setting->includeLowerCase)
            password[i++] = lowerAlphabet[rand() / ((RAND_MAX + 1u) / strlen(lowerAlphabet))];
        else if(decision == 2 && setting->includeUpperCase)
            password[i++] = upperAlphabet[rand() / ((RAND_MAX + 1u) / strlen(upperAlphabet))];
        else if(decision == 3 && setting->includeSymbols)
            password[i++] = symbols[rand() / ((RAND_MAX + 1u) / strlen(symbols))];
    }
    password[i] = '\0';
}

void printSettings(Settings setting) {
    printf("Length of password: %zu\n", setting->length);
    printf("   Include Symbols: %s\n", setting->includeSymbols ? "true" : "false");
    printf("   Include Numbers: %s\n", setting->includeNumbers ? "true"  : "false");
    printf("Include Lower Case: %s\n", setting->includeLowerCase ? "true"  : "false");
    printf("Include Upper Case: %s\n", setting->includeUpperCase ? "true"  : "false");
    printf(" Save to clipboard: %s\n", setting->toBuffer ? "true"  : "false");
    printf("Auto Save Settings: %s\n", setting->autoSaveSettings ? "true"  : "false");
}

char* changeSettings(Settings setting, char* password) {
    size_t length = setting->length;
    const size_t BUFFERSIZE = 16;
    char* buffer = malloc(sizeof(char) * BUFFERSIZE + 1);
    char* tmp;
    fputs("Enter new length of password: ", stdout);
    mygetline(buffer, BUFFERSIZE);
    setting->length = strtoul(buffer, &tmp, 10);

    fputs("Include Symbols? (1 for yes, 0 for no): ", stdout);
    mygetline(buffer, BUFFERSIZE);
    setting->includeSymbols = atoi(buffer);

    fputs("Include Numbers? (1 for yes, 0 for no): ", stdout);
    mygetline(buffer, BUFFERSIZE);
    setting->includeNumbers = atoi(buffer);
    
    fputs("Include Lower Case? (1 for yes, 0 for no): ", stdout);
    mygetline(buffer, BUFFERSIZE);
    setting->includeLowerCase = atoi(buffer);

    fputs("Include Upper Case? (1 for yes, 0 for no): ", stdout);
    mygetline(buffer, BUFFERSIZE);
    setting->includeUpperCase = atoi(buffer);

    fputs(" Save to clipboard? (1 for yes, 0 for no): ", stdout);
    mygetline(buffer, BUFFERSIZE);
    setting->toBuffer = atoi(buffer);

    fputs("Auto Save Settings? (1 for yes, 0 for no): ", stdout);
    mygetline(buffer, BUFFERSIZE);
    setting->autoSaveSettings = atoi(buffer);

    if(setting->autoSaveSettings)
        saveSettings(setting);

    char* ptr;
    if(length != setting->length) {
        ptr = realloc(password, sizeof(char) * setting->length + 1);
        if(!ptr) {
            free(password);
            free(setting);
            fprintf(stderr, "Cannot allocate memory %s, %d\n", __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
        ptr[setting->length] = '\0';
    }
    free(buffer);
    return ptr;
}

void saveSettings(Settings setting) {
    FILE* f = fopen("settings.csv", "w");
    if(!f) {
        fprintf(stderr, "Cannot open file: %s", "settings.csv");
        return;
    }
    fprintf(f, "%zu,%d,%d,%d,%d,%d,%d", setting->length,
    setting->includeSymbols, setting->includeNumbers, setting->includeLowerCase,
    setting->includeUpperCase, setting->toBuffer, setting->autoSaveSettings);
    fclose(f);
}

Settings readSettingsFromFile(Settings setting) {
    FILE* f = fopen("settings.csv", "r");
    if(!f) {
        fprintf(stderr, "Cannot open file, create a default settings' file: %s\n", "settings.csv");
        saveSettings(setting);
        return setting;
    }

    fscanf(f, "%zu,%d,%d,%d,%d,%d,%d", &setting->length,
    &setting->includeSymbols, &setting->includeNumbers, &setting->includeLowerCase,
    &setting->includeUpperCase, &setting->toBuffer, &setting->autoSaveSettings);
    fclose(f);
    return setting;
}

int mygetline(char *buf, size_t size) {
    assert(size > 0 && size <= INT_MAX);
    size_t i = 0;
    int ch;
    while ((ch = getchar()) != EOF && ch != '\n' && i < size)
            buf[i++] = ch;
    buf[i] = '\0';
    if (i == 0) { 
        return EOF;
    }
    return i;
}