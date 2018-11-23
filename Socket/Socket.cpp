#include "Socket.h"

//Socket------------------------------------------------------------------

Socket::Socket(string ip, uint16_t port) : ip(ip), port(port), connectFD(0){}

Socket::Socket(uint16_t port) : port(port), connectFD(0){}

string Socket::GetData()
{
    string s;
    s = accumulate(dataBuff.begin(), dataBuff.end(), s);
    return s;
}

int Socket::Close()
{
    close(socketFD);
}


int Socket::getAddress()
{
    memset(&stSockAddr, 0, sizeof (stSockAddr));
    stSockAddr.sin_family = PF_INET;
    stSockAddr.sin_port = htons(port);
    regex r("^([1-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])(\.([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])){3}$");
    if(!regex_match(ip, r))
    {
        cout << "Incorrect IP" << endl;
        stSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        char *_ip = new char[ip.length() + 1];
        strcpy(_ip, ip.c_str());
        int i32Res = inet_pton(PF_INET, _ip, &stSockAddr.sin_addr);

        if (i32Res < 0) {
            perror("Error in first parameter");
            close(socketFD);
            return -1;

        } else if (!i32Res) {
            perror("Error in second parameter");
            close(socketFD);
            return -1;

        }
      }
        return 0;
}

bool Socket::SetSocketBlockingMode(bool blocking)
{
   if (socketFD < 0) return false;

   int flags = fcntl(socketFD, F_GETFL, 0);
   if (flags == -1) return false;
   flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
   return (fcntl(socketFD, F_SETFL, flags) == 0) ? true : false;

}


void Socket::ClearDataBuff()
{
    dataBuff.clear();
}

//SocketTCP-----------------------------------------------------------------------------------------------
int SocketTCP::Init()
{
    socketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(socketFD == -1)
    {
        cout << "Socket creation error!" << endl ;
        return -1;
    }
    else
        cout << "TCP Socket created" << endl;
    getAddress();
}

int SocketTCP::Connect()
{
    connectFD = connect(socketFD, (struct sockaddr*) &stSockAddr, sizeof (stSockAddr));
    if (connectFD == -1)
    {
        perror("Connection error: ");
        close(socketFD);
        return -1;
    }
}

int SocketTCP::Listen()
{
    if (listen(socketFD, 10) == -1) {
        perror("Listening error ");
        close(socketFD);
        return -1;
    }
}

int SocketTCP::Bind()
{

    if (bind(socketFD, (struct sockaddr*) &stSockAddr, sizeof (stSockAddr)) == -1)
    {
         perror("Binding error");
         close(socketFD);
         return -1;
    }
}

int SocketTCP::Accept()
{
    connectFD = accept(socketFD, 0, 0);
    if (connectFD < 0)
    {
        return -1;
    }
    return connectFD;
}

int SocketTCP::Send(string data)
{
    int FD = socketFD;
    if(connectFD > 0)
        FD = connectFD;
    int bytesToSend = data.length();
    char *buff = new char[data.length()];
    strcpy (buff, data.c_str());
  //  cout << "Sending " << data << " Bytes: " << bytesToSend << endl;
     int bytesSent = send(FD, (void*)buff, bytesToSend, 0);
     if (bytesSent < 0)
         cout << "Sending error " << endl;

}

int SocketTCP::SendResponse(string text)
{
    Send(text);
}

int SocketTCP::Receive()
{
    int FD = socketFD;
    if(connectFD > 0)
        FD = connectFD;
    char buff[1024];
    memset(buff, '\0', 1024);
    int bytesReceived = recv(FD,(void*)&buff,sizeof(buff),0);
    if(bytesReceived > 0)
    {
        //cout << bytesReceived << endl;
        char *data = new char[bytesReceived];
        strcpy (data, buff);
        data[bytesReceived] = '\0';
        dataBuff.push_back(data);
        cout << "after push" << endl;
        //cout << "Received data: " << data << " Bytes: " << bytesReceived << endl;
    }
    return bytesReceived;
}

int SocketTCP::CloseConnection()
{
    int r =close(connectFD);
    shutdown(connectFD, SHUT_RDWR);

    return r;
}



//SocketUDP-------------------------------------------------------------
int SocketUDP::Bind()
{
    if (bind(socketFD, (struct sockaddr*) &stSockAddr, sizeof (stSockAddr)) == -1)
    {
            perror("Ошибка: связывания");

            close(socketFD);
            return -1;
     }
}

int SocketUDP::Connect()
{
    return 1;
}

