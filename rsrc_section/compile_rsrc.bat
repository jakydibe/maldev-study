@ECHO OFF

rc resources.rc
cvtres /MACHINE:x64 /OUT:resources.o resources.res
cl.exe /nologo /Ox /MT /W0 /GS- /DNDEBUG /Tceasy_dropper_rsrc.cpp /link /OUT:easy_dropper_rsrc.exe /SUBSYSTEM:CONSOLE /MACHINE:x64 resources.o