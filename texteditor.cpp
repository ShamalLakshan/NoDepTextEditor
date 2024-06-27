// ---------- Includes ---------- //
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <iostream>
#include <errno.h>  
// #include <stdio.h>


// ---------- Defines ---------- //

//Define ctrl key
#define CTRL_KEY(k) ((k) & 0x1f)


// ---------- Data ---------- //

//Disabling Raw mode
struct termios orig_termios;

// ---------- Terminal ---------- //

//Error Handling
void die(const char *s) {
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
        die("tcsetattr");
}

//Turning off echo.
//Enables Raw mode instead of the Canonical mode.
void enableRawMode(){
    //Error Handling
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");

    //Disables raw mode at exit
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    
    tcgetattr(STDIN_FILENO, &raw);

    //IXON - Disables ctrl + s , ctrl + q...   
    //ICRNL - Disables ctrl + m.
    raw.c_iflag &= ~(ICRNL | IXON);

    //Turning off some miscellaneous flags
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    //Disables output processing
    raw.c_oflag &= ~(OPOST);

    //Turning off some miscellaneous flags
    raw.c_cflag |= (CS8);

    //ICANON - reading input byte-by-byte, instead of line-by-line.
    //IEXTEN - Disables ctrl + v.
    //ISIG - Disables ctrl + c , ctrl + z ....
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); 

    //Timeout for Read()
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    //Error Handling
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}