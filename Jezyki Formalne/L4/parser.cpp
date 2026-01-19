// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "parser.hpp"


// Unqualified %code blocks.
#line 58 "parser.yy"

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

  // static void push_ra() {
  //   // push(ra): mem[h]=ra; h++
  //   print("RSTORE h"); k++;
  //   print("INC h");    k++;
  // }

  // static void pop_ra() {
  //   // pop(ra): h--; ra=mem[h]
  //   print("DEC h");    k++;
  //   print("RLOAD h");  k++;
  // }

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

  void equals(val one, val two){
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
    print("SWP c"); k++;

    if(one.is_num){
      gen_const_to_reg(one.bag,'a');
    }else{
      print("LOAD " + std::to_string(one.bag)); k++;
    }
    print("SWP b"); k++;

    print("SUB b"); k++;
    print("ADD c"); k++;

    print("RST g"); k++;
    print("INC g"); k++;
    unsigned long j_end = (unsigned long)pc();
    print("JZERO 0"); k++;
    print("RST g"); k++;
    patch(j_end, pc());
}

  void ne(val one, val two){
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
    print("SWP c"); k++;

    if(one.is_num){
      gen_const_to_reg(one.bag,'a');
    }else{
      print("LOAD " + std::to_string(one.bag)); k++;
    }
    print("SWP b"); k++;

    print("SUB b"); k++;
    print("ADD c"); k++;

    print("RST g"); k++;
    print("INC g"); k++;
    unsigned long j_end = (unsigned long)pc();
    print("JPOS 0"); k++;
    print("RST g"); k++;
    patch(j_end, pc());
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
  

#line 707 "parser.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 780 "parser.cpp"

  /// Build a parser object.
  parser::parser ()
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr)
#else

#endif
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_type: // type
        value.YY_MOVE_OR_COPY< TYPE > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_condition: // condition
        value.YY_MOVE_OR_COPY< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_identifier: // identifier
        value.YY_MOVE_OR_COPY< pid > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_PIDENTIFIER: // PIDENTIFIER
        value.YY_MOVE_OR_COPY< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUM: // NUM
      case symbol_kind::S_expression: // expression
        value.YY_MOVE_OR_COPY< unsigned long > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value: // value
        value.YY_MOVE_OR_COPY< val > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s)
  {
    switch (that.kind ())
    {
      case symbol_kind::S_type: // type
        value.move< TYPE > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_condition: // condition
        value.move< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_identifier: // identifier
        value.move< pid > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_PIDENTIFIER: // PIDENTIFIER
        value.move< std::string > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_NUM: // NUM
      case symbol_kind::S_expression: // expression
        value.move< unsigned long > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_value: // value
        value.move< val > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_type: // type
        value.copy< TYPE > (that.value);
        break;

      case symbol_kind::S_condition: // condition
        value.copy< bool > (that.value);
        break;

      case symbol_kind::S_identifier: // identifier
        value.copy< pid > (that.value);
        break;

      case symbol_kind::S_PIDENTIFIER: // PIDENTIFIER
        value.copy< std::string > (that.value);
        break;

      case symbol_kind::S_NUM: // NUM
      case symbol_kind::S_expression: // expression
        value.copy< unsigned long > (that.value);
        break;

      case symbol_kind::S_value: // value
        value.copy< val > (that.value);
        break;

      default:
        break;
    }

    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_type: // type
        value.move< TYPE > (that.value);
        break;

      case symbol_kind::S_condition: // condition
        value.move< bool > (that.value);
        break;

      case symbol_kind::S_identifier: // identifier
        value.move< pid > (that.value);
        break;

      case symbol_kind::S_PIDENTIFIER: // PIDENTIFIER
        value.move< std::string > (that.value);
        break;

      case symbol_kind::S_NUM: // NUM
      case symbol_kind::S_expression: // expression
        value.move< unsigned long > (that.value);
        break;

      case symbol_kind::S_value: // value
        value.move< val > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " (";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            symbol_type yylookahead (yylex ());
            yyla.move (yylookahead);
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_type: // type
        yylhs.value.emplace< TYPE > ();
        break;

      case symbol_kind::S_condition: // condition
        yylhs.value.emplace< bool > ();
        break;

      case symbol_kind::S_identifier: // identifier
        yylhs.value.emplace< pid > ();
        break;

      case symbol_kind::S_PIDENTIFIER: // PIDENTIFIER
        yylhs.value.emplace< std::string > ();
        break;

      case symbol_kind::S_NUM: // NUM
      case symbol_kind::S_expression: // expression
        yylhs.value.emplace< unsigned long > ();
        break;

      case symbol_kind::S_value: // value
        yylhs.value.emplace< val > ();
        break;

      default:
        break;
    }



      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // $@1: %empty
#line 802 "parser.yy"
{
    // gen_const_to_reg(100000UL , 'h');
    start_point =(unsigned long) pc();
    //std::cerr<<"SAM START "<<start_point<<std::endl;
    print("JUMP 0"); k++;
}
#line 1291 "parser.cpp"
    break;

  case 3: // program_all: $@1 procedures main
