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

using namespace std;


class Socket
{
    protected:
        int socketFD;
        int connectFD;
        int port;
        string ip;
        sockaddr_in stSockAddr;
        void getAddress();
        char buffer[1024];
        std::queue<std::vector<char[1024]> > data;

    public:
        bool SetSocketBlockingMode(bool blocking);
        Socket(string ip, uint16_t port);
        Socket(uint16_t port);
        virtual int Bind()=0;
        virtual int Init()=0;
        virtual int Send(const char* text){}
        virtual int Receive(){}

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
        int Send(const char* text);
        int CloseConnection();
        int Receive();
        ~SocketTCP(){}


};

class SocketUDP : public Socket
{
public:
    SocketUDP(string ip, uint16_t port) : Socket(ip, port){}
    SocketUDP(uint16_t port) : Socket(port){}
    int Init();
    int Bind();
    int Send(const char* text);
    int Receive();
    ~SocketUDP(){}
};

class Sender
{
    private:
        Socket *socket;
        string source;
        int partSize;
    public:
        Sender(Socket *soket, string source);
        Sender(Socket *soket, string source, int partSize);
        vector<string> Parts();
        int Send();
};


