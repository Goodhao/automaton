#include "grammar.h"
#include <fstream>
#include <iostream>
#include <windows.h>
using namespace std;

int main() {
	grammar a;
	a.input();
	//a.remove_direct_left_recursion();
	//a.remove_indirect_left_recursion();
	a.pick_up_left_factor();
	a.output();
	return 0;
}
