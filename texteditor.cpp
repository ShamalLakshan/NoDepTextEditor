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
