#include "func.h"

int main(int argc, char **argv) {
    
    signal(SIGINT, signal_handler); 
    loop();

    return EXIT_SUCCESS;
}
