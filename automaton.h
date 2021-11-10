#include <iostream>
#include <string>
#include <map>
#include <set>
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
	void draw(string filename);
	void input();
private:
	string _fa_type;
	set<string> _state;
	string _initial_state;
	set<string> _final_state;
	set<string> _sigma;
	map<pair<string, string>, set<string>> _to;
};