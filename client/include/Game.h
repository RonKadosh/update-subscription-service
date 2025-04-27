#pragma once
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <boost/algorithm/string.hpp>
using namespace std;
class Game {
public:
    Game(const std::string& user,const string& team_a,const string& team_b);
    std::string getUsername();
    void update(const string& time ,const string& event_name,const string& general_updates , const string& description);
    string getSummary();
private:
    map<string,string> mapStats;
    map<string,string> aStats;
    map<string,string> bStats;
    vector<string> updates;
    string team_a;
    string team_b;
    string username;
};



