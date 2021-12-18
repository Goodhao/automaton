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

void automaton::get_edge() {
	for (auto x : _to) {
		for (auto y : x.second) {
			_edge[make_pair(x.first.first, y)].insert(x.first.second);
		}
	}
}

void automaton::merge_edge() {
	for (auto x : _state) for (auto y : _state) {
		if (_edge[make_pair(x, y)].size() > 1) {
			string s = "";
			bool eps = 0;
			for (auto str : _edge[make_pair(x, y)]) {
				if (str != "eps") {
					if (s == "") s += str;
					else s += "+" + str;
				}
				else eps = 1;
			}
			if (s == "" && eps) s = "eps";
			_edge[make_pair(x, y)].clear();
			_edge[make_pair(x, y)].insert(s);
		}
	}
}

void automaton::get_reachable(int code = 0) {
	// 这里的可达是指可以作为从初始状态到接受状态的中间态
	// 书上的可达有时只是指可以从初始状态出发走到，有时也采取这里的定义
	// 这里的可达排除了陷阱状态（初始状态可以走到它，但它走不到接受状态）
	map<pair<string, string>, set<string>> reverse;
	for (auto x : _to) {
		for (auto y : x.second) {
			reverse[make_pair(y, x.first.second)].insert(x.first.first);
		}
	}
	set<string> reachable1, reachable2;
	queue<string> q;
	string s;

	for (auto x : _final_state) {
		q.push(x);
		reachable1.insert(x);
	}
	while (!q.empty()) {
		s = q.front();
		q.pop();
		for (auto chr : _sigma) {
			for (auto x : reverse[make_pair(s, chr)]) {
				if (reachable1.count(x) == 0) {
					q.push(x);
					reachable1.insert(x);
				}
			}
		}
	}
	while (!q.empty()) q.pop();
	q.push(_initial_state);
	reachable2.insert(_initial_state);
	while (!q.empty()) {
		s = q.front();
		q.pop();
		for (auto chr : _sigma) {
			for (auto x : _to[make_pair(s, chr)]) {
				if (reachable2.count(x) == 0) {
					q.push(x);
					reachable2.insert(x);
				}
			}
		}
	}
	for (auto x : _state) {
		if (code == 0 && reachable1.count(x) && reachable2.count(x)) {
			_reachable.insert(x);
		} else if (code == 1 && reachable1.count(x)) {
			_reachable.insert(x);
		} else if (code == 2 && reachable2.count(x)) {
			_reachable.insert(x);
		}
	}
}

