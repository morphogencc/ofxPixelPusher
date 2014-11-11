/*
 * PPObserver
 * Aug 2014
 * nathan lachenmyer
 * An implementation of the observer class to observe the device registry.
 */
#include "PPObserver.h"
#include "Observable.h"

PPObserver::PPObserver() {
  hasStrips = false;
}

PPObserver::~PPObserver() {
}

void PPObserver::Update(Observable* o) {
  std::cout << "Registry changed!";
  if(o != NULL) {
    std::cout << "Device change: " << o;
  }
  hasStrips = true;
}
