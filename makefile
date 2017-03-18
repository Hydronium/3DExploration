#20170317 created

#To avoid issues with "target architecture is not defined", you need /Ze which will define it based on what /T is
#/Ze = enables microsoft extensions to C, required on windows computers
TimeSinkNew : TimeSinkNew.c glcorearb.h fnld.h
	pocc /Ze /Tx64-coff TimeSinkNew.c
	polink /VERBOSE TimeSinkNew.obj /LIBPATH:C:\Windows\System32 /DEFAULTLIB:opengl32.dll #kernel32.lib advapi32.lib delayimp64.lib