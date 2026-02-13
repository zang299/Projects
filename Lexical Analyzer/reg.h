#ifndef REG_H
#define REG_H

#include <vector>
#include <set>
#include <string>

using namespace std;

struct REG_node {
    std::vector<REG_node*> neighbors;
    std::vector<char> labels;
};

struct REG {
    REG_node* start;
    REG_node* accept;
};

struct TokenDef {
    string name;
    REG* reg;
};

// Global variables
extern string input_text;
extern int current_pos;
extern vector<TokenDef> token_definitions;

// Function declarations
REG* makechar(char c);
REG* makeconcat(REG* r1, REG* r2);
REG* makeunion(REG* r1, REG* r2);
REG* makestar(REG* r);
REG* makeepsilon();
set<REG_node*> epsilon_closure(set<REG_node*> S_p);
set<REG_node*> Match_One_Char(set<REG_node*> S, char c);
int match(REG* r, const string& s, int p);
void my_GetToken();

#endif