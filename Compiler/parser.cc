// Name: Thian Zhang Hao
// ASU ID: 1235703107
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include "execute.h"
#include "lexer.h"

using namespace std;

LexicalAnalyzer lexer;

// location table entry: one (variable name, mem[] index) pair
struct loc_entry {
    string name;
    int loc;
};

// location table: we use a vector and search it linearly (simple way)
vector<loc_entry> loc_table;

// function used to print a syntax error and exit
void syntax_error()
{
    // prints the syntax error message
    cout << "SYNTAX ERROR" << endl;
    // exits the program
    exit(1);
}

// function used to expect a token and return it if it is the expected token
Token expect(TokenType expected)
{
    // gets the next token from the lexer
    Token t = lexer.GetToken();
    // checks if the token type is not the expected token type and prints the syntax error message
    if (t.token_type != expected)
    {
        syntax_error();
    }
    // returns the token
    return t;
}

// function used to find the mem[] index for a variable name
// returns -1 if not found (should not happen for valid programs)
int find_location(string name)
{
    // loops through the location table
    for (int i = 0; i < (int)loc_table.size(); i++)
    {
        // checks if the entry name matches
        if (loc_table[i].name == name)
        {
            // returns the location of the entry
            return loc_table[i].loc;
        }
    }
    // returns -1 if not found
    return -1;
}

// function used to add a new variable to the location table
// gives it the next available slot in mem[] and initializes it to 0
void add_variable(string name)
{
    // creates a new location table entry
    loc_entry entry;
    // sets the name of the entry to the variable name
    entry.name = name;
    // sets the location of the entry to the next available slot in mem[]
    entry.loc = next_available;
    // pushes the entry to the location table
    loc_table.push_back(entry);
    // initializes the memory slot to 0
    mem[next_available] = 0;
    // increments the next available slot counter
    next_available++;
}

// parses id_list -> ID COMMA id_list | ID
// for each ID we see, we add a new variable to the location table
void parse_id_list()
{
    // expects an ID token
    Token t = expect(ID);
    // adds the ID to the location table
    add_variable(t.lexeme);
    // creates a new token and peeks the next token
    Token p = lexer.peek(1);
    // if the next token is a comma, consume it and parse the id list again
    if (p.token_type == COMMA)
    {
        // consumes the comma
        expect(COMMA);
        // parses the id list again
        parse_id_list();
    }
}

// parses var_section -> id_list SEMICOLON
void parse_var_section()
{
    // parses the id list
    parse_id_list();
    // expects a semicolon token
    expect(SEMICOLON);
}

// parses num_list -> NUM | NUM num_list
// reads NUM tokens one by one and pushes their integer values to the global inputs vector
void parse_num_list()
{
    // expects a number token
    Token t = expect(NUM);
    // converts the lexeme string to an integer
    int value = std::stoi(t.lexeme);
    // pushes the integer to the inputs vector
    inputs.push_back(value);
    // creates a new token and peeks the next token
    Token p = lexer.peek(1);
    // if the next token is also a number, parse the num list again
    if (p.token_type == NUM)
    {
        // parses the num list again
        parse_num_list();
    }
}

// parses inputs -> num_list
void parse_inputs()
{
    // parses the num list
    parse_num_list();
}

// parses primary -> ID | NUM
// returns the mem[] index of the primary
// if it is an ID, we look up the location in the location table
// if it is a NUM, we allocate a new slot in mem[], store the int there, return that index
int parse_primary()
{
    // gets the next token
    Token t = lexer.GetToken();
    // if the token is an ID, return the mem[] index of the variable
    if (t.token_type == ID)
    {
        // returns the mem[] index of the variable
        return find_location(t.lexeme);
    }
    // if the token is a NUM, allocate a new slot in mem[] and return its index
    else if (t.token_type == NUM)
    {
        // converts the lexeme string to an integer
        int value = std::stoi(t.lexeme);
        // remembers the slot we are about to use
        int slot = next_available;
        // stores the constant at that slot
        mem[slot] = value;
        // increments the next available slot counter
        next_available++;
        // returns the slot index
        return slot;
    }
    else
    {
        // not a valid primary, call syntax error 
        syntax_error();
    }
    
    return -1;
}

