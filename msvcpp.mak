# Microsoft Visual C++ generated build script - Do not modify

PROJ = ARCHIVZT
DEBUG = 0
PROGTYPE = 6
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = C:\PROJECTS\OUTBOX\CAUZINTX\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = CAUZINTX.C  
FIRSTCPP =             
RC = rc
CFLAGS_D_DEXE = /nologo /G3 /FPc /W3 /Zi /AL /Od /D "_DEBUG" /D "_DOS" /FR /Fd"CAUZINTX.PDB"
CFLAGS_R_DEXE = /nologo /Gs /G3 /FPc /W3 /AL /Ox /D "NDEBUG" /D "_DOS" /FR 
LFLAGS_D_DEXE = /NOLOGO /NOI /STACK:5120 /ONERROR:NOEXE /CO 
LFLAGS_R_DEXE = /NOLOGO /NOI /STACK:5120 /ONERROR:NOEXE 
LIBS_D_DEXE = oldnames llibce 
LIBS_R_DEXE = oldnames llibce 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_DEXE)
LFLAGS = $(LFLAGS_D_DEXE)
LIBS = $(LIBS_D_DEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_DEXE)
LFLAGS = $(LFLAGS_R_DEXE)
LIBS = $(LIBS_R_DEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = CAUZINTX.SBR \
		BITFLD32.SBR \
		COMMAND.SBR \
		CRC16.SBR \
		EDITOR.SBR \
		FILELIST.SBR \
		LAYOUT.SBR \
		PARSEOPT.SBR \
		PLATFORM.SBR \
		SETTINGS.SBR \
		SUPPORT.SBR \
		TXDIRECT.SBR \
		TXREDUCE.SBR \
		TXSTRIP.SBR


CAUZINTX_DEP = src/strptx16/editor.h \
	src/strptx16/command.h \
	src/strptx16/stdtypes.h \
	src/strptx16/editor.h \
	src/strptx16/parseopt.h \
	src/strptx16/layout.h \
	src/strptx16/txdirect.h \
	src/strptx16/txstrip.h \
	src/strptx16/bitfld32.h \
	src/strptx16/crc16.h \
	src/strptx16/filelist.h \
	src/strptx16/support.h \
	src/strptx16/settings.h \
	src/strptx16/platform.h \
	src/strptx16/txreduce.h


BITFLD32_DEP = src/strptx16/bitfld32.h \
	src/strptx16/stdtypes.h


COMMAND_DEP = src/strptx16/command.h \
	src/strptx16/stdtypes.h \
	src/strptx16/editor.h \
	src/strptx16/parseopt.h \
	src/strptx16/layout.h \
	src/strptx16/txdirect.h \
	src/strptx16/txstrip.h \
	src/strptx16/bitfld32.h \
	src/strptx16/crc16.h \
	src/strptx16/filelist.h \
	src/strptx16/support.h \
	src/strptx16/settings.h \
	src/strptx16/platform.h \
	src/strptx16/txreduce.h


CRC16_DEP = src/strptx16/crc16.h \
	src/strptx16/stdtypes.h


EDITOR_DEP = src/strptx16/editor.h \
	src/strptx16/command.h \
	src/strptx16/stdtypes.h \
	src/strptx16/platform.h \
	src/strptx16/settings.h \
	src/strptx16/layout.h \
	src/strptx16/txreduce.h \
	src/strptx16/support.h


FILELIST_DEP = src/strptx16/filelist.h \
	src/strptx16/stdtypes.h \
	src/strptx16/txdirect.h


LAYOUT_DEP = src/strptx16/layout.h \
	src/strptx16/stdtypes.h \
	src/strptx16/txdirect.h \
	src/strptx16/txstrip.h \
	src/strptx16/bitfld32.h \
	src/strptx16/crc16.h \
	src/strptx16/filelist.h \
	src/strptx16/settings.h \
	src/strptx16/platform.h


PARSEOPT_DEP = src/strptx16/command.h \
	src/strptx16/stdtypes.h \
	src/strptx16/editor.h \
	src/strptx16/parseopt.h \
	src/strptx16/layout.h \
	src/strptx16/txdirect.h \
	src/strptx16/txstrip.h \
	src/strptx16/bitfld32.h \
	src/strptx16/crc16.h \
	src/strptx16/filelist.h \
	src/strptx16/support.h \
	src/strptx16/settings.h \
	src/strptx16/platform.h \
	src/strptx16/txreduce.h


PLATFORM_DEP = src/strptx16/platform.h \
	src/strptx16/stdtypes.h


SETTINGS_DEP = src/strptx16/settings.h \
	src/strptx16/stdtypes.h \
	src/strptx16/platform.h \
	src/strptx16/layout.h


SUPPORT_DEP = src/strptx16/platform.h \
	src/strptx16/support.h \
	src/strptx16/stdtypes.h \
	src/strptx16/filelist.h \
	src/strptx16/parseopt.h \
	src/strptx16/settings.h \
	src/strptx16/layout.h \
	src/strptx16/txdirect.h


TXDIRECT_DEP = src/strptx16/platform.h \
	src/strptx16/settings.h \
	src/strptx16/stdtypes.h \
	src/strptx16/layout.h \
	src/strptx16/txdirect.h \
	src/strptx16/txstrip.h \
	src/strptx16/bitfld32.h \
	src/strptx16/crc16.h \
	src/strptx16/filelist.h \
	src/strptx16/txreduce.h \
	src/strptx16/support.h


TXREDUCE_DEP = src/strptx16/platform.h \
	src/strptx16/txreduce.h \
	src/strptx16/stdtypes.h \
	src/strptx16/settings.h \
	src/strptx16/support.h


TXSTRIP_DEP = src/strptx16/platform.h \
	src/strptx16/settings.h \
	src/strptx16/stdtypes.h \
	src/strptx16/layout.h \
	src/strptx16/txstrip.h \
	src/strptx16/bitfld32.h \
	src/strptx16/crc16.h \
	src/strptx16/filelist.h \
	src/strptx16/txdirect.h \
	src/strptx16/txreduce.h \
	src/strptx16/support.h


all:    $(PROJ).EXE $(PROJ).BSC

CAUZINTX.OBJ:   SRC/CAUZINTX.C $(CAUZINTX_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c SRC/CAUZINTX.C

BITFLD32.OBJ:   SRC/STRPTX16/BITFLD32.C $(BITFLD32_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC/STRPTX16/BITFLD32.C

COMMAND.OBJ:    SRC/STRPTX16/COMMAND.C $(COMMAND_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC/STRPTX16/COMMAND.C

CRC16.OBJ:      SRC/STRPTX16/CRC16.C $(CRC16_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC/STRPTX16/CRC16.C

EDITOR.OBJ:     SRC/STRPTX16/EDITOR.C $(EDITOR_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC/STRPTX16/EDITOR.C

FILELIST.OBJ:   SRC/STRPTX16/FILELIST.C $(FILELIST_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC/STRPTX16/FILELIST.C

LAYOUT.OBJ:     SRC/STRPTX16/LAYOUT.C $(LAYOUT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC/STRPTX16/LAYOUT.C

PARSEOPT.OBJ:   SRC/STRPTX16/PARSEOPT.C $(PARSEOPT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC/STRPTX16/PARSEOPT.C

PLATFORM.OBJ:   SRC/STRPTX16/PLATFORM.C $(PLATFORM_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC/STRPTX16/PLATFORM.C

SETTINGS.OBJ:   SRC/STRPTX16/SETTINGS.C $(SETTINGS_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC/STRPTX16/SETTINGS.C

SUPPORT.OBJ:    SRC/STRPTX16/SUPPORT.C $(SUPPORT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC/STRPTX16/SUPPORT.C

TXDIRECT.OBJ:   SRC/STRPTX16/TXDIRECT.C $(TXDIRECT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC/STRPTX16/TXDIRECT.C

TXREDUCE.OBJ:   SRC/STRPTX16/TXREDUCE.C $(TXREDUCE_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC/STRPTX16/TXREDUCE.C

TXSTRIP.OBJ:    SRC/STRPTX16/TXSTRIP.C $(TXSTRIP_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SRC/STRPTX16/TXSTRIP.C

$(PROJ).EXE::   CAUZINTX.OBJ BITFLD32.OBJ COMMAND.OBJ CRC16.OBJ EDITOR.OBJ FILELIST.OBJ \
	LAYOUT.OBJ PARSEOPT.OBJ PLATFORM.OBJ SETTINGS.OBJ SUPPORT.OBJ TXDIRECT.OBJ TXREDUCE.OBJ \
	TXSTRIP.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
CAUZINTX.OBJ +
BITFLD32.OBJ +
COMMAND.OBJ +
CRC16.OBJ +
EDITOR.OBJ +
FILELIST.OBJ +
LAYOUT.OBJ +
PARSEOPT.OBJ +
PLATFORM.OBJ +
SETTINGS.OBJ +
SUPPORT.OBJ +
TXDIRECT.OBJ +
TXREDUCE.OBJ +
TXSTRIP.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
