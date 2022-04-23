#include <iostream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <sstream>
using namespace std;

class grammar {
public:
	grammar() {};
	void input();
	void output();
	void remove_direct_left_recursion(string head);
	void remove_indirect_left_recursion();
	void pick_up_left_factor();
	set<string> FIRST();
	set<string> FOLLOW();
private:
	map<string, vector<vector<string> > > g;
	set<string> terminal;
};