int SocketUDP::Send(string data)
{
    int bytesToSend = data.length();
    char *buff = new char[data.length()];
    strcpy (buff, data.c_str());
    socklen_t peer_addr_len = sizeof(struct sockaddr_storage);
     int bytesSent = sendto(socketFD, (void*)buff, bytesToSend, 0, (struct sockaddr *) &stSockAddr, peer_addr_len);
    // cout << "Sending " << data << " " << sizeof(data) << endl;
    // cout << sizeof(data) << " " << bytesSent << endl;
     if (bytesSent < 0)
            cout << "Sending error " << endl;
   //  else
     //    cout << "Sending succesful. Bytes sended: " << bytesSent << endl;
}

int SocketUDP::SendResponse(string data)
{
    int bytesToSend = data.length();
    char *buff = new char[data.length()];
    strcpy (buff, data.c_str());
    socklen_t peer_addr_len = sizeof(senderAddr);
     int bytesSent = sendto(socketFD, (void*)buff, bytesToSend, 0, (struct sockaddr *) &senderAddr, peer_addr_len);
    // cout << "Sending " << data << " " << sizeof(data) << endl;
    // cout << sizeof(data) << " " << bytesSent << endl;
     if (bytesSent < 0)
            cout << "Sending error " << endl;
  //   else
        // cout << "Sending succesful. Bytes sended: " << bytesSent << endl;
}



int SocketUDP::Receive()
{
    char buff[1024];
    memset(buff, '\0', 1024);
   socklen_t peer_addr_len = sizeof(senderAddr);
    int bytesReceived = recvfrom(socketFD,(void*)&buff,1024,0, (struct sockaddr *) &senderAddr, &peer_addr_len);
    string s;
    if(bytesReceived > 0)
    {
        char *data = new char[bytesReceived];
        strcpy (data, buff);
        s = data;
        dataBuff.push_back(data);
        //cout << "Received data: " << s << "Bytes: " << bytesReceived << endl;
    }
    return bytesReceived;
}


int SocketUDP::Init()
{
    socketFD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(socketFD == -1)
    {
        cout << "Socket creation error!" << endl;
         return -1;
    }
    else
        cout << "UDP Socket created!\n" << endl;

    getAddress();
}

//Sender------------------------------------------------------------------

Sender::Sender(Socket *socket, int partSize): socket(socket), partSize(partSize){}
Sender::Sender(Socket *socket): socket(socket), partSize(0){}


int Sender::Send()
{
    Send(false);
}

int Sender::Send(bool response)
{
    for(vector<string>::iterator it = parts.begin(); it != parts.end(); it++)
    {
        const char *text = new char[(*it).length()];
        text = (*it).c_str();
        if(!response)
            socket->Send(text);
        else
        {
            socket->SendResponse(text);
         }
    }
}

void Sender::AddPart(string s)
{
    parts.push_back(s);
}

void Sender::SetData(string s)
{
    regex re("[^u0000-u007F]+");

    source = regex_replace(s.c_str(),
                     re,  "");
    parts.clear();

    int p = 0;
    int len = source.length();
    while(p < len)
    {
        parts.push_back(source.substr(p, p + partSize < len ? partSize : len-p ));
        p+=partSize;
    }
}

void Sender::extraxtValues(string s)
{
    char temp[50];
    int n=0;
    bool isDigit = false;
    for (int i=0; s[i]!='\0'; i++)
       {
           if ((s[i]>='0')&&(s[i]<='9'))
           {
               temp[n]=s[i];//-'0';
               n++;
               isDigit = true;
           }
           else
           {
               if(isDigit)
               {
                   n=0;
                   int number = atoi(temp);
                   memset(temp, '\0', 50);
                   values.push_back(number);
                   isDigit = false;
               }
           }
       }
    if(isDigit)
    {
        int number = atoi(temp);
        values.push_back(number);
    }
        sort(values.begin(), values.end());
}

int Sender::SendAnswer()
{
    string result;
    for(vector<int>::iterator iter = values.begin(); iter != values.end(); iter++)
    {        
        result.append(to_string(*iter) + " ");
    }

    int summ = accumulate(values.begin(), values.end(), 0);
    result.append(to_string(summ));
    cout << "Result " << result << endl;
    parts.clear();
    int p = 0;
    int len = result.length();
    while(p < len)
    {
        parts.push_back(result.substr(p, p + partSize < len ? partSize : len-p ));
        p+=partSize;
    }
    Send(true);
}

void Sender::Reset()
{
    values.clear();
    source.clear();
    parts.clear();
}
void  Sender::ReceiveAnswer()
{
    int bytesReceived = socket->Receive();
    if(bytesReceived >= partSize)
    {
        do
        {
           bytesReceived = socket->Receive();
        } while (bytesReceived >= partSize);
    }
     string r = socket->GetData();
     cout << r << r.length() << endl;
     for(int i = r.length()-1; i > 0 ; i--)
     {
         if(r[i] == ' ')
         {
             r.replace(i,1, "\n");
             break;
         }

     }
     r[bytesReceived] = '\0';
     cout << "Server response: " << r << endl;

}

