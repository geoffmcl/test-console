# test-console

Just some tests, and experiments, on standard io, stdout and stderr, in a SUBSYSTEM:WINDOWS app... ie has WinMain() as OS entry point, rather than main() for a console app.

The idea if to find WIN32 kludge code to still be able to attach to the console it is run in, if any, and deal with any redirection taking place... and do something if not...

I have called it a mess, on a mess, on a mess... maybe it is worse than that!

There are three apps -

 1. test-con1 - a standard windows console app, where such io redirection works fine, perfectly...
 2. test-con2 - a standard windows windows app, where redirection is **broken**!
 3. test-locale - console app to show active language

That is what more **mess** needs be added to test-con2 to make it work somewhat similar to test-con1, at least as far as stdout and stderr are concerned...

There is also a small library, sprtf, to log output to a file and stdout...

So this comes down to a question as to whether any **redirection** is taking place, when the app is started.

In a previous app, FixF32, in C:\GTools\ConApps\FixF32\Fixf32.c, to detect redirection, used -

````
 HANDLE ghStdOut = GetStdHandle( STD_OUTPUT_HANDLE );   // Standard out
   if( VFH(ghStdOut) ) {
      if( !GetConsoleMode( ghStdOut, &gdwMode ) )
         gbRedON = TRUE;
   }
````

In test-con2 here, now using, thanks to Jasin -

````
    HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD type = GetFileType(hout);
    if (type == FILE_TYPE_DISK)
        gbRedON = TRUE;
    // And similarly with STD_ERROR_HANDLE
````

And maybe there are other ways to detet this from within the app code...

And when a specific valid io channel has not been established, `freopen` can be used to restore it, like -

````
    if (type != FILE_TYPE_DISK)
        freopen("conout$", "w", stdout);
    if (etyp != FILE_TYPE_DISK)
        freopen("conout$", "w", stderr);
````

So, in general, it seems it is possible to add kludge code to a SUBSYSTEM:windows program, and make it run somewhat like the much simpler SUBSYSTEM:console. Is it worth the additional `WIN32` kludge code to get this to happen? I guess that has to be up to the specific core project developers. The only known `saving` is that a console window is **not** always established, maybe a very important consideration for some.

This exploration was mainly when run in a console. When run from windows explorer, or a desktop icon, such an app could have more kludge code **added** to actually `allocate` a console... if need be...

For sure, still a WIP!

The building is cmake, so just change diectory to build, and/or build.x64, and maybe the `build-me.bat` there in each will work... but you can use the standard cmake mantra 

````
 $ cd build
 $ cmake ..
 $ cmake --build . -config Release
````

Have FUN!

Geoff.  
20170522 - 20160611 - 20160609

; eof
