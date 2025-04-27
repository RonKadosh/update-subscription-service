#pragma once

#include "../include/ConnectionHandler.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include "event.h"
#include "User.h"
// TODO: implement the STOMP protocol
class StompProtocol
{
private:    
    enum stompCommand{
        eCONNECTED,
        eERROR,
        eRECEIPT,
        eMESSAGE
    };
	ConnectionHandler* ch;
    User::command check(const std::string&);
    stompCommand checkStompCommand(const std::string&);
    User* user;
    std::function<void(StompProtocol* , ConnectionHandler*)> socketListener;
    bool shouldTerminate;
public:
	StompProtocol();
    ~StompProtocol();
    StompProtocol(const StompProtocol&)=default;
    StompProtocol& operator=(const StompProtocol&)=default;
    bool shouldStopListening();
	void stringToStomp(const std::string& msg);
    void stompToAction(const std::string& msg);
    void logout();
};
