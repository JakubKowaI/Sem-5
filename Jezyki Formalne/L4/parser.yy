%require "3.2"
%language "c++"
%defines "parser.hpp"
%define api.value.type variant
%define api.token.constructor

%code requires {
  #include <iostream>
  #include <string>
  #include <vector>
  #include <cmath>
  #include <cstdio>
  #include <map>
  #include <algorithm>

  #undef at

  enum TYPE{
    T,
    I,
    O,
    U
  };

  struct VAR {
      unsigned long memory_address;
      enum TYPE type;
      unsigned long array_start;
      unsigned long array_end;
  };

  struct val {
    bool is_num;
    unsigned long bag; 
  };

  struct pid{
    bool is_t;
    bool var_index;
    unsigned long idx;
    unsigned long address;
  };

  
}

%code {
  namespace yy {
      parser::symbol_type yylex();
  }

  

  int k=0;
  std::map<std::string, VAR> var_table;
  std::map<std::string, unsigned long> procedures;
  std::vector<std::string> code;
  unsigned long memory_offset = 0;
  std::vector<std::vector<std::string>> temp_var_table;
  int depth=0;

  //Do tymczasowych zmiennych
  static std::vector<unsigned long> scope_mem_mark;
  static std::vector<std::vector<std::string>> scope_names;

  // reclaim_memory = true  -> scope blokowy: cofamy memory_offset (reuse w obrębie procedury)
  // reclaim_memory = false -> scope procedury: NIE cofamy memory_offset (unikalne adresy między procedurami)
  static void end_scope(bool reclaim_memory) {
    if (scope_names.empty()) return;

    for (const auto& name : scope_names.back()) {
      var_table.erase(name);
    }
    scope_names.pop_back();

    if (reclaim_memory) {
      memory_offset = scope_mem_mark.back();
    }
    scope_mem_mark.pop_back();
  }

  static void begin_scope() {
    scope_mem_mark.push_back(memory_offset);
    scope_names.emplace_back();
  }

  static void declare_scalar_in_scope(const std::string& name, TYPE type = U) {
    if (var_table.count(name)) {
      std::cerr << "Error: Redeclaration of " << name << std::endl;
      exit(1);
    }
    VAR t{};
    t.memory_address = memory_offset++;
    t.type = type;
    t.array_start = 0;
    t.array_end = 0;
    var_table[name] = t;

    if (!scope_names.empty()) scope_names.back().push_back(name);
  }

  std::string current;
  unsigned long Rs[8];

  void print(std::string instruction) {
      code.push_back(instruction);
  }

  unsigned long get_addr(std::string name) {
      if (var_table.find(name) == var_table.end()) {
          std::cerr << "Error: Undeclared variable " << name << std::endl;
          exit(1);
      }
      return var_table[name].memory_address;
  }

  void gen_const_to_reg(unsigned long value, char reg = 'a') {
      std::string r(1, reg);
      print("RST " + r);
      k++;
      if (value == 0) return;
      
      // Znajdź najbardziej znaczący bit
      unsigned long temp = value;
      std::string binary = "";
      while(temp > 0){
          binary += (temp % 2 == 0 ? "0" : "1");
          temp /= 2;
      }
      std::reverse(binary.begin(), binary.end());

      for (size_t i = 0; i < binary.length(); i++) {
          if (i > 0) print("SHL " + r); 
          k++;
          if (binary[i] == '1') print("INC " + r); // Dodaj 1
          k++;
      }
  }

  VAR* get_variable(std::string name) {
      if (var_table.find(name) == var_table.end()) {
          std::cerr << "Error: Undeclared variable " << name << std::endl;
          exit(1);
      }
      return  &var_table[name];
  }

  VAR* get_variable(unsigned long addr) {
      for(auto [name,v] : var_table){
        if(v.memory_address==addr){
          return &var_table[name];
        }
      }
      return  nullptr;
  }

  

  unsigned long save_at(pid p){
    if(p.var_index==1){
      print("SWP c");//wartosc
      k++;
      VAR* v = get_variable(p.address);
      if(v==nullptr)return 0;

      print("LOAD " + p.idx);
      k++;
      gen_const_to_reg(v->array_start,'b');
      print("SUB b");
      k++;
      gen_const_to_reg(p.address,'b');
      print("ADD b");
      k++;
      print("SWP c");
      k++;
      print("RSTORE c");
      k++;
    }else{
      VAR* v = get_variable(p.address);
      if(v==nullptr)return 0;
      unsigned long target=p.address+(p.idx - v->array_start);
      print("STORE " + target);
      k++;

    }
    return 1;
    

  }

  static void patch(unsigned long ins_index, int target_pc) {
    auto& s = code[ins_index];
    auto sp = s.find(' ');
    if (sp == std::string::npos) return;
    s = s.substr(0, sp + 1) + std::to_string(target_pc);
  }

  void stack_push() {
    print("RSTORE h"); 
    k++;
    print("INC h");    
    k++;
  }

  void stack_pop_to_rb() {
      print("DEC h");    
      k++;
      print("SWP b");    
      k++;
      print("RLOAD h");  
      k++;
      print("SWP b");    
      k++;
  }
  
}

