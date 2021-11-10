#include "automaton.h"
#include <assert.h>
#include <queue>
#include <fstream>
#include <sstream>
//#define fin cin
using namespace std;

string encode(set<string>& s) {
	if (s.empty()) return "";
	string res = "";
	for (auto x : s) {
		if (res == "") res = x;
		else res += "," + x;
	}
	return res;
}
void append(string& x, string y) {
	if (y=="eps") y = "&epsilon;";
	if (x == "") x = y;
	else x += "," + y;
}
string bin(string x) {
	return "\"[" + x + "]\"";
}
string style(string s,string x) {
	if (s == "DFA") return bin(x);
	else return x;
}
void automaton::input() {
	ifstream fin("in.txt");
	cout << "Please input the type of FA, 0 for DFA, 1 for NFA, 2 for eps_NFA" << endl;
	int num;
	fin >> num;
	assert(num >= 0 && num <= 2);
	if (num == 0) _fa_type = "DFA";
	else if (num == 1) _fa_type = "NFA";
	else _fa_type = "eps_NFA";
	cout << "Please input the number of states" << endl;
	int n;
	fin >> n;
	assert(n >= 1);
	cout << "Please input the names of states" << endl;
	while (n--) {
		string s;
		fin >> s;
		_state.insert(s);
	}
	cout << "Please input the name of the initial state" << endl;
	string s;
	fin >> s;
	_initial_state = s;
	cout << "Please input the number of the final states" << endl;
	fin >> n;
	assert(n >= 0);
	if (n > 0) cout << "Please input the names of the final states" << endl;
	while (n--) {
		string s;
		fin >> s;
		_final_state.insert(s);
	}
	cout << "Please input the size of sigma set" << endl;
	fin >> n;
	assert(n >= 1);
	cout << "Please input the sigma set, \"eps\" keeps for epsilon" << endl;
	while (n--) {
		string s;
		fin >> s;
		_sigma.insert(s);
	}
	cout << "Please input the transfer tables, each table separated by single line \"#\"" << endl;
	cout << "In each table, \"chr st1 st2\" means the current state can go to st1 or st2 after reading chr" << endl;
	for (auto x : _state) {
		cout << x + ":" << endl;;
		while (1) {
			string s = "";
			while (s == "") getline(fin, s);
			if (s == "#") break;
			stringstream ss(s);
			bool f = 0;
			string chr;
			while (ss >> s) {
				if (!f) {
					chr = s;
				}
				else {
					_to[make_pair(x, chr)].insert(s);
					cout << x << " -" << chr << "-> " << s << endl;
				}
				f = 1;
			}
		}
	}
}
set<string> automaton::eps_closure(string &st) {
	set<string> res;
	queue<string> q;
	q.push(st);
	res.insert(st);
	while (!q.empty()) {
		string t = q.front();
		q.pop();
		if (_to.count(make_pair(t, "eps")) != 0) {
			set<string> s = _to[make_pair(t, "eps")];
			for (auto x : s) {
				if (res.count(x) == 0) {
					res.insert(x);
					q.push(x);
				}
			}
		}
	}
	return res;
}
automaton automaton::turn_to_NFA() {
	assert(_fa_type == "eps_NFA");
	automaton res=*(this);
	res._fa_type = "NFA";
	if (res._sigma.count("eps")!=0) res._sigma.erase("eps");
	for (auto x : _state) {
		set<string> c = eps_closure(x);
		for (auto y : c) {
			if (y != x) {
				for (auto chr : _sigma) if (chr != "eps") {
					if (_to.count(make_pair(y, chr)) != 0) {
						for (auto z : _to[make_pair(y, chr)]) {
							res._to[make_pair(x, chr)].insert(z);
						}
					}
				}
			}
		}
	}
	for (auto it = res._to.begin(); it != res._to.end();) {
		if (it->first.second == "eps") {
			res._to.erase(it++);
		}
		else it++;
	}
	return res;
}
automaton automaton::turn_to_DFA() {
	assert(_fa_type == "NFA");
	automaton res;
	queue<set<string>> q;
	map<set<string>, bool> vis;
	set<string> s{ _initial_state };
	q.push(s);
	vis[s] = 1;
	while (!q.empty()) {
		set<string> t = q.front();
		res._state.insert(encode(t));
		for (auto x : _final_state) {
			if (t.count(x) != 0) {
				res._final_state.insert(encode(t));
				break;
			}
		}
		q.pop();
		for (auto c : _sigma) {
			set<string> tmp;
			for (auto x : t) {
				if (_to.count(make_pair(x, c)) != 0)
					tmp.insert(_to[make_pair(x, c)].begin(), _to[make_pair(x, c)].end());
			}
			if (!tmp.empty()) {
				res._to[make_pair(encode(t), c)] = set<string>{ encode(tmp) };
				if (!vis[tmp]) {
					q.push(tmp);
					vis[tmp] = 1;
				}
			}
		}
	}
	res._fa_type = "DFA";
	res._initial_state = encode(s);
	res._sigma = _sigma;
	return res;
}
void automaton::draw(string filename) {
	string str = "node [shape = doublecircle]; ";
	for (auto x : _final_state) str += style(_fa_type, x) + " ";
	str += ";\n";
	vector<string> s = { "digraph cinite_state_machine {\n",
	"rankdir=LR;\n",
	"size=\"50,50\"\n",
	str,
	string("node [shape = circle]; ") + style(_fa_type,_initial_state) + string(";\n"),
	"node [shape = plain]; \"S\";\n",
	string("\"S\" -> ") + style(_fa_type,_initial_state) + string(";\n"),
	"node [shape = circle];\n" };
	string script = "";
	for (auto x : s) script += x;
	map<pair<string, string>, string> compress;
	for (auto x : _to) {
		if (!x.second.empty()) 
			for (auto y : x.second)
				append(compress[make_pair(x.first.first, y)], x.first.second);
	}
	for (auto x : compress) {
		script += style(_fa_type, x.first.first) + " -> " + style(_fa_type, x.first.second) + " [label = \"" + x.second + "\"];\n";
	}
	script += "}";
	ofstream fout("out.dot");
	fout << script << endl;
	fout.close();
	string cmd = "Graphviz\\bin\\dot.exe -T png out.dot -o " + filename + ".png";
	system(cmd.c_str());
}
