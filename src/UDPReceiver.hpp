/*
* PixelPusher
* Dec 2014
* nathan lachenmyer
* http://morphogen.cc/
*/

#pragma once

#include <string>

struct UDPMessage {
	std::string content;
	std::string source_ip;
	unsigned short source_port;

	UDPMessage(std::string msg) {
		content = msg;
	};
};

class UDPReceiver {
public:
	UDPReceiver(int port);
	UDPReceiver(const std::string &localAddress, int port);
private:
	virtual void on_receive(); //callback 
};