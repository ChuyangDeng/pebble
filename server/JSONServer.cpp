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
#include <cstdio>
#include <map>
#include <thread>
#include <chrono>
#include <queue>
#include <vector>
#include <algorithm>
#include <numeric>
using namespace std;

int fd;
int fd2;
queue<double> temperatures;
vector<double> recenttemps;
double avgTemp = 0.0;
double minTemp = 0.0;
double maxTemp = 0.0;
double curtTemp = 0.0;
double total = 0.0;
bool stop = false;
int clicks = 0;
int file_descriptor = 0;
char* file_name;
string command = "auto";
bool isServer = true;
bool isArduino = true;

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

void timer() {
  this_thread::sleep_for(chrono::seconds(60));
  stop = true;
}
void doIterate(){
  int size = temperatures.size();
  double max = 0.0;
  double min = 100.0;
  for(int i = 0; i < size; i++){
    double head = temperatures.front();
    if(head > max){
      max = head;
    } if(head < min){
      min = head;
    }
    temperatures.pop();
    temperatures.push(head);
  }
  maxTemp = max;
  minTemp = min;
}

void updateTemp() {
  bool doIt = false;
  int size = temperatures.size();
  doIterate();
  if (size > 3600) {
    total -= temperatures.front();
    if(maxTemp - temperatures.front() == 0.0000000001 || minTemp - temperatures.front() == 0.0000000001){
      doIt = true;
    }
    temperatures.pop();
    size = 3600;
  }
  if(doIt){
    doIterate();
  }
  
  avgTemp = total / size;
  
 
  }


double getslope(vector<double> x, vector<double> y){
    double n = x.size();

    double avgX = accumulate(x.begin(), x.end(), 0.0) / n;
    double avgY = accumulate(y.begin(), y.end(), 0.0) / n;

    double numerator = 0.0;
    double denominator = 0.0;

    for(int i=0; i<n; ++i){
        numerator += (x[i] - avgX) * (y[i] - avgY);
        denominator += (x[i] - avgX) * (x[i] - avgX);
    }

    if(denominator == 0){
        denominator = numerator;
    }

    return numerator / denominator;
}


double predictTemp(){
  vector<double> times;
  for (int i = 1; i <= recenttemps.size(); i++){
    times.push_back(i);
  }

  double size = times.size();
  double avgtimes = accumulate(times.begin(), times.end(), 0.0) / size;
  double avgtemp = accumulate(recenttemps.begin(), recenttemps.end(), 0.0) / size;

  double slope = getslope(times,recenttemps);
  double yinter = avgtemp - slope*avgtimes;

  double predicted = slope*(size+1)+yinter;
  return predicted;
}