// parses op -> PLUS | MINUS | MULT | DIV
// returns the corresponding ArithmeticOperatorType
ArithmeticOperatorType parse_op()
{
    // gets the next token
    Token t = lexer.GetToken();
    // checks the token type and returns the corresponding operator
    if (t.token_type == PLUS)
    {
        return OPERATOR_PLUS;
    }
    else if (t.token_type == MINUS)
    {
        return OPERATOR_MINUS;
    }
    else if (t.token_type == MULT)
    {
        return OPERATOR_MULT;
    }
    else if (t.token_type == DIV)
    {
        return OPERATOR_DIV;
    }
    else
    {
        // not a valid operator, calls syntax error 
        syntax_error();
    }
    
    return OPERATOR_NONE;
}

// parses assign_stmt -> ID EQUAL primary SEMICOLON | ID EQUAL expr SEMICOLON
// where expr -> primary op primary
// builds one InstructionNode with type = ASSIGN
struct InstructionNode* parse_assign_stmt()
{
    // expects an ID token (the LHS of the assignment)
    Token lhs = expect(ID);
    // expects an equal sign
    expect(EQUAL);
    // parses the first primary (or the single primary, we do not know yet)
    int op1_loc = parse_primary();
    // creates a new instruction node
    struct InstructionNode* inst = new InstructionNode();
    // sets the type to ASSIGN
    inst->type = ASSIGN;
    // sets the lhs_loc to the mem[] index of the LHS variable
    inst->assign_inst.lhs_loc = find_location(lhs.lexeme);
    // sets the op1_loc to the mem[] index of the first primary
    inst->assign_inst.op1_loc = op1_loc;
    // peeks the next token to see if this is a single primary or an expr
    Token p = lexer.peek(1);
    // if the next token is an operator, this is an expr
    if (p.token_type == PLUS || p.token_type == MINUS
        || p.token_type == MULT || p.token_type == DIV)
    {
        // parses the operator
        inst->assign_inst.op = parse_op();
        // parses the second primary
        inst->assign_inst.op2_loc = parse_primary();
    }
    else
    {
        // single primary, so the operator is OPERATOR_NONE
        inst->assign_inst.op = OPERATOR_NONE;
        // op2_loc is unused in this case, but we set it to 0 to be safe
        inst->assign_inst.op2_loc = 0;
    }
    // expects a semicolon token
    expect(SEMICOLON);
    // sets the next pointer to NULL
    inst->next = NULL;
    // returns the instruction node
    return inst;
}

// parses input_stmt -> INPUT ID SEMICOLON
// builds one InstructionNode with type = IN
struct InstructionNode* parse_input_stmt()
{
    // expects the INPUT keyword
    expect(INPUT);
    // expects an ID token (the variable to read into)
    Token t = expect(ID);
    // expects a semicolon token
    expect(SEMICOLON);
    // creates a new instruction node
    struct InstructionNode* inst = new InstructionNode();
    // sets the type to IN
    inst->type = IN;
    // sets the var_loc to the mem[] index of the variable
    inst->input_inst.var_loc = find_location(t.lexeme);
    // sets the next pointer to NULL
    inst->next = NULL;
    // returns the instruction node
    return inst;
}

// parses output_stmt -> OUTPUT ID SEMICOLON
// builds one InstructionNode with type = OUT
struct InstructionNode* parse_output_stmt()
{
    // expects the OUTPUT keyword
    expect(OUTPUT);
    // expects an ID token (the variable to print)
    Token t = expect(ID);
    // expects a semicolon token
    expect(SEMICOLON);
    // creates a new instruction node
    struct InstructionNode* inst = new InstructionNode();
    // sets the type to OUT
    inst->type = OUT;
    // sets the var_loc to the mem[] index of the variable
    inst->output_inst.var_loc = find_location(t.lexeme);
    // sets the next pointer to NULL
    inst->next = NULL;
    // returns the instruction node
    return inst;
}

