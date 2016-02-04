/*
* PixelPusher
* Dec 2014
* nathan lachenmyer
* http://morphogen.cc/
*/

#pragma once

#include <array>
#include <string>

class UDPSender {
public:
	UDPSender(int localPort);
	UDPSender(const std::string &localAddress, int localPort=0);
private:
	virtual void on_send();
};