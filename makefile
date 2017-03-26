#20170317 created

INCLUDE = $(PellesCDir)\Include\Win;$(PellesCDir)\Include
LIB = $(PellesCDir)\Lib\Win64;$(PellesCDir)\Lib

#Looked up LoadIcon on MSDN and saw that it was part of User32.lib, so added it in here
#wgl functions supposedly exist in opengl32...which is loadewd with LoadLibrary..
#LINKFLAGS = gdi32.lib User32.lib kernel32.lib advapi32.lib delayimp64.lib /VERBOSE 
LINKFLAGS = gdi32.lib User32.lib kernel32.lib advapi32.lib delayimp64.lib /VERBOSE

TimeSinkNew.exe : TimeSinkNew.obj 
	polink.exe $(LINKFLAGS) $**

#To avoid issues with "target architecture is not defined", you need /Ze which will define it based on what /T is
#/Ze = enables microsoft extensions to C, required on windows computers
TimeSinkNew.obj : TimeSinkNew.c glcorearb.h fnld.h
	pocc.exe /std:C11 /Ze /Tx64-coff TimeSinkNew.c