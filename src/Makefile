# Makefile

EASYHOOK_DIR=C:\opt\easyhook
EASYHOOK_INCDIR=$(EASYHOOK_DIR)\include
EASYHOOK_LIBDIR=$(EASYHOOK_DIR)\lib
EASYHOOK_LIBS=EasyHook32.lib

CFLAGS=/nologo /MT /W3 /WX /Ox /GL /EHsc /Gz
CFLAGS=$(CFLAGS) /Gy /fp:precise /Zc:wchar_t /Zc:forScope
CFLAGS=$(CFLAGS) /I"$(EASYHOOK_INCDIR)"
CFLAGS=$(CFLAGS) /DWIN32 /D_WINDOWS /D_USRDLL /D_WINDLL
CFLAGS=$(CFLAGS) /D_UNICODE /DUNICODE /DDWRITE_EXPORTS
#CFLAGS=$(CFLAGS) /DDEBUG
CPPFLAGS=$(CFLAGS)

LDFLAGS=/nologo /DLL /RELEASE /MACHINE:X86 /SUBSYSTEM:WINDOWS
LDFLAGS=$(LDFLAGS) /OPT:REF /LTCG /DEF:Exports.def
LDFLAGS=$(LDFLAGS) /LIBPATH:"$(EASYHOOK_LIBDIR)"

LIBS=$(EASYHOOK_LIBS) d2d1.lib d3d11.lib Advapi32.lib Ole32.lib

OBJ=d2d1.obj dxgi.obj dwrite.obj

all: DWrite.dll

DWrite.dll: $(OBJ)
	link /out:$@ $(LDFLAGS) $(LIBS) $(OBJ)

install: all
	-copy /b DWrite.dll ..\DWrite32.dll
	-del *.obj *.sbr *.exp *.pdb *.lib 2>nul
	-del DWrite.dll 2>nul

clean:
	-del *.obj *.sbr *.exp *.pdb *.lib 2>nul
	-del DWrite.dll 2>nul
