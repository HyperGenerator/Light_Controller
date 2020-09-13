#pragma once
#include "ofMain.h"
#include <WS2tcpip.h> //for tcp socket
#pragma comment(lib, "ws2_32.lib")//for tcp socket

/// *******  BeatRecognitionThread ************
class BeatRecognitionThread : public ofThread {
private:
	//--- private variables
	bool					is_connected;
	std::string				ip_address;
	unsigned short			port;

	//---socket variables
	WSADATA* data = new WSADATA;
	SOCKET* sock = new SOCKET;
	sockaddr_in* hint = new sockaddr_in;
	// answer buffer
	char* answ_buf;

	void socket_setup() {
		_print_message("establishing connection to " + ofToString(ip_address) + ":" + ofToString(port));
		int wsResult = WSAStartup(MAKEWORD(2, 2), data);
		if (wsResult != 0) {
			_print_message("Can't start Winsock, Err#" + to_string(wsResult));
			return;
		}


		*sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (*sock == INVALID_SOCKET) {
			_print_message("Can't create socket, Err #" + to_string(WSAGetLastError()));
			return;
		}



		//fill in a hint structure
		hint->sin_family = AF_INET;
		hint->sin_port = htons(port);
		inet_pton(AF_INET, ip_address.c_str(), &(hint->sin_addr));
		//connect to server
		if (connect(*sock, (sockaddr*)hint, sizeof(*hint)) == SOCKET_ERROR) {
			_print_message("Can't connect to server, Err #" + to_string(WSAGetLastError()));
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
			_print_message("Disconnected");
		}
		delete data;
		delete sock;
		delete hint;
	}
	void _print_message(std::string _msg) {
		std::cout << "[BeatRecognition] " << _msg << std::endl;
	}
	int retrieve_data(std::string& _answer) {
		_answer = std::string(answ_buf);
		return 1; //if successful
	}
public:
	BeatRecognitionThread(std::string _ip, int _port, char* _buffer) :is_connected(false), ip_address(_ip), port(_port), answ_buf(_buffer){

	}
	void BeatRecognitionThread::threadedFunction() {

		// start
#if BEATDETECTION_ENABLED == 1
		socket_setup();
#endif // BEATDETECTION_ENABLED

		while (isThreadRunning()) {
			sleep(1000);
			//--- send the channels of the lights
			if (is_connected) {
				std::string send_msg = "GET";
				int sendResult = send(*sock, send_msg.c_str(), send_msg.size() + 1, 0); //send data to socket
				lock();
				int bytesReceived = recv(*sock, answ_buf, 256, 0); //receive data (tcp is two-way)
				unlock();
			}
		}
		socket_cleanup();
		// done
	}
};

/// *******  BeatRecognitionClass ************
class BeatRecognitionClass{
	private:
		std::string				ip_address;
		unsigned short			port;
		char					answer_buffer[256];

		void setup() {
			activeThread = new BeatRecognitionThread(ip_address, port, answer_buffer);
			activeThread->startThread();			
		}
		void cleanup() {
			if (activeThread->isThreadRunning()) {
				activeThread->waitForThread(true);
				delete activeThread;
				activeThread = nullptr;
			}
		}
		

	public:
		BeatRecognitionThread* activeThread;

		BeatRecognitionClass(std::string _ip, int _port): ip_address(_ip), port(_port), activeThread(nullptr){
			setup();
		}

		int get_answer() {
			//format: "125-hh-mm-ss-msm" ->[BPM]+[hour]+[min]+[seconds]+[miliseconds] = 16 char
			for (int i = 0; i < 16; i++) {
				std::cout << answer_buffer[i];				
			}
			std::cout << std::endl;
			return 1;
		}

		~BeatRecognitionClass() {
			cleanup();
		}
};



