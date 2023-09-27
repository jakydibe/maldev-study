@ECHO OFF

cl.exe /nologo /Ox /MT /W0 /GS- /DNDEBUG /Tceasy_dropper.cpp /link /OUT:easy_dropper.exe /SUBSYSTEM:CONSOLE /MACHINE:x64