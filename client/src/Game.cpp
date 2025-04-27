//
// Created by morshuk@wincs.cs.bgu.ac.il on 1/12/23.
//

#include "../include/Game.h"

Game::Game(const string &user, const string& team_a, const string& team_b):mapStats(),aStats(),bStats(),updates(),team_a(team_a),team_b(team_b),username(user) {
}

std::string Game::getUsername() {
    return username;
}

void Game::update(const string &time, const string &event_name, const string &general_updates, const string &description) {
    ostringstream oss;
    oss<<time<<"-"<<event_name<<":\n\n"<<description;
    updates.push_back(oss.str());
    int pos_of_team_a = general_updates.find("team a updates:");
    int pos_of_team_b = general_updates.find("team b updates:");
    string general;
    string team_a;
    string team_b;
    if (pos_of_team_a != 0) {
        general = general_updates.substr(0, pos_of_team_a);
    }
    team_a = general_updates.substr(pos_of_team_a+1);
    team_a = team_a.substr(team_a.find('\n')+1);
    team_a = team_a.substr(0, team_a.find("team b updates:"));
    team_b = general_updates.substr(pos_of_team_b);
    team_b = team_b.substr(team_b.find('\n') + 1);
    
    while (!general.empty()) {
        string this_line;
        if (general.find('\n') == string::npos) {
            this_line = general;
            general = "";
        }
        else {
            this_line = general.substr(0, general.find('\n'));
            general = general.substr(general.find('\n'));
        }
        boost::trim_left(this_line);
        mapStats[this_line.substr(0,this_line.find(':'))]=this_line.substr(this_line.find(':') + 1);
        boost::trim(general);
    }
    boost::trim(team_a);
    while (!team_a.empty()) {
        string this_line;
        if (team_a.find('\n') == string::npos) {
            this_line = team_a;
            team_a = "";
        }
        else {
            this_line = team_a.substr(0, team_a.find('\n'));
            team_a = team_a.substr(team_a.find('\n'));
        }
        boost::trim_left(this_line);
        aStats[this_line.substr(0,this_line.find(':'))]=this_line.substr(this_line.find(':') + 1);
        boost::trim(team_a);
    }
    boost::trim(team_b);
    while (!team_b.empty()) {
        string this_line;
        if (team_b.find('\n') == string::npos) {
            this_line = team_b;
            team_b = "";
        }
        else {
            this_line = team_b.substr(0, team_b.find('\n'));
            team_b = team_b.substr(team_b.find('\n'));
        }
        boost::trim_left(this_line);
        bStats[this_line.substr(0,this_line.find(':'))]=this_line.substr(this_line.find(':') + 1);
        boost::trim(team_b);
    }
}

string Game::getSummary() {
    ostringstream oss;
    oss<<team_a<<" vs "<<team_b<<"\nGame stats:\nGeneral stats:\n";
    for (const auto& kv : mapStats){
        oss<<kv.first << ": " << kv.second << "\n";
    }
    oss<<team_a<<" stats:\n";
    for (const auto& kv : aStats){
        oss<<kv.first << ": " << kv.second << "\n";
    }
    oss<<team_b<<" stats:\n";
    for (const auto& kv : bStats){
        oss<<kv.first << ": " << kv.second << "\n";
    }
    oss<<"\nGame event reports:\n";
    for (const string& s:updates){
        oss<<s<<"\n\n";
    }
    return oss.str();
}
