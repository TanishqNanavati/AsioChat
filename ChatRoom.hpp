
#include<bits/stdc++.h>
#include<memory>
#include<sys/socket.h>
#include<unistd.h>
#include<boost/asio.hpp>
#include "message.hpp"
using namespace std;

#ifndef CHATROOM_HPP
#define CHATROOM_HPP


using boost::asio::ip::tcp;

class Participant{
   public:
        virtual void deliver(Message &message)= 0;
        virtual void write(Message &message) = 0;
        virtual ~Participant() = default; 
};

typedef shared_ptr<Participant> ParticipantPtr;

class Room{
    public:
        void join(ParticipantPtr participant);
        void leave(ParticipantPtr participant);
        void deliver(ParticipantPtr participant,Message &message);
    private:
        deque<Message>messageQueue;
        set<ParticipantPtr>participants;
};

class Session : public Participant,public enable_shared_from_this<Session>{
    public:
        Session(tcp::socket s,Room &room);
        void start();
        void deliver(Message &message);
        void write(Message &message);
        void async_read();
        void async_write(string MessageBody,size_t messageLength);

        int getId() const { return id; }

    private:
        tcp::socket clientSocket;
        boost::asio::streambuf buffer;
        Room &room;
        deque<Message>messageQueue;
        int id;
        static inline int nextId=1;

};

#endif