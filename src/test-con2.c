/*\
 * test-con2.c
 *
 * Copyright (c) 2016 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
 * Experiments creating a windows app, ie /SUBSYSTEM:WINDOWS, and try to attach 
 * to any existing console, and maintain std io, stdout, stderr redirection...
 *
 * Reference *Daniel Tillet
 *https://www.tillett.info/2013/05/13/how-to-create-a-windows-program-hat-works-as-both-as-a-gui-and-console-application/comment-page-1/#comment-20904
 *
\*/

#include <stdio.h>
#include <Windows.h>
#include "sprtf.hxx"

static const char *module = "test-con2";

static int add_free_console = 1;
static int sleep_delay_secs = 0;

// add shorter, clearer message of 'type'
typedef struct tagDW2STG {
    DWORD val;
    char *type;
    char *desc;
}DW2STG, *PDW2STG;

DW2STG ft2stg[] = {
    { FILE_TYPE_CHAR, "FILE_TYPE_CHAR", "is a character file, typically an LPT device or a console." },
    { FILE_TYPE_DISK, "FILE_TYPE_DISK", "is a disk file." },
    { FILE_TYPE_PIPE, "FILE_TYPE_PIPE", "is a socket, a named pipe, or an anonymous pipe." },
    { FILE_TYPE_REMOTE, "FILE_TYPE_REMOTE", "Unused." },
    { FILE_TYPE_UNKNOWN, "FILE_TYPE_UNKNOWN", "Either the type of the specified file is unknown, or the function failed." },

    // LAST
    { 0, 0 }
};

