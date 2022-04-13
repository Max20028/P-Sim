#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings  -lstdc++ -g -Wall 
#  tar -cvf student.tar driver.cpp Makefile inputs.txt student_db.cpp student_db.hpp
#  tar -xvf student.tar

#  mingw32-make.exe partclean could be in output, but dont want it

#  C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um

CC = g++
CFLAGS  = -g -std=c++17 -municode -static-libgcc -static-libstdc++ -static
# LIBFLAGS = -LC:\\msys64\\mingw64\\x86_64-w64-mingw32\\lib -ld3d11 -ld3dx11 -ld3dx10 -ld3dcompiler
LIBFLAGS = -L"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.20348.0\um\x64" -ld3d11 -ld3dcompiler -luuid -lole32 -loleaut32 -ld2d1 -ldwrite
HEADFLAGS = -I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um"

RM = del /f

all: output
debug: CFLAGS += -DDEBUG
debug: output

output:  main WICTextureLoader
	$(CC) $(CFLAGS) -o output my_window.o WicTextureLoader.o $(LIBFLAGS)


main:  
	$(CC) $(CFLAGS) -c my_window.cpp

WICTextureLoader:
	$(CC) $(CFLAGS) -c "headers\WicTextureLoader.cpp"

	


# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
#
clean: 
	$(RM) output.exe *.o *~

partclean:
	$(RM) *.o *~