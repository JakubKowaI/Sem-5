%require "3.2"
%language "c++"
%defines "parser.hpp"
%define api.value.type variant
%define api.token.constructor
%define parse.error verbose

%code requires {
  #include <iostream>
  #include <string>
  #include <vector>
  #include <cmath>
  #include <cstdio>
  #include <fstream>
  #include <map>
  #include <algorithm>
  #include <stack>
  #include <unordered_set>

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

  struct FormalParam { TYPE type; unsigned long addr; };
  struct LoopPatch { unsigned long start; unsigned long jexit; };
}

%code {
  namespace yy {
      parser::symbol_type yylex();
  }

  

  unsigned long k=0;
  std::map<std::string, VAR> var_table;
  std::map<std::string, unsigned long> procedures;
  std::vector<std::string> code;
  unsigned long memory_offset = 0;
  std::stack<unsigned long> temp;

  static std::stack<unsigned long> if_false_stack;
  static std::stack<unsigned long> if_end_stack;
  static std::stack<unsigned long> repeat_stack;
  static std::stack<unsigned long> for_stack;

  static std::stack<LoopPatch> while_patches;
  static std::stack<LoopPatch> for_patches;

  static unsigned long start_point=0;

  static std::map<std::string, std::vector<FormalParam>> proc_formals;
  static std::string current_proc_name;
  static std::string current_call_name;
  static std::vector<std::string> current_call_args;
  static std::vector<FormalParam> current_formals;

  
  static std::vector<unsigned long> scope_mem_mark;
  static std::vector<std::vector<std::string>> scope_names;

  static std::unordered_set<unsigned long> ref_cells;
  static std::unordered_set<unsigned long> array_ref_cells;

  static inline bool is_ref_cell(unsigned long addr) {
    return ref_cells.find(addr) != ref_cells.end();
  }
  static inline bool is_array_ref_cell(unsigned long addr) {
    return array_ref_cells.find(addr) != array_ref_cells.end();
  }

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

  static void declare_var_in_scope(VAR v,std::string name) {
    if (var_table.find(name) != var_table.end()) {
      std::cerr << "Error: Redeclaration of " << name << std::endl;
      exit(1);
    }
    
    var_table[name] = v;
    scope_names.back().push_back(name);
  }

  std::string current;
  unsigned long Rs[8];

  void print(std::string instruction) {
      code.push_back(instruction);
  }

  static inline unsigned long pc() { return (unsigned long)code.size(); }

  unsigned long get_addr(std::string name) {
      if (var_table.find(name) == var_table.end()) {
          std::cerr << "Error: Undeclared variable " << name << std::endl;
          exit(1);
      }
      return var_table[name].memory_address;
  }

  // Odczyt skalaru: jeśli to ref-param, to RLOAD spod adresu z komórki parametru.
  static void load_scalar_by_addr_or_ref(unsigned long addr) {
    if (!is_ref_cell(addr)) {
      print("LOAD " + std::to_string(addr)); k++;
      return;
    }
    // a = mem[ mem[addr] ]
    print("LOAD " + std::to_string(addr)); k++; // a = ptr
    print("SWP e"); k++;
    print("RLOAD e"); k++;
  }

  // Zapis skalaru: jeśli to ref-param, to RSTORE pod adres z komórki parametru.
  // Zakłada: wartość jest w rejestrze a.
  static void store_scalar_by_addr_or_ref(unsigned long addr) {
    if (!is_ref_cell(addr)) {
      print("STORE " + std::to_string(addr)); k++;
      return;
    }
    // mem[ mem[addr] ] = a
    print("SWP c"); k++;                    // c = value
    print("LOAD " + std::to_string(addr)); k++; // a = ptr
    print("SWP e"); k++;                    // e = ptr
    print("SWP c"); k++;                    // a = value
    print("RSTORE e"); k++;
  }

  void gen_const_to_reg(unsigned long value, char reg = 'a') {
      std::string r(1, reg);
      print("RST " + r);
      k++;
      if (value == 0){
        return;
      } 
      
      // Znajdź najbardziej znaczący bit
      unsigned long t = value;
      std::string binary = "";
      while(t > 0){
          binary += (t % 2 ? "1" : "0");
          t /= 2;
      }
      std::reverse(binary.begin(), binary.end());

      for (size_t i = 0; i < binary.length(); i++) {
          if (i > 0) print("SHL " + r); 
          k++;
          if (binary[i] == '1') print("INC " + r); // Dodaj 1
          k++;
      }
  }

  static void store_through_pointer_cell(unsigned long pointer_cell_addr) {
    print("SWP c"); k++;                                    
    print("LOAD " + std::to_string(pointer_cell_addr)); k++; 
    print("SWP b"); k++;                                    
    print("SWP c"); k++;                                    
    print("RSTORE b"); k++;                                 
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

      print("LOAD " + std::to_string(p.idx));
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
      print("STORE " + std::to_string(target));
      k++;

    }
    return 1;
    

  }

  static void patch(unsigned long ins_index, int target_pc) {
    if (ins_index >= code.size()) return;
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

  static void push_ra() {
    // push(ra): mem[h]=ra; h++
    print("RSTORE h"); k++;
    print("INC h");    k++;
  }

  static void pop_ra() {
    // pop(ra): h--; ra=mem[h]
    print("DEC h");    k++;
    print("RLOAD h");  k++;
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

  static void invert_g() {
  // g := 1 - g, zakładając że g jest 0/1
  // Używa ra do testu, patchuje skoki po pc()
  print("RST a"); k++;
  print("ADD g"); k++;                 // a = g
  unsigned long j_is_zero = (unsigned long)pc();
  print("JZERO 0"); k++;               // jeśli g==0 -> ustawimy g=1

  // g != 0 => g = 0
  print("RST g"); k++;
  unsigned long j_end = (unsigned long)pc();
  print("JUMP 0"); k++;

  // g == 0 => g = 1
  patch(j_is_zero, pc());
  print("RST g"); k++;
  print("INC g"); k++;

  patch(j_end, pc());
}

  void gt(val one,val two){
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
    unsigned long jskip = (unsigned long)pc();
    print("JZERO 0");
    k++;
    print("INC g");
    k++;
    patch(jskip, pc());
  }

  void lt(val one, val two){
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
    unsigned long jskip = (unsigned long)pc();
    print("JZERO 0");
    k++;
    print("INC g");
    k++;
    patch(jskip, pc());
  }

  // void ge(val one, val two){
  //   if(two.is_num){
  //     gen_const_to_reg(two.bag,'b');
  //   }else{
  //     print("LOAD " + std::to_string(two.bag));
  //     k++;
  //     print("SWP b");
  //     k++;
  //   }


  //   if(one.is_num){
  //     gen_const_to_reg(one.bag,'a');
  //   }else{
  //     print("LOAD " + std::to_string(one.bag));
  //     k++;
  //   }

  //   print("SUB b"); k++;
  //   print("RST g"); k++;
  //   print("INC g"); k++;
  //   unsigned long jset0 = (unsigned long)pc();
  //   print("JPOS 0");
  //   k++;
  //   unsigned long jend = (unsigned long)pc();
  //   print("JUMP 0"); k++;
  //   patch(jset0, pc());
  //   print("RST g"); k++;
  //   patch(jend,pc());
  // }

  void ge(val one, val two){
  // one >= two  <=>  NOT(two > one)
  gt(two, one);
  invert_g();
}

  // void le(val one, val two){
  //   if(one.is_num){
  //     gen_const_to_reg(one.bag,'b');
  //   }else{
  //     print("LOAD " + std::to_string(one.bag));
  //     k++;
  //     print("SWP b");
  //     k++;
  //   }

  //   if(two.is_num){
  //     gen_const_to_reg(two.bag,'a');
  //   }else{
  //     print("LOAD " + std::to_string(two.bag));
  //     k++;
  //   }

  //   print("SUB b"); k++;
  //   print("RST g"); k++;
  //   print("INC g"); k++;
  //   unsigned long jset0 = (unsigned long)pc();
  //   print("JPOS 0"); k++;
  //   unsigned long jend = (unsigned long)pc();
  //   print("JUMP 0"); k++;
  //   patch(jset0, pc());
  //   print("RST g"); k++;
  //   patch(jend, pc());
  // }

  void le(val one, val two){
  // one <= two  <=>  NOT(one > two)
  gt(one, two);
  invert_g();
}

  // void equals(val one, val two){
  //   if(two.is_num){
  //     gen_const_to_reg(two.bag,'b');
  //   }else{
  //     print("LOAD " + std::to_string(two.bag));
  //     k++;
  //     print("SWP b");
  //     k++;
  //   }

  //   if(one.is_num){
  //     gen_const_to_reg(one.bag,'a');
  //   }else{
  //     print("LOAD " + std::to_string(one.bag));
  //     k++;
  //   }

  //   print("SUB b");
  //   k++;
  //   print("SWP c");
  //   k++;
    
    
  //   if(one.is_num){
  //     print("SWP b");
  //     k++;
  //     gen_const_to_reg(one.bag,'b');
  //   }else{
  //     print("LOAD " + std::to_string(one.bag));
  //     k++;
  //     print("SWP b");
  //     k++;
  //   }

  //   print("SUB b");
  //   k++;
  //   print("ADD c");
  //   k++;
  //   print("RST g");
  //   k++;
  //   unsigned long jset = pc(); 
  //   print("JPOS 0");
  //   k++;
  //   print("INC g");
  //   k++;
  //   patch(jset, pc());
  // }

  void equals(val one, val two){
  // equals: (one==two) <=> max(one-two,0)+max(two-one,0) == 0
  // r1 = max(one-two,0) -> c
  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag)); k++;
    print("SWP b"); k++;
  }

  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag)); k++;
  }

  print("SUB b"); k++;
  print("SWP c"); k++; // c = r1

  // r2 = max(two-one,0) -> a
  if(one.is_num){
    gen_const_to_reg(one.bag,'b');
  }else{
    print("LOAD " + std::to_string(one.bag)); k++;
    print("SWP b"); k++;
  }

  if(two.is_num){
    gen_const_to_reg(two.bag,'a');
  }else{
    print("LOAD " + std::to_string(two.bag)); k++;
  }

  print("SUB b"); k++;  // a = r2
  print("ADD c"); k++;  // a = r1+r2 (sum)

  // g = 1 jeśli sum==0, inaczej 0
  print("RST g"); k++;
  print("INC g"); k++;            // g=1
  unsigned long j_end = (unsigned long)pc();
  print("JZERO 0"); k++;          // jeśli sum==0 -> zostaw g=1 i skocz na koniec
  print("RST g"); k++;            // sum>0 -> g=0
  patch(j_end, pc());
}

  // void ne(val one, val two){
  //   if(two.is_num){
  //     gen_const_to_reg(two.bag,'b');
  //   }else{
  //     print("LOAD " + std::to_string(two.bag));
  //     k++;
  //     print("SWP b");
  //     k++;
  //   }

  //   if(one.is_num){
  //     gen_const_to_reg(one.bag,'a');
  //   }else{
  //     print("LOAD " + std::to_string(one.bag));
  //     k++;
  //   }

  //   print("SUB b");
  //   k++;
  //   print("SWP c");
  //   k++;

  //   if(one.is_num){
  //     gen_const_to_reg(one.bag,'b');
  //   }else{
  //     print("LOAD " + std::to_string(one.bag));
  //     k++;
  //     print("SWP b");
  //     k++;
  //   }

  //   if(two.is_num){
  //     gen_const_to_reg(two.bag,'a');
  //   }else{
  //     print("LOAD " + std::to_string(two.bag));
  //     k++;
  //   }
  //   print("ADD c");
  //   k++;
  //   print("RST g");
  //   k++;
  //   unsigned long jset = pc();
  //   print("JZERO 0");
  //   k++;
  //   print("INC g");
  //   k++;
  //   patch(jset, pc());
  // }

  void ne(val one, val two){
  // ne: (one!=two) <=> r1+r2 > 0
  // r1 = max(one-two,0) -> c
  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag)); k++;
    print("SWP b"); k++;
  }

  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag)); k++;
  }

  print("SUB b"); k++;
  print("SWP c"); k++; // c = r1

  // r2 = max(two-one,0) -> a
  if(one.is_num){
    gen_const_to_reg(one.bag,'b');
  }else{
    print("LOAD " + std::to_string(one.bag)); k++;
    print("SWP b"); k++;
  }

  if(two.is_num){
    gen_const_to_reg(two.bag,'a');
  }else{
    print("LOAD " + std::to_string(two.bag)); k++;
  }

  print("SUB b"); k++;  // a = r2
  print("ADD c"); k++;  // a = sum

  // g = 1 jeśli sum>0, inaczej 0
  print("RST g"); k++;
  unsigned long jskip = (unsigned long)pc();
  print("JZERO 0"); k++;  // sum==0 -> pomiń INC
  print("INC g"); k++;    // sum>0 -> true
  patch(jskip, pc());
}


  static void emit(const std::string& ins) {
    print(ins);
    k++;
  }

  static void load_value_to_a(const val& v) {
    if (v.is_num) {
      gen_const_to_reg(v.bag, 'a');
    } else {
      emit("LOAD " + std::to_string(v.bag));
    }
  }

  // Wylicz adres elementu tablicy dla pid (tab[indeks]) i zostaw go w rejestrze e.
  // Obsługuje:
  // - zwykłą tablicę: base = tab.memory_address, index = i, adres = base + (i - array_start)
  // - tablicę T-param (array_ref_cells): w komórce tab.memory_address siedzi base0 = base - array_start,
  //   więc adres = base0 + i
  static void array_elem_addr_to_e(const pid& p) {
    VAR* tab = get_variable(p.address);

  const bool is_ref = is_array_ref_cell(tab->memory_address);

  // b = base (albo base0 przy T-param)
  if (is_ref) {
    print("LOAD " + std::to_string(tab->memory_address)); k++; // a = base0
    print("SWP b"); k++;
  } else {
    gen_const_to_reg(tab->memory_address, 'a'); // baza indeksu 0
    print("SWP b"); k++;
  }

  // a = index (bez -array_start)
  if (p.var_index) {
    print("LOAD " + std::to_string(p.idx)); k++;
  } else {
    gen_const_to_reg(p.idx, 'a');
  }

  print("ADD b"); k++;
  print("SWP e"); k++;
  }

  // Mnożenie w czasie O(log b): "Russian peasant multiplication".
  // Wejście: left, right; Wyjście: ra = left * right
  // Używa rejestrów: b (A), c (B), d (res), e (temp)
  static void gen_mul(const val& left, const val& right) {
    // rb = A
    load_value_to_a(left);
    emit("SWP b");

    // rc = B
    load_value_to_a(right);
    emit("SWP c");

    emit("RST d"); // res=0

    int loop_pc = pc();

    // if (B == 0) break
    emit("RST a");
    emit("ADD c");
    int jend = pc();
    emit("JZERO 0");

    // e = B; e = 2*(B/2)
    emit("RST a");
    emit("ADD c");
    emit("SWP e");
    emit("SHR e");
    emit("SHL e");

    // a = B - e (0 gdy parzyste, 1 gdy nieparzyste)
    emit("RST a");
    emit("ADD c");
    emit("SUB e");
    int jskip_add = pc();
    emit("JZERO 0");

    // res += A
    emit("SWP d");
    emit("ADD b");
    emit("SWP d");

    patch((unsigned long)jskip_add, pc());

    // A <<= 1; B >>= 1
    emit("SHL b");
    emit("SHR c");
    emit("JUMP " + std::to_string(loop_pc));

    patch((unsigned long)jend, pc());

    // return res in ra
    emit("RST a");
    emit("ADD d");
  }

  // Dzielenie / modulo w czasie O(log n): binarne "long division".
  // Wejście: n = left, d = right; Wyjście: ra = (want_quotient ? q : r)
  // Rejestry: b (divisor), c (remainder), d (quotient), e (dshift), f (shift)
  static void gen_divmod(const val& left, const val& right, bool want_quotient) {
    // rc = n
    load_value_to_a(left);
    emit("SWP c");

    // rb = d
    load_value_to_a(right);
    emit("SWP b");

    emit("RST d"); // q=0

    // if d == 0 -> wynik 0
    emit("RST a");
    emit("ADD b");
    int jdiv0 = pc();
    emit("JZERO 0");

    // e = d, f = 0
    emit("RST a");
    emit("ADD b");
    emit("SWP e");
    emit("RST f");

    // znajdź największe przesunięcie: while (e <= r) { e<<=1; f++; }
    int sh_loop = pc();
    emit("RST a");
    emit("ADD e");
    emit("SUB c");
    int jbreak_sh = pc();
    emit("JPOS 0"); // jeśli e-r > 0 to e>r, przerwij
    emit("SHL e");
    emit("INC f");
    emit("JUMP " + std::to_string(sh_loop));
    patch((unsigned long)jbreak_sh, pc());

    // backshift jeśli f>0 (bo pętla wyszła z e>r)
    emit("RST a");
    emit("ADD f");
    int jskip_back = pc();
    emit("JZERO 0");
    emit("SHR e");
    emit("DEC f");
    patch((unsigned long)jskip_back, pc());

    // główna pętla dzielenia: dla i=f..0
    int div_loop = pc();
    emit("SHL d"); // q <<= 1

    // if (r >= e) { r -= e; q++; }
    emit("RST a");
    emit("ADD e");
    emit("SUB c");
    int jskip_sub = pc();
    emit("JPOS 0"); // jeśli e>r, pomiń odejmowanie

    emit("RST a");
    emit("ADD c");
    emit("SUB e");
    emit("SWP c"); // r = r - e
    emit("INC d"); // q++

    patch((unsigned long)jskip_sub, pc());

    emit("SHR e");

    emit("RST a");
    emit("ADD f");
    int jend_div = pc();
    emit("JZERO 0");
    emit("DEC f");
    emit("JUMP " + std::to_string(div_loop));
    patch((unsigned long)jend_div, pc());

    // wynik normalny
    if (want_quotient) {
      emit("RST a");
      emit("ADD d");
    } else {
      emit("RST a");
      emit("ADD c");
    }

    int jend_all = pc();
    emit("JUMP 0");

    // div0 handler
    patch((unsigned long)jdiv0, pc());
    emit("RST a");
    emit("RST c");
    emit("RST d");

    patch((unsigned long)jend_all, pc());
  }
  
}

