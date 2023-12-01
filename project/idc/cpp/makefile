# Development framework header file path.
PUBINCL = -I/project/public

# Development framework cpp file name, included directly here instead of using a link library for ease of debugging.
PUBCPP = /project/public/_public.cpp

################################
# Oracle header file path
ORAINCL = -I$(ORACLE_HOME)/rdbms/public -I/project/public/db/oracle

# Oracle library file path
ORALIB =  -L$(ORACLE_HOME)/lib -L.

# Oracle's OCI library
ORALIBS = -lclntsh

# Development framework Oracle cpp file name, included directly here instead of using a link library for ease of debugging.
ORACPP = /project/public/db/oracle/_ooci.cpp
################################

# Compilation parameters.
CFLAGS = -g

all: crtsurfdata obtcodetodb obtmindtodb  

crtsurfdata: crtsurfdata.cpp
    g++ $(CFLAGS) -o crtsurfdata crtsurfdata.cpp $(PUBINCL) $(PUBCPP) 
    cp -f crtsurfdata ../bin/.

obtcodetodb: obtcodetodb.cpp
    g++ $(CFLAGS) -o obtcodetodb obtcodetodb.cpp $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP) 
    cp obtcodetodb ../bin/.

obtcodetodb1: obtcodetodb1.cpp
    g++ $(CFLAGS) -o obtcodetodb1 obtcodetodb1.cpp $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP) 
    cp obtcodetodb1 ../bin/.

obtmindtodb: obtmindtodb.cpp idcapp.h idcapp.cpp
    g++ $(CFLAGS) -o obtmindtodb obtmindtodb.cpp idcapp.cpp $(PUBINCL) $(PUBCPP) $(ORAINCL) $(ORALIB) $(ORALIBS) $(ORACPP) 
    cp obtmindtodb ../bin/.

clean:
    rm -f crtsurfdata obtcodetodb obtmindtodb
