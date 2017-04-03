/* 
This code primarily comes from 
http://www.prasannatech.net/2008/07/socket-programming-tutorial.html
and
http://www.binarii.com/files/papers/c_sockets.txt
 */
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
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

string readTemp(int fd) {
  

  /*
    Write the rest of the program below, using the read and write system calls.
  */
  
  
  string info = "";
  
  bool inSideDegree;

  while (true) {
    char buf[1];
    int bytes_read = read(fd, buf,1);
    //cout << bytes_read << "\n\n\n";
    if(bytes_read <= 0) {
      continue;
    }
    if (fd > 0) {
       // cout << "  " << buf[0];
      if (buf[0]  >= '0' && buf[0]  <= '9' ) {

        info += buf[0];
        inSideDegree = true;
      }
      else if(inSideDegree == true && buf[0] == '.'){
        info += buf[0];
      }
      else if(inSideDegree == false && info.length() >= 2){
        cout << "~~~ " << info;
          return info;
        }
        else{
          inSideDegree = false;
        }

        // info = "";
        // inSideDegree = false;
    }
    
  }

}

int start_server(int PORT_NUMBER, char* file_name)
{
      int fd2 = open(file_name, O_RDWR | O_NOCTTY | O_NDELAY);
  
      if (fd2 < 0) {
        perror("Could not open file");
        exit(1);
      }
      else {
        cout << "Successfully opened " << file_name << " for reading/writing" << endl;
      }

      configure(fd2);
      // structs to represent the server and client
      struct sockaddr_in server_addr,client_addr;    
      
      int sock; // socket descriptor

      // 1. socket: creates a socket descriptor that you later use to make other system calls
      if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("Socket");
	exit(1);
      }
      int temp;
      if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1) {
	perror("Setsockopt");
	exit(1);
      }

      // configure the server
      server_addr.sin_port = htons(PORT_NUMBER); // specify port number
      server_addr.sin_family = AF_INET;         
      server_addr.sin_addr.s_addr = INADDR_ANY; 
      bzero(&(server_addr.sin_zero),8); 
      
      // 2. bind: use the socket and associate it with the port number
      if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
	perror("Unable to bind");
	exit(1);
      }

      // 3. listen: indicates that we want to listn to the port to which we bound; second arg is number of allowed connections
      if (listen(sock, 5) == -1) {
	perror("Listen");
	exit(1);
      }
          
      // once you get here, the server is set up and about to start listening
      cout << endl << "Server configured to listen on port " << PORT_NUMBER << endl;
      fflush(stdout);
     

      // 4. accept: wait here until we get a connection on that port
      int sin_size = sizeof(struct sockaddr_in);
      int fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
      cout << "Server got a connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;
      
      // buffer to read data into
      char request[1024];
      
      // 5. recv: read incoming message into buffer
      int bytes_received = recv(fd,request,1024,0);
      // null-terminate the string
      request[bytes_received] = '\0';
      cout << "Here comes the message:" << endl;
      cout << request << endl;


      
      // this is the message that we'll send back
      /* it actually looks like this:
        {
           "name": "cit595"
        }
      */
      // while (true) {

        string temp2 = readTemp(fd2);
        cout << "aaa " << temp2 << endl;
// "{\n\"name\": \"cit595\"\n}\n";
      
         if (temp2.length() >= 2) {
          string reply = "{\n\"name\": \"" + temp2 + "\"\n}\n";
      
        // 6. send: send the message over the socket
        // note that the second argument is a char*, and the third is the number of chars
          send(fd, reply.c_str(), reply.length(), 0);
        } 
       // }
     
      //printf("Server sent message: %s\n", reply);

      // 7. close: close the socket connection
      close(fd);
      close(sock);
      cout << "Server closed connection" << endl;
  
      return 0;
} 



int main(int argc, char *argv[])
{
  // check the number of arguments
  if (argc != 3)
    {
      cout << endl << "Usage: server [port_number]" << endl;
      exit(0);
    }

  int PORT_NUMBER = atoi(argv[1]);
  char* file_name = argv[2];
  start_server(PORT_NUMBER, file_name);
}

