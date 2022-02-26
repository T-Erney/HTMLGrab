#include "htmlgrab.h"
#include <stdio.h>

int main(int argc, char** argv) {

    if (argc < 2) {
        printf("htmlgrab: error: no url provided");
        return -1;
    }
    char* html = htmlgrab(argv[1]);
    printf("%s\n", html);

    return 0;
}
