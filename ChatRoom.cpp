#include<bits/stdc++.h>
#include "ChatRoom.hpp"
#include "message.hpp"
using namespace std;

void Room::join(ParticipantPtr participant){
    this->participants.insert(participant);
}

void Room::leave(ParticipantPtr Participant){
    this->participants.erase(Participant);
}

void Room::deliver(ParticipantPtr participant,Message &message){
    messageQueue.push_back(message);
    while(!messageQueue.empty()){
        Message msg = messageQueue.front();
        messageQueue.pop_front();

        for(ParticipantPtr par:participants){
            if(participant != par){
                par->write(msg);
            }
        }
    }
}


Session::Session(tcp::socket s, Room &room)
    : clientSocket(std::move(s)), room(room), id(nextId++) {}


void Session::async_read(){
    auto self(shared_from_this());
    boost::asio::async_read_until(clientSocket,buffer,'\n',[this,self]
        (boost::system::error_code ec,size_t bytes_transffered){
            if(!ec){
                string data(boost::asio::buffers_begin(buffer.data()),boost::asio::buffers_begin(buffer.data())+bytes_transffered);
                buffer.consume(bytes_transffered);
                cout << "Received : client " << id << " : " << data << endl;
                Message message(data);
                deliver(message);
                async_read();
            }else{
                room.leave(shared_from_this());
                if(ec==boost::asio::error::eof){
                    cout<<"Connection closed by peer"<<endl;
                }
                else {
                    cout<<"Read error : "<<ec.message()<<endl;
                }
            }
    });
}

void Session::async_write(string messageBody,size_t messageLength){
    auto write_handler = [&](boost::system::error_code ec,size_t bytes_transffered){
        if(!ec){
            cout<<"Data is written to the socket"<<endl;
        }else{
            cout<<"Write error : "<<ec.message()<<endl;
        }
    };

    boost::asio::async_write(clientSocket,boost::asio::buffer(messageBody,messageLength),write_handler);
}

void Session::write(Message &message){
    messageQueue.push_back(message);
    while(!messageQueue.empty()){
        Message msg = messageQueue.front();
        messageQueue.pop_front();
        bool decode_header = msg.decodeHeader();
        if(decode_header){
            string body = msg.getBody();
            async_write(body,msg.getBodyLength());
        }
        else{
            cout<<"Message length exceeds the limit of max length"<<endl;
        }
    }
}

void Session::deliver(Message &message) {
    std::string taggedMsg = "client " + std::to_string(id) + " : " + message.getBody();
    Message newMessage(taggedMsg);
    room.deliver(shared_from_this(), newMessage);
}


void Session::start(){
    room.join(shared_from_this());
    async_read();
}

using boost::asio::ip::address_v4;

void accept_connection(boost::asio::io_context &io,char* port,tcp::acceptor &acceptor,Room &room,const tcp::endpoint &endpoint){
    tcp::socket socket(io);
    acceptor.async_accept([&](boost::system::error_code ec,tcp::socket socket){
        if(!ec){
            std::shared_ptr<Session> session = std::make_shared<Session>(std::move(socket),room);
            session->start();
        }
        accept_connection(io,port,acceptor,room,endpoint);
    });
}


int main(int argc,char *argv[]){
    try{
        if(argc<2){
            cerr<<"Usage : server <port>"<<endl;
            return 1;
        }
        Room room;
        boost::asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(),atoi(argv[1]));
        tcp::acceptor acceptor(io_context,endpoint);

        accept_connection(io_context,argv[1],acceptor,room,endpoint);

        io_context.run();

    }
    catch(exception &e){
        cerr<<"Exception : "<<e.what()<<endl;
        return 1;
    }
}