#line 808 "parser.yy"
                {
    //std::cerr<<"START "<<start_point<<std::endl;
    patch(start_point, (int)procedures["PROGRAM"]);
    print("HALT");
    k++;
}
#line 1302 "parser.cpp"
    break;

  case 4: // procedures: procedures PROCEDURE proc_head IS declarations IN commands END
#line 817 "parser.yy"
                                                               { 
  print("RTRN");    k++;

  end_scope(false);
 }
#line 1312 "parser.cpp"
    break;

  case 5: // procedures: procedures PROCEDURE proc_head IS IN commands END
#line 822 "parser.yy"
                                                    { 
  print("RTRN");    k++;

  end_scope(false); 
  }
#line 1322 "parser.cpp"
    break;

  case 7: // $@2: %empty
#line 831 "parser.yy"
           {
  procedures["PROGRAM"]=pc();
  begin_scope();
}
#line 1331 "parser.cpp"
    break;

  case 8: // main: PROGRAM IS $@2 declarations IN commands END
#line 834 "parser.yy"
                               {

}
#line 1339 "parser.cpp"
    break;

  case 9: // $@3: %empty
#line 837 "parser.yy"
             {
  procedures["PROGRAM"]=pc();
  begin_scope();
}
#line 1348 "parser.cpp"
    break;

  case 10: // main: PROGRAM IS $@3 IN commands END
#line 840 "parser.yy"
                  {}
#line 1354 "parser.cpp"
    break;

  case 11: // commands: commands command
#line 844 "parser.yy"
                 {}
#line 1360 "parser.cpp"
    break;

  case 12: // commands: command
#line 845 "parser.yy"
          {}
#line 1366 "parser.cpp"
    break;

  case 13: // subroutine: %empty
#line 849 "parser.yy"
       {print("SWP g");
  k++;
  if_false_stack.push((unsigned long)pc());
  print("JZERO 0");
  k++;
  }
#line 1377 "parser.cpp"
    break;

  case 14: // command: identifier ":=" expression ";"
#line 858 "parser.yy"
                                    {
  VAR* v = get_variable(yystack_[3].value.as < pid > ().address);
  if (v && v->type == I) YYERROR; // I nie może być nadpisywane

  if (yystack_[3].value.as < pid > ().is_t) {
    // zapis do elementu tablicy
    // value (expr) jest w rejestrze a
    print("SWP c"); k++;          // c = value
    array_elem_addr_to_e(yystack_[3].value.as < pid > ());     // e = addr(tab[idx])
    print("SWP c"); k++;          // a = value
    print("RSTORE e"); k++;      
  } else {
    store_scalar_by_addr_or_ref(yystack_[3].value.as < pid > ().address);
  }
}
#line 1397 "parser.cpp"
    break;

  case 15: // command: IF condition subroutine THEN commands ENDIF
#line 873 "parser.yy"
                                              {
  patch(if_false_stack.top(), pc());
  if_false_stack.pop();
  }
#line 1406 "parser.cpp"
    break;

  case 16: // $@4: %empty
#line 877 "parser.yy"
                                             {
  if_end_stack.push((unsigned long)pc());
  print("JUMP 0"); k++;
  patch(if_false_stack.top(), pc());
  if_false_stack.pop();
}
#line 1417 "parser.cpp"
    break;

  case 17: // command: IF condition subroutine THEN commands ELSE $@4 commands ENDIF
#line 882 "parser.yy"
                 {
  patch(if_end_stack.top(), pc());
  if_end_stack.pop();
}
#line 1426 "parser.cpp"
    break;

  case 18: // $@5: %empty
#line 886 "parser.yy"
        {
  while_patches.push(LoopPatch{(unsigned long)pc(), 0});   // start pętli
}
#line 1434 "parser.cpp"
    break;

  case 19: // $@6: %empty
#line 888 "parser.yy"
               {
  print("SWP g"); k++;
  while_patches.top().jexit = (unsigned long)pc();
  print("JZERO 0"); k++;
}
#line 1444 "parser.cpp"
    break;

  case 20: // command: WHILE $@5 condition DO $@6 commands ENDWHILE
#line 892 "parser.yy"
                    {
  auto lp = while_patches.top(); while_patches.pop();
  print("JUMP " + std::to_string(lp.start)); k++;
  patch(lp.jexit, pc());
}
#line 1454 "parser.cpp"
    break;

  case 21: // $@7: %empty
#line 897 "parser.yy"
         {
  repeat_stack.push((unsigned long)pc()); // początek pętli (body)
}
#line 1462 "parser.cpp"
    break;

  case 22: // command: REPEAT $@7 commands UNTIL condition ";"
#line 899 "parser.yy"
                                     {
  print("SWP g");
  k++;
  print("JZERO " + std::to_string(repeat_stack.top()));
  k++;
  repeat_stack.pop();
}
#line 1474 "parser.cpp"
    break;

  case 23: // $@8: %empty