// forward declaration so parse_if_stmt can call parse_body
struct InstructionNode* parse_body();

// parses condition -> primary relop primary
// fills condition_op, op1_loc, op2_loc into a given CJMP node
void parse_condition(struct InstructionNode* inst)
{
    // parse first primary (left operand)
    inst->cjmp_inst.op1_loc = parse_primary();
    // get the relop token
    Token t = lexer.GetToken();
    // set the condition operator
    if (t.token_type == GREATER)
    {
        inst->cjmp_inst.condition_op = CONDITION_GREATER;
    }
    else if (t.token_type == LESS)
    {
        inst->cjmp_inst.condition_op = CONDITION_LESS;
    }
    else if (t.token_type == NOTEQUAL)
    {
        inst->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
    }
    else
    {
        syntax_error();
    }
    // parse second primary (right operand)
    inst->cjmp_inst.op2_loc = parse_primary();
}

// forward declarations needed for parse_switch_stmt
struct InstructionNode* parse_case(int var_loc, struct InstructionNode* end_noop);
struct InstructionNode* parse_case_list(int var_loc, struct InstructionNode* end_noop);

// parses case -> CASE NUM COLON body
// creates a CJMP node that checks if var is not equal to the case value
// if var != case_val (condition true), next goes to next case (skip body)
// if var == case_val (condition false), target goes to body then JMP to end_noop
struct InstructionNode* parse_case(int var_loc, struct InstructionNode* end_noop)
{
    // consume CASE
    expect(CASE);
    // get the case number
    Token num = expect(NUM);
    // allocate a mem[] slot for the case constant
    int case_val_loc = next_available;
    mem[case_val_loc] = std::stoi(num.lexeme);
    next_available++;
    // consume COLON
    expect(COLON);
    // parse the body
    struct InstructionNode* body = parse_body();
    // create JMP to end_noop after body executes
    struct InstructionNode* jmp = new InstructionNode();
    jmp->type = JMP;
    jmp->jmp_inst.target = end_noop;
    jmp->next = NULL;
    // append jmp to end of body
    struct InstructionNode* cur = body;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    cur->next = jmp;
    // create CJMP node: if var NOTEQUAL case_val is true, go to next (skip body)
    // if var NOTEQUAL case_val is false (they are equal), go to target (body)
    struct InstructionNode* inst = new InstructionNode();
    inst->type = CJMP;
    inst->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
    inst->cjmp_inst.op1_loc = var_loc;
    inst->cjmp_inst.op2_loc = case_val_loc;
    // target = body (executed when condition is false, meaning var == case_val)
    inst->cjmp_inst.target = body;
    // next will be set by caller to point to next case or end_noop
    inst->next = NULL;
    return inst;
}

// parses case_list -> case case_list | case
// returns head of the chain of case nodes
struct InstructionNode* parse_case_list(int var_loc, struct InstructionNode* end_noop)
{
    // parse one case
    struct InstructionNode* inst = parse_case(var_loc, end_noop);
    // peek to see if another case follows
    Token p = lexer.peek(1);
    if (p.token_type == CASE)
    {
        // parse the rest of the case list
        struct InstructionNode* rest = parse_case_list(var_loc, end_noop);
        // if var != case_val (true), next goes to the next case's CJMP
        inst->next = rest;
    }
    else
    {
        // last case: if var != case_val (true), skip to end_noop
        inst->next = end_noop;
    }
    return inst;
}