%start program_all

%nterm program_all
%nterm procedures
%nterm main
%nterm commands
%nterm command
%nterm proc_head
%nterm proc_call
%nterm declarations
%nterm args_decl
%nterm <TYPE> type
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

%token typeT
%token typeI
%token typeO

%left PLUS MINUS
%left STAR SLASH MOD

%nterm subroutine

%%

program_all:
{
    gen_const_to_reg(100000UL , 'h');
    start_point =(unsigned long) pc();
    //std::cerr<<"SAM START "<<start_point<<std::endl;
    print("JUMP 0"); k++;
}
procedures main {
    //std::cerr<<"START "<<start_point<<std::endl;
    patch(start_point, (int)procedures["PROGRAM"]);
    print("HALT");
    k++;
}
;

procedures:
procedures PROCEDURE proc_head IS declarations IN commands END { 
  pop_ra();
  print("RTRN");    k++;

  end_scope(false);
 }
| procedures PROCEDURE proc_head IS IN commands END { 
  pop_ra();
  print("RTRN");    k++;

  end_scope(false); 
  }
| %empty
;

main:
PROGRAM IS {
  procedures["PROGRAM"]=pc();
  begin_scope();
} declarations IN commands END {

}
| PROGRAM IS {
  procedures["PROGRAM"]=pc();
  begin_scope();
} IN commands END {}
;

