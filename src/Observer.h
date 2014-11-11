/*
 * An observer class, straight from Gang of Four
 */

#pragma once

class Observable; //forward declaration

class Observer {
  //Lets keep a track of all the shops we have observing

 public:
  virtual void Update(Observable* o) = 0;
};
