#### Civilization 4 SDK Makefile 1.0 ####
####  Copyright 2010 Danny Daemonic  ####
#########################################

#### Paths ####
TOOLKIT=C:\Program Files (x86)\Microsoft Visual C++ Toolkit 2003
PSDK=C:\Program Files\Microsoft SDKs\Windows\v7.1
## Uncomment to have newly compiled dlls copied to your mod's Assets directory
YOURMOD=D:\Civ4 Reimagined\Civ4 Reimagined

#### Tools ####
CC="$(TOOLKIT)\bin\cl.exe"
CPP="$(TOOLKIT)\bin\cl.exe"
LD="$(TOOLKIT)\bin\link.exe"
RC="$(PSDK)\bin\rc.exe"
## Uncomment to build dependencies using fastdep
FD="fastdep.exe"

#### BLACKLIST ####
## Uncomment to block CvTextScreen (accidentally included by Firaxis)
BLACKLIST=CvTextScreens

#### You shouldn't need to modify anything beyond this point ####
#################################################################

#### Target Files ####
Debug_BIN=Debug\CvGameCoreDLL.dll
Release_BIN=Release\CvGameCoreDLL.dll
Profile_BIN=Profile\CvGameCoreDLL.dll

!IF [IF NOT EXIST CvGameCoreDLL.rc EXIT 1] == 0
Debug_RESOURCE=Debug\CvGameCoreDLL.res
Release_RESOURCE=Release\CvGameCoreDLL.res
Profile_RESOURCE=Profile\CvGameCoreDLL.res
!ENDIF

Debug_STATICLIB=Debug\CvGameCoreDLL.lib
Release_STATICLIB=Release\CvGameCoreDLL.lib
Profile_STATICLIB=Profile\CvGameCoreDLL.lib

Debug_LIBDEF=Debug\CvGameCoreDLL.def
Release_LIBDEF=Release\CvGameCoreDLL.def
Profile_LIBDEF=Profile\CvGameCoreDLL.def

Debug_PCH=Debug\CvGameCoreDLL.pch
Release_PCH=Release\CvGameCoreDLL.pch
Profile_PCH=Profile\CvGameCoreDLL.pch

Debug_PDB=Debug\CvGameCoreDLL.pdb
Release_PDB=Release\CvGameCoreDLL.pdb
Profile_PDB=Profile\CvGameCoreDLL.pdb

Debug_OTHER=Debug\CvGameCoreDLL.exp Debug\CvGameCoreDLL.ilk
Release_OTHER=Release\CvGameCoreDLL.exp
Profile_OTHER=Profile\CvGameCoreDLL.exp

#### CFLAGS ####
GLOBAL_CFLAGS=/GR /Gy /W3 /EHsc /Gd /Gm- /DWIN32 /D_WINDOWS /D_USRDLL /DLOG_AI /DCVGAMECOREDLL_EXPORTS /Yu"CvGameCoreDLL.h"
Debug_CFLAGS=/MD /Zi /Od /D_DEBUG /RTC1 /Fp"$(Debug_PCH)" $(GLOBAL_CFLAGS)
Release_CFLAGS=/MD /O2 /Oy /Oi /Og /G7 /DNDEBUG /DFINAL_RELEASE /Fp"$(Release_PCH)" $(GLOBAL_CFLAGS)
Profile_CFLAGS=/MD /O2 /Oy /Oi /Og /G7 /DNDEBUG /DFP_PROFILE_ENABLE /DUSE_INTERNAL_PROFILER /Fp"$(Profile_PCH)" $(GLOBAL_CFLAGS)

#### LDFLAGS ####
GLOBAL_LDFLAGS=/DLL /NOLOGO /SUBSYSTEM:WINDOWS /LARGEADDRESSAWARE /TLBID:1
Debug_LDFLAGS=/INCREMENTAL /DEBUG /PDB:"$(Debug_PDB)" /IMPLIB:"$(Debug_STATICLIB)" $(GLOBAL_LDFLAGS)
Release_LDFLAGS=/INCREMENTAL:NO /OPT:REF /OPT:ICF /PDB:"$(Release_PDB)" $(GLOBAL_LDFLAGS)
Profile_LDFLAGS=/INCREMENTAL:NO /OPT:REF /OPT:ICF /PDB:"$(Profile_PDB)" $(GLOBAL_LDFLAGS)

