#include "func.h"

int main() {
    signal(SIGINT, signal_handler); 
    loop();
    return EXIT_SUCCESS;
}
