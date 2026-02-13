#include <iostream>
#include <cstdlib>
#include <vector>
#include <set>
#include <string>
#include <cctype> 

using namespace std;

// global variables input_text which is used to hold the full string inside the semicolons
// and current_pos which is used to keep track of where we are within input_text when we call my_GetToken() to get the next token from the input text
string input_text = "";
int current_pos = -1;

// struct used for a REG node using vectors for both neighbors and labels based on the recommendation in the project file
struct REG_node{
    vector<REG_node*> neighbors;
    vector<char> labels;
};

    
// struct used for a regular expression graph with two pointers to two REG nodes being start and accept
// where start is the entry state of the graph and accept is the final state of the graph
// the whole structure will be built utilising the neighbors and labels vectors of both start and accept nodes
struct REG{
    struct REG_node *start;
    struct REG_node *accept;

};


// struct used to hold the token name and the corresponding regular expression graph for it 
struct TokenDef
{
    string name;
    REG *reg;
};
// global variable used to hold the vector of all token definitions parsed from the input (the real token list)
vector<TokenDef> token_definitions;



// function used to return the regular expression graph for a single character a
// the creation of this graph is quite literally just a copy and paste of the one in the project file 
REG* makechar(char a)
{
    // Creates the start and accept nodes for the graph
    REG_node *start = new REG_node();
    REG_node *accept = new REG_node();

    // Adds the character a transition to the start's neighbors (means there is a transition connecting the start and accept node)
    // also adds it to the start's labels
    start->neighbors.push_back(accept);
    start->labels.push_back(a);

    // Now creates a REG structure to hold the start and accept nodes we just created and modified just now and returns a pointer to it
    REG *char_reg = new REG();
    char_reg->start = start;
    char_reg->accept = accept;

    // returns the REG structure we just created
    return char_reg;

}

// function used to return the regular expression graph for the epsilon regular expression which accepts the empty string
// the creation of this graph is quite literally just a copy and paste of the one in the project file 
REG* makeepsilon(){
    // Creates the start and accept nodes for the graph
    REG_node *start = new REG_node();
    REG_node *accept = new REG_node();

    // Adds the epsilon transition ('_') to the start's neighbors (means there is a transition connecting the start and accept node)
    // also adds it to the start's labels
    start->neighbors.push_back(accept);
    start->labels.push_back('_');

    // Now creates a REG structure to hold the start and accept nodes we just created and modified just now and returns a pointer to it
    REG* epsilon_reg = new REG();
    epsilon_reg->start = start;
    epsilon_reg->accept = accept;

    // returns the REG structure we just created
    return epsilon_reg;
}

// function used to return the regular expression graph for the concantenation between two REG structures r1 and r2: "(r1).(r2)"
// the creation of this graph is also quite literally just a copy and paste of the one in the project file 
REG* makeconcat(REG* r1, REG* r2)
{
    // adds an epsilon transition from the accept node of r1 to the start node of r2 by adding the start node of r2 to the neighbors of the accept node of r1
    // and adds an underscore to the labels of the accept node of r1 to indicate that this is an epsilon transition
    r1->accept->neighbors.push_back(r2->start);
    r1->accept->labels.push_back('_');

    // Creates a REG structure to hold the start node of r1 and its start node and the accept node of r2 as its accept node and returns a pointer to it
    REG *concat_reg = new REG();
    concat_reg->start = r1->start;
    concat_reg->accept = r2->accept;

    // returns the REG structure we just created
    return concat_reg;

}

