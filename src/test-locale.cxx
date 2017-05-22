/*\
 * test-locale.cxx
 *
 * Copyright (c) 2017 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/

#include <stdio.h>
#include <string.h> // for strdup(), ...
#include <Windows.h>
#include <string>
#include <vector>
#include <iostream> // for cout, ...
// other includes

typedef std::vector<std::string> string_list;

#define SG_LOG(C,P,M)   std::cout << M << std:endl

static const char *module = "test-locale";

static const char *usr_input = 0;

void give_help( char *name )
{
    printf("%s: usage: [options] usr_input\n", module);
    printf("Options:\n");
    printf(" --help  (-h or -?) = This help and exit(0)\n");
    // TODO: More help
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
            // TODO: Other arguments
            default:
                printf("%s: Unknown argument '%s'. Try -? for help...\n", module, arg);
                return 1;
            }
        } else {
            // bear argument
            if (usr_input) {
                printf("%s: Already have input '%s'! What is this '%s'?\n", module, usr_input, arg );
                return 1;
            }
            usr_input = strdup(arg);
        }
    }
    //if (!usr_input) {
    //    printf("%s: No user input found in command!\n", module);
    //    return 1;
    //}
    return 0;
}

#ifdef _WIN32
/**
* Determine locale/language settings on Windows.
*
* Copyright (C) 1997, 2002, 2003 Martin von Loewis
*
* Permission to use, copy, modify, and distribute this software and its
* documentation for any purpose and without fee is hereby granted,
* provided that the above copyright notice appear in all copies.
*
* This software comes with no warranty. Use at your own risk.
*/

string_list
FGLocale_getUserLanguage()
{
    string_list result;
    static char locale[100] = { 0 };

    if (GetLocaleInfo(LOCALE_USER_DEFAULT,
        LOCALE_SISO639LANGNAME,
        locale, sizeof(locale)))
    {
        // SG_LOG(SG_GENERAL, SG_DEBUG, "Detected locale's language setting: " << locale);
        printf("Detected locale's language setting: %s\n", locale);
        size_t i = strlen(locale);
        locale[i++] = '_';
        if (GetLocaleInfo(LOCALE_USER_DEFAULT,
            LOCALE_SISO3166CTRYNAME,
            locale + i, (int)(sizeof(locale) - i)))
        {
            result.push_back(locale);
            return result;
        }

        locale[--i] = 0;
        //SG_LOG(SG_GENERAL, SG_WARN, "Failed to detected locale's country setting.");
        printf("Failed to detected locale's country setting.\n");
        result.push_back(locale);
        return result;
    }

    return result;
}

#else
/**
* Determine locale/language settings on Linux/Unix.
*/
string_list
FGLocale_getUserLanguage()
{
    string_list result;
    const char* langEnv = ::getenv("LANG");
    if (langEnv) {
        result.push_back(langEnv);
    }

    return result;
}


#endif

// main() OS entry
int main( int argc, char **argv )
{
    int iret = 0;
    iret = parse_args(argc,argv);
    if (iret) {
        if (iret == 2)
            iret = 0;
        return iret;
    }

    string_list sl = FGLocale_getUserLanguage(); // actions of app
    size_t ii, max = sl.size();
    printf("Got %d languages...\n", (int)max);
    for (ii = 0; ii < max; ii++) {
        printf("%d: %s\n", (int)(ii + 1), sl[ii].c_str());
    }
    return iret;
}


// eof = test-locale.cxx