%start program_all

%nterm program_all
%nterm procedures
%nterm main
%nterm commands
%nterm proc_head
%nterm proc_call
%nterm declarations
%nterm args_decl
%nterm <char> type
%nterm args
%nterm <unsigned long> expression
%nterm <bool> condition
%nterm <val> value
%nterm <pid> identifier


%token EOL "end of line"

%token <unsigned long> NUM
%token <std::string> PIDENTIFIER

%token PLUS "+"
%token MINUS "-"
%token STAR "*"
%token SLASH "/"
%token MOD "%"

%token EQUALS "="
%token NE "!="
%token GT ">"
%token LT "<"
%token GE ">="
%token LE "<="

%token LPAREN "("
%token RPAREN ")"
%token LSQUARE "["
%token RSQUARE "]"
%token COMMA ","

%token PROCEDURE
%token IS
%token IN
%token END

%token PROGRAM

%token SET ":="
%token SEMICOLON ";"
%token IF
%token THEN
%token ELSE
%token ENDIF
%token WHILE
%token DO
%token ENDWHILE
%token REPEAT
%token UNTIL
%token FOR
%token FROM
%token TO
%token ENDFOR
%token DOWNTO
%token READ
%token WRITE

%token COLON ":"

%token T
%token I
%token O

%left PLUS MINUS
%left STAR SLASH MOD

/* %left MINUS PLUS
%left STAR SLASH
%precedence NEG
%right HAT */

%%

program_all:
procedures main {
    if(procedures["PROGRAM"]!=0){
      print("JUMP " + procedures["PROGRAM"]);
    }
      for (const auto& ins : code) {
          std::cout << ins << std::endl;
      }
      print("HALT");
  }
;

procedures:
procedures PROCEDURE proc_head IS declarations IN commands END {}
| procedures PROCEDURE proc_head IS IN commands END {}
| %empty
;

main:
PROGRAM IS {
  procedures["PROGRAM"]=k;
} declarations IN commands END {

}
| PROGRAM IS {
  procedures["PROGRAM"]=k;
} IN commands END {}
;

commands:
commands command {}
| command {}
;

command:
identifier SET expression SEMICOLON {
  VAR* v = get_variable($1.address);
  switch(v->type){
    case T:
    if(!save_at($1))YYERROR;
    break;
    case I:
    YYERROR;
    break;
    case O:
    print("STORE " + std::to_string($1.address));
    k++;
    v->type = U;
    break;
    case U:
    print("STORE " + std::to_string($1.address));
    k++;
    break;
  }
}
| IF condition {
  print("SWP g");
  k++;
  print("JZERO 0");
  k++;
  $$=k;
} THEN commands {
  patch($3,code.size());
} ELSE commands ENDIF {}
| IF condition {
  print("SWP g");
  k++;
  print("JZERO 0");
  k++;
  $$=k;
} THEN commands ENDIF {
  patch($3,code.size());
}
| WHILE condition DO commands ENDWHILE {}
| REPEAT {
  $$=code.size();
} commands UNTIL condition SEMICOLON {
  print("SWP g");
  k++;
  print("JZERO " + $2);
  k++;
}
| FOR PIDENTIFIER FROM value TO value DO {
  begin_scope();
  declare_scalar_in_scope($2,U);
} commands ENDFOR {
  end_scope(true);
}
| FOR PIDENTIFIER FROM value DOWNTO value DO commands ENDFOR {}
| proc_call SEMICOLON {}
| READ identifier SEMICOLON {
  print("READ");
  k++;
  print("STORE " + std::to_string($2.address));
  k++;
}
| WRITE value SEMICOLON {
  if($2.is_num){
    gen_const_to_reg($2.bag,'a');
  }else{
    print("LOAD " + $2.bag);
    k++;
  }

  print("WRITE");
  k++;
}
| error SEMICOLON { yyerrok;}
;

proc_head:
PIDENTIFIER LPAREN {
  begin_scope();
} args_decl RPAREN {}
;

proc_call:
PIDENTIFIER LPAREN args RPAREN {}
;

