#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings  -lstdc++ -g -Wall 
#  tar -cvf student.tar driver.cpp Makefile inputs.txt student_db.cpp student_db.hpp
#  tar -xvf student.tar

CC = g++
CFLAGS  = -g -std=c++17 -municode
RM = del

all: output

output:  main.o
	$(CC) $(CFLAGS) -o output.exe my_window.o 
	mingw32-make.exe partclean

main.o:  
	$(CC) $(CFLAGS) -c my_window.cpp


# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
#
clean: 
	$(RM) output.exe *.o *~

partclean:
	$(RM) *.o *~