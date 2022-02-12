#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings  -lstdc++ -g -Wall 
#  tar -cvf student.tar driver.cpp Makefile inputs.txt student_db.cpp student_db.hpp
#  tar -xvf student.tar

#  mingw32-make.exe partclean could be in output, but dont want it

CC = g++
CFLAGS  = -g -std=c++17 -municode
LIBFLAGS = -LC:\\msys64\\mingw64\\x86_64-w64-mingw32\\lib -ld3d11 -ld3dx11 -ld3dx10 -ld3dcompiler
RM = del /f

all: output

output:  main.o
	$(CC) $(CFLAGS) -o output my_window.o $(LIBFLAGS)



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