set<string> automaton::grammer(string dir) {
	get_reachable();
	if (dir == "right") {
		map<string, set<string>> res;
		set<string> G;
		for (auto x : _to) if (_reachable.count(x.first.first) != 0) {
			if (!x.second.empty())
				for (auto y : x.second) if (_reachable.count(y) != 0) {
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
		for (auto x : reverse) if (_reachable.count(x.first.first) != 0) {
			if (!x.second.empty()) {
				for (auto y : x.second) if (_reachable.count(y) != 0) {
					res[x.first.first].insert(y + x.first.second);
					if (_initial_state == y) res[x.first.first].insert(x.first.second);
				}
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
		bool eps = 0;
		for (auto chr : _sigma) for (auto x : _final_state) {
			if (_initial_state == x) eps = 1;
			for (auto y : reverse[make_pair(x, chr)]) if (_reachable.count(y) != 0) {
				if (_initial_state == y) {
					if (f == 0) s += chr;
					else s += "|" + chr;
					f = 1;
				}
				if (f == 0) s += y + chr;
				else s += "|" + y + chr;
				f = 1;
			}
		}
		if (eps) {
			if (f == 0) s += "\u03B5";
			else s += "|\u03B5";
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

pair<int, int> automaton::deg(string x) {
	// 不考虑自环对度的贡献
	pair<int, int> res;
	for (auto y : _state) if (x != y) {
		res.first += _edge[make_pair(x, y)].size();
		res.second += _edge[make_pair(y, x)].size();
	}
	return res;
}
inline string bracket(string s) {
	if (s == "eps") return "";
	if (s.size() == 1 || s.find("+") == -1) return s;
	else return "(" + s + ")";
}
inline string star(string s) {
	if (s == "eps") return "";
	if (s.size() == 1) return s + "*";
	else return "(" + s + ")*";
}
void automaton::to_reg(string filename, vector<string> order) {
	assert(_state.count("X") == 0 && _state.count("Y") == 0);
	get_reachable();
	if (_reachable.count(_initial_state) == 0) return;
	for (auto x : _to) {
		if (_reachable.count(x.first.first) == 0) {
			_to.erase(x.first);
		}
		else {
			for (auto y : x.second) {
				if (_reachable.count(y) == 0) {
					x.second.erase(y);
				}
			}
		}
	}
	for (auto x : _state) if (_reachable.count(x) == 0) {
		_state.erase(x);
		_final_state.erase(x);
	}
	if (_final_state.empty()) return;
	_to[make_pair("X", "eps")].insert(_initial_state);
	for (auto x : _final_state) _to[make_pair(x, "eps")].insert("Y");
	_initial_state = "X";
	_final_state.clear();
	_final_state.insert("Y");
	for (auto x : _state) order.push_back(x);
	_state.insert("X");
	_state.insert("Y");
	get_edge();
	/*
	for (auto x : _state) for (auto y : _state) {
		cout << x << " " << y << ": ";
		for (auto s : _edge[make_pair(x, y)]) {
			cout << s << " ";
		}
		cout << endl;
	}*/
	int p = -1;
	while (1) {
		merge_edge();
		if (_state.size() <= 2) break;
		p++;
		string to_delete = "";// 一次最多删除一个点
		string y = order[p];
		if (to_delete == "") for (auto x : _state) {
			for (auto z : _state) if ((x != y && !_edge[make_pair(x, y)].empty() || x == y && deg(y).first == 0) && (y != z && !_edge[make_pair(y, z)].empty() || y == z && deg(y).second == 0)) {
				//cout << x << " " << y << " " << z << endl;
				if (_edge[make_pair(y, y)].empty()) {
					if (x != y) for (auto s1 : _edge[make_pair(x, y)]) if (z != y) for (auto s2 : _edge[make_pair(y, z)]) {
						_edge[make_pair(x, z)].insert(bracket(s1) + bracket(s2));
						to_delete = y;
					}
					//if (to_delete != "") cout << x << " " << y << " " << z << " type: 1" << endl;
				}
				else {
					if (x != y) for (auto s1 : _edge[make_pair(x, y)]) for (auto s2 : _edge[make_pair(y, y)]) if (z != y) for (auto s3 : _edge[make_pair(y, z)]) {
						_edge[make_pair(x, z)].insert(bracket(s1) + star(s2) + bracket(s3));
						to_delete = y;
					}
					if (x != y) for (auto s1 : _edge[make_pair(x, y)]) for (auto s2 : _edge[make_pair(y, y)]) if (z == y) {
						_edge[make_pair(x, z)].insert(bracket(s1) + star(s2));
						to_delete = y;
					}
					if (x == y) for (auto s2 : _edge[make_pair(y, y)]) if (z != y) for (auto s3 : _edge[make_pair(y, z)]) {
						_edge[make_pair(x, z)].insert(star(s2) + bracket(s3));
						to_delete = y;
					}
					//if (to_delete != "") cout << x << " " << y << " " << z << " type: 2" << endl;
				}
			}
		}
		for (auto x : _state) {
			_edge[make_pair(to_delete, x)].clear();
			_edge[make_pair(x, to_delete)].clear();
		}
		_state.erase(to_delete);
		//cout << "delete: " << to_delete << endl;
		/*
		for (auto x : _state) for (auto y : _state) {
			cout << x << " " << y << ": ";
			for (auto s : _edge[make_pair(x, y)]) {
				cout << s << " ";
			}
			cout << endl;
		}*/
	}
	if (_edge[make_pair("X", "Y")].empty()) {
		return;
	}
	else {
		cout << "regular expression is generated successfully" << endl;
		filename += ".txt";
		ofstream fout(filename.c_str());
		fout << *_edge[make_pair("X", "Y")].begin() << endl;
	}
}

void automaton::recursive_deletion(int x,int y) {
	if (_table[x][y]) return;
	_table[x][y] = 1;
	for (auto p : _link[x][y]) {
		recursive_deletion(p.first, p.second);
	}
}

string find(map<string, string> & fa, string x) {
	if (x == fa[x]) return x;
	else return (fa[x] = find(fa, fa[x]));
}

void automaton::minimalization() {
	assert(_fa_type == "DFA");
	get_reachable(2);
	if (_reachable.count(_initial_state) == 0) return;
	for (auto x : _to) {
		if (_reachable.count(x.first.first) == 0) {
			_to.erase(x.first);
		}
		else {
			for (auto y : x.second) {
				if (_reachable.count(y) == 0) {
					x.second.erase(y);
				}
			}
		}
	}
	for (auto x : _state) if (_reachable.count(x) == 0) {
		_state.erase(x);
		_final_state.erase(x);
	}
	int n = _state.size();
	vector<string> state;
	map<string, int> idx;
	int cnt = 0;
	for (auto x : _state) {
		state.push_back(x);
		idx[x] = cnt++;
	}
	_table.resize(n);
	_link.resize(n);
	for (int i = 0; i < n; i++) {
		_table[i].resize(n);
		_link[i].resize(n);
	}
	for (int i = 0; i < n - 1; i++) {
		for (int j = i + 1; j < n; j++) {
			auto x = state[i], y = state[j];
			if (_final_state.count(x) && !_final_state.count(y) || _final_state.count(y) && !_final_state.count(x)) {
				recursive_deletion(i, j);
			}
		}
	}
	for (int i = 0; i < n - 1; i++) {
		for (int j = i + 1; j < n; j++) {
			auto x = state[i], y = state[j];
			for (auto chr : _sigma) if (!_to[make_pair(x, chr)].empty() && !_to[make_pair(y, chr)].empty()) {
				auto nxt_x = *_to[make_pair(x, chr)].begin();
				auto nxt_y = *_to[make_pair(y, chr)].begin();
				if (nxt_x > nxt_y) swap(nxt_x, nxt_y);
				if (_table[idx[nxt_x]][idx[nxt_y]]) {
					recursive_deletion(i, j);
					break;
				}
				else if (nxt_x != nxt_y && make_pair(x, y) != make_pair(nxt_x, nxt_y)) {
					_link[idx[nxt_x]][idx[nxt_y]].push_back(make_pair(i, j));
				}
			}
		}
	}
	cout << "Equivalence Parition:" << endl;
	for (int i = 0; i < n - 1; i++) {
		for (int j = i + 1; j < n; j++) {
			if (_table[i][j] == 0) {
				auto x = state[i], y = state[j];
				cout << "(" << x << "," << y << ")" << endl;
			}
		}
	}
	// 并查集合并等价类
	map<string, string> fa;
	for (auto x : _state) fa[x] = x;
	for (int i = 0; i < n - 1; i++) {
		for (int j = i + 1; j < n; j++) {
			if (_table[i][j] == 0) {
				string u = find(fa, state[i]), v = find(fa, state[j]);
				if (u != v) {
					if (v < u) fa[u] = v;
					else fa[v] = u;
				}
			}
		}
	}
	automaton res;
	res._fa_type = "DFA";
	for (auto x : _state) {
		cout << x << " " << find(fa, x) << endl;
		res._state.insert(find(fa, x));
	}
	res._initial_state = find(fa, _initial_state);
	for (auto x : _final_state) res._final_state.insert(find(fa, x));
	res._sigma = _sigma;
	for (auto x : _to) {
		auto p = make_pair(find(fa, x.first.first), x.first.second);
		for (auto y : x.second) res._to[p].insert(find(fa, y));
	}
	(*this) = res;
}