#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include "Game.h"

using namespace std;
class User {
public:
    enum command {
        eLogin,
        eJoin,
        eExit,
        eReport,
        eSummary,
        eLogout,
        unknown
    };
    User();
    ~User();
    string getUsername();
    string getSubIdFromGameName(const string& game_name);
    string getGamenameFromSubId(int sub_id);
    string receiptGenerator(command,int);
    string addNewDest(const string& dest);
    void update(string& game_name, const string& user, const string& time, const string& event_name, const string& general_updates, const string& description);
    string getSummary(string& username, const string& game_name);
    void connect();
    void disconnect();
    bool isConnected();
    void setUsername(string);
    command getCommandFromReceipt(int);
    int getActionFromReceipt(int);
private:
	string username;
	vector<string> subscriptions;
    vector<command> receipts;
    vector<int> receiptsAction;
    std::map<std::string, vector<Game*>*> mapGameNameToGameObjs;
    bool is_connected;
    void resetUserObjs();

};