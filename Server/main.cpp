#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include "../Socket/Socket.h"

using namespace std;

const int buffSize = 1024;  //Максимальное число передаваемых байт

int main()
{

    SocketTCP socketTCP("127.0.0.1",5001);
    SocketUDP socketUDP ( "127.0.0.1", 5003);

    socketTCP.Init();
    socketUDP.Init();

    socketTCP.Bind();
    socketUDP.Bind();

    socketTCP.Listen();

    socketUDP.SetSocketBlockingMode(false);
    socketTCP.SetSocketBlockingMode(false);

    Sender senderTCP(&socketTCP, buffSize);
    Sender senderUDP(&socketUDP, buffSize);
    string data;
    while(true)
    {
        sleep(1);
       int bytesReceived = 0;
       if(socketTCP.Accept() > 0)
       {
         cout << "Incoming TCP connection" << endl;
         senderTCP.Reset();
         do
         {
            bytesReceived = socketTCP.Receive();

            if(bytesReceived < 1)
            {                
                socketTCP.CloseConnection();
                break;
            }
        } while (bytesReceived >= buffSize);

         string receivedData = socketTCP.GetData();
         cout << "Received data: " << receivedData << endl;
         senderTCP.extraxtValues(receivedData);
         senderTCP.SendAnswer();
         senderTCP.Reset();
         socketTCP.ClearDataBuff();
       }
       bytesReceived = socketUDP.Receive();
       if(bytesReceived > 0)
       {
           do
           {
              bytesReceived = socketUDP.Receive();
           } while (bytesReceived >= buffSize);
           senderUDP.extraxtValues(socketUDP.GetData());
           senderUDP.SendAnswer();
           senderUDP.Reset();
           socketUDP.ClearDataBuff();

       }

    }

        return 0;
}
