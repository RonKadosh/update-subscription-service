#include "../include/StompProtocol.h"

using namespace std;

StompProtocol::StompProtocol() :ch(new ConnectionHandler()),user(new User()),socketListener(),shouldTerminate(false)
{
    socketListener = [](StompProtocol* sp, ConnectionHandler* ch) {
        while (!sp->shouldStopListening()) {
            string frame;
            if (!ch->getFrame(frame)) {
                break;
            }
            sp->stompToAction(frame);
        }
    };
}

StompProtocol::~StompProtocol()
{
    delete user;
}

bool StompProtocol::shouldStopListening()
{
    return shouldTerminate;
}

void StompProtocol::stringToStomp(const std::string &msg) {
    ostringstream oss;
	string space = " ";
	string thismsg = msg;
    string command;
    if (msg.find(space) != string::npos) {
        command = msg.substr(0, msg.find(space));
        thismsg = thismsg.substr(thismsg.find(space));
    }
    else {
        command = msg;
    }
    if(!(user->isConnected()||command=="login")){        
            std::cout<<"you must first login!"<<std::endl;        
    }
    else {
        switch (check(command)) {
        case User::eLogin: {
                //check if client already connected
                if (user->isConnected()) cout << "The client is already logged in, log out before trying again" << endl;
                else {
                    string dots = ":";
                    string host = thismsg.substr(1, thismsg.find(dots) - 1);
                    thismsg = thismsg.substr(thismsg.find(dots));
                    string port = thismsg.substr(1, thismsg.find(space) - 1);
                    short s_port = stoi(port);
                    thismsg = thismsg.substr(thismsg.find(space) + 1);
                    string username = thismsg.substr(0, thismsg.find(space));
                    thismsg = thismsg.substr(thismsg.find(space));
                    string password = thismsg.substr(1);
                    //connect
                    if (!ch->connect(host, s_port)) {
                        cout << "Could not connect to the server" << endl;
                    }
                    else {
                        //open socket listener thread
                        new thread(socketListener, this, ch);
                        oss << "CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:" << username << "\npasscode:"
                            << password << "\n\n\0";
                        ch->sendFrameAscii(oss.str(), '\0');
                        user->setUsername(username);
                    }
                }
            }
                break;
            case User::eJoin: {
                string dest = thismsg.substr(1);
                string dest_id = user->addNewDest(dest);
                string receipt = user->receiptGenerator(User::eJoin,stoi(dest_id));
                oss << "SUBSCRIBE\ndestination:/" << dest << "\nid:" << dest_id<< "\nreceipt:" << receipt << "\n\n\0";
                string stomp = oss.str();
                ch->sendFrameAscii(oss.str(), '\0');
            }
                break;
            case User::eExit: {
                string dest = thismsg.substr(1);
                string dest_id = user->getSubIdFromGameName(dest);
                if (dest_id != "") {
                    string receipt = user->receiptGenerator(User::eExit, stoi(dest_id));
                    oss << "UNSUBSCRIBE\nid:" << dest_id << "\nreceipt:" << receipt
                        << "\n\n\0";
                    string stomp = oss.str();
                    ch->sendFrameAscii(oss.str(), '\0');
                }
                else cout << "Cannot exit channel " << dest << " which you are not subscribed to" << endl;
            }
                break;
            case User::eReport: {
                string file_name = thismsg.substr(1);
                names_and_events nae = parseEventsFile(file_name);
                int size=(int)nae.events.size();
                for (int i = 0; i < size; i++) {
                    ostringstream send;
                    send << "SEND\ndestination:/" << nae.team_a_name << "_" << nae.team_b_name << "\n\n" << "user:"
                         << user->getUsername() << "\n" <<
                         "event name:" << nae.events[i].get_name() << "\ntime:" << nae.events[i].get_time()
                         << "\ngeneral game updates:\n";
                    for (auto &kv: nae.events[i].get_game_updates()) {
                        send << "     " << kv.first << ":" << kv.second << "\n";
                    }
                    send << "team a updates:\n";
                    for (auto &kv: nae.events[i].get_team_a_updates()) {
                        send << "     " << kv.first << ":" << kv.second << "\n";
                    }
                    send << "team b updates:\n";
                    for (auto &kv: nae.events[i].get_team_b_updates()) {
                        send << "     " << kv.first << ":" << kv.second << "\n";
                    }
                    send << "\ndescription:" << nae.events[i].get_discription() << "\n\0";
                    ch->sendFrameAscii(send.str(), '\0');
                }


            }
                break;
            case User::eLogout: {
                oss << "DISCONNECT\nreceipt:" << user->receiptGenerator(User::eLogout,-1) << "\n\n\0";
                ch->sendFrameAscii(oss.str(), '\0');
            }
                              break;
            case User::eSummary: {
                boost::trim(thismsg);
                string game_name = thismsg.substr(0, thismsg.find(' '));
                thismsg = thismsg.substr(thismsg.find(' ') + 1);
                string username = thismsg.substr(0, thismsg.find(' '));
                thismsg = thismsg.substr(thismsg.find(' ') + 1);
                string sum = user->getSummary(username, game_name);
                //check summary
                if (sum == "username") {
                    cout << "user not found." << endl;
                }
                else if (sum == "gamename") {
                    cout << "game name not found." << endl;
                }
                //write to file
                else {
                    ofstream file(thismsg);
                    file << sum;
                    file.close();
                }
            }
                break;
            case User::unknown: {
                cout << "Unknown command" << endl;
            }
                break;
        }
    }
}
User::command StompProtocol::check(const std::string& c)
{
    if (c == "login") return User::eLogin;
    if (c == "join") return User::eJoin;
    if (c == "exit") return User::eExit;
    if (c == "report") return User::eReport;
    if (c == "summary") return User::eSummary;
    if (c == "logout") return User::eLogout;
    return User::unknown;
}

