#include "automaton.h"
#include <fstream>
#include <iostream>
using namespace std;

int main() {
	automaton a;
	a.input();
	a.draw("origin2");
	automaton b=a.turn_to_DFA();
	b.draw("result2");
	return 0;
}