#line 906 "parser.yy"
                                         {
  begin_scope();

  VAR it;
  it.memory_address=memory_offset++;
  it.type=U;
  it.array_start=0;
  it.array_end=0;
  declare_var_in_scope(it,yystack_[5].value.as < std::string > ());

  if(yystack_[3].value.as < val > ().is_num){
    gen_const_to_reg(yystack_[3].value.as < val > ().bag,'a');
  }else{
    print("LOAD " + std::to_string(yystack_[3].value.as < val > ().bag)); k++;
  }
  print("STORE " + std::to_string(it.memory_address)); k++;

  val iter;
  iter.is_num=0;
  iter.bag=it.memory_address;

  unsigned long jstart = (unsigned long)pc();

  le(iter,yystack_[1].value.as < val > ());
  print("SWP g"); k++;
  unsigned long jexit = (unsigned long)pc();
  print("JZERO 0"); k++;
  for_patches.push(LoopPatch{jstart, jexit});
  for_stack.push(it.memory_address);
}
#line 1509 "parser.cpp"
    break;

  case 24: // $@9: %empty
#line 935 "parser.yy"
           {

  print("LOAD " + std::to_string(for_stack.top())); k++;
  print("INC a"); k++;
  print("STORE " + std::to_string(for_stack.top())); k++;
  for_stack.pop();

  unsigned long jexit = temp.top(); temp.pop();
  unsigned long jstart = temp.top(); temp.pop();

  auto lp = for_patches.top(); // nie pop jeszcze (pop w ENDFOR)
  print("JUMP " + std::to_string(lp.start)); k++;
}
#line 1527 "parser.cpp"
    break;

  case 25: // command: FOR PIDENTIFIER FROM value TO value DO $@8 commands $@9 ENDFOR
#line 947 "parser.yy"
         {
  auto lp = for_patches.top(); for_patches.pop();
  patch(lp.jexit, pc());
  end_scope(true);
}
#line 1537 "parser.cpp"
    break;

  case 26: // $@10: %empty
#line 952 "parser.yy"
                                             {
  begin_scope();

  VAR it;
  it.memory_address=memory_offset++;
  it.type=U;
  it.array_start=0;
  it.array_end=0;
  declare_var_in_scope(it,yystack_[5].value.as < std::string > ());

  if(yystack_[3].value.as < val > ().is_num){
    gen_const_to_reg(yystack_[3].value.as < val > ().bag,'a');
  }else{
    print("LOAD " + std::to_string(yystack_[3].value.as < val > ().bag)); k++;
  }
  print("STORE " + std::to_string(it.memory_address)); k++;

  val iter;
  iter.is_num=0;
  iter.bag=it.memory_address;

  unsigned long jstart = (unsigned long)pc();

  ge(iter,yystack_[1].value.as < val > ());
  print("SWP g"); k++;
  unsigned long jexit = (unsigned long)pc();
  print("JZERO 0"); k++;
  for_patches.push(LoopPatch{jstart, jexit});
  for_stack.push(it.memory_address);
}
#line 1572 "parser.cpp"
    break;

  case 27: // $@11: %empty
#line 981 "parser.yy"
           {
  print("LOAD " + std::to_string(for_stack.top())); k++;
  print("DEC a"); k++;
  print("STORE " + std::to_string(for_stack.top())); k++;
  for_stack.pop();

  unsigned long jexit = temp.top(); temp.pop();
  unsigned long jstart = temp.top(); temp.pop();

  auto lp = for_patches.top(); // nie pop jeszcze (pop w ENDFOR)
  print("JUMP " + std::to_string(lp.start)); k++;
}
#line 1589 "parser.cpp"
    break;

  case 28: // command: FOR PIDENTIFIER FROM value DOWNTO value DO $@10 commands $@11 ENDFOR
#line 992 "parser.yy"
         {
  auto lp = for_patches.top(); for_patches.pop();
  patch(lp.jexit, pc());
  end_scope(true);
}
#line 1599 "parser.cpp"
    break;

  case 29: // command: proc_call ";"
#line 997 "parser.yy"
                      {
  //end_scope(false);
}
#line 1607 "parser.cpp"
    break;

  case 30: // command: READ identifier ";"
#line 1000 "parser.yy"
                            {
  print("READ"); k++;
  // READ do tablicy też powinien działać: jeśli tab[i], to RSTORE pod adres elementu.
  if (yystack_[1].value.as < pid > ().is_t) {
    // a = input; zapisz do tab[idx]
    print("SWP c"); k++;
    array_elem_addr_to_e(yystack_[1].value.as < pid > ());
    print("SWP c"); k++;
    print("RSTORE e"); k++;
  } else {
    print("STORE " + std::to_string(yystack_[1].value.as < pid > ().address)); k++;
  }
}
#line 1625 "parser.cpp"
    break;

  case 31: // command: WRITE value ";"