#### INCLUDES ####
GLOBAL_INCS=/I"$(TOOLKIT)/include" /I"$(PSDK)/Include" /I"$(PSDK)/Include/mfc"
PROJECT_INCS=/IBoost-1.32.0/include /IPython24/include
Debug_INCS=$(PROJECT_INCS) $(GLOBAL_INCS)
Release_INCS=$(PROJECT_INCS) $(GLOBAL_INCS)
Profile_INCS=$(PROJECT_INCS) $(GLOBAL_INCS)

#### LIBS ####
GLOBAL_LIBS=/LIBPATH:"$(TOOLKIT)/lib" /LIBPATH:"$(PSDK)/Lib" winmm.lib user32.lib
PROJECT_LIBS=/LIBPATH:Python24/libs /LIBPATH:boost-1.32.0/libs/ boost_python-vc71-mt-1_32.lib
Debug_LIBS=$(PROJECT_LIBS) $(GLOBAL_LIBS) msvcprt.lib
Release_LIBS=$(PROJECT_LIBS) $(GLOBAL_LIBS)
Profile_LIBS=$(PROJECT_LIBS) $(GLOBAL_LIBS)

#### Objects ####
Debug_LINKOBJS=$(Debug_OBJS)
Release_LINKOBJS=$(Release_OBJS)
Profile_LINKOBJS=$(Profile_OBJS)

#### Auto SOURCES/OBJS ####
!IF [ECHO SOURCES= \> sources.mk] == 0 && \
    [FOR %i IN (*.cpp) DO @ECHO. "%i" \>> sources.mk] == 0 && \
    [ECHO.>> sources.mk] == 0 && \
    [ECHO Debug_OBJS= \>> sources.mk] == 0 && \
    [FOR /F "delims=." %i IN ('dir /b *.cpp') DO @ECHO. Debug\%i.obj \>> sources.mk] == 0 && \
    [ECHO.>> sources.mk] == 0 && \
    [ECHO Release_OBJS= \>> sources.mk] == 0 && \
    [FOR /F "delims=." %i IN ('dir /b *.cpp') DO @ECHO. Release\%i.obj \>> sources.mk] == 0 && \
    [ECHO.>> sources.mk] == 0 && \
    [ECHO Profile_OBJS= \>> sources.mk] == 0 && \
    [FOR /F "delims=." %i IN ('dir /b *.cpp') DO @ECHO. Profile\%i.obj \>> sources.mk] == 0 && \
    [ECHO.>> sources.mk] == 0
!INCLUDE sources.mk
!IF [DEL sources.mk]
!ENDIF
!ENDIF

#### Targets ####
#################

.PHONY: all clean Debug_clean Release_clean Profile_clean Debug Release Profile

all: Debug Release

clean: Debug_clean Release_clean Profile_clean

Debug_clean:
	@FOR %i IN ($(Debug_BIN) $(Debug_STATICLIB) $(Debug_LIBDEF) \
		Debug\*.obj Debug\*.@ $(Debug_RESOURCE) \
		$(Debug_PCH) $(Debug_PDB) $(Debug_OTHER)) DO @IF EXIST "%i" DEL "%i"

Release_clean:
	@FOR %i IN ($(Release_BIN) $(Release_STATICLIB) $(Release_LIBDEF) \
		Release\*.obj Release\*.@ $(Release_RESOURCE) \
		$(Release_PCH) $(Release_PDB) $(Release_OTHER)) DO @IF EXIST "%i" DEL "%i"