// parses switch_stmt -> SWITCH ID LBRACE case_list RBRACE
//                     | SWITCH ID LBRACE case_list default_case RBRACE
struct InstructionNode* parse_switch_stmt()
{
    // create instruction node inst (the head, which is the first case's CJMP)
    // consume SWITCH
    expect(SWITCH);
    // get the switch variable
    Token id = expect(ID);
    int var_loc = find_location(id.lexeme);
    // consume LBRACE
    expect(LBRACE);
    // create end_noop upfront so all JMPs can point to it
    struct InstructionNode* end_noop = new InstructionNode();
    end_noop->type = NOOP;
    end_noop->next = NULL;
    // parse the case list
    struct InstructionNode* inst = parse_case_list(var_loc, end_noop);
    // check for optional default case
    Token p = lexer.peek(1);
    if (p.token_type == DEFAULT)
    {
        // consume DEFAULT COLON
        expect(DEFAULT);
        expect(COLON);
        // parse default body
        struct InstructionNode* def_body = parse_body();
        // walk to end of default body and attach end_noop
        struct InstructionNode* cur = def_body;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        cur->next = end_noop;
        // the last CJMP's next currently points to end_noop
        // change it to point to def_body instead so no-match falls into default
        struct InstructionNode* cur2 = inst;
        while (cur2 != NULL)
        {
            if (cur2->type == CJMP && cur2->next == end_noop)
            {
                cur2->next = def_body;
            }
            cur2 = cur2->next;
        }
    }
    // consume RBRACE
    expect(RBRACE);
    return inst;
}

// parses if_stmt -> IF condition body
struct InstructionNode* parse_if_stmt()
{
    // create instruction node inst
    struct InstructionNode* inst = new InstructionNode();
    // if next token is IF
    Token p = lexer.peek(1);
    if (p.token_type == IF)
    {
        // consume IF
        expect(IF);
        inst->type = CJMP;
        // parse the condition and set condition_op, op1_loc, op2_loc
        parse_condition(inst);
        // inst->next = parse_body()
        // when condition is true the next instruction is the first of the body
        inst->next = parse_body();
        // create no_op node, make sure to set next field to nullptr
        struct InstructionNode* no_op = new InstructionNode();
        no_op->type = NOOP;
        no_op->next = NULL;
        // append no_op node to the body of the if
        struct InstructionNode* cur = inst->next;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        cur->next = no_op;
        // set inst->cjmp_inst.target to point to no_op node
        inst->cjmp_inst.target = no_op;
        return inst;
    }
    else
    {
        syntax_error();
        return NULL;
    }
}

// parses while_stmt -> WHILE condition body
struct InstructionNode* parse_while_stmt()
{
    // create instruction node inst
    struct InstructionNode* inst = new InstructionNode();
    // if next token is WHILE
    Token p = lexer.peek(1);
    if (p.token_type == WHILE)
    {
        // consume WHILE
        expect(WHILE);
        // inst->type = CJMP (handling WHILE using if and goto nodes)
        inst->type = CJMP;
        // parse the condition and set condition_op, op1_loc, op2_loc
        parse_condition(inst);
        // inst->next = parse_body()
        // when condition is true the next instruction is the first of the body
        inst->next = parse_body();
        // create jmp node of type JMP, do not forget to set next field to nullptr
        struct InstructionNode* jmp = new InstructionNode();
        jmp->type = JMP;
        jmp->next = NULL;
        // set jmp->jmp_inst.target to inst
        jmp->jmp_inst.target = inst;
        // append jmp node to end of body of while
        struct InstructionNode* cur = inst->next;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        cur->next = jmp;
        // create no_op node and attach it to the list after the jmp node
        struct InstructionNode* no_op = new InstructionNode();
        no_op->type = NOOP;
        no_op->next = NULL;
        jmp->next = no_op;
        // set inst->cjmp_inst.target to point to no_op node
        inst->cjmp_inst.target = no_op;
        return inst;
    }
    else
    {
        syntax_error();
        return NULL;
    }
}