// function used to return the regular expression graph for the union between two REG structures r1 and r2: "(r1)|(r2)"
// the creation of this graph is also quite literally just a copy and paste of the one in the project file
REG* makeunion(REG* r1, REG* r2)
{
    // creates separate and new start and accept nodes for the graph
    REG_node *new_start = new REG_node();
    REG_node *new_accept = new REG_node();

    // adds epsilon transitions from the new start node to the start nodes of both r1 and r2 by adding the start nodes of both r1 and r2 to the neighbors of the new start node
    // and adds underscores to the labels of the new start node to indicate that these are epsilon transitions
    new_start->neighbors.push_back(r1->start);
    new_start->labels.push_back('_');

    new_start->neighbors.push_back(r2->start);
    new_start->labels.push_back('_');

    // adds epsilon transitions from the accept nodes of both r1 and r2 to the new accept node by adding the new accept node to the neighbors of the accept nodes of both r1 and r2
    // and adds underscores to the labels of the accept nodes of both r1 and r2 to indicate that these are epsilon transitions
    r1->accept->neighbors.push_back(new_accept);
    r1->accept->labels.push_back('_');

    r2->accept->neighbors.push_back(new_accept);
    r2->accept->labels.push_back('_');

    // creates a REG structure to hold the new start and accept nodes we just created and modified just now and returns a pointer to it
    REG *union_reg = new REG();
    union_reg->start = new_start;
    union_reg-> accept = new_accept;

    // returns the REG structure we just created
    return union_reg;



}

// function used to return the regular expression graph for the star of a REG structure r: "(r)*"
REG* makestar(REG* r)
{
    // creates separate and new start and accept nodes for the graph
    REG_node* new_start = new REG_node();
    REG_node* new_accept = new REG_node();

    // adds an epsilon transition from the new start node to the start node of r 
    new_start->neighbors.push_back(r->start);
    new_start->labels.push_back('_');

    // adds an epsilon transition from the new start node to the new accept node to allow for the case where there are zero occurrences of r
    new_start->neighbors.push_back(new_accept);
    new_start->labels.push_back('_');

    // adds an epsilon transition from the accept node of r to the start node of r to allow for repetition
    r->accept->neighbors.push_back(r->start);
    r->accept->labels.push_back('_');

    // adds an epsilon transition from the accept node of r to the new accept node to allow for the case where there is only one occurrence of r
    r->accept->neighbors.push_back(new_accept);
    r->accept->labels.push_back('_');

    // creates a REG structure to hold the new start and accept nodes we just created and modified just now and returns a pointer to it
    REG* star_reg = new REG();
    star_reg->start = new_start;
    star_reg->accept = new_accept;
    // returns the REG structure we just created
    return star_reg;
}



// function used to return the eosilon closure of a set of REG nodes S_p which includes all states reachable from the input set by following only epsilon transitions
// the creation of this function is mainly based on the second part of the pseudocode of the Match_One_Char() function from the project file
// created based on the recommendation from the document to encapsulate the second part into a separate function
set<REG_node*> epsilon_closure(set<REG_node*>S_p) 
{
    // boolean flag to track whether we found new states in the current iteration
    bool changed = true;
    // temporary set to accumulate newly discovered states
    set<REG_node*> S_doublep;

    // continue iterating until no new states can be added (closure is complete)
    while(changed)
    {
        // reset the changed flag at the beginning of each iteration
        changed = false;
        
        // iterate through each state in the set of statesreached by character transitions
        for(REG_node* n: S_p)
        {
            // include the current state itself in the closure set
            S_doublep.insert(n);
            // iterates through every neighbor of the current state n 
            for(int i=0; i<n->neighbors.size(); i++)
            {
                REG_node *m = n->neighbors[i];
                // if there is an epsilon transition and this neighbor is not in the new states set, add it to the new states set
                if(n->labels[i] == '_' && (S_doublep.find(m) == S_doublep.end()) )
                {
                    S_doublep.insert(m);


                }
            }
        }
        // if we discovered new sets in this iteration
        if(S_p != S_doublep)
        {
            // set the changed flag to true to indicate that we need to continue iterating since we found new states
            changed = true;
            // update current states to include the newly discovered states
            S_p = S_doublep;
            // clear the temporary set for the next iteration
            S_doublep.clear();
            

        }
    }

    return S_p;
}




