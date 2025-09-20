// -*- mode: c++ -*-
// $Id: objinst.g++,v 1.10 2001/10/09 23:52:37 doug Exp $
// http://www.bagley.org/~doug/shootout/

#include <stdlib.h>
#include <iostream>

using namespace std;

class Toggle {
public:
    Toggle(bool start_state) : state(start_state) { }
    virtual ~Toggle() {  }
    bool value() {
	return(state);
    }
    virtual Toggle& activate() {
	state = !state;
	return(*this);
    }
    bool state;
};

class NthToggle : public Toggle {
public:
    NthToggle(bool start_state, int max_counter) :
	Toggle(start_state), count_max(max_counter), counter(0) {
    }
    Toggle& activate() {
	if (++this->counter >= this->count_max) {
	    state = !state;
	    counter = 0;
	}
	return(*this);
    }
private:
    int count_max;
    int counter;
};

int
main(int argc, char *argv[]) {
    int i,n = ((argc == 2) ? atoi(argv[1]) : 1);

    Toggle *toggle1 = new Toggle(true);
    for (i=0; i<5; i++) {
	cout << ((toggle1->activate().value()) ? "true" : "false") << endl;
    }
    delete toggle1;
    for (i=0; i<n; i++) {
	Toggle *toggle = new Toggle(true);
	delete toggle;
    }
    
    cout << endl;
    
    NthToggle *ntoggle1 = new NthToggle(true, 3);
    for (i=0; i<8; i++) {
	cout << ((ntoggle1->activate().value()) ? "true" : "false") << endl;
    }
    delete ntoggle1;
    for (i=0; i<n; i++) {
	NthToggle *ntoggle = new NthToggle(true, 3);
	delete ntoggle;
    }
    return 0;
}
