
all : part1.exe part2.exe part3.exe part4.exe cpubound iobound
	
	
part1.exe: part1.o string_parser.o
	gcc -o part1.exe part1.o string_parser.o

part1.o: part1.c
	gcc -c part1.c

part2.exe: part2.o string_parser.o
	gcc -o part2.exe part2.o string_parser.o

part2.o: part2.c
	gcc -c part2.c

part3.exe: part3.o string_parser.o
	gcc -o part3.exe part3.o string_parser.o

part3.o: part3.c
	gcc -c part3.c

part4.exe: part4.o
	gcc -o part4.exe part4.o

part4.o: part4.c
	gcc -c part4.c
	
string_parser.o: string_parser.c string_parser.h
	gcc -c string_parser.c

cpubound: cpubound.o
	gcc -o cpubound cpubound.o

cpubound.o: cpubound.c
	gcc -c cpubound.c

iobound: iobound.o
	gcc -o iobound iobound.o

iobound.o: iobound.c
	gcc -c iobound.c
	
clean:
	rm -f core *.o *.exe
