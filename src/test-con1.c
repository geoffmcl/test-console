/*\
 * test-con1.c
 *
 * Copyright (c) 2016 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include "sprtf.hxx"

static const char *module = "test-con1";

static void log_output( int n )
{
    fprintf(stdout,"%s:%d: Output to stdout\n", module, n);
    fprintf(stderr,"%s:%d: Output to stderr\n", module, n);
}


// main() OS entry
int main( int argc, char **argv )
{
    int iret = 0;
    SPRTF("%s: standard console application...\n", module );
    log_output( 1 );

    return iret;
}


// eof = test-con1.c
