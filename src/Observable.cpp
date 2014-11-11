#include "Observable.h"
#include "Observer.h"

void Observable::Attach(Observer* o) {
  this->observers.insert(o);
}

void Observable::Detach(Observer* o) {
  this->observers.erase(o);
}

void Observable::Notify() {
  std::set<Observer*>::iterator it;
  for(it = observers.begin(); it != observers.end(); ++it) {
    (*it)->Update(this);
  }
}