StompProtocol::stompCommand StompProtocol::checkStompCommand(const string & c) {
    if (c == "CONNECTED") return eCONNECTED;
    if (c == "ERROR") return eERROR;
    if (c == "RECEIPT") return eRECEIPT;
    if (c == "MESSAGE") return eMESSAGE;
    return eERROR;
}

void StompProtocol::stompToAction(const string &msg) {
    string space = " ";
    string command = msg.substr(0, msg.find('\n'));
    string thismsg = msg.substr(msg.find('\n'));
    switch(checkStompCommand(command)){
        case eCONNECTED:
        {
            user->connect();
            cout << "Login successful" << endl;
        }
            break;
        case eERROR: 
        {
            cout << msg << endl;
            string error = msg.substr(msg.find("message:"));
            error = error.substr(8, error.find('\n')-8);
            cout << error << endl;
            if (error == "Login Error") {
                logout();
            }
        }
            break;
        case eRECEIPT:
        {
            string find_id = thismsg.substr(thismsg.find(':')+1);
            find_id = find_id.substr(0,find_id.find('\n'));
            int receipt_id = stoi(find_id);
            int action_id = user->getActionFromReceipt(receipt_id);
            switch (user->getCommandFromReceipt(receipt_id)) {
                case User::eJoin:
                {
                    cout << "Joined channel " << user->getGamenameFromSubId(action_id) << endl;
                }
                break;
                case User::eExit:
                {
                    cout << "Exited channel " << user->getGamenameFromSubId(action_id) << endl;
                }
                break;
                case User::eLogout:
                {
                    logout();
                }
                break;
                default: break;
            }
        }
            break;
        case eMESSAGE:
        {
            string dots = ":";
            string slash = "/";
            string game_name=(thismsg.substr(thismsg.find(slash)));
            game_name = game_name.substr(1, game_name.find('\n') - 1);      
            string username=thismsg.substr(thismsg.find("user:"));
            username = username.substr(5,username.find('\n')-5);
            string event_name=thismsg.substr(thismsg.find("event name:"));
            event_name = event_name.substr(11, event_name.find('\n') - 11);
            string time=thismsg.substr(thismsg.find("time:"));
            time = time.substr(5, time.find('\n') - 5);
            string game_updates=thismsg.substr(thismsg.find("general game updates:"));
            game_updates = game_updates.substr(22);
            game_updates = game_updates.substr(0,game_updates.find("description:"));
            string description = thismsg.substr(thismsg.find("description:"));
            description = description.substr(12);
            user->update(game_name, username, time, event_name, game_updates,description);
        }
            break;
    }
    //cout << "Message received from the Server:\n" << msg << endl;
}

    void StompProtocol::logout() {
        shouldTerminate = true;
        ch->close();
        user->disconnect();
        cout << "Disconnected" << endl;
        shouldTerminate = false;
    }
