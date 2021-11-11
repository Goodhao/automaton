#include "automaton.h"
#include <fstream>
#include <iostream>
using namespace std;

int main() {
	automaton a;
	a.input();
	a.output_grammer("DFA_grammer");
	return 0;
}
