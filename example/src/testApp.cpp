#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
  listener = DiscoveryListener::getInstance();
  ofSetLogLevel("ofThread", OF_LOG_VERBOSE);
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
  std::vector<shared_ptr<PixelPusher> > pushers = listener->getPushers();
  ofColor stripColor = ofColor::fromHsb(255*sin(ofGetElapsedTimef()*0.1), 255, 255);
  ofLog(OF_LOG_NOTICE, "Color: %f %f %f", stripColor.getHue(), stripColor.getSaturation(), stripColor.getBrightness());
  for(auto pusher : pushers) {
    pusher->setStripValues(0, stripColor.r, stripColor.g, stripColor.b);
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
