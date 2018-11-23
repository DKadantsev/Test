#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>

#include "../Socket/Socket.h"

using namespace std;


const int buffSize = 1024;  //Максимальное число передаваемых байт


int main(int argc, char* argv[])
{
    Socket *socket;
    //cout << argc << endl;
    //cout << argv[0] << endl;

    if(argc > 1)
    {
        cout << argv[1];
        if(strcmp(argv[1], "-udp") == 0)
        {
            cout << "UDP Transfer mode " << endl;
                socket = new SocketUDP("127.0.0.1", 5003);
        }
        else if(strcmp(argv[1], "-tcp") == 0)
        {
            cout << "TCP Transfer mode " << endl;
                socket = new SocketTCP("127.0.0.1", 5001);
        }
        else
        {
                cout << "Incorrect parameter, TCP Transfer mode selected " << endl;
                    socket = new SocketTCP("127.0.0.1", 5001);
        }
    }
    else
    {
        cout << "No parameters, TCP Transfer mode selected " << endl;
            socket = new SocketTCP("127.0.0.1", 5001);
    }
    if(socket->Init() < 0)
    {
        cout << "Socket initialization failed" << endl;
        return 0;
    }
    if(socket->Connect() < 0)
    {
        cout << "Socket connection failed" << endl;
        return 0;
    }


    Sender sender(socket, buffSize);
    string message;

    cout << "Enter message" << endl;
    getline(cin, message);
    sender.SetData(message);
    sender.Send();
    sender.ReceiveAnswer();

    socket->Close();
    delete(socket);

    return 0;


}