commands:
commands command {}
| command {}
;

subroutine:
%empty {print("SWP g");
  k++;
  if_false_stack.push((unsigned long)pc());
  print("JZERO 0");
  k++;
  }
  ;

command:
identifier SET expression SEMICOLON {
  VAR* v = get_variable($1.address);
  if (v && v->type == I) YYERROR; // I nie może być nadpisywane

  if ($1.is_t) {
    // zapis do elementu tablicy
    // value (expr) jest w rejestrze a
    print("SWP c"); k++;          // c = value
    array_elem_addr_to_e($1);     // e = addr(tab[idx])
    print("SWP c"); k++;          // a = value
    print("RSTORE e"); k++;      
  } else {
    store_scalar_by_addr_or_ref($1.address);
  }
}
| IF condition subroutine THEN commands ENDIF {
  patch(if_false_stack.top(), pc());
  if_false_stack.pop();
  }
| IF condition subroutine THEN commands ELSE {
  if_end_stack.push((unsigned long)pc());
  print("JUMP 0"); k++;
  patch(if_false_stack.top(), pc());
  if_false_stack.pop();
} commands ENDIF {
  patch(if_end_stack.top(), pc());
  if_end_stack.pop();
}
| WHILE {
  while_patches.push(LoopPatch{(unsigned long)pc(), 0});   // start pętli
} condition DO {
  print("SWP g"); k++;
  while_patches.top().jexit = (unsigned long)pc();
  print("JZERO 0"); k++;
} commands ENDWHILE {
  auto lp = while_patches.top(); while_patches.pop();
  print("JUMP " + std::to_string(lp.start)); k++;
  patch(lp.jexit, pc());
}
| REPEAT {
  repeat_stack.push((unsigned long)pc()); // początek pętli (body)
} commands UNTIL condition SEMICOLON {
  print("SWP g");
  k++;
  print("JZERO " + std::to_string(repeat_stack.top()));
  k++;
  repeat_stack.pop();
}
| FOR PIDENTIFIER FROM value TO value DO {
  begin_scope();

  VAR it;
  it.memory_address=memory_offset++;
  it.type=U;
  it.array_start=0;
  it.array_end=0;
  declare_var_in_scope(it,$2);

  if($4.is_num){
    gen_const_to_reg($4.bag,'a');
  }else{
    print("LOAD " + std::to_string($4.bag)); k++;
  }
  print("STORE " + std::to_string(it.memory_address)); k++;

  val iter;
  iter.is_num=0;
  iter.bag=it.memory_address;

  unsigned long jstart = (unsigned long)pc();

  le(iter,$6);
  print("SWP g"); k++;
  unsigned long jexit = (unsigned long)pc();
  print("JZERO 0"); k++;
  for_patches.push(LoopPatch{jstart, jexit});
  for_stack.push(it.memory_address);
} commands {

  print("LOAD " + std::to_string(for_stack.top())); k++;
  print("INC a"); k++;
  print("STORE " + std::to_string(for_stack.top())); k++;
  for_stack.pop();

  unsigned long jexit = temp.top(); temp.pop();
  unsigned long jstart = temp.top(); temp.pop();

  auto lp = for_patches.top(); // nie pop jeszcze (pop w ENDFOR)
  print("JUMP " + std::to_string(lp.start)); k++;
} ENDFOR {
  auto lp = for_patches.top(); for_patches.pop();
  patch(lp.jexit, pc());
  end_scope(true);
}
| FOR PIDENTIFIER FROM value DOWNTO value DO {
  begin_scope();

  VAR it;
  it.memory_address=memory_offset++;
  it.type=U;
  it.array_start=0;
  it.array_end=0;
  declare_var_in_scope(it,$2);

  if($4.is_num){
    gen_const_to_reg($4.bag,'a');
  }else{
    print("LOAD " + std::to_string($4.bag)); k++;
  }
  print("STORE " + std::to_string(it.memory_address)); k++;

  val iter;
  iter.is_num=0;
  iter.bag=it.memory_address;

  unsigned long jstart = (unsigned long)pc();

  ge(iter,$6);
  print("SWP g"); k++;
  unsigned long jexit = (unsigned long)pc();
  print("JZERO 0"); k++;
  for_patches.push(LoopPatch{jstart, jexit});
  for_stack.push(it.memory_address);
} commands {
  print("LOAD " + std::to_string(for_stack.top())); k++;
  print("DEC a"); k++;
  print("STORE " + std::to_string(for_stack.top())); k++;
  for_stack.pop();

  unsigned long jexit = temp.top(); temp.pop();
  unsigned long jstart = temp.top(); temp.pop();

  auto lp = for_patches.top(); // nie pop jeszcze (pop w ENDFOR)
  print("JUMP " + std::to_string(lp.start)); k++;
} ENDFOR {
  auto lp = for_patches.top(); for_patches.pop();
  patch(lp.jexit, pc());
  end_scope(true);
}
| proc_call SEMICOLON {
  //end_scope(false);
}
| READ identifier SEMICOLON {
  print("READ"); k++;
  // READ do tablicy też powinien działać: jeśli tab[i], to RSTORE pod adres elementu.
  if ($2.is_t) {
    // a = input; zapisz do tab[idx]
    print("SWP c"); k++;
    array_elem_addr_to_e($2);
    print("SWP c"); k++;
    print("RSTORE e"); k++;
  } else {
    print("STORE " + std::to_string($2.address)); k++;
  }
}
| WRITE value SEMICOLON {
  if($2.is_num){
    gen_const_to_reg($2.bag,'a');
  }else{
    print("LOAD " + std::to_string($2.bag));
    k++;
  }

  print("WRITE");
  k++;
}
| error SEMICOLON { yyerrok;}
;