// parses for_stmt -> FOR LPAREN assign_stmt condition SEMICOLON assign_stmt RPAREN body
// this is equivalent to running assign_stmt_1 once, then looping with WHILE condition 
// executing stmt_list followed by assign_stmt_2 each iteration
struct InstructionNode* parse_for_stmt()
{
    // consume FOR LPAREN
    expect(FOR);
    expect(LPAREN);

    // assign_stmt_1 (init)
    struct InstructionNode* assign_stmt_1 = parse_assign_stmt();

    // condition
    struct InstructionNode* cjmp = new InstructionNode();
    cjmp->type = CJMP;
    parse_condition(cjmp);

    // SEMICOLON between condition and assign_stmt_2
    expect(SEMICOLON);

    // parse assign_stmt_2 (the increment step), we attach it to the body later
    struct InstructionNode* assign_stmt_2 = parse_assign_stmt();

    // consume RPAREN
    expect(RPAREN);

    // body (stmt_list)
    struct InstructionNode* body = parse_body();

    assign_stmt_1->next = cjmp;

    
    cjmp->next = body;
    // append assign_stmt_2 to end of body
    struct InstructionNode* cur = body;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    cur->next = assign_stmt_2;
    // JMP back to cjmp, loops back to condition
    struct InstructionNode* jmp = new InstructionNode();
    jmp->type = JMP;
    jmp->jmp_inst.target = cjmp;
    jmp->next = NULL;
    assign_stmt_2->next = jmp;
    // no_op, where false condition lands, exits the while
    struct InstructionNode* no_op = new InstructionNode();
    no_op->type = NOOP;
    no_op->next = NULL;
    jmp->next = no_op;
    cjmp->cjmp_inst.target = no_op;

    return assign_stmt_1;
}

// parses stmt -> assign_stmt | while_stmt | if_stmt | switch_stmt
//              | for_stmt | output_stmt | input_stmt
// calls the right parse function based on the first token
struct InstructionNode* parse_stmt()
{
    // peek the first token of the statement
    Token p = lexer.peek(1);
    if (p.token_type == ID)
    {
        return parse_assign_stmt();
    }
    else if (p.token_type == WHILE)
    {
        return parse_while_stmt();
    }
    else if (p.token_type == IF)
    {
        return parse_if_stmt();
    }
    else if (p.token_type == SWITCH)
    {
        return parse_switch_stmt();
    }
    else if (p.token_type == FOR)
    {
        return parse_for_stmt();
    }
    else if (p.token_type == OUTPUT)
    {
        return parse_output_stmt();
    }
    else if (p.token_type == INPUT)
    {
        return parse_input_stmt();
    }
    else
    {
        syntax_error();
        return NULL;
    }
}

// parses stmt_list -> stmt stmt_list | stmt
struct InstructionNode* parse_stmt_list()
{
    struct InstructionNode* instl1;
    struct InstructionNode* instl2;
    instl1 = parse_stmt();
    // if nextToken == start of a statement list
    Token p = lexer.peek(1);
    if (p.token_type == ID || p.token_type == WHILE || p.token_type == IF
        || p.token_type == SWITCH || p.token_type == FOR
        || p.token_type == OUTPUT || p.token_type == INPUT)
    {
        instl2 = parse_stmt_list();
        // append instl2 to instl1
        struct InstructionNode* cur = instl1;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        cur->next = instl2;
    }
    return instl1;
}

// parses body -> LBRACE stmt_list RBRACE
struct InstructionNode* parse_body()
{
    // expects a left brace token
    expect(LBRACE);
    // parses the statement list
    struct InstructionNode* head = parse_stmt_list();
    // expects a right brace token
    expect(RBRACE);
    // returns the head of the statement list
    return head;
}

// main entry point that the provided execute.cc calls
// parses program -> var_section body inputs
struct InstructionNode* parse_Generate_Intermediate_Representation()
{
    // parses the var section
    parse_var_section();

    // parses the body and gets the head of the instruction list
    struct InstructionNode* program = parse_body();

    // parses the inputs section
    parse_inputs();

    return program;
}