// function used to find all states reachable by consuming character 'c' from the current state set
// then computes the epsilon closure of those states and returns it
// also mainly inspired by the pseudocode of the same function in project file
set<REG_node*> Match_One_Char(set<REG_node*> S, char c) 
{
    // set to store states reached by consuming character 'c'
    set<REG_node*> S_p;

    // for each current state in the input set
    for(REG_node* node : S )
    {
        // checks all outgoing transition from the current state
        for (int i = 0; i < node->neighbors.size(); i++) 
        {
            REG_node* m = node->neighbors[i];
            // if theres a transition labeled with the input character c and we haven't already added the target state to the new states set
            if(node->labels[i] == c && (S_p.find(m) == S_p.end()))
            {
                // add it to the new states set
                S_p.insert(m);
            }
        }
    }

    // returns the set of states reachable by consuming character c followed by any number of epsilon transitions 
    // using the epsilon_closure function we defined earlier
    return epsilon_closure(S_p);
}

// function used to match a regular expression graph against the input string starting at position p
// returns the end position of the longest match found 
int match(REG *r, string s, int p)
{
    // initialize the function with epsilon closure of the start state
    set<REG_node*> current;
    current.insert(r->start);
    current = epsilon_closure(current);

    // track the end position of the longest valid match found so far 
    // intiailize it to the starting position p (means empty string matches if accept state is reachable initially)
    int last_accept_pos = p;
    if (current.find(r->accept) != current.end()) {
        // epsilon match at starting position
        last_accept_pos = p; 
    }

    // process each character in the input string starting from position p
    for (int i = p + 1; i < (int)s.length(); i++) {
        char c = s[i];

         // advance by one character transition and then take epsilon closure to find the new set of current states
        current = Match_One_Char(current, c);

        // if no states are reachable, break out of the loop since matching cannot continue
        if (current.empty()) {
            break;
        }

        // if any current state is an accet state, record it as a valid match end position
        if (current.find(r->accept) != current.end()) {
            // update to the longest match found 
            last_accept_pos = i;
        }
    }

    // return the end position of the longest match found
    return last_accept_pos;


}

// the general my_GetToken function
// used to extract and print the next token from the input text based on the regular expression graphs we built for each token definition in the first part of the input
// skips any leading whitespace, finds the longest match among all the token definitions and prints the corresponding token name and lexeme 
// that matches the longest substring
void my_GetToken() {
    
    // skip any whitespace characters before the next token
    while (current_pos + 1 < (int)input_text.length() && 
           isspace(input_text[current_pos + 1])) {
        current_pos++;
    }

    
    
    // used to check if the end of the input has been reached 
    if (current_pos + 1 >= (int)input_text.length()) {
        return; 
    }
    
    // finds the longest matching token among all defined tokens
    // best_end variable used to track the end position of the longest match found so far
    // best_token variable used to track the name of the token that corresponds to the longest match found so far 
    int best_end = current_pos;
    string best_token = "";
    
    // iterates through all token defintions
    for (TokenDef token: token_definitions) {
        // uses the match function defined earlier to find the end position of the longest match for the current token definition
        int end = match(token.reg, input_text, current_pos);
        // if the match for the current token definition is longer than the longest match found so far 
        if (end > best_end) {
            // updates the longest match end position and the corresponding token name to reflect the best match found so far
            best_end = end;
            best_token = token.name;
        }
    }
    
    // if there is no token matched (best_end is unchanged), print the error message and exit 
    if (best_end == current_pos) {
        cout << "ERROR\n";
        exit(1);
    }
    
    // extract the lexeme and print the best matching token name and lexeme
    string lexeme = input_text.substr(current_pos + 1, best_end - current_pos);
    cout << best_token << " , \"" << lexeme << "\"\n";
    
    // advance the current position to continue searching for the next token in the input text
    current_pos = best_end;
}

    









