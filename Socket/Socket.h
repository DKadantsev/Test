#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <stdio.h>
#include <cstdlib>
#include <fcntl.h>
#include <queue>
#include <regex>

using namespace std;


class Socket
{
    protected:
        int socketFD;
        int connectFD;
        int port;
        string ip;
        sockaddr_in stSockAddr;
        int getAddress();
        vector<string> dataBuff;

    public:
        bool SetSocketBlockingMode(bool blocking);
        Socket(string ip, uint16_t port);
        Socket(uint16_t port);
        virtual int Bind()=0;
        virtual int Init()=0;
        virtual int Connect()=0;
        int Close();
        virtual int Send(string data)=0;
        virtual int SendResponse(string text)=0;
        virtual int Receive()=0;
        void ClearDataBuff();
        string GetData();
        virtual ~Socket(){};
};

class SocketTCP : public Socket
{
    private:
        int connectFD;
    public:
    SocketTCP(string ip, uint16_t port) : Socket(ip, port){}
    SocketTCP(uint16_t port) : Socket(port){}
        int Init();
        int Connect();
        int Listen();
        int Bind();
        int Accept();
        int Send(string data);
        int SendResponse(string text);
        int CloseConnection();
        int Receive();

        ~SocketTCP(){}


};

class SocketUDP : public Socket
{
private:
    sockaddr_in senderAddr;
public:
    SocketUDP(string ip, uint16_t port) : Socket(ip, port){}
    SocketUDP(uint16_t port) : Socket(port){}
    int Init();
    int Bind();
    int Connect();
    int Send(string text);
    int Receive();
    int SendResponse(string text);
    ~SocketUDP(){}
};

class Sender
{
    private:
        Socket *socket;
        string source;
        int partSize;
        vector<string> parts;
        vector<int> values;
    public:
        void extraxtValues(string s);
        int SendAnswer();
        Sender(Socket *soket);
        Sender(Socket *soket, int partSize);
        int Send();
        int Send(bool response);
        void SetData(string s);
        void AddPart(string s);
        void Reset();
        void ReceiveAnswer();
};


