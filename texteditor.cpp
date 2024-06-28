// ---------- Includes ---------- //
#include <unistd.h>
#include <termios.h>
#include <ctype.h>
#include <iostream>
#include <errno.h>  
#include <sys/ioctl.h>
// #include <stdio.h>


// ---------- Defines ---------- //

//Define ctrl key
#define CTRL_KEY(k) ((k) & 0x1f)


// ---------- Data ---------- //

//To get the width and the height of the terminal.
struct editorConfig {
    int screenrows;
    int screencols;
    //Disabline raw mode
    struct termios orig_termios;
};
struct editorConfig E;

// ---------- Terminal ---------- //

//Error Handling
void die(const char *s) {
    //Clear Screen on Exit
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);


    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr");
}

//Turning off echo.
//Enables Raw mode instead of the Canonical mode.
void enableRawMode(){
    //Error Handling
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");

    //Disables raw mode at exit
    atexit(disableRawMode);

    // struct termios raw = orig_termios;
    
    // tcgetattr(STDIN_FILENO, &raw);

    // //IXON - Disables ctrl + s , ctrl + q...   
    // //ICRNL - Disables ctrl + m.
    // raw.c_iflag &= ~(ICRNL | IXON);
    struct termios raw = E.orig_termios;

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

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } else {
      *cols = ws.ws_col;
      *rows = ws.ws_row;
        return 0;
    }
}

// ---------- Ouput ---------- //

//Tildes                                                                                                    
void editorDrawRows() {
    int y;
    for (y = 0; y < E.screenrows; y++) {
        write(STDOUT_FILENO, "~\r\n", 3);
    }
}
void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3); //Repositioning the cursor

    //Write Tildes at the Beginnning of each rows as Vim does
    editorDrawRows();
    write(STDOUT_FILENO, "\x1b[H", 3);
}


// Wait for a keypress and return it 
char editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}


// ---------- Input ---------- //

// Wait for a keypress and handles it 
// Mapping various ctrl + combinations to different editor functions
void editorProcessKeypress() {
    char c = editorReadKey();
    switch (c) {
        case CTRL_KEY('q'):
            //Clearing the screen on exit
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
    }   
}

// ---------- Init ---------- //


void initEditor() {
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

int main(){
    enableRawMode();
    initEditor();

    while (1) {
        editorProcessKeypress(); // Check for key combinations
        editorRefreshScreen(); // Refresh the editor screen after each keypress
    }

    return 0;
}