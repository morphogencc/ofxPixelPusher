/*
 * PPObserver
 * Aug 2014
 * nathan lachenmyer
 * An implementation of the observer class to observe the device registry.
 */
#pragma once

#include <iostream>
#include "Observer.h"

class PPObserver : public Observer {
  public:
  PPObserver();
  ~PPObserver();
  void Update(Observable* o);

  bool hasStrips;
};
