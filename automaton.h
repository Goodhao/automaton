#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <sstream>
using namespace std;

class automaton {
public:
	automaton() {};
	automaton(string& fa_type, set<string>& state, string initial_state, set<string> final_state,
		set<string>& sigma, map<pair<string, string>, set<string>>& to) : _fa_type(fa_type), _state(state),
		_initial_state(initial_state),
		_final_state(final_state), _sigma(sigma), _to(to) {};
	automaton turn_to_NFA();
	automaton turn_to_DFA();
	set<string> eps_closure(string &st);
	set<string> grammer(string dir);
	void get_reachable(int code);
	void get_edge();
	void merge_edge();
	void output_grammer(string dir, string filename);
	//void translate(set<string> G);
	void to_reg(string filename, vector<string> order);
	pair<int,int> deg(string x);
	void recursive_deletion(int x,int y);
	void minimalization();
	void to_FA();
	void draw(string filename);
	void input();
private:
	string _fa_type;
	set<string> _state;
	string _initial_state;
	set<string> _final_state;
	set<string> _sigma;
	map<pair<string, string>, set<string>> _to;
	map<pair<string, string>, set<string>> _edge;
	set<string> _reachable;
	vector<vector<vector<pair<int, int>>>> _link;
	vector<vector<int>> _table;
};
