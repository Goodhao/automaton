#include "automaton.h"
#include <fstream>
#include <iostream>
#include <windows.h>
using namespace std;

int main() {
	cout << "What do you want to do ? Please input the number." << endl;
	cout << "1.Turn a eps-NFA to a NFA" << endl;
	cout << "2.Turn a eps-NFA to a DFA" << endl;
	cout << "3.Turn a NFA to a DFA" << endl;
	cout << "4.Turn a DFA to a right grammer" << endl;
	cout << "5.Turn a DFA to a left grammer ( not allow Z, which keeps for special meaning )" << endl;
	cout << "6.Just Draw Picture of FA" << endl;
	int num;
	cin >> num;
	if (num == 1) {
		automaton a;
		a.input();
		automaton b = a.turn_to_NFA();
		b.draw("NFA");
		cout << " NFA picture has been saved at NFA.png" << endl;
	}
	else if (num == 2) {
		automaton a;
		a.input();
		automaton b = a.turn_to_NFA();
		automaton c = a.turn_to_DFA();
		c.draw("DFA");
		cout << " DFA picture has been saved at DFA.png" << endl;
	}
	else if (num == 3) {
		automaton a;
		a.input();
		automaton b = a.turn_to_DFA();
		b.draw("DFA");
		cout << " DFA picture has been saved at DFA.png" << endl;
	}
	else if (num == 4) {
		automaton a;
		a.input();
		a.output_grammer("right","right_grammer");
		cout << " right grammer has been saved at right_grammer.txt" << endl;
	}
	else if (num == 5) {
		automaton a;
		a.input();
		a.output_grammer("left", "left_grammer");
		cout << " left grammer has been saved at left_grammer.txt" << endl;
	}
	else if (num == 6) {
		automaton a;
		a.input();
		a.draw("draw");
	}
	Sleep(1000);
	return 0;
}