proc_head:
PIDENTIFIER LPAREN {
  procedures[$1] = (unsigned long)pc();

  begin_scope();

  push_ra();

  current_proc_name = $1;
  current_formals.clear();
} args_decl RPAREN {
  proc_formals[current_proc_name] = current_formals;
}
;

proc_call:
PIDENTIFIER {
  current_call_name = $1;
  current_call_args.clear();
}LPAREN args RPAREN {
  const auto& formals = proc_formals[current_call_name];
  if (formals.size() != current_call_args.size()) YYERROR;

  for (size_t i = 0; i < formals.size(); ++i) {
    unsigned long formal_cell = formals[i].addr;

    if (formals[i].type == T) {
      VAR* actual = get_variable(current_call_args[i]);
      if (!actual || actual->type != T) YYERROR;

      // base0 = baza indeksu 0
      unsigned long base0 = actual->memory_address;

      gen_const_to_reg(base0, 'a');
      print("STORE " + std::to_string(formal_cell)); k++;
      continue;
    }

    // I/O: przechowuj adres zmiennej
    unsigned long actual_addr = get_addr(current_call_args[i]);
    gen_const_to_reg(actual_addr, 'a');
    print("STORE " + std::to_string(formal_cell)); k++;
  }

  print("CALL " + std::to_string(procedures[current_call_name])); k++;
}
;

