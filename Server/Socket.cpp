#include "Socket.h"

Socket::Socket(string ip, uint16_t port) : ip(ip), port(port)
{

}

Socket::Socket(uint16_t port) : port(port)
{

}

void Socket::getAddress()
{
    memset(&stSockAddr, 0, sizeof (stSockAddr));
    stSockAddr.sin_family = PF_INET;
    stSockAddr.sin_port = htons(port);
    if(ip.length() < 8)
        stSockAddr.sin_addr.s_addr = 0;//htonl(INADDR_ANY);
    else
    {
        char *_ip = new char[ip.length() + 1];
        strcpy(_ip, ip.c_str());
        cout << "IP: " << _ip << endl;
        int i32Res = inet_pton(PF_INET, _ip, &stSockAddr.sin_addr);

        if (i32Res < 0) {
            perror("Ошибка: первый параметр не относится к категории корректных адресов");
            close(socketFD);

        } else if (!i32Res) {
            perror("Ошибка: второй параметр не содержит корректного IP-адреса");
            close(socketFD);

        }
      }
        return;
}

bool Socket::SetSocketBlockingMode(bool blocking)
{
   if (socketFD < 0) return false;


   int flags = fcntl(socketFD, F_GETFL, 0);
   cout << flags << endl;
   if (flags == -1) return false;
   flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
   cout << flags << endl;
   return (fcntl(socketFD, F_SETFL, flags) == 0) ? true : false;

}

int SocketTCP::Init()
{
    socketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    cout << "TCP id: " << socketFD << endl;
    if(socketFD == -1)
    {
        printf("Socket creation error!\n");
        return -1;
    }
    else
        printf("Socket created!\n");

    getAddress();
}

int SocketTCP::Connect()
{
    if (connect(socketFD, (struct sockaddr*) &stSockAddr, sizeof (stSockAddr)) == -1)
    {
        perror("Connection error: ");
        close(socketFD);
        return -1;
    }
}

int SocketTCP::Listen()
{
    if (listen(socketFD, 10) == -1) {
        perror("Ошибка: прослушивания");

        close(socketFD);
        return -1;
    }
}

int SocketTCP::Bind()
{
    cout << "Binding TCP";
    cout << stSockAddr.sin_addr.s_addr <<  " " << stSockAddr.sin_port << endl;


        if (bind(socketFD, (struct sockaddr*) &stSockAddr, sizeof (stSockAddr)) == -1) {
            perror("Ошибка: связывания");

            close(socketFD);
            return -1;
        }


}

int SocketUDP::Bind()
{
    cout << "Binding UDP";
    cout << stSockAddr.sin_addr.s_addr <<  " " << stSockAddr.sin_port << endl;


        if (bind(socketFD, (struct sockaddr*) &stSockAddr, sizeof (stSockAddr)) == -1) {
            perror("Ошибка: связывания");

            close(socketFD);
            return -1;
        }


}

int SocketTCP::Accept()
{
    connectFD = accept(socketFD, 0, 0);
    cout << "Accepting" << endl;
    if (connectFD < 0) {
             cout << "accept error" << endl;
            // perror("Ошибка: принятия");
             //close(i32SocketFD);
        return -1;
    }

    return connectFD;
}




int SocketTCP::Send(const char* data)
{
    //cout << "Sending " << pot<< endl;
    //socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
     int bytesSent = send(connectFD, (void*)data, sizeof(data), 0);
     cout << "Sending " << data << " " << sizeof(data) << endl;
     cout << sizeof(data) << " " << bytesSent << endl;
     if (bytesSent < 0)
     {

     }
}

int SocketTCP::Receive()
{
    cout << "Receiving" << endl;
    char buff[1024];
    //socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
    int bytesReceived = recv(connectFD,(void*)&buff,1024,0);
    cout << bytesReceived << endl;
    if(bytesReceived > 0)
            cout << "Received data: " << buff << endl;
    return bytesReceived;
}

int SocketTCP::CloseConnection()
{
    int r =close(socketFD);
    shutdown(socketFD, SHUT_RDWR);

    return r;
}

int SocketUDP::Init()
{
    socketFD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    cout << "UDP id: " << socketFD << endl;
    if(socketFD == -1)
    {
        printf("Socket creation error!\n");
         return -1;
    }
    else
        printf("Socket created!\n");

    getAddress();
}


int SocketUDP::Send(const char* array)
{

     int bytesSent = sendto(socketFD, (void*)array, sizeof(array), 0, ( struct sockaddr* )&stSockAddr, sizeof( struct sockaddr_in));
     cout << sizeof(array) << " " << bytesSent << endl;
     if (bytesSent < 0)
     {

     }
}

int SocketUDP::Receive()
{
    char Hz[1024];
    if(recv(connectFD,(void*)&Hz,1024,0)>0){
          printf("Received from server: %s\n",Hz);
      }
      else
          perror("Server response failed");
}

Sender::Sender(Socket *socket, string source, int partSize): socket(socket), source(source), partSize(partSize){}
Sender::Sender(Socket *socket, string source): socket(socket), source(source), partSize(0){}

vector<string> Sender::Parts()
{

    vector<string> array;

    int p = 0;
    int len = source.length();
    while(p < len)
    {
        array.push_back(source.substr(p, p + partSize < len ? partSize : len-p ));
        p+=partSize;
    }

    return array;
}

int Sender::Send()
{
    vector<string> array = Parts();
    for(vector<string>::iterator it = array.begin(); it != array.end(); it++)
    {
        const char *text = new char[(*it).length()];
        text = (*it).c_str();
        socket->Send(text);
        socket->Receive();
    }

}





