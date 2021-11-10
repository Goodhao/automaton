#include "automaton.h"
#include <fstream>
#include <iostream>
using namespace std;

int main() {
	automaton a;
	a.input();
	a.draw("origin");
	automaton b=a.turn_to_NFA();
	b.draw("result");
	return 0;
}
