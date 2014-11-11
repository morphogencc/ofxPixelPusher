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
  udpConnection.BindMcast("0.0.0.0",portNo);
  udpConnection.SetNonBlocking(true);

  newMessage = 0;
}

void DiscoveryListener::update() {
  char udpMessage[incomingPacketSize];
  newMessage = udpConnection.Receive(udpMessage,incomingPacketSize); //returns number of bytes received
  unsigned char *message = (unsigned char *)udpMessage;
  std::cout << "Packet Size: " << newMessage << std::endl;
  std::cout << "UDP Packet: " << std::endl;
  if(newMessage != -1) {
    DeviceHeader* header;
    header = new DeviceHeader(&message, incomingPacketSize);
    std::string readout = header->getMacAddressString();
    std::cout << readout << std::endl;
  }
}
