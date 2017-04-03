#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <iostream>
using namespace std;


/*
This code configures the file descriptor for use as a serial port.
*/
void configure(int fd) {
  struct  termios pts;
  tcgetattr(fd, &pts);
  cfsetospeed(&pts, 9600);   
  cfsetispeed(&pts, 9600);   
  tcsetattr(fd, TCSANOW, &pts);
}


int main(int argc, char *argv[]) {

  if (argc < 2) {
    cout << "Please specify the name of the serial port (USB) device file!" << endl;
    exit(0);
  }

  // get the name from the command line
  char* file_name = argv[1];
  
  // try to open the file for reading and writing
  int fd = open(argv[1], O_RDWR | O_NOCTTY | O_NDELAY);
  
  if (fd < 0) {
    perror("Could not open file");
    exit(1);
  }
  else {
    cout << "Successfully opened " << argv[1] << " for reading/writing" << endl;
  }

  configure(fd);

  char buf[40];
  int bytes_read = read(fd, buf, 40);
  string str;

  while (bytes_read ! = 0) {
    while (bytes_read > 0 && fd >= 0) {
      for (int i = 0; i < 40; i++) {
        if (buf[i] == '\n') {
          cout << str << endl;
          str = "";
          for (int j = i + 1; j < 40; j++) {
            str += buf[j];
          }
          break;
        } else {
          str += buf[i];
        }
      }
      bytes_read = read(fd, buf, 40);
    }
  }
  /*
    Write the rest of the program below, using the read and write system calls.
  */

}