declarations:
declarations COMMA PIDENTIFIER {
  VAR t;
  t.memory_address = memory_offset++;
  t.type = U;
  t.array_start = 0;
  t.array_end = 0;
  declare_var_in_scope(t, $3);
}
| declarations COMMA PIDENTIFIER LSQUARE NUM COLON NUM RSQUARE {
  VAR t;
  t.array_start = $5;
  t.array_end = $7;
  if(t.array_start>=t.array_end) YYERROR;
  t.memory_address = memory_offset;
  memory_offset += (t.array_end + 1);
  t.type = T;
  declare_var_in_scope(t, $3);
}
| PIDENTIFIER {
  VAR t;
  t.memory_address = memory_offset++;
  t.type = U;
  t.array_start = 0;
  t.array_end = 0;
  declare_var_in_scope(t, $1);
}
| PIDENTIFIER LSQUARE NUM COLON NUM RSQUARE {
  VAR t; 
  t.type = T; 
  t.array_start = $3;
  t.array_end = $5;
  if(t.array_start>=t.array_end) YYERROR;
  t.memory_address = memory_offset;
  memory_offset += (t.array_end + 1);
  declare_var_in_scope(t, $1);
  }
;

args_decl:
args_decl COMMA type PIDENTIFIER {
VAR p;
  p.memory_address = memory_offset++;
  p.type = $3;
  p.array_start = 0; p.array_end = 0;

  declare_var_in_scope(p, $4);

  // oznacz, że to "komórka-wskaźnik"
  if (p.type == T) array_ref_cells.insert(p.memory_address);
  else             ref_cells.insert(p.memory_address);

  current_formals.push_back(FormalParam{p.type, p.memory_address});
  }
