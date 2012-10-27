#Oracle envirement
	#ORACLELIB = $(ORACLE_HOME)/lib
	ORACLE_LIB1 = $(ORACLE_HOME)/lib/
	ORACLE_LIB2 = $(ORACLE_HOME)/rdbms/lib
	ORACLE_INCLUDE = $(ORACLE_HOME)/rdbms/public
	RANJAN_ROOT = /home/aistdev/ranjan/includes
	TSS_ROOT = /home/aistdev/SN/Spatial-Network-Data/SNDT
	TSS_ALL = $(TSS_ROOT)/engine $(TSS_ROOT)/path $(TSS_ROOT)/TSSParser

# the linker is also "gcc". It might be something else with other compilers.
	CC = g++ -g -c
	LD = g++ -g

# Compiler flags go here.
	CFLAGS = -ggdb -fpic -Wall

#locations maybe not in path to include
	INCLUDEIBLOB = -I $(RANJAN_ROOT)/iBLOB_V3/iBLOB -I $(RANJAN_ROOT)/iBLOB_V3/vector -I $(RANJAN_ROOT)/OCILobWrapper
	INCLUDE_TSS = -I $(TSS_ROOT)/engine -I $(TSS_ROOT)/path -I $(TSS_ROOT)/TSSParser

#include headers from the oracle_include
	INCLUDEORACLEHEADERS = -I $(ORACLE_INCLUDE)

#include UDT
	INCLUDE_UDT = -I $(TSS_ROOT)/UDT

#include OBJLOC
	INCLUDE_OBJLOC = -I $(TSS_ROOT)/ObjLoc

# use this command to erase files.
	RM = /bin/rm -f

#Linker flags go here.
	LDFLAGSSTAL = -L$(ORACLE_LIB1) -L$(ORACLE_LIB2)  -L$(RJNLIBS) -liblob -locci

#sources
	TSSPARSER = TSSParser/TSSParser.cpp
	SRCTSS = path/Path.cpp $(TSSPARSER) engine/TSS.cpp UDT/UDT.cpp ObjLoc/ObjLocOracle.cpp
	OBJPATH = Path.o TSSParser.o TSS.o UDT.o ObjLoc.o
	SNETSRC = snet/snet.cpp
	SNETOBJ = snet.o

#test related components
	TESTSNETOBJ = testsnet.o
	TESTSNETSRC = testsnet.cpp


snet.out: $(OBJPATH) $(SNETOBJ) $(TESTSNETOBJ)
	@echo "start LINKING and LOADING"
	$(LD) -o testsnet.out $(SNETOBJ) $(TESTSNETOBJ) $(ORACLELIB) $(OBJPATH) $(LDFLAGSSTAL)
	@echo "----------------- Done!-------------------"

TSSParser.o: $(TSSPARSER)
	@echo "TSSParser begin compilation ..."
	$(CC) $(CFLAGS) $(INCLUDEIBLOB) $(INCLUDEORACLEHEADERS) $(TSSPARSER)
	@echo "TSSParser done"

Path.o: path/Path.cpp path/Path.h
	@echo "Path begin compilation ..."
	$(CC) $(CFLAGS) $(INCLUDEIBLOB)  $(INCLUDEORACLEHEADERS) path/Path.cpp
	@echo "Path Done!"

TSS.o:engine/TSS.cpp
	@echo "TSS Engine begin compilation..."
	$(CC) $(CFLAGS) $(INCLUDEIBLOB) $(INCLUDEORACLEHEADERS)  $(INCLUDE_TSS) engine/TSS.cpp
	@echo "TSS Engine Done "

ObjLoc.o:ObjLoc/ObjLocOracle.cpp
	@echo "ObjLoc begin compilation..."
	$(CC) -o ObjLoc.o  $(CFLAGS) $(INCLUDEIBLOB) $(INCLUDEORACLEHEADERS) $(INCLUDE_OBJLOC) $(INCLUDE_UDT) $(ORACLELIB) ObjLoc/ObjLocOracle.cpp
	@echo "ObjLoc Done "

UDT.o:UDT/UDT.cpp
	@echo "UDT begin compilation..."
	$(CC) $(CFLAGS) $(INCLUDEIBLOB) $(INCLUDEORACLEHEADERS) $(INCLUDE_UDT) UDT/UDT.cpp
	@echo "UDT Done "

snet.o:snet/snet.cpp
	@echo "::::::!!!!snet begin compilation..."
	$(CC) $(CFLAGS) $(INCLUDEIBLOB) $(INCLUDEORACLEHEADERS) $(INCLUDE_UDT) $(INCLUDE_TSS) snet/snet.cpp
	@echo "::::!!!!::snet Done "

testsnet.o:$(TESTSNETSRC) $(SNETOBJ)
	@echo "testfile begin compilation..."
	$(CC) $(CFLAGS) $(INCLUDEIBLOB) $(INCLUDEORACLEHEADERS) $(INCLUDE_OBJLOC) $(INCLUDE_UDT) $(INCLUDE_TSS) -L$(SNETOBJ) $(TESTSNETSRC)
	@echo "test file Done "

clean:
	-rm *.o
	-rm *.out
