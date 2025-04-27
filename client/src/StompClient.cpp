#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include "../include/User.h"
#include "../include/StompProtocol.h"

int main(int argc, char *argv[]) {
	// TODO: implement the STOMP client
	//set up socket listener
    //if (argc < 3) {
    //    std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
    //    return -1;
    //}
    //std::string host = argv[1];
    //short port = atoi(argv[2]);
    //ConnectionHandler* connectionHandler = new ConnectionHandler();
    StompProtocol* stompProtocol=new StompProtocol();
    //first login

    //if (!connectionHandler.connect()) {
      //  std::cerr << "Cannot connect to " << "127.0.0.1" << ":" << "7777" << std::endl;
        //return 1;
    //}
    //create thread to listen to socket
    while(true){
        string frame;
        getline(cin, frame);
        stompProtocol->stringToStomp(frame);
}
    //login 127.0.0.1:7777 meni films
    delete stompProtocol;
	return 0;
}