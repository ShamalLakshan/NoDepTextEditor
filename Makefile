CC = g++
CCFLAGS =  -Wall -Wextra -g

texteditor: texteditor.cpp
	$(CC) $C(CFLAGS) texteditor.cpp -o texteditor -Wall -Wextra -pedantic -lstdc++

clean:
	texteditor.cpp