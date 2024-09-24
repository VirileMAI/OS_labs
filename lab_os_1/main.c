#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main() {
    char* pass = "AnDrEy10";
    printf("%ld\n", strlen(pass));
    for (int i = 0; i < strlen(pass); i++) {
        // printf("%d\n", i);
        printf("%c", pass[i]);
        if (pass[i] == ' ' || (!isalpha(pass[i]) && !isdigit(pass[i]))) {
            return -1; // Здесь вы выходите, если символ не буква или пробел
        }
    }
    return 0; // Успешный выход
}