#line 1013 "parser.yy"
                        {
  if(yystack_[1].value.as < val > ().is_num){
    gen_const_to_reg(yystack_[1].value.as < val > ().bag,'a');
  }else{
    print("LOAD " + std::to_string(yystack_[1].value.as < val > ().bag));
    k++;
  }

  print("WRITE");
  k++;
}
#line 1641 "parser.cpp"
    break;

  case 32: // command: error ";"
#line 1024 "parser.yy"
                  { yyerrok;}
#line 1647 "parser.cpp"
    break;

  case 33: // $@12: %empty
#line 1028 "parser.yy"
                   {
  procedures[yystack_[1].value.as < std::string > ()] = (unsigned long)pc();

  begin_scope();

  current_proc_name = yystack_[1].value.as < std::string > ();
  current_formals.clear();
}
#line 1660 "parser.cpp"
    break;

  case 34: // proc_head: PIDENTIFIER "(" $@12 args_decl ")"
#line 1035 "parser.yy"
                   {
  proc_formals[current_proc_name] = current_formals;
}
#line 1668 "parser.cpp"
    break;

  case 35: // $@13: %empty
#line 1041 "parser.yy"
            {
  current_call_name = yystack_[0].value.as < std::string > ();
  current_call_args.clear();
}
#line 1677 "parser.cpp"
    break;

  case 36: // proc_call: PIDENTIFIER $@13 "(" args ")"