declarations:
declarations COMMA PIDENTIFIER {
  VAR temp;
  temp.memory_address = memory_offset++;
  temp.type = U;
  var_table[$3]=temp;
}
| declarations COMMA PIDENTIFIER LSQUARE NUM COLON NUM RSQUARE {
  VAR t;
  t.array_start = $5;
  t.array_end = $7;
  if(t.array_start>=t.array_end) YYERROR;
  t.memory_address = memory_offset;
  memory_offset += ($7 - $5 + 1);
  t.type = T;
  var_table[$3]=t;
}
| PIDENTIFIER {
  VAR t; 
  t.memory_address = memory_offset++; 
  t.type = U;
  var_table[$1] = t;
}
| PIDENTIFIER LSQUARE NUM COLON NUM RSQUARE {
  VAR t; 
  t.type = T; 
  t.array_start = $3;
  t.array_end = $5;
  if(t.array_start>=t.array_end) YYERROR;
  t.memory_address = memory_offset;
  memory_offset += ($5 - $3 + 1);
  var_table[$1] = t;}
;

args_decl:
args_decl COMMA type PIDENTIFIER {}
| type PIDENTIFIER {
  // VAR temp;
  // temp.memory_address = memory_offset++;
  // temp.type = U;
  // temp_var_table[depth][$3]=temp;
}
;

type:
T {$<char>$='T';}
| I {$<char>$='I';}
| O {$<char>$='O';}
| %empty {$<char>$='U';}
;

args:
args COMMA PIDENTIFIER {}
| PIDENTIFIER {}
;

expression:
value {
  if($1.is_num){
    gen_const_to_reg($1.bag,'a');
  }else{
    print("LOAD " + std::to_string($1.bag));
    k++;
  }
}
| value PLUS value {
  val one = $1;
  val two = $3;

  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag));
    k++;
    print("SWP b");
    k++;
  }

  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag));
    k++;
  }

  print("ADD b");
  k++;
}
| value MINUS value {
  val one = $1;
  val two = $3;

  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag));
    k++;
    print("SWP b");
    k++;
  }

  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag));
    k++;
  }

  print("SUB b");
  k++;
}
| value STAR value {
//   RST rd         # Zerujemy wynik (rd = 0)

// LOOP_MUL:
//     RST ra     
//     ADD rc     # ra = rc (aktualne B)
//     JZERO END_MUL # Jeśli B == 0, koniec

//     # Sprawdzenie czy rc jest nieparzyste (rc % 2 != 0)
//     # Robimy to tak: re = rc; re = re / 2; re = re * 2;
//     # Jeśli (rc - re) > 0, to była reszta (czyli nieparzysta)
//     RST re
//     ADD rc     # re = rc
//     SHR re     # re = rc / 2
//     SHL re     # re = (rc / 2) * 2
//     RST ra
//     ADD rc     # ra = rc
//     SUB re     # ra = rc - re
    
//     JZERO SKIP_ADD # Jeśli ra == 0, to liczba parzysta, pomiń dodawanie

//     # Jeśli nieparzysta, dodajemy A (rb) do wyniku (rd)
//     RST ra
//     ADD rd
//     ADD rb
//     SWP rd     # rd = rd + rb

// SKIP_ADD:
//     SHL rb     # A = A * 2
//     SHR rc     # B = B / 2
//     JUMP LOOP_MUL

// END_MUL:
//     RST ra
//     ADD rd     # Przeniesienie wyniku do ra
}
| value SLASH value {}
| value MOD value {}
;