Profile_clean:
	@FOR %i IN ($(Profile_BIN) $(Profile_STATICLIB) $(Profile_LIBDEF) \
		Profile\*.obj Profile\*.@ $(Profile_RESOURCE) \
		$(Profile_PCH) $(Profile_PDB) $(Profile_OTHER)) DO @IF EXIST "%i" DEL "%i"

Debug: Debug_DIR Debug_unfinished $(Debug_PCH) $(Debug_BIN)
!IFDEF YOURMOD
	-COPY "$(Debug_BIN)" "$(YOURMOD)\Assets\."
!ENDIF

Release: Release_DIR Release_unfinished $(Release_PCH) $(Release_BIN)
!IFDEF YOURMOD
	-COPY "$(Release_BIN)" "$(YOURMOD)\Assets\."
!ENDIF

Profile: Profile_DIR Profile_unfinished $(Profile_PCH) $(Profile_BIN)
!IFDEF YOURMOD
	-COPY "$(Profile_BIN)" "$(YOURMOD)\Assets\."
!ENDIF

Debug_DIR:
	-@IF NOT EXIST "Debug\." MKDIR "Debug"

Release_DIR:
	-@IF NOT EXIST "Release\." MKDIR "Release"

Profile_DIR:
	-@IF NOT EXIST "Profile\." MKDIR "Profile"

Debug_unfinished:
	@ECHO.>Debug\unfinished.@
	@FOR /F "delims=@" %i IN ('dir /b Debug\*.@') DO \
		@IF EXIST "Debug\%i" DEL "Debug\%i"
	@FOR /F %i IN ('dir /b Debug\*.@') DO \
		@IF EXIST "Debug\%i" DEL "Debug\%i"

Release_unfinished:
	@ECHO.>Release\unfinished.@
	@FOR /F "delims=@" %i IN ('dir /b Release\*.@') DO \
		@IF EXIST "Release\%i" DEL "Release\%i"
	@FOR /F %i IN ('dir /b Release\*.@') DO \
		@IF EXIST "Release\%i" DEL "Release\%i"

Profile_unfinished:
	@ECHO.>Profile\unfinished.@
	@FOR /F "delims=@" %i IN ('dir /b Profile\*.@') DO \
		@IF EXIST "Profile\%i" DEL "Profile\%i"
	@FOR /F %i IN ('dir /b Profile\*.@') DO \
		@IF EXIST "Profile\%i" DEL "Profile\%i"
		
$(Debug_BIN): $(Debug_LINKOBJS) $(Debug_RESOURCE)
	$(LD) /out:$(Debug_BIN) $(Debug_LDFLAGS) $(Debug_LIBS) $(Debug_LINKOBJS) $(Debug_RESOURCE)

$(Release_BIN): $(Release_LINKOBJS) $(Release_RESOURCE)
	$(LD) /out:$(Release_BIN) $(Release_LDFLAGS) $(Release_LIBS) $(Release_LINKOBJS) $(Release_RESOURCE)

$(Profile_BIN): $(Profile_LINKOBJS) $(Profile_RESOURCE)
	$(LD) /out:$(Profile_BIN) $(Profile_LDFLAGS) $(Profile_LIBS) $(Profile_LINKOBJS) $(Profile_RESOURCE)

