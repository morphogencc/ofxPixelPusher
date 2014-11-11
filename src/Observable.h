/*
 * An observable, straight from Gang of Four
 */

#pragma once
#include <set>

class Observer; //forward declaration

class Observable {
 public:
  virtual ~Observable();
  virtual void Attach(Observer* o);
  virtual void Detach(Observer* o);
  virtual void Notify();

 private:
  std::set<Observer*> observers;
};