condition:
value EQUALS value {//do optymalizacji
  val one = $1;
  val two = $3;

  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag));
    k++;
    print("SWP b");
    k++;
  }

  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag));
    k++;
  }

  print("SUB b");
  k++;
  print("SWP c");
  k++;

  if(one.is_num){
    gen_const_to_reg(one.bag,'b');
  }else{
    print("LOAD " + std::to_string(one.bag));
    k++;
    print("SWP b");
    k++;
  }

  if(two.is_num){
    gen_const_to_reg(two.bag,'a');
  }else{
    print("LOAD " + std::to_string(two.bag));
    k++;
  }
  print("ADD c");
  k++;
  print("RST g");
  k++;
  print("JPOS " + k + 2);
  k++;
  print("INC g");
  k++;
}
| value NE value {//do optymalizacji
  val one = $1;
  val two = $3;

  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag));
    k++;
    print("SWP b");
    k++;
  }

  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag));
    k++;
  }

  print("SUB b");
  k++;
  print("SWP c");
  k++;

  if(one.is_num){
    gen_const_to_reg(one.bag,'b');
  }else{
    print("LOAD " + std::to_string(one.bag));
    k++;
    print("SWP b");
    k++;
  }

  if(two.is_num){
    gen_const_to_reg(two.bag,'a');
  }else{
    print("LOAD " + std::to_string(two.bag));
    k++;
  }
  print("ADD c");
  k++;
  print("RST g");
  k++;
  print("JZERO " + k + 2);
  k++;
  print("INC g");
  k++;
}
| value GT value {
  val one = $1;
  val two = $3;

  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag));
    k++;
    print("SWP b");
    k++;
  }


  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag));
    k++;
  }

  print("SUB b");
  k++;
  print("RST g");
  k++;
  print("JPOS " + k + 2);
  k++;
  print("INC g");
  k++;
}
| value LT value {
  val one = $1;
  val two = $3;

  if(one.is_num){
    gen_const_to_reg(one.bag,'b');
  }else{
    print("LOAD " + std::to_string(one.bag));
    k++;
    print("SWP b");
    k++;
  }

  if(two.is_num){
    gen_const_to_reg(two.bag,'a');
  }else{
    print("LOAD " + std::to_string(two.bag));
    k++;
  }

  print("SUB b");
  k++;
  print("RST g");
  k++;
  print("JPOS " + k + 2);
  k++;
  print("INC g");
  k++;
}
| value GE value {
  val one = $1;
  val two = $3;

  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag));
    k++;
    print("SWP b");
    k++;
  }

  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag));
    k++;
  }

  print("SUB b");
  k++;
  print("RST g");
  k++;
  
  print("JPOS " + k + 2);//skok do x
  k++;
  int temp=code.size();

  if(one.is_num){
    gen_const_to_reg(one.bag,'c');
  }else{
    print("LOAD " + std::to_string(one.bag));
    k++;
    print("SWP c");
    k++;
  }

  print("SWP b");
  k++;
  print("SUB c");
  k++;

  int jump=k+2;
  print("JPOS " + jump);
  k++;
  code[temp]="JPOS " + std::to_string(jump);
  print("INC g");//x
  k++;
}
| value LE value {
  val one = $1;
  val two = $3;

  if(one.is_num){
    gen_const_to_reg(one.bag,'b');
  }else{
    print("LOAD " + std::to_string(one.bag));
    k++;
    print("SWP b");
    k++;
  }

  if(two.is_num){
    gen_const_to_reg(two.bag,'a');
  }else{
    print("LOAD " + std::to_string(two.bag));
    k++;
  }

  print("SUB b");
  k++;
  print("RST g");
  k++;
  
  print("JPOS " + k + 2);//skok do x
  k++;
  int temp=code.size();

  if(two.is_num){
    gen_const_to_reg(two.bag,'c');
  }else{
    print("LOAD " + std::to_string(two.bag));
    k++;
    print("SWP c");
    k++;
  }

  print("SWP b");
  k++;
  print("SUB c");
  k++;

  int jump=k+2;
  print("JPOS " + jump);
  k++;
  code[temp]="JPOS " + std::to_string(jump);
  print("INC g");//x
  k++;
}
;

/* Zwraca wartość lub adres*/
value:
NUM {
  val t;
  t.is_num=1;
  t.bag=$1;
  $<val>$=t;
}
| identifier {
  val t;
  t.is_num=0;
  t.bag=$1.address;
  $<val>$=t;
}
;

/* Zapisuj adres do re */
identifier:
PIDENTIFIER {
  if(get_variable($1)->type==O)YYERROR;
  pid t;
  t.is_t=0;
  t.var_index=0;
  t.idx=0;
  t.address=get_addr($1);
  $<pid>$=t;
}
| PIDENTIFIER LSQUARE PIDENTIFIER RSQUARE {
  if(get_variable($1)->type!=T)YYERROR;
  // print("LOAD " + get_addr($3));
  // VAR* tab = get_variable($1);
  // gen_const_to_reg(tab->array_start,'b');
  // print("SUB b");
  // gen_const_to_reg(tab->memory_address,'b');
  // print("ADD b");
  // print("SWP e");

  pid t;
  t.is_t=1;
  t.var_index=1;
  t.idx=get_addr($3);
  t.address=get_addr($1);
  $<pid>$=t;
}
| PIDENTIFIER LSQUARE NUM RSQUARE {
  VAR* tab = get_variable($1);
  if(tab->type!=T)YYERROR;
  if($3<tab->array_start||$3>tab->array_end)YYERROR;
  gen_const_to_reg($3-tab->array_start+tab->memory_address,'e');

  pid t;
  t.is_t=1;
  t.var_index=0;
  t.idx=$3;
  t.address=get_addr($1);
  $<pid>$=t;
}
;

%%

namespace yy {
  void parser::error (const std::string& msg) {
    std::cerr << "Error: " << msg << '\n';
  }
}

int main () {
  yy::parser parse;
  return parse ();
}