| type PIDENTIFIER {
  VAR p;
  p.memory_address = memory_offset++;
  p.type = $1;
  p.array_start = 0; p.array_end = 0;

  declare_var_in_scope(p, $2);

  if (p.type == T) array_ref_cells.insert(p.memory_address);
  else             ref_cells.insert(p.memory_address);

  current_formals.push_back(FormalParam{p.type, p.memory_address});
}
;

type:
typeT {$$=T;}
| typeI {$$=I;}
| typeO {$$=O;}
| %empty {$$=U;}
;

args:
args COMMA PIDENTIFIER {current_call_args.push_back($3);}
| PIDENTIFIER {current_call_args.push_back($1);}
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
  gen_mul($1, $3);
  // print("LOAD " + std::to_string($3.bag));
  // k++;
  // print("SHL a");
  // k++;
}
| value SLASH value {
  gen_divmod($1, $3, true);
  // print("LOAD " + std::to_string($1.bag));
  // k++;
  // print("SHR a");
  // k++;
}
| value MOD value {
  gen_divmod($1, $3, false);
}
;

condition:
value EQUALS value {//do optymalizacji
  equals($1,$3);
}
| value NE value {//do optymalizacji
  ne($1,$3);
}
| value GT value {
  gt($1,$3);
}
| value LT value {
  lt($1,$3);
}
| value GE value {
  ge($1,$3);
}
| value LE value {
  le($1,$3);
}
;