int start_server(int PORT_NUMBER, char* file_name)
{
      
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

      this_thread::sleep_for(chrono::seconds(5));
          
      // once you get here, the server is set up and about to start listening
      cout << endl << "Server configured to listen on port " << PORT_NUMBER << endl;
      fflush(stdout);
      // int fd;
      bool isC = true;
      while (true) {
        // 4. accept: wait here until we get a connection on that port
      int sin_size = sizeof(struct sockaddr_in);
      fd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
      cout << "Server got a connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;
      
      // buffer to read data into
      char request[1024];
      
      // 5. recv: read incoming message into buffer
      int bytes_received = recv(fd,request,1024,0);
      // null-terminate the string
      request[bytes_received] = '\0';
      cout << "Here comes the message:" << endl;
      cout << request << endl;
      char* token = strtok(request, " ");
      token = strtok(NULL, " ");
      command = token + 1;

      string curtStr = to_string(curtTemp);
      
      if (command == "f") {

        if (isArduino) cout << "arduino ON" << endl;

        if (!isServer) {
          cout << "Disconnected from server." << endl;
          curtStr = "Disconnected Server.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          write(fd2, "s", 1);
          break;
        }

        if (!isArduino) {
          cout << "Disconnected from arduino." << endl;
          curtStr = "Disconnected Arduino.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          cout << reply << endl;
        } else {
        clicks = 0;
        isC = false;
        cout << "Show Fahrenheit Temperature." << endl;
        double curtFTemp = curtTemp * (9.0/5) + 32;
        curtStr = to_string(curtFTemp);
        string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
        send(fd, reply.c_str(), reply.length(), 0);
        string writeTo = "f";
        write(fd2, writeTo.c_str(), 2);
        cout << "write to arduino: " << writeTo.c_str() << endl;
        }
      } 
      else if(command == "avg"){

        if (isArduino) cout << "arduino ON" << endl;

        if (!isServer) {
          cout << "Disconnected from server." << endl;
          curtStr = "Disconnected Server.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          write(fd2, "s", 1);
          break;
        }

        if (!isArduino) {
          cout << "Disconnected from arduino." << endl;
          curtStr = "Disconnected Arduino.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          cout << reply << endl;
        } else {
        clicks = 0;
        cout << "Show avg Temperature. " << endl;
        string avgStr = to_string(avgTemp);
        string reply = "{\n\"name\": \"" + avgStr + "\"\n}\n";
        send(fd, reply.c_str(), reply.length(), 0);
        }

      } else if (command == "min") {

        if (isArduino) cout << "arduino ON" << endl;

        if (!isServer) {
          cout << "Disconnected from server." << endl;
          curtStr = "Disconnected Server.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          write(fd2, "s", 1);
          break;
        }

        if (!isArduino) {
          cout << "Disconnected from arduino." << endl;
          curtStr = "Disconnected Arduino.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          cout << reply << endl;
        } else {
        clicks = 0;
        cout << "Show min Temperature. " << endl;
        string minStr = to_string(minTemp);
        cout << "min temp: " << minStr << endl;
        string reply = "{\n\"name\": \"" + minStr + "\"\n}\n";
        send(fd, reply.c_str(), reply.length(), 0);
        }

      } 
      else if (command == "max") {

        if (isArduino) cout << "arduino ON" << endl;

        if (!isServer) {
          cout << "Disconnected from server." << endl;
          curtStr = "Disconnected Server.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          write(fd2, "s", 1);
          break;
        }

        if (!isArduino) {
          cout << "Disconnected from arduino." << endl;
          curtStr = "Disconnected Arduino.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          cout << reply << endl;
        } else {
        clicks = 0;
        cout << "Show max Temperature. " << endl;
        string maxStr = to_string(maxTemp);
        cout << "max temp: " << maxStr << endl;
        string reply = "{\n\"name\": \"" + maxStr + "\"\n}\n";
        send(fd, reply.c_str(), reply.length(), 0);
        }
      }  
      else if (command == "s") {

        if (isArduino) cout << "arduino ON" << endl;

        if (!isServer) {
          cout << "Disconnected from server." << endl;
          curtStr = "Disconnected Server.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          write(fd2, "s", 1);
          break;
        }

        if (!isArduino) {
          cout << "Disconnected from arduino." << endl;
          curtStr = "Disconnected Arduino.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          cout << reply << endl;
        } else {
        clicks = 0;
        cout << "Stand By." << endl;
        curtStr = "Stand By";
        string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
        send(fd, reply.c_str(), reply.length(), 0);
        string writeTo = "s";
        write(fd2, writeTo.c_str(), 2);
        cout << "write to arduino: " << writeTo.c_str() << endl;
        }

      } else if(command == "click") {

        if (isArduino) cout << "arduino ON" << endl;

        if (!isServer) {
          cout << "Disconnected from server." << endl;
          curtStr = "Disconnected Server.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          write(fd2, "s", 1);
          break;
        }

        if (!isArduino) {
          cout << "Disconnected from arduino." << endl;
          curtStr = "Disconnected Arduino.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          cout << reply << endl;
        } else {
        cout << "Count. "<< endl;
        clicks++;
        curtStr = "Current clicks: " + to_string(clicks);
        string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
        send(fd, reply.c_str(), reply.length(), 0);
        string writeTo = "k";
        write(fd2, writeTo.c_str() , 2);
        cout << "write to arduino: " << writeTo.c_str() << endl;
        }

      } else if(command == "auto"){

        if (isArduino) cout << "arduino ON" << endl;

        if (!isServer) {
          cout << "Disconnected from server." << endl;
          curtStr = "Disconnected Server.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          write(fd2, "s", 1);
          break;
        }

        if (!isArduino) {
          cout << "Disconnected from arduino." << endl;
          curtStr = "Disconnected Arduino.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          cout << reply << endl;
        } else {
        curtStr = to_string(curtTemp);
        clicks = 0;
        cout << "Show auto Celcius Temperature." << endl;
        string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
        send(fd, reply.c_str(), reply.length(), 0);
        cout << reply << endl;
        string writeTo = "c";
        write(fd2, writeTo.c_str(), 2);
        cout << "write to arduino: " << writeTo.c_str() << endl;
        }

    } else if (command == "ulong"){

      if (isArduino) cout << "arduino ON" << endl;

      if (!isServer) {
          cout << "Disconnected from server." << endl;
          curtStr = "Disconnected Server.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          write(fd2, "s", 1);
          break;
      }

      if (!isArduino) {
          cout << "Disconnected from arduino." << endl;
          curtStr = "Disconnected Arduino.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          cout << reply << endl;
        } else {
        clicks = 0;
      cout << "Red." << endl;
      curtStr = "Red.";
      string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
      send(fd, reply.c_str(), reply.length(), 0);
      string writeTo = "r";
      write(fd2, writeTo.c_str(), 2);
      cout << "write to arduino: " << writeTo.c_str() << endl;
        }

    } else if (command == "dlong"){

      if (isArduino) cout << "arduino ON" << endl;

      if (!isServer) {
          cout << "Disconnected from server." << endl;
          curtStr = "Disconnected Server.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          write(fd2, "s", 1);
          break;
      }

      if (!isArduino) {
          cout << "Disconnected from arduino." << endl;
          curtStr = "Disconnected Arduino.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          cout << reply << endl;
        } else {
      clicks = 0;
      cout << "Green." << endl;
      curtStr = "Green";
      string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
      send(fd, reply.c_str(), reply.length(), 0);
      string writeTo = "g";
      write(fd2, writeTo.c_str(), 2);
      cout << "write to arduino: " << writeTo.c_str() << endl;
        }

    } else if (command == "pd"){

      if (isArduino) cout << "arduino ON" << endl;

      if (!isServer) {
          cout << "Disconnected from server." << endl;
          curtStr = "Disconnected Server.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          write(fd2, "s", 1);
          break;
      }

      if (!isArduino) {
          cout << "Disconnected from arduino." << endl;
          curtStr = "Disconnected Arduino.";
          string reply = "{\n\"name\": \"" + curtStr + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
          cout << reply << endl;
        } else {
          cout << "Predict temp" << endl;
          clicks = 0;
          double predicted = predictTemp();

          string predict = to_string(predicted);
          cout << "Predicted: " << predict << endl;
          string reply = "{\n\"name\": \"" + predict + "\"\n}\n";
          send(fd, reply.c_str(), reply.length(), 0);
        }


    }
          
          close(fd);

      }

      // 7. close: close the socket connection
      close(sock);
      cout << "Server closed connection" << endl;
  
      return 0;
} 

