/*\
 * test-con2.c
 *
 * Copyright (c) 2016 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * Experiments creating a windows app, ie /SUBSYSTEM:WINDOWS, and try to attach 
 * to any existing console, and maintain std io, stdout, stderr redirection...
 *
\*/

#include <stdio.h>
#include <Windows.h>
#include "sprtf.hxx"

static const char *module = "test-con2";

typedef struct tagDW2STG {
    DWORD val;
    char *desc;
}DW2STG, *PDW2STG;

DW2STG ft2stg[] = {
    { FILE_TYPE_CHAR, "is a character file, typically an LPT device or a console." },
    { FILE_TYPE_DISK, "is a disk file." },
    { FILE_TYPE_PIPE, "is a socket, a named pipe, or an anonymous pipe." },
    { FILE_TYPE_REMOTE, "Unused." },
    { FILE_TYPE_UNKNOWN, "Either the type of the specified file is unknown, or the function failed." },

    // LAST
    { 0, 0 }
};

char *GetFileTypeStg( DWORD dw )
{
    PDW2STG pdw = &ft2stg[0];
    while (pdw->desc) {
        if (pdw->val == dw )
            return pdw->desc;
        pdw++;
    }
    return "Unexpected type";
}

static void log_output( int n )
{
    fprintf(stdout,"%s:%d: Output to stdout\n", module, n);
    fflush(stdout);
    fprintf(stderr,"%s:%d: Output to stderr\n", module, n);
    fflush(stderr);
}


int main( int argc, char **argv )
{
    int iret = 0;
    int attached = 0;
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD type = GetFileType(hout);
    char *pft = GetFileTypeStg(type);
    char *nb = GetNxtBuf();
    DWORD err;
    char *pmsg;
    int len = sprintf(nb, "HOut is %s (type %d) - handle: %p (%d)", pft, type, hout, STD_OUTPUT_HANDLE);

    // ##################################
    log_output(1);    // 1
    // ##################################

    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        int dnfro = 0;
        // ##################################
        log_output(2);  // 2
        // ##################################
        if (type == FILE_TYPE_DISK) {
            freopen("conout$", "w", stdout);
            freopen("conout$", "w", stderr);
            dnfro = 1;
        }
        SPRTF("%s: AttachConsole succeeded... %s\n", module,
            (dnfro ? "Done fropen" : "not disk file"));
        attached = 1;
    } else {
        err = GetLastError();
        pmsg = (err == ERROR_ACCESS_DENIED) ? "ERROR_ACCESS_DENIED" :
            (err == ERROR_INVALID_HANDLE) ? "ERROR_INVALID_HANDLE" :
            (err == ERROR_GEN_FAILURE) ? "ERROR_GEN_FAILURE" :
            "UNEXPECTED_ERROR";

        SPRTF("%s: AttachConsole FAILED... %s (%d)\n", module, pmsg, err );
        iret = 1;
    }

    // do some logging...
    SPRTF("%s: %s\n", module, nb );
    // ##################################
    log_output(3); // 3
    // ##################################

    if (attached) {
        SPRTF("%s: Doing FreeConsole...\n", module );
        if (FreeConsole()) {
            attached = 0;
        }
        if (attached) {
            SPRTF("%s: FreeConsole FAILED...\n", module );
        } else {
            SPRTF("%s: Done FreeConsole...\n", module );
        }
    } else {
        SPRTF("%s: Not attached to a console...\n", module );
    }

    // ##################################
    log_output(4);  // 4
    // ##################################
    return iret;
}

// main() OS entry
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int iret;
    iret = main( __argc, __argv );
    return iret;
}

// eof = test-con2.c