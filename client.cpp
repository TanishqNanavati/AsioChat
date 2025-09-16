#include<bits/stdc++.h>
#include "message.hpp"
#include<boost/asio.hpp>

using boost::asio::ip::tcp;

using namespace std;

void async_read(tcp::socket &socket){
    auto buffer = make_shared<boost::asio::streambuf>();
    boost::asio::async_read_until(socket,*buffer,"\n",[&socket,buffer]
        (boost::system::error_code ec,size_t length){
            if(!ec){
                istream is(buffer.get());
                string received;
                getline(is,received);
                cout<<"Server : "<<received<<endl;
                async_read(socket);
            }
    });
}

int main(int argc,char *argv[]){
    if(argc < 2){
        cerr<<"Provide port too"<<endl;
        return 1;
    }

    cout << "This is client " << endl;

    boost::asio::io_context io_context;
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);

    boost::asio::connect(socket,resolver.resolve("127.0.0.1",argv[1]));
    async_read(socket);
    thread t([&io_context,&socket](){
        while(true){
            string data;
            cout<<"Enter the message : ";
            getline(cin,data);
            data+="\n";
            boost::asio::write(socket,boost::asio::buffer(data));
        }
    });

    io_context.run();
    t.join();
    return 0;
}