/* Zwraca wartość lub adres*/
value:
NUM {
  val t;
  t.is_num=1;
  t.bag=$1;
  $$=t;
}
| identifier {
  VAR* v = get_variable($1.address);

  // O nie wolno czytać jako wartości
  if (v && v->type == O) YYERROR;

  if (!$1.is_t) {
    unsigned long tmp = memory_offset++;
    load_scalar_by_addr_or_ref($1.address);
    print("STORE " + std::to_string(tmp)); k++;

    val t; t.is_num=0; t.bag=tmp; $$=t;
  } else {
    // odczyt elementu tablicy jako wartość
    VAR* tab = get_variable($1.address);
    if (!tab || tab->type != T) YYERROR;

    const bool is_ref = is_array_ref_cell(tab->memory_address);

    // Jeśli to zwykła tablica i indeks stały, możemy zwrócić bezpośredni adres komórki.
    if (!is_ref && !$1.var_index) {
      unsigned long target = tab->memory_address + $1.idx;
      val t; t.is_num=0; t.bag=target; $$=t;
    } else {
      // T-param albo indeks zmienny => RLOAD spod obliczonego adresu
      unsigned long tmp = memory_offset++;

      if(!$1.is_t)YYERROR;
      array_elem_addr_to_e($1);     // e = addr(tab[idx])
      print("RLOAD e"); k++;        // a = mem[e]
      print("STORE " + std::to_string(tmp)); k++;

      val t; t.is_num=0; t.bag=tmp; $$=t;
    }
  }
}
;

