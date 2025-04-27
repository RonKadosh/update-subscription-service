#include <utility>

#include "../include/User.h"

User::User():username(),subscriptions(),receipts(),receiptsAction(),mapGameNameToGameObjs(),is_connected(false)
{}


User::~User()
{
    for (auto item : mapGameNameToGameObjs) {
        for (Game* g : *item.second) {
            delete g;
        }
        delete item.second;
    }
}

string User::getUsername()
{
	return username;
}

string User::getSubIdFromGameName(const string& game_name)
{
    int size=(int)subscriptions.size();
	for (int i = 0;i < size;i++) {
		if (subscriptions[i] == game_name)
			return to_string(i);
	}
    return "";
}

string User::getGamenameFromSubId(int sub_id)
{
    return subscriptions[sub_id];
}

string User::receiptGenerator(command this_command,int id)
{
    receipts.push_back(this_command);
    receiptsAction.push_back(id);
    return to_string(receipts.size() - 1);
}

string User::addNewDest(const string& dest)
{
	subscriptions.push_back(dest);
	return to_string(subscriptions.size() - 1);
}

void User::update(string &game_name, const string &user, const string &time, const string &event_name,
                  const string &general_updates, const string &description) {
    string gamename = game_name;
    boost::trim(gamename);
if(mapGameNameToGameObjs.find(gamename)==mapGameNameToGameObjs.end()){
    string team_a=gamename.substr(0,gamename.find('_'));
    string team_b=gamename.substr(gamename.find('_')+1);
    vector<Game*>* vector1 = new vector<Game*>;
    Game *game=new Game(user,team_a,team_b);
    vector1->push_back(game);
    mapGameNameToGameObjs.insert({gamename,vector1});
}
bool has_found=false;
for(Game *g:*mapGameNameToGameObjs[gamename]){
    if (g->getUsername()==user){
        g->update(time,event_name,general_updates,description);
        has_found=true;
        break;
    }
}
if (!has_found){
    string team_a=gamename.substr(0,gamename.find('_'));
    string team_b=gamename.substr(gamename.find('_')+1);
    Game *game=new Game(user,team_a,team_b);
    game->update(time,event_name,general_updates,description);
    mapGameNameToGameObjs[gamename]->push_back(game);
}
}

string User::getSummary(string &user, const string &game_name) {
    string gamename = game_name;
    boost::trim(gamename);
    if (mapGameNameToGameObjs.find(gamename) == mapGameNameToGameObjs.end()) return "gamename";
    else {
        for (Game* g : *mapGameNameToGameObjs[gamename]) {
            if (g->getUsername() == user) {
                return g->getSummary();
            }
        }
        return "username";
    }
}

void User::connect() {
    is_connected=true;
}

void User::disconnect()
{
    is_connected = false;
    resetUserObjs();
}

bool User::isConnected() {
    return is_connected;
}

void User::setUsername(string username)
{
    this->username = username;
}

User::command User::getCommandFromReceipt(int receipt_id)
{
    return receipts[receipt_id];
}

int User::getActionFromReceipt(int receipt_id)
{
    return receiptsAction[receipt_id];
}

void User::resetUserObjs(){
    for (auto item : mapGameNameToGameObjs) {
        for (Game* g : *item.second) {
            delete g;
        }
        delete item.second;
    }
    mapGameNameToGameObjs.clear();
    subscriptions.clear();
    receipts.clear();
    receiptsAction.clear();
}