void* read_temp(void* ){
      fd2 = open(file_name, O_RDWR | O_NOCTTY | O_NDELAY);
      // file_descriptor = fd2; /****************************/

      if (fd2 < 0) {
        perror("Could not open file");
        exit(1);
      }
      else {
        cout << "Successfully opened " << file_name << " for reading/writing" << endl;
      }

      configure(fd2);

      int i = 0;
  string info = "";
  
  bool inSideDegree;

  while (isServer) {

    while (!isArduino);

    char buf[1];
    int bytes_read = read(fd2, buf,1);
    if(bytes_read <= 0) {
      continue;
    }
    if (fd2 > 0) {
      if (buf[0]  >= '0' && buf[0]  <= '9' ) {
        info += buf[0];
        inSideDegree = true;
      }
      else if(inSideDegree == true && buf[0] == '.'){
        info += buf[0];
      }
      else if(inSideDegree == false && info.length() >= 2){
          total += atof(info.c_str()); 
          curtTemp = atof(info.c_str());
          temperatures.push(atof(info.c_str()));

          if (recenttemps.size() < 30){
            recenttemps.push_back(atof(info.c_str()));
          } else {
            recenttemps.erase(recenttemps.begin());
            recenttemps.push_back(atof(info.c_str()));
          }
          updateTemp();
          inSideDegree = true;
          info = "";
        }
        else{
          inSideDegree = false;
        }
    }
  }
  pthread_exit(NULL);
}





void* quitServer(void*) {
  cout << "Ready to quit server." << endl;
  string input = "";
  do {
    cin >> input;
  } while (input != "q");
  isServer = false;
  pthread_exit(NULL);
}

void* quitArduino(void*) {
  cout << "Ready to quit arduino." << endl;
  isArduino = true;
  while (isServer) {
    this_thread::sleep_for(chrono::seconds(1));
    fd2 = open(file_name, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd2 < 0) isArduino = false;
    else isArduino = true;
  }
  pthread_exit(NULL);
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
  file_name = argv[2];

  // pthread_mutex_init(&lock, NULL);

  pthread_t thread_id2;
  pthread_t killServer;
  pthread_t killArduino;
  pthread_create(&thread_id2, NULL, read_temp, NULL);
  this_thread::sleep_for(chrono::seconds(2));
  pthread_create(&killServer, NULL, quitServer, NULL);
  this_thread::sleep_for(chrono::seconds(2));
  pthread_create(&killArduino, NULL, quitArduino, NULL);
  this_thread::sleep_for(chrono::seconds(2));

  start_server(PORT_NUMBER, file_name);
}

