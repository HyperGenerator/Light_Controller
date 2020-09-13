#pragma once
#include "ofMain.h"
#include <WS2tcpip.h> //for tcp socket
#pragma comment(lib, "ws2_32.lib")//for tcp socket

class WIFI_light_thread: public ofThread{
private:
	//--- private variables
	bool					is_connected;
	std::string				ip_address;
	unsigned short			port;
	std::vector<ALight*>	alight_ptr; //in order to access the pointers of all lights that need to be sent
	//---socket variables
	WSADATA* data =			new WSADATA;
	SOCKET* sock =			new SOCKET;
	sockaddr_in* hint =		new sockaddr_in;
	//--- private member functions
	void socket_setup() {
		_print_message("establishing connection to " + ofToString(ip_address) + ":" + ofToString(port));
		int wsResult = WSAStartup(MAKEWORD(2, 2), data);
		if (wsResult != 0) {
			_print_message("Can't start Winsock, Err#" + to_string(wsResult));
			return;
		}


		*sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (*sock == INVALID_SOCKET) {
			_print_message("Can't create socket, Err #"+to_string(WSAGetLastError()));
			return;
		}

		
		
		//fill in a hint structure
		hint->sin_family = AF_INET;
		hint->sin_port = htons(port);
		inet_pton(AF_INET, ip_address.c_str(), &(hint->sin_addr));
		//connect to server
		if (connect(*sock, (sockaddr*)hint, sizeof(*hint)) == SOCKET_ERROR) {
			_print_message("Can't connect to server, Err #" +to_string(WSAGetLastError()));
			closesocket(*sock);
			WSACleanup();
		}

		else {
			_print_message(ofToString(ip_address) + ":" + ofToString(port) + " Connection established");
			is_connected = true;
		}


	}
	void socket_cleanup() {
		if (is_connected) {
			closesocket(*sock);
			WSACleanup();
			std::cout << ofToString(ip_address) << ":" << ofToString(port) << " Disconnected" << std::endl;
		}
		delete data;
		delete sock;
		delete hint;
	}
	int send_data() {
		//new protocol: SND<3char channel><value>SNE	e.g SND001255004242SNE
		std::string _final_send_string = "SND";
		const int BUF_SIZE = 256; //answer buffer size
		char answ_buf[BUF_SIZE];  //do nothing with answer

		for (auto& _alight : alight_ptr) {
			for (auto& _achannel : _alight->channel_list) {
				_final_send_string.append(_int_to_3char_string(_achannel.channel +1 )); //_achannel.channel + 1 due to channels begining with 0!!
				_final_send_string.append(_int_to_3char_string(_achannel.value));
			}
		}
		_final_send_string.append("SNE");
		//eventually send string
		int sendResult = send(*sock, _final_send_string.c_str(), _final_send_string.size() + 1, 0); //send data to socket
		int bytesReceived = recv(*sock, answ_buf, BUF_SIZE, 0); //receive data (tcp is two-way)
		std::cout << answ_buf << std::endl;
		return 1;
	}
	std::string _int_to_3char_string(int _int)
	{
		//convert int to a 3 digit string e.g    12 -> "012"
		std::string a = to_string(_int);
		std::string s = "000";
		if (a.size() == 1)
			s[2] = a[0];

		else if (a.size() == 2) {
			s[2] = a[1];
			s[1] = a[0];
		}
		else
			s = a;

		return s;
	}
	void _print_message(std::string _msg) {
		std::cout << "[" << this->getThreadName() << "] " << _msg << std::endl;
	}
public:
    WIFI_light_thread(std::vector<ALight*> _temp):
		is_connected(false),
		alight_ptr(_temp), 
		ip_address(_temp[0]->ip_address),
		port(_temp[0]->port)
        
	{}
    

	void WIFI_light_thread::threadedFunction() {

        // start
		#if NETWORK_ENABLED == 1
		socket_setup();
		#endif // NETWORK_DEBUG

		
		
        while(isThreadRunning()) {          
            sleep(1000);
            
			//--- send the channels of the lights
			if (is_connected) {
				lock();
				send_data();
				unlock();
			}
				           
            
        }
		socket_cleanup();
        // done
    }
	

};