.cpp{Debug}.obj:
	@ECHO.>"$*.obj.@"
    $(CPP) /nologo $(Debug_CFLAGS) $(Debug_INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

.cpp{Release}.obj:
	@ECHO.>"$*.obj.@"
    $(CPP) /nologo $(Release_CFLAGS) $(Release_INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

.cpp{Profile}.obj:
	@ECHO.>"$*.obj.@"
    $(CPP) /nologo $(Profile_CFLAGS) $(Profile_INCS) /Fo$*.obj /c $<
	@DEL "$*.obj.@"

$(Debug_PCH) Debug\_precompile.obj:
	@ECHO.>"$(Debug_PCH).@"
	@ECHO.>"Debug\_precompile.obj.@"
	$(CPP) /nologo $(Debug_CFLAGS) $(Debug_INCS) /YcCvGameCoreDLL.h /Fo"Debug\_precompile.obj" /c _precompile.cpp
	@DEL "$(Debug_PCH).@"
	@DEL "Debug\_precompile.obj.@"

$(Release_PCH) Release\_precompile.obj:
	@ECHO.>"$(Release_PCH).@"
	@ECHO.>"Release\_precompile.obj.@"
    $(CPP) /nologo $(Release_CFLAGS) $(Release_INCS) /YcCvGameCoreDLL.h /Fo"Release\_precompile.obj" /c _precompile.cpp
	@DEL "$(Release_PCH).@"
	@DEL "Release\_precompile.obj.@"

$(Profile_PCH) Profile\_precompile.obj:
	@ECHO.>"$(Profile_PCH).@"
	@ECHO.>"Profile\_precompile.obj.@"
    $(CPP) /nologo $(Profile_CFLAGS) $(Profile_INCS) /YcCvGameCoreDLL.h /Fo"Profile\_precompile.obj" /c _precompile.cpp
	@DEL "$(Profile_PCH).@"
	@DEL "Profile\_precompile.obj.@"

.rc{Debug}.res:
	@ECHO.>"$*.res.@"
	$(RC) /Fo$@ $(Debug_INCS) $<
	@DEL "$*.res.@"

.rc{Release}.res:
	@ECHO.>"$*.res.@"
	$(RC) /Fo$@ $(Release_INCS) $<
	@DEL "$*.res.@"

.rc{Profile}.res:
	@ECHO.>"$*.res.@"
	$(RC) /Fo$@ $(Profile_INCS) $<
	@DEL "$*.res.@"

!IFDEF BLACKLIST

Debug\$(BLACKLIST).obj: $(BLACKLIST).cpp
	@ECHO.>"$*.obj.@"
	@ECHO.>"$*-dummy.cpp"
	$(CPP) /nologo $(Debug_CFLAGS) $(Debug_INCS) /Y- /Fo$@ /c "$*-dummy.cpp"
	@DEL "$*-dummy.cpp"
	@DEL "$*.obj.@"

Release\$(BLACKLIST).obj: $(BLACKLIST).cpp
	@ECHO.>"$*.obj.@"
	@ECHO.>"$*-dummy.cpp"
	$(CPP) /nologo $(Release_CFLAGS) $(Release_INCS) /Y- /Fo$@ /c "$*-dummy.cpp"
	@DEL "$*-dummy.cpp"
	@DEL "$*.obj.@"

Profile\$(BLACKLIST).obj: $(BLACKLIST).cpp
	@ECHO.>"$*.obj.@"
	@ECHO.>"$*-dummy.cpp"
	$(CPP) /nologo $(Profile_CFLAGS) $(Profile_INCS) /Y- /Fo$@ /c "$*-dummy.cpp"
	@DEL "$*-dummy.cpp"
	@DEL "$*.obj.@"

!ENDIF

!IFDEF FD

!IF [IF NOT EXIST $(FD) EXIT 1] == 0
!IF [$(FD) --objectextension=pch -q -O Debug CvGameCoreDLL.cpp > depends] != 0 || \
    [$(FD) --objectextension=obj -q -O Debug $(SOURCES) >> depends] != 0 || \
    [$(FD) --objectextension=pch -q -O Release CvGameCoreDLL.cpp >> depends] != 0 || \
    [$(FD) --objectextension=obj -q -O Release $(SOURCES) >> depends] != 0 || \
    [$(FD) --objectextension=pch -q -O Profile CvGameCoreDLL.cpp >> depends] != 0 || \
    [$(FD) --objectextension=obj -q -O Profile $(SOURCES) >> depends] != 0
!MESSAGE Error running fastdep.
!ENDIF
!ELSE
!IF [ECHO "fastdep.exe" NOT FOUND! && \
     ECHO Please edit Makefile to reflect the correct path of fastdep. && \
     ECHO. ]
!ENDIF
!ENDIF

!ENDIF

!IF EXIST(depends)
!INCLUDE depends
!ENDIF
