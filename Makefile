OBJ_LIST = hashmap.obj main.obj stringmanip.obj inoutmanip.obj utils.obj preprocessing.obj
CFLAGS   = /nologo /W4 /EHsc /Za /MD

EXE_NAME = so-cpp.exe

build : $(EXE_NAME)

$(EXE_NAME) : $(OBJ_LIST)
	$(CPP) $(CFLAGS) /Fe$@ $**

clean : exe_clean obj_clean

obj_clean :
	del *.obj

exe_clean :
	del $(EXE_NAME)