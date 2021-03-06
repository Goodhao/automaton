#include "automaton.h"
#include <fstream>
#include <iostream>
#include <windows.h>
using namespace std;

int main() {
	cout << "What do you want to do ? Please input the number." << endl;
	cout << "1.Turn an eps-NFA to a NFA" << endl;
	cout << "2.Turn an eps-NFA to a DFA" << endl;
	cout << "3.Turn a NFA to a DFA" << endl;
	cout << "4.Turn a FA to a right grammar" << endl;
	cout << "5.Turn a FA to a left grammar ( not allow inputing Z, which keeps for special meaning )" << endl;
	cout << "6.Just Draw Picture of FA" << endl;
	cout << "7.Turn a FA to its regular expression ( not allow inputing X or Y, which keep for special meaning )" << endl;
	cout << "8.Minimalize a DFA" << endl;
	cout << "9.Turn a regular expression to a FA" << endl;
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
		automaton c = b.turn_to_DFA();
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
		a.output_grammar("right","right_grammar");
		cout << " right grammar has been saved at right_grammar.txt" << endl;
	}
	else if (num == 5) {
		automaton a;
		a.input();
		a.output_grammar("left", "left_grammar");
		cout << " left grammar has been saved at left_grammar.txt" << endl;
	}
	else if (num == 6) {
		automaton a;
		a.input();
		a.draw("draw");
	}
	else if (num == 7) {
		automaton a;
		a.input();
		vector<string> order;
		cout << "please input to-delete states in order, you can input # to automaticlly do it" << endl;
		string s = "";
		while (s == "") getline(cin, s);
		if (s != "#") {
			stringstream ss(s);
			while (ss >> s) {
				order.push_back(s);
			}
		}
		a.to_reg("reg",order);
	}
	else if (num == 8) {
		automaton a;
		a.input();
		a.minimalization();
		a.draw("draw");
	}
	else if (num == 9) {
		string s;
		cout << "Please input the regular expression:" << endl;
		cin >> s;
		automaton a;
		a = a.to_FA(s);
		a.draw("draw");
		automaton b = a.turn_to_NFA();
		automaton c = b.turn_to_DFA();
		c = c.simplify();
		c.minimalization();
		c.draw("DFA");
	}
	Sleep(1000);
	return 0;
}