#line 1044 "parser.yy"
                    {
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
#line 1709 "parser.cpp"
    break;

  case 37: // declarations: declarations "," PIDENTIFIER
#line 1074 "parser.yy"
                               {
  VAR t;
  t.memory_address = memory_offset++;
  t.type = U;
  t.array_start = 0;
  t.array_end = 0;
  declare_var_in_scope(t, yystack_[0].value.as < std::string > ());
}
#line 1722 "parser.cpp"
    break;

  case 38: // declarations: declarations "," PIDENTIFIER "[" NUM ":" NUM "]"
#line 1082 "parser.yy"
                                                               {
  VAR t;
  t.array_start = yystack_[3].value.as < unsigned long > ();
  t.array_end = yystack_[1].value.as < unsigned long > ();
  if(t.array_start>=t.array_end) YYERROR;
  t.memory_address = memory_offset;
  memory_offset += (t.array_end + 1);
  t.type = T;
  declare_var_in_scope(t, yystack_[5].value.as < std::string > ());
}
#line 1737 "parser.cpp"
    break;

  case 39: // declarations: PIDENTIFIER
#line 1092 "parser.yy"
              {
  VAR t;
  t.memory_address = memory_offset++;
  t.type = U;
  t.array_start = 0;
  t.array_end = 0;
  declare_var_in_scope(t, yystack_[0].value.as < std::string > ());
}
#line 1750 "parser.cpp"
    break;

  case 40: // declarations: PIDENTIFIER "[" NUM ":" NUM "]"
#line 1100 "parser.yy"
                                            {
  VAR t; 
  t.type = T; 
  t.array_start = yystack_[3].value.as < unsigned long > ();
  t.array_end = yystack_[1].value.as < unsigned long > ();
  if(t.array_start>=t.array_end) YYERROR;
  t.memory_address = memory_offset;
  memory_offset += (t.array_end + 1);
  declare_var_in_scope(t, yystack_[5].value.as < std::string > ());
  }
#line 1765 "parser.cpp"
    break;

  case 41: // args_decl: args_decl "," type PIDENTIFIER
#line 1113 "parser.yy"
                                 {
VAR p;
  p.memory_address = memory_offset++;
  p.type = yystack_[1].value.as < TYPE > ();
  p.array_start = 0; p.array_end = 0;

  declare_var_in_scope(p, yystack_[0].value.as < std::string > ());

  // oznacz, że to "komórka-wskaźnik"
  if (p.type == T) array_ref_cells.insert(p.memory_address);
  else             ref_cells.insert(p.memory_address);

  current_formals.push_back(FormalParam{p.type, p.memory_address});
  }
#line 1784 "parser.cpp"
    break;

  case 42: // args_decl: type PIDENTIFIER
#line 1127 "parser.yy"
                   {
  VAR p;
  p.memory_address = memory_offset++;
  p.type = yystack_[1].value.as < TYPE > ();
  p.array_start = 0; p.array_end = 0;

  declare_var_in_scope(p, yystack_[0].value.as < std::string > ());

  if (p.type == T) array_ref_cells.insert(p.memory_address);
  else             ref_cells.insert(p.memory_address);

  current_formals.push_back(FormalParam{p.type, p.memory_address});
}
#line 1802 "parser.cpp"
    break;

  case 43: // type: typeT
#line 1143 "parser.yy"
      {yylhs.value.as < TYPE > ()=T;}
#line 1808 "parser.cpp"
    break;

  case 44: // type: typeI
#line 1144 "parser.yy"
        {yylhs.value.as < TYPE > ()=I;}
#line 1814 "parser.cpp"
    break;

  case 45: // type: typeO
#line 1145 "parser.yy"
        {yylhs.value.as < TYPE > ()=O;}
#line 1820 "parser.cpp"
    break;

  case 46: // type: %empty
#line 1146 "parser.yy"
         {yylhs.value.as < TYPE > ()=U;}
#line 1826 "parser.cpp"
    break;

  case 47: // args: args "," PIDENTIFIER
#line 1150 "parser.yy"
                       {current_call_args.push_back(yystack_[0].value.as < std::string > ());}
#line 1832 "parser.cpp"
    break;

  case 48: // args: PIDENTIFIER
#line 1151 "parser.yy"
              {current_call_args.push_back(yystack_[0].value.as < std::string > ());}
#line 1838 "parser.cpp"
    break;

  case 49: // expression: value
#line 1155 "parser.yy"
      {
  if(yystack_[0].value.as < val > ().is_num){
    gen_const_to_reg(yystack_[0].value.as < val > ().bag,'a');
  }else{
    print("LOAD " + std::to_string(yystack_[0].value.as < val > ().bag));
    k++;
  }
}
#line 1851 "parser.cpp"
    break;

  case 50: // expression: value "+" value
#line 1163 "parser.yy"
                   {
  val one = yystack_[2].value.as < val > ();
  val two = yystack_[0].value.as < val > ();

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
#line 1879 "parser.cpp"
    break;

  case 51: // expression: value "-" value
#line 1186 "parser.yy"
                    {
  val one = yystack_[2].value.as < val > ();
  val two = yystack_[0].value.as < val > ();

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
#line 1907 "parser.cpp"
    break;

  case 52: // expression: value "*" value
#line 1209 "parser.yy"
                   {
  gen_mul(yystack_[2].value.as < val > (), yystack_[0].value.as < val > ());
  // print("LOAD " + std::to_string($3.bag));
  // k++;
  // print("SHL a");
  // k++;
}
#line 1919 "parser.cpp"
    break;

  case 53: // expression: value "/" value
#line 1216 "parser.yy"
                    {
  gen_divmod(yystack_[2].value.as < val > (), yystack_[0].value.as < val > (), true);
  // print("LOAD " + std::to_string($1.bag));
  // k++;
  // print("SHR a");
  // k++;
}
#line 1931 "parser.cpp"
    break;

  case 54: // expression: value "%" value
#line 1223 "parser.yy"
                  {
  gen_divmod(yystack_[2].value.as < val > (), yystack_[0].value.as < val > (), false);
}
#line 1939 "parser.cpp"
    break;

  case 55: // condition: value "=" value
#line 1229 "parser.yy"
                   {//do optymalizacji
  equals(yystack_[2].value.as < val > (),yystack_[0].value.as < val > ());
}
#line 1947 "parser.cpp"
    break;

  case 56: // condition: value "!=" value
#line 1232 "parser.yy"
                 {//do optymalizacji
  ne(yystack_[2].value.as < val > (),yystack_[0].value.as < val > ());
}
#line 1955 "parser.cpp"
    break;

  case 57: // condition: value ">" value
#line 1235 "parser.yy"
                 {
  gt(yystack_[2].value.as < val > (),yystack_[0].value.as < val > ());
}
#line 1963 "parser.cpp"
    break;

  case 58: // condition: value "<" value
#line 1238 "parser.yy"
                 {
  lt(yystack_[2].value.as < val > (),yystack_[0].value.as < val > ());
}
#line 1971 "parser.cpp"
    break;

  case 59: // condition: value ">=" value
#line 1241 "parser.yy"
                 {
  ge(yystack_[2].value.as < val > (),yystack_[0].value.as < val > ());
}
#line 1979 "parser.cpp"
    break;

  case 60: // condition: value "<=" value
#line 1244 "parser.yy"
                 {
  le(yystack_[2].value.as < val > (),yystack_[0].value.as < val > ());
}
#line 1987 "parser.cpp"
    break;

  case 61: // value: NUM
#line 1251 "parser.yy"
    {
  val t;
  t.is_num=1;
  t.bag=yystack_[0].value.as < unsigned long > ();
  yylhs.value.as < val > ()=t;
}
#line 1998 "parser.cpp"
    break;

  case 62: // value: identifier
#line 1257 "parser.yy"
             {
  VAR* v = get_variable(yystack_[0].value.as < pid > ().address);

  // O nie wolno czytać jako wartości
  if (v && v->type == O) YYERROR;

  if (!yystack_[0].value.as < pid > ().is_t) {
    unsigned long tmp = memory_offset++;
    load_scalar_by_addr_or_ref(yystack_[0].value.as < pid > ().address);
    print("STORE " + std::to_string(tmp)); k++;

    val t; t.is_num=0; t.bag=tmp; yylhs.value.as < val > ()=t;
  } else {
    // odczyt elementu tablicy jako wartość
    VAR* tab = get_variable(yystack_[0].value.as < pid > ().address);
    if (!tab || tab->type != T) YYERROR;

    const bool is_ref = is_array_ref_cell(tab->memory_address);

    // Jeśli to zwykła tablica i indeks stały, możemy zwrócić bezpośredni adres komórki.
    if (!is_ref && !yystack_[0].value.as < pid > ().var_index) {
      unsigned long target = tab->memory_address + yystack_[0].value.as < pid > ().idx;
      val t; t.is_num=0; t.bag=target; yylhs.value.as < val > ()=t;
    } else {
      // T-param albo indeks zmienny => RLOAD spod obliczonego adresu
      unsigned long tmp = memory_offset++;

      if(!yystack_[0].value.as < pid > ().is_t)YYERROR;
      array_elem_addr_to_e(yystack_[0].value.as < pid > ());     // e = addr(tab[idx])
      print("RLOAD e"); k++;        // a = mem[e]
      print("STORE " + std::to_string(tmp)); k++;

      val t; t.is_num=0; t.bag=tmp; yylhs.value.as < val > ()=t;
    }
  }
}
#line 2039 "parser.cpp"
    break;

  case 63: // identifier: PIDENTIFIER
#line 1297 "parser.yy"
            {
  //if(get_variable($1)->type==O)YYERROR;
  pid t;
  t.is_t=0;
  t.var_index=0;
  t.idx=0;
  t.address=get_addr(yystack_[0].value.as < std::string > ());
  yylhs.value.as < pid > ()=t;
}
#line 2053 "parser.cpp"
    break;

  case 64: // identifier: PIDENTIFIER "[" PIDENTIFIER "]"
#line 1306 "parser.yy"
                                          {
  if(get_variable(yystack_[3].value.as < std::string > ())->type!=T)YYERROR;
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
  t.idx=get_addr(yystack_[1].value.as < std::string > ());
  t.address=get_addr(yystack_[3].value.as < std::string > ());
  yylhs.value.as < pid > ()=t;
}
#line 2075 "parser.cpp"
    break;

  case 65: // identifier: PIDENTIFIER "[" NUM "]"
#line 1323 "parser.yy"
                                  {
  VAR* tab = get_variable(yystack_[3].value.as < std::string > ());
  if(tab->type!=T)YYERROR;
  if(yystack_[1].value.as < unsigned long > ()<tab->array_start||yystack_[1].value.as < unsigned long > ()>tab->array_end)YYERROR;

  pid t;
  t.is_t=1;
  t.var_index=0;
  t.idx=yystack_[1].value.as < unsigned long > ();
  t.address=get_addr(yystack_[3].value.as < std::string > ());
  yylhs.value.as < pid > ()=t;
}
#line 2092 "parser.cpp"
    break;


#line 2096 "parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (YY_MOVE (msg));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;


      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // parser::context.
  parser::context::context (const parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char parser::yypact_ninf_ = -54;

  const signed char parser::yytable_ninf_ = -64;

  const short
  parser::yypact_[] =
  {
     -54,     7,   -54,   -54,   -17,     8,     6,   -54,    16,    21,
      22,   -54,    -2,    47,    30,    27,    36,   247,    -7,    29,
     247,   -54,   -54,   -54,    50,    57,    65,    59,   -15,    14,
     -54,   -54,    80,    83,    14,     5,   -54,    66,    68,    93,
     247,   247,   104,   -54,    27,   -54,    54,   -54,    75,    86,
     -54,    81,   -54,    12,   -54,    14,   247,    69,    76,    79,
     -54,   -54,   -54,    14,    94,   138,   150,   -54,   109,   126,
     112,   114,   130,   106,    14,    14,    14,    14,    14,    14,
     107,   172,    14,   -54,   -54,   116,   110,   134,   -54,   -54,
     -54,   125,   -54,   -54,   -54,    63,   247,   -54,   -54,   -54,
     -54,   -54,   -54,   -54,    14,     0,   -54,    14,    14,    14,
      14,    14,   101,   -54,   -54,   144,    34,   247,   122,    14,
      14,   -54,   -54,   -54,   -54,   -54,   148,   -54,   -54,   -54,
     184,   -54,   119,   120,   141,   247,   -54,   -54,   -54,   -54,
     197,   247,   247,   -54,   213,   231,   121,   123,   -54,   -54
  };

  const signed char
  parser::yydefact_[] =
  {
       2,     0,     6,     1,     0,     0,     0,     3,     0,     0,
       7,    33,     0,     0,     0,    46,    39,     0,     0,     0,
       0,    43,    44,    45,     0,     0,     0,     0,    35,     0,
      18,    21,     0,     0,     0,     0,    12,     0,     0,     0,
       0,     0,     0,    34,    46,    42,     0,    32,     0,     0,
      61,    63,    13,     0,    62,     0,     0,     0,     0,     0,
       5,    11,    29,     0,    37,     0,     0,    10,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    30,    31,     0,    49,     0,     4,     8,
      41,     0,    65,    64,    48,     0,     0,    55,    56,    57,
      58,    59,    60,    19,     0,     0,    14,     0,     0,     0,
       0,     0,     0,    40,    36,     0,     0,     0,     0,     0,
       0,    50,    51,    52,    53,    54,     0,    47,    16,    15,
       0,    22,     0,     0,     0,     0,    20,    23,    26,    38,
       0,     0,     0,    17,     0,     0,     0,     0,    25,    28
  };

  const short
  parser::yypgoto_[] =
  {
     -54,   -54,   -54,   -54,   -54,   -54,   -54,   -20,   -54,   -34,
     -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,
     -54,   -54,   152,   -54,   128,   -54,   -54,   -53,    49,   -18
  };

  const unsigned char
  parser::yydefgoto_[] =
  {
       0,     1,     2,     4,     7,    13,    14,    35,    73,    36,
     135,    55,   117,    56,   141,   146,   142,   147,     9,    15,
      37,    49,    18,    24,    25,    95,    85,    52,    53,    38
  };

  const short
  parser::yytable_[] =
  {
      42,    61,    80,    16,    48,     5,    27,     3,    61,     6,
      28,    54,   -63,     8,    39,    58,    54,    40,    50,    51,
      65,    66,    17,    74,    75,    76,    77,    78,    79,    10,
      60,    61,    61,    11,    29,    27,    81,    54,    30,    28,
     119,    31,   120,    32,    12,    54,    -9,    61,    33,    34,
      39,   118,    16,    41,    20,    26,    54,    54,    54,    54,
      54,    54,    45,    29,    54,   128,   129,    30,    43,    46,
      31,    44,    32,    21,    22,    23,   116,    33,    34,    70,
      71,   114,    61,    59,   115,    57,    54,    47,    51,    54,
      54,    54,    54,    54,    62,    63,    61,   130,    64,    69,
      48,    54,    54,    72,    83,    27,    61,    84,    82,    28,
      61,    61,    86,    87,    90,   140,   107,   108,   109,   110,
     111,   144,   145,    97,    98,    99,   100,   101,   102,    67,
      91,   105,    92,    29,    93,    94,    96,    30,   112,    27,
      31,   103,    32,    28,   106,   113,   126,    33,    34,   127,
     131,    27,   134,   137,   138,    28,   121,   122,   123,   124,
     125,   139,   148,    88,   149,    19,     0,    29,   132,   133,
       0,    30,    68,    27,    31,    89,    32,    28,     0,    29,
       0,    33,    34,    30,     0,    27,    31,     0,    32,    28,
       0,     0,     0,    33,    34,     0,     0,     0,    27,     0,
       0,    29,    28,     0,     0,    30,     0,     0,    31,   104,
      32,     0,     0,    29,    27,    33,    34,    30,    28,   136,
      31,     0,    32,     0,     0,     0,    29,    33,    34,   143,
      30,     0,    27,    31,     0,    32,    28,     0,     0,     0,
      33,    34,    29,     0,     0,     0,    30,     0,    27,    31,
       0,    32,    28,     0,   -24,     0,    33,    34,     0,     0,
      29,     0,     0,     0,    30,     0,     0,    31,     0,    32,
       0,     0,   -27,     0,    33,    34,    29,     0,     0,     0,
      30,     0,     0,    31,     0,    32,     0,     0,     0,     0,
      33,    34
  };

  const short
  parser::yycheck_[] =
  {
      20,    35,    55,     5,    19,    22,     1,     0,    42,    26,
       5,    29,    27,     5,    21,    33,    34,    24,     4,     5,
      40,    41,    24,    11,    12,    13,    14,    15,    16,    23,
      25,    65,    66,    17,    29,     1,    56,    55,    33,     5,
      40,    36,    42,    38,    23,    63,    24,    81,    43,    44,
      21,   104,     5,    24,    24,    19,    74,    75,    76,    77,
      78,    79,     5,    29,    82,    31,    32,    33,    18,     4,
      36,    21,    38,    46,    47,    48,    96,    43,    44,     4,
       5,    18,   116,    34,    21,     5,   104,    28,     5,   107,
     108,   109,   110,   111,    28,    27,   130,   117,     5,    45,
      19,   119,   120,    17,    28,     1,   140,    28,    39,     5,
     144,   145,    63,    19,     5,   135,     6,     7,     8,     9,
      10,   141,   142,    74,    75,    76,    77,    78,    79,    25,
       4,    82,    20,    29,    20,     5,    30,    33,     4,     1,
      36,    34,    38,     5,    28,    20,    45,    43,    44,     5,
      28,     1,     4,    34,    34,     5,   107,   108,   109,   110,
     111,    20,    41,    25,    41,    13,    -1,    29,   119,   120,
      -1,    33,    44,     1,    36,    25,    38,     5,    -1,    29,
      -1,    43,    44,    33,    -1,     1,    36,    -1,    38,     5,
      -1,    -1,    -1,    43,    44,    -1,    -1,    -1,     1,    -1,
      -1,    29,     5,    -1,    -1,    33,    -1,    -1,    36,    37,
      38,    -1,    -1,    29,     1,    43,    44,    33,     5,    35,
      36,    -1,    38,    -1,    -1,    -1,    29,    43,    44,    32,
      33,    -1,     1,    36,    -1,    38,     5,    -1,    -1,    -1,
      43,    44,    29,    -1,    -1,    -1,    33,    -1,     1,    36,
      -1,    38,     5,    -1,    41,    -1,    43,    44,    -1,    -1,
      29,    -1,    -1,    -1,    33,    -1,    -1,    36,    -1,    38,
      -1,    -1,    41,    -1,    43,    44,    29,    -1,    -1,    -1,
      33,    -1,    -1,    36,    -1,    38,    -1,    -1,    -1,    -1,
      43,    44
  };

  const signed char
  parser::yystos_[] =
  {
       0,    50,    51,     0,    52,    22,    26,    53,     5,    67,
      23,    17,    23,    54,    55,    68,     5,    24,    71,    71,
      24,    46,    47,    48,    72,    73,    19,     1,     5,    29,
      33,    36,    38,    43,    44,    56,    58,    69,    78,    21,
      24,    24,    56,    18,    21,     5,     4,    28,    19,    70,
       4,     5,    76,    77,    78,    60,    62,     5,    78,    77,
      25,    58,    28,    27,     5,    56,    56,    25,    73,    45,
       4,     5,    17,    57,    11,    12,    13,    14,    15,    16,
      76,    56,    39,    28,    28,    75,    77,    19,    25,    25,
       5,     4,    20,    20,     5,    74,    30,    77,    77,    77,
      77,    77,    77,    34,    37,    77,    28,     6,     7,     8,
       9,    10,     4,    20,    18,    21,    56,    61,    76,    40,
      42,    77,    77,    77,    77,    77,    45,     5,    31,    32,
      56,    28,    77,    77,     4,    59,    35,    34,    34,    20,
      56,    63,    65,    32,    56,    56,    64,    66,    41,    41
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    49,    51,    50,    52,    52,    52,    54,    53,    55,
      53,    56,    56,    57,    58,    58,    59,    58,    60,    61,
      58,    62,    58,    63,    64,    58,    65,    66,    58,    58,
      58,    58,    58,    68,    67,    70,    69,    71,    71,    71,
      71,    72,    72,    73,    73,    73,    73,    74,    74,    75,
      75,    75,    75,    75,    75,    76,    76,    76,    76,    76,
      76,    77,    77,    78,    78,    78
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     0,     3,     8,     7,     0,     0,     7,     0,
       6,     2,     1,     0,     4,     6,     0,     9,     0,     0,
       7,     0,     6,     0,     0,    11,     0,     0,    11,     2,
       3,     3,     2,     0,     5,     0,     5,     3,     8,     1,
       6,     4,     2,     1,     1,     1,     0,     3,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     1,     1,     4,     4
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "\"end of line\"",
  "NUM", "PIDENTIFIER", "\"+\"", "\"-\"", "\"*\"", "\"/\"", "\"%\"",
  "\"=\"", "\"!=\"", "\">\"", "\"<\"", "\">=\"", "\"<=\"", "\"(\"",
  "\")\"", "\"[\"", "\"]\"", "\",\"", "PROCEDURE", "IS", "IN", "END",
  "PROGRAM", "\":=\"", "\";\"", "IF", "THEN", "ELSE", "ENDIF", "WHILE",
  "DO", "ENDWHILE", "REPEAT", "UNTIL", "FOR", "FROM", "TO", "ENDFOR",
  "DOWNTO", "READ", "WRITE", "\":\"", "typeT", "typeI", "typeO", "$accept",
  "program_all", "$@1", "procedures", "main", "$@2", "$@3", "commands",
  "subroutine", "command", "$@4", "$@5", "$@6", "$@7", "$@8", "$@9",
  "$@10", "$@11", "proc_head", "$@12", "proc_call", "$@13", "declarations",
  "args_decl", "type", "args", "expression", "condition", "value",
  "identifier", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   802,   802,   802,   817,   822,   827,   831,   831,   837,
     837,   844,   845,   849,   858,   873,   877,   877,   886,   888,
     886,   897,   897,   906,   935,   906,   952,   981,   952,   997,
    1000,  1013,  1024,  1028,  1028,  1041,  1041,  1074,  1082,  1092,
    1100,  1113,  1127,  1143,  1144,  1145,  1146,  1150,  1151,  1155,
    1163,  1186,  1209,  1216,  1223,  1229,  1232,  1235,  1238,  1241,
    1244,  1251,  1257,  1297,  1306,  1323
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG


} // yy
#line 2683 "parser.cpp"

#line 1337 "parser.yy"


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
