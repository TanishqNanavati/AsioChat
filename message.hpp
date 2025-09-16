#include<bits/stdc++.h>
using namespace std;

#ifndef MESSAGE_HPP
#define MESSAGE_HPP


class Message{
    public:
        Message():bodyLength_(0){}
        enum {maxBytes=512};
        enum {header=4};

        size_t getNewBodyLength(size_t newLength){
            if(newLength>maxBytes) newLength=maxBytes;
            return newLength;
        }

        void encodeHeader(){
            char newHeader[header+1]="";
            sprintf(newHeader,"%4d",static_cast<int>(bodyLength_));
            memcpy(data,newHeader,header);
        }

        bool decodeHeader(){
            char newHeader[header+1]="";
            strncpy(newHeader,data,header);
            newHeader[header]='\0';
            int headerValue = atoi(newHeader);
            if(headerValue > maxBytes){
                bodyLength_=0;
                return false;
            }

            bodyLength_=headerValue;
            return true;
        }

        string getData(){
            int len = header + bodyLength_;
            string result(data,len);
            return result;
        }

        string getBody(){
            string dataStr = getData();
            string result = dataStr.substr(header,bodyLength_);
            return result;
        }

        int getBodyLength(){
            return bodyLength_;
        }

        Message(string message){
            bodyLength_ = getNewBodyLength(message.size());
            encodeHeader();
            memcpy(data+header,message.c_str(),bodyLength_);
        }

    private:
        char data[header+maxBytes];
        size_t bodyLength_;
};

#endif 