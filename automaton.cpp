#include "automaton.h"
#include <assert.h>
#include <queue>
#include <fstream>
#include <sstream>
#define fin cin
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
	//ifstream fin("in.txt");
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
			if (x == _initial_state && _final_state.count(y) != 0) res._final_state.insert(x);
			for (auto chr : _sigma) if (chr != "eps") {
				if (_to.count(make_pair(y, chr)) != 0) {
					for (auto z : _to[make_pair(y, chr)]) {
						set<string> c2 = eps_closure(z);
						for (auto w : c2) res._to[make_pair(x, chr)].insert(w);
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
	bool additional_point = 0;
	for (auto s : res._state) {
		for (auto chr : res._sigma) {
			if (res._to.count(make_pair(s, chr)) == 0) {
				res._to[make_pair(s, chr)].insert("trap");
				additional_point = 1;
			}
		}
	}
	if (additional_point) {
		res._state.insert("trap");
		for (auto chr : res._sigma) {
			res._to[make_pair("trap", chr)].insert("trap");
		}
	}
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

set<string> automaton::grammer(string dir) {
	assert(_fa_type == "DFA");
	if (dir == "right") {
		map<string, set<string>> res;
		set<string> G;
		for (auto x : _to) {
			if (!x.second.empty())
				for (auto y : x.second) {
					res[x.first.first].insert(x.first.second + y);
					if (_final_state.count(y)) res[x.first.first].insert(x.first.second);
				}
		}
		if (_final_state.count(_initial_state)) res[_initial_state].insert("\u03B5");
		for (auto x : res) {
			if (!x.second.empty()) {
				bool f = 0;
				string s = "";
				for (auto y : x.second) {
					if (f == 0) s += x.first + " -> " + y;
					else s += "|" + y;
					f = 1;
				}
				G.insert(s);
			}
		}
		return G;
	}
	else if (dir == "left") {
		map<string, set<string>> res;
		set<string> G;
		map<pair<string, string>, set<string>> reverse;
		for (auto x : _to) {
			for (auto y : x.second) {
				reverse[make_pair(y, x.first.second)].insert(x.first.first);
			}
		}
		for (auto x : reverse) {
			if (!x.second.empty())
				for (auto y : x.second) {
					res[x.first.first].insert(y + x.first.second);
					if (_initial_state == y) res[x.first.first].insert(x.first.second);
				}
		}
		for (auto x : res) {
			if (!x.second.empty()) {
				bool f = 0;
				string s = "";
				for (auto y : x.second) {
					if (f == 0) s += x.first + " -> " + y;
					else s += "|" + y;
					f = 1;
				}
				G.insert(s);
			}
		}
		string s = "Z -> ";
		bool f = 0;
		for (auto x : _final_state) {
			if (f == 0) s += x;
			else s += "|" + x;
			f = 1;
		}
		G.insert(s);
		return G;
	}
}

void automaton::output_grammer(string dir,string filename) {
	assert(_fa_type == "DFA");
	set<string> G = grammer(dir);
	filename += ".txt";
	ofstream fout(filename.c_str());
	if (dir == "right") {
		fout << "variable: ";
		for (auto s : _state) fout << s << " ";
		fout << endl;
		fout << "terminal: ";
		for (auto chr : _sigma) fout << chr << " ";
		fout << endl;
		fout << "start symbol: " << _initial_state << endl;
		fout << "production: " << endl;
		for (auto s : G) {
			fout << s << endl;
		}
	}
	else if (dir == "left") {
		fout << "variable: ";
		for (auto s : _state) fout << s << " ";
		fout << "Z ";
		fout << endl;
		fout << "terminal: ";
		for (auto chr : _sigma) fout << chr << " ";
		fout << endl;
		fout << "start symbol: " << "Z" << endl;
		fout << "production: " << endl;
		for (auto s : G) {
			fout << s << endl;
		}
	}
}