/* Zapisuj adres do re */
identifier:
PIDENTIFIER {
  //if(get_variable($1)->type==O)YYERROR;
  pid t;
  t.is_t=0;
  t.var_index=0;
  t.idx=0;
  t.address=get_addr($1);
  $$=t;
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
  $$=t;
}
| PIDENTIFIER LSQUARE NUM RSQUARE {
  VAR* tab = get_variable($1);
  if(tab->type!=T)YYERROR;
  if($3<tab->array_start||$3>tab->array_end)YYERROR;

  pid t;
  t.is_t=1;
  t.var_index=0;
  t.idx=$3;
  t.address=get_addr($1);
  $$=t;
}
;

%%

namespace yy {
  void parser::error (const std::string& msg) {
    std::cerr << "Error: " << msg << '\n';
  }
}

extern FILE* yyin;

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: ./kompilator <plik wejściowy> <plik wyjściowy>\n";
    return 1;
  }

  FILE* in = std::fopen(argv[1], "r");
  if (!in) {
    std::perror("fopen");
    return 1;
  }
  yyin = in;

  yy::parser parse;
  int res = parse();
  std::fclose(in);
  if (res != 0) {
    return res;
  }

  std::ofstream out(argv[2]);
  if (!out) {
    std::cerr << "Error: nie można otworzyć pliku wyjściowego\n";
    return 1;
  }
  for (const auto& ins : code) {
    out << ins << '\n';
  }
  return 0;
}