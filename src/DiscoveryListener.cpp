/*
 * DiscoveryListener
 * Aug 2014
 * nathan lachenmyer
 */

#include "DiscoveryListener.h"
#include "DeviceHeader.h"

DiscoveryListener::DiscoveryListener() {
  udpConnection.Create();
  udpConnection.SetEnableBroadcast(true);
  //  udpConnection.BindMcast("0.0.0.0",portNo);
  udpConnection.Bind(portNo);
  udpConnection.SetNonBlocking(true);

  newMessage = 0;
  incomingUdpMessage.reserve(incomingPacketSize + 1);
  std::fill(incomingUdpMessage.begin(), incomingUdpMessage.end(), 0);
  udpMessage.reserve(incomingPacketSize + 1);
  std::fill(udpMessage.begin(), udpMessage.end(), 0);
}

void DiscoveryListener::update() {
  newMessage = udpConnection.Receive(&incomingUdpMessage[0],incomingPacketSize); //returns number of bytes received
  std::copy(incomingUdpMessage.begin(), incomingUdpMessage.end(), udpMessage.begin());
  std::cout << "Packet Size: " << newMessage << std::endl;
  std::cout << "UDP Packet: " << std::endl;
  if(newMessage != -1) {
    DeviceHeader* header;
    header = new DeviceHeader(&udpMessage[0], incomingPacketSize);
    std::string readout = header->getMacAddressString();
    std::cout << readout << std::endl;
  }
}
