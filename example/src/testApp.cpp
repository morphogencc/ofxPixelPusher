#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
  ofSetFrameRate(60);
  listener = DiscoveryListener::getInstance();
  ofSetLogLevel("ofThread", OF_LOG_VERBOSE);
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
  std::vector<shared_ptr<PixelPusher> > pushers = listener->getPushers();
  ofLog(OF_LOG_NOTICE, "----------Current Device Registry----------");
  for(auto pusher : pushers) {
    std::cout << "Pusher MAC Address: " << pusher->getMacAddress() << " IP Address: " << pusher->getIpAddress() << " Strips attached: " << pusher->getNumberOfStrips() << " Number of Pixels: " << pusher->getPixelsPerStrip(0) << std::endl;
  }
  for(auto pusher : pushers) {
    auto strips = pusher->getStrips();
    for(auto strip : strips) {
      strip->setPixels(fmodf(ofGetElapsedTimef(), 255), 255, 0);
    }
  }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::exit() {
  if(listener != NULL) {
    listener->stopThread();
  }
}
