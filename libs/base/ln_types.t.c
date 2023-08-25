#include <ln_types.h>
#include <stdio.h>
#include <stdlib.h>

#define SHOW_VARIABLE(V) printf("%s=%d\n", #V, V)

int main(int argc, char** argv)
{

    SHOW_VARIABLE(TARGET_OS);

    return 0;
}