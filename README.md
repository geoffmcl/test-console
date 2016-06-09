# test-console

Just some tests, and experiments, on standard io, stdout and stderr, in a SUBSYSTEM:WINDOWS app... ie has WinMain() as OS entry point, rather than main() for a console app.

The idea if to find WIN32 kludge code to still be able to attach to the console it is run in, if any, and then only if redirection is taking place...

I have called it a mess, on a mess, on a mess... maybe it is worse than that!

There are two apps -

 1. test-con1 - a standard windows console app, where such io redirection works fine, perfectly...
 2. test-con2 - a standard windows windows app, where redirection is **broken**!
 
That is what more **mess** needs be added to test-con2 to make it work somewhat similar to test-con1, at least as far as stdout and stderr are concerned...

There is also a small library, sprtf, to log output to a file and stdout...

For sure, still a WIP!

The building is cmake, so just change diectory to build, and/or build.x64, and maybe the `build-me.bat` there in each will work... but you can use the standard cmake mantra 

````
 $ cd build
 $ cmake ..
 $ cmake --build . -config Release
````

Have FUN!

Geoff.  
20160609

; eof
