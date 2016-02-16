#include "testApp.h"

using namespace ofxPixelPusher;
//--------------------------------------------------------------
void testApp::setup(){
  mListener = DiscoveryListener::getInstance();
  mCount = 1;
  mIncrement = 1;
}

//--------------------------------------------------------------
void testApp::update() {
	mListener->receive();
  for (auto pusher : mListener->getPushers()) {
    for (auto strip : pusher->getStrips()) {
      strip->setPixels(255 - mCount, 0, mCount);
    }
    mCount += mIncrement;
    if (mCount > 255 || mCount < 0) {
      mIncrement *= -1.0;
    }
  }
}

//--------------------------------------------------------------
void testApp::draw(){
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
}
