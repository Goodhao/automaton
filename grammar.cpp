#include "grammar.h"
#include <assert.h>
#include <queue>
#include <fstream>
#include <sstream>
#include <windows.h>
using namespace std;
#define cin fin
void grammar::input() {
	ifstream fin("in.txt");
	int n;
	string s;
	cin >> n;
	for (int i = 1; i <= n; i++) {
		do {
			getline(cin, s);
		} while (s == "");
		int pos = s.find("->");
		string name;
		for (int j = 0; j < pos; j++) {
			if (s[j] != ' ') {
				name += s[j];
			}
		}
		vector<string> tmp;
		string name2;
		for (int j = pos + 2; j < (int)s.size(); j++) {
			if (s[j] == '|') {
				g[name].push_back(tmp);
				tmp.clear();
				name2 = "";
			}
			else if (s[j] != ' ') {
				name2 += s[j];
			}
			else if (j > pos + 2 && s[j - 1] != ' ' && s[j - 1] != '|' && s[j - 1] != '>') {
				tmp.push_back(name2);
				name2 = "";
			}
		}
		if (name2 != "") {
			tmp.push_back(name2);
			g[name].push_back(tmp);
			tmp.clear();
			name2 = "";
		}
	}
	int m = 0;
	cin >> m;
	for (int i = 0; i < m; i++) {
		cin >> s;
		terminal.insert(s);
	}
}
void grammar::output() {
	for (auto productions : g) {
		string left_head = productions.first;
		for (auto production : productions.second) {
			cout << left_head << " -> ";
			for (auto item : production) cout << item << " ";
			cout << endl;
		}
	}
}
void grammar::remove_direct_left_recursion(string head) {
	// A -> Ax | Ay | a | b
	// =>
	// A -> aA' | bA'
	// A' -> eps | xA' | yA'
	map<string, vector<vector<string> > > g2;
	map<string, bool> replaced;
	for (auto productions : g) {
		string left_head = productions.first;
		if (left_head != head) continue;
		string right_head;
		string new_head = left_head + string(1,'\'');
		for (auto production : productions.second) {
			right_head = production.front();
			vector<string> remain;
			for (int i = 1; i < production.size(); i++) remain.push_back(production[i]);
			if (left_head == right_head) {
				replaced[left_head] = true;
				if (g2.count(new_head) == 0) {
					vector<string> tmp = { "eps" };
					g2[new_head].push_back(tmp);
				}
				remain.push_back(new_head);
				g2[new_head].push_back(remain);
			}
		}
	}
	for (auto productions : g) {
		string left_head = productions.first;
		string right_head;
		string new_head = left_head + string(1, '\'');
		for (auto production : productions.second) {
			right_head = production.front();
			if (left_head != right_head && replaced[left_head] == true) {
				production.push_back(new_head);
				g2[left_head].push_back(production);
			}
		}
	}
	for (auto productions : g) {
		string left_head = productions.first;
		if (!replaced[left_head]) {
			g2[left_head] = productions.second;
		}
	}
	g = g2;
}
void grammar::remove_indirect_left_recursion() {
	map<string, int> variable2num;
	map<int, string> num2variable;
	int tot = 0;
	for (auto productions : g) {
		string left_head = productions.first;
		if (terminal.count(left_head)) continue;
		if (variable2num[left_head] == 0) {
			variable2num[left_head] = ++tot;
			num2variable[tot] = left_head;
		}
	}
	for (auto data : variable2num) {
		auto variable = data.first;
		bool loop = true;
		while (loop) {
			loop = false;
			for (auto& productions : g) {
				string left_head = productions.first;
				if (left_head != variable) continue;
				string right_head;
				vector<vector<string> > new_productions;
				for (auto production : productions.second) {
					right_head = production.front();
					if (terminal.count(right_head) == 0 && variable2num[left_head] > variable2num[right_head]) {
						loop = true;
						for (auto production2 : g[right_head]) {
							vector<string> tmp;
							for (auto item : production2) tmp.push_back(item);
							for (int i = 1; i < (int)production.size(); i++) tmp.push_back(production[i]);
							new_productions.push_back(tmp);
						}
					}
					else new_productions.push_back(production);
				}
				productions.second = new_productions;
			}
			remove_direct_left_recursion(variable);
		}
	}
}

void grammar::pick_up_left_factor() {
	int num = 0;
	while (true) {
		bool ok = false;
		for (auto& productions : g) if (!ok) {
			string left_head = productions.first;
			vector<vector<string> > new_productions;
			for (int i = 0; i < productions.second.size(); i++) if (!ok) {
				for (int j = i + 1; j < productions.second.size(); j++) if (!ok) {
					auto production1 = productions.second[i];
					auto production2 = productions.second[j];
					if (production1.front() == production2.front()) {
						for (int k = 0; k < productions.second.size(); k++) {
							if (k != i && k != j) {
								new_productions.push_back(productions.second[k]);
							}
						}
						vector<string> tmp;
						tmp.push_back(production1.front());
						tmp.push_back(left_head + to_string(++num));
						new_productions.push_back(tmp);
						productions.second = new_productions;
						tmp.clear();
						for (int i1 = 1; i1 < production1.size(); i1++) tmp.push_back(production1[i1]);
						g[left_head + to_string(num)].push_back(tmp);
						tmp.clear();
						for (int i2 = 1; i2 < production2.size(); i2++) tmp.push_back(production2[i2]);
						g[left_head + to_string(num)].push_back(tmp);
						ok = true;
						break;
					}
				}
			}
		}
		if (!ok) break;
	}
}