char *GetFileTypeStg( DWORD dw )
{
    PDW2STG pdw = &ft2stg[0];
    while (pdw->desc) {
        if (pdw->val == dw )
            return pdw->type;   // pdw->desc;
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

void give_help( char *name )
{
    SPRTF("%s: usage: [options]\n", module);
    SPRTF("Options:\n");
    SPRTF(" --help  (-h or -?) = This help and exit(2)\n");
    SPRTF(" --no-free     (-n) = If 'attached' to a console, do not do 'FreeConsole'\n");
    SPRTF("\n");
    SPRTF("  This program is to ONLY test various API actions in this SUBSYSTEM:WINDOWS\n");
    SPRTF("  app. First how to keep redirection of stdout and/or stderr from being 'broken'.\n");
    SPRTF("  Next provide the missing channels, stdout or stderr, if run in a console.\n");
    SPRTF("  And finally, how to cause the prompt to be refreshed on certain exits.\n");
    SPRTF("\n");
    SPRTF("  The tests: Each is run **in** an existing console -\n");
    SPRTF("  1. test-con2 > temp1.txt 2>&1\n");
    SPRTF("  2. test-con2 > temp1.txt\n");
    SPRTF("  3. test-con2 2> temp1.txt\n");
    SPRTF("  4. test-con2\n");
    SPRTF("  It is expected the command prompt will be repainted on app exit.\n");
    SPRTF("\n");
    SPRTF("  Since this is about certain messages being seen, or being in the redirected\n");
    SPRTF("  file, many messages are also output to a file '%s'.\n", get_log_file());

}

int parse_args( int argc, char **argv )
{
    int i,i2,c;
    char *arg, *sarg;
    for (i = 1; i < argc; i++) {
        arg = argv[i];
        i2 = i + 1;
        if (*arg == '-') {
            sarg = &arg[1];
            while (*sarg == '-')
                sarg++;
            c = *sarg;
            switch (c) {
            case 'h':
            case '?':
                give_help(argv[0]);
                return 2;
                break;
            case 'n':
                add_free_console = 0;
                break;
            case 's':
                if (i2 < argc) {
                    i++;
                    sarg = argv[i];
                    sleep_delay_secs = atoi(sarg);
                } else {
                    SPRTF("%s: Error: Expected integer following '%s'! Aborting...\n", module, arg);
                    return 1;
                }
                break;

            // TODO: Other arguments
            default:
                SPRTF("%s: Unknown argument '%s'. Try -? for help...\n", module, arg);
                return 1;
            }
        } else {
            // bear argument
            SPRTF("%s: Unknown bear argument '%s'. Try -? for help...\n", module, arg);
            return 1;
        }
    }
    return 0;
}

// See Reference
static void sendEnterKey(void) 
{
    INPUT ip; 
    // Set up a generic keyboard event. 
    ip.type = INPUT_KEYBOARD; 
    ip.ki.wScan = 0; // hardware scan code for key 
    ip.ki.time = 0; 
    ip.ki.dwExtraInfo = 0;  
    // Send the "Enter" key 
    ip.ki.wVk = 0x0D; // virtual-key code for the "Enter" key 
    ip.ki.dwFlags = 0; // 0 for key press 
    SendInput(1, &ip, sizeof(INPUT));  
    // Release the "Enter" key 
    ip.ki.dwFlags = KEYEVENTF_KEYUP; // KEYEVENTF_KEYUP for key release 
    SendInput(1, &ip, sizeof(INPUT)); 
}

// main app entry
int main( int argc, char **argv )
{
    HWND parent;
    int dnfro1 = 0;
    int dnfro2 = 0;
    int iret = 0;
    int attached = 0;
    // get 'stdin' info
    HANDLE hin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD itype = GetFileType(hin);
    char *pint = GetFileTypeStg(itype);
    // get 'stdout' info
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD type = GetFileType(hout);
    char *pft = GetFileTypeStg(type);
    // get 'stderr' info
    HANDLE herr = GetStdHandle(STD_ERROR_HANDLE);
    DWORD etyp = GetFileType(herr);
    char *pfte = GetFileTypeStg(etyp);
    char *nb = GetNxtBuf();
    DWORD err;
    char *pmsg;
    int len = sprintf(nb, "HOut is %s (type %d) - handle: %p (%d)", pft, type, hout, STD_OUTPUT_HANDLE);
    len += sprintf(EndBuf(nb), "\nHErr is %s (type %d) - handle: %p (%d)", pfte, etyp, herr, STD_ERROR_HANDLE);
    len += sprintf(EndBuf(nb), "\nHIn  is %s (type %d) - handle: %p (%d)", pint, itype, hin, STD_INPUT_HANDLE);

    iret = parse_args( argc, argv );
    if (iret) {
        if (iret == 2)
            iret = 0;
        return iret;
    }

    // ##################################
    log_output(1);    // 1
    // ##################################

    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        // ##################################
        log_output(2);  // 2
        // ##################################

        // selectively use 'freopen', if not already REDIRECTED!
        if (type != FILE_TYPE_DISK) {
            freopen("conout$", "w", stdout);
            dnfro1 = 1;
        }
        if (etyp != FILE_TYPE_DISK) {
            freopen("conout$", "w", stderr);
            dnfro2 = 1;
        }
        SPRTF("\n%s: AttachConsole succeeded... stdout: %s, stderr: %s\n", module,
            (dnfro1 ? "Done fropen" : "is disk file"),
            (dnfro2 ? "Done fropen" : "is disk file"));
        attached = 1;
        parent = GetConsoleWindow(); // preserve the handle to window
    } else {
        // obviously **NOT** run in a 'console'
        err = GetLastError();
        pmsg = (err == ERROR_ACCESS_DENIED) ? "ERROR_ACCESS_DENIED" :
            (err == ERROR_INVALID_HANDLE) ? "ERROR_INVALID_HANDLE" :
            (err == ERROR_GEN_FAILURE) ? "ERROR_GEN_FAILURE" :
            "UNEXPECTED_ERROR";

        SPRTF("\n%s: AttachConsole FAILED... %s (%d)\n", module, pmsg, err );
        iret = 1;
    }

    // do some logging...
    SPRTF("Info:\n%s\n", nb );
    // ##################################
    log_output(3); // 3
    // ##################################

    if (attached && add_free_console) {
        SPRTF("%s: Doing FreeConsole() call...\n", module);
        if (!FreeConsole()) {
            SPRTF("%s: FreeConsole FAILED...\n", module );
        }
        else {
            SPRTF("%s: Done FreeConsole...\n", module ); 
        }
    } else {
        if (add_free_console) {
            SPRTF("%s: Not attached to a console...\n", module );
        } else {
            SPRTF("%s: As requested, did not do FreeConsole...\n", module );
        }
    }

    // ##################################
    log_output(4);  // 4
    // ##################################

    if (sleep_delay_secs) {
        int sav = add_sys_time(1);
        SPRTF("%s: Sleeping for %d secs...\n", module, sleep_delay_secs);
        Sleep( sleep_delay_secs * 1000 );
        SPRTF("%s: Back after %d secs sleep...\n", module, sleep_delay_secs);
        add_sys_time(sav);
    }
    // Check to see if we were attached & console is foreground, then simulate 'Enter' key strike to re-paint prompt
    if (attached && (parent == GetForegroundWindow())) {
        if ((dnfro1 == 1) || (dnfro2 == 1)) {
            sendEnterKey();
            SPRTF("%s: Sending Enter key...\n", module);
        }
    }
    return iret;
}

// OS entry, when SUBSYSTEM:WINDOWS is used
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int iret;
    iret = main( __argc, __argv );
    return iret;
}

// eof = test-con2.c
