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
#line 47 "parser.yy"

  namespace yy {
      parser::symbol_type yylex();
  }

  

  int k=0;
  std::map<std::string, VAR> var_table;
  std::vector<std::string> code;
  unsigned long memory_offset = 0;
  std::vector<std::vector<std::string>> temp_var_table;
  int depth=0;

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
          if (binary[i] == '1') print("INC " + r); // Dodaj 1
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
      VAR* v = get_variable(p.address);
      if(v==nullptr)return 0;

      print("LOAD " + p.idx);
      gen_const_to_reg(v->array_start,'b');
      print("SUB b");
      gen_const_to_reg(p.address,'b');
      print("ADD b");
      print("SWP c");
      print("RSTORE c");
    }else{
      VAR* v = get_variable(p.address);
      if(v==nullptr)return 0;
      unsigned long target=p.address+(p.idx - v->array_start);
      print("STORE " + target);

    }
    return 1;
    

  }

  void stack_push() {
    print("RSTORE h"); 
    print("INC h");    
  }

  void stack_pop_to_rb() {
      print("DEC h");    
      print("SWP b");    
      print("RLOAD h");  
      print("SWP b");    
  }
  

#line 153 "parser.cpp"


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
#line 226 "parser.cpp"

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
      case symbol_kind::S_condition: // condition
        value.YY_MOVE_OR_COPY< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type: // type
        value.YY_MOVE_OR_COPY< char > (YY_MOVE (that.value));
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
      case symbol_kind::S_condition: // condition
        value.move< bool > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type: // type
        value.move< char > (YY_MOVE (that.value));
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
      case symbol_kind::S_condition: // condition
        value.copy< bool > (that.value);
        break;

      case symbol_kind::S_type: // type
        value.copy< char > (that.value);
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
      case symbol_kind::S_condition: // condition
        value.move< bool > (that.value);
        break;

      case symbol_kind::S_type: // type
        value.move< char > (that.value);
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
      case symbol_kind::S_condition: // condition
        yylhs.value.emplace< bool > ();
        break;

      case symbol_kind::S_type: // type
        yylhs.value.emplace< char > ();
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
  case 2: // program_all: procedures main
#line 239 "parser.yy"
                {
      for (const auto& ins : code) {
          std::cout << ins << std::endl;
      }
      print("HALT");
  }
#line 737 "parser.cpp"
    break;

  case 3: // procedures: procedures PROCEDURE proc_head IS declarations IN commands END
#line 248 "parser.yy"
                                                               {}
#line 743 "parser.cpp"
    break;

  case 4: // procedures: procedures PROCEDURE proc_head IS IN commands END
#line 249 "parser.yy"
                                                    {}
#line 749 "parser.cpp"
    break;

  case 6: // main: PROGRAM IS declarations IN commands END
#line 254 "parser.yy"
                                        {}
#line 755 "parser.cpp"
    break;

  case 7: // main: PROGRAM IS IN commands END
#line 255 "parser.yy"
                             {}
#line 761 "parser.cpp"
    break;

  case 8: // commands: commands command
#line 259 "parser.yy"
                 {}
#line 767 "parser.cpp"
    break;

  case 9: // commands: command
#line 260 "parser.yy"
          {}
#line 773 "parser.cpp"
    break;

  case 10: // command: identifier ":=" expression ";"
#line 264 "parser.yy"
                                    {
  VAR* v = get_variable(yystack_[3].value.as < pid > ().address);
  switch(v->type){
    case T:
    if(!save_at(yystack_[3].value.as < pid > ()))YYERROR;
    break;
    case I:
    YYERROR;
    break;
    case O:
    print("STORE " + std::to_string(yystack_[3].value.as < pid > ().address));
    v->type = U;
    break;
    case U:
    print("STORE " + std::to_string(yystack_[3].value.as < pid > ().address));
    break;
  }
}
#line 796 "parser.cpp"
    break;

  case 11: // command: IF condition THEN commands ELSE commands ENDIF
#line 282 "parser.yy"
                                                 {}
#line 802 "parser.cpp"
    break;

  case 12: // command: IF condition THEN commands ENDIF
#line 283 "parser.yy"
                                   {}
#line 808 "parser.cpp"
    break;

  case 13: // command: WHILE condition DO commands ENDWHILE
#line 284 "parser.yy"
                                       {}
#line 814 "parser.cpp"
    break;

  case 14: // command: REPEAT commands UNTIL condition ";"
#line 285 "parser.yy"
                                            {}
#line 820 "parser.cpp"
    break;

  case 15: // command: FOR PIDENTIFIER FROM value TO value DO commands ENDFOR
#line 286 "parser.yy"
                                                         {}
#line 826 "parser.cpp"
    break;

  case 16: // command: FOR PIDENTIFIER FROM value DOWNTO value DO commands ENDFOR
#line 287 "parser.yy"
                                                             {}
#line 832 "parser.cpp"
    break;

  case 17: // command: proc_call ";"
#line 288 "parser.yy"
                      {}
#line 838 "parser.cpp"
    break;

  case 18: // command: READ identifier ";"
#line 289 "parser.yy"
                            {
  print("READ");
  print("STORE " + std::to_string(yystack_[1].value.as < pid > ().address));
}
#line 847 "parser.cpp"
    break;

  case 19: // command: WRITE value ";"
#line 293 "parser.yy"
                        {
  if(yystack_[1].value.as < val > ().is_num){
    gen_const_to_reg(yystack_[1].value.as < val > ().bag,'a');
  }else{
    print("LOAD " + yystack_[1].value.as < val > ().bag);
  }

  print("WRITE");
}
#line 861 "parser.cpp"
    break;

  case 20: // command: error ";"
#line 302 "parser.yy"
                  { yyerrok;}
#line 867 "parser.cpp"
    break;

  case 21: // proc_head: PIDENTIFIER "(" args_decl ")"
#line 306 "parser.yy"
                                    {}
#line 873 "parser.cpp"
    break;

  case 22: // proc_call: PIDENTIFIER "(" args ")"
#line 310 "parser.yy"
                               {}
#line 879 "parser.cpp"
    break;

  case 23: // declarations: declarations "," PIDENTIFIER
#line 314 "parser.yy"
                               {
  VAR temp;
  temp.memory_address = memory_offset++;
  temp.type = U;
  var_table[yystack_[0].value.as < std::string > ()]=temp;
}
#line 890 "parser.cpp"
    break;

  case 24: // declarations: declarations "," PIDENTIFIER "[" NUM ":" NUM "]"
#line 320 "parser.yy"
                                                               {
  VAR t;
  t.array_start = yystack_[3].value.as < unsigned long > ();
  t.array_end = yystack_[1].value.as < unsigned long > ();
  if(t.array_start>=t.array_end) YYERROR;
  t.memory_address = memory_offset;
  memory_offset += (yystack_[1].value.as < unsigned long > () - yystack_[3].value.as < unsigned long > () + 1);
  t.type = T;
  var_table[yystack_[5].value.as < std::string > ()]=t;
}
#line 905 "parser.cpp"
    break;

  case 25: // declarations: PIDENTIFIER
#line 330 "parser.yy"
              {
  VAR t; 
  t.memory_address = memory_offset++; 
  t.type = U;
  var_table[yystack_[0].value.as < std::string > ()] = t;
}
#line 916 "parser.cpp"
    break;

  case 26: // declarations: PIDENTIFIER "[" NUM ":" NUM "]"
#line 336 "parser.yy"
                                            {
  VAR t; 
  t.type = T; 
  t.array_start = yystack_[3].value.as < unsigned long > ();
  t.array_end = yystack_[1].value.as < unsigned long > ();
  if(t.array_start>=t.array_end) YYERROR;
  t.memory_address = memory_offset;
  memory_offset += (yystack_[1].value.as < unsigned long > () - yystack_[3].value.as < unsigned long > () + 1);
  var_table[yystack_[5].value.as < std::string > ()] = t;}
#line 930 "parser.cpp"
    break;

  case 27: // args_decl: args_decl "," type PIDENTIFIER
#line 348 "parser.yy"
                                 {}
#line 936 "parser.cpp"
    break;

  case 28: // args_decl: type PIDENTIFIER
#line 349 "parser.yy"
                   {
  // VAR temp;
  // temp.memory_address = memory_offset++;
  // temp.type = U;
  // temp_var_table[depth][$3]=temp;
}
#line 947 "parser.cpp"
    break;

  case 29: // type: T
#line 358 "parser.yy"
  {yylhs.value.as < char > ()='T';}
#line 953 "parser.cpp"
    break;

  case 30: // type: I
#line 359 "parser.yy"
    {yylhs.value.as < char > ()='I';}
#line 959 "parser.cpp"
    break;

  case 31: // type: O
#line 360 "parser.yy"
    {yylhs.value.as < char > ()='O';}
#line 965 "parser.cpp"
    break;

  case 32: // type: %empty
#line 361 "parser.yy"
         {yylhs.value.as < char > ()='U';}
#line 971 "parser.cpp"
    break;

  case 33: // args: args "," PIDENTIFIER
#line 365 "parser.yy"
                       {}
#line 977 "parser.cpp"
    break;

  case 34: // args: PIDENTIFIER
#line 366 "parser.yy"
              {}
#line 983 "parser.cpp"
    break;

  case 35: // expression: value
#line 370 "parser.yy"
      {
  if(yystack_[0].value.as < val > ().is_num){
    gen_const_to_reg(yystack_[0].value.as < val > ().bag,'a');
  }else{
    print("LOAD " + std::to_string(yystack_[0].value.as < val > ().bag));
  }
}
#line 995 "parser.cpp"
    break;

  case 36: // expression: value "+" value
#line 377 "parser.yy"
                   {
  val one = yystack_[2].value.as < val > ();
  val two = yystack_[0].value.as < val > ();

  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag));
    print("SWP b");
  }

  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag));
  }

  print("ADD b");
}
#line 1019 "parser.cpp"
    break;

  case 37: // expression: value "-" value
#line 396 "parser.yy"
                    {
  val one = yystack_[2].value.as < val > ();
  val two = yystack_[0].value.as < val > ();

  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag));
    print("SWP b");
  }

  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag));
  }

  print("SUB b");
}
#line 1043 "parser.cpp"
    break;

  case 38: // expression: value "*" value
#line 415 "parser.yy"
                   {
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
#line 1084 "parser.cpp"
    break;

  case 39: // expression: value "/" value
#line 451 "parser.yy"
                    {}
#line 1090 "parser.cpp"
    break;

  case 40: // expression: value "%" value
#line 452 "parser.yy"
                  {}
#line 1096 "parser.cpp"
    break;

  case 41: // condition: value "=" value
#line 456 "parser.yy"
                   {//do optymalizacji
  val one = yystack_[2].value.as < val > ();
  val two = yystack_[0].value.as < val > ();

  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag));
    print("SWP b");
  }

  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag));
  }

  print("SUB b");
  print("SWP c");

  if(one.is_num){
    gen_const_to_reg(one.bag,'b');
  }else{
    print("LOAD " + std::to_string(one.bag));
    print("SWP b");
  }

  if(two.is_num){
    gen_const_to_reg(two.bag,'a');
  }else{
    print("LOAD " + std::to_string(two.bag));
  }
  print("ADD c");
  print("RST g");
  print("JPOS " + k + 2);
  print("INC g");
}
#line 1138 "parser.cpp"
    break;

  case 42: // condition: value "!=" value
#line 493 "parser.yy"
                 {//do optymalizacji
  val one = yystack_[2].value.as < val > ();
  val two = yystack_[0].value.as < val > ();

  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag));
    print("SWP b");
  }

  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag));
  }

  print("SUB b");
  print("SWP c");

  if(one.is_num){
    gen_const_to_reg(one.bag,'b');
  }else{
    print("LOAD " + std::to_string(one.bag));
    print("SWP b");
  }

  if(two.is_num){
    gen_const_to_reg(two.bag,'a');
  }else{
    print("LOAD " + std::to_string(two.bag));
  }
  print("ADD c");
  print("RST g");
  print("JZERO " + k + 2);
  print("INC g");
}
#line 1180 "parser.cpp"
    break;

  case 43: // condition: value ">" value
#line 530 "parser.yy"
                 {
  val one = yystack_[2].value.as < val > ();
  val two = yystack_[0].value.as < val > ();

  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag));
    print("SWP b");
  }

  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag));
  }

  print("SUB b");
  print("RST g");
  print("JPOS " + k + 2);
  print("INC g");
}
#line 1207 "parser.cpp"
    break;

  case 44: // condition: value "<" value
#line 552 "parser.yy"
                 {
  val one = yystack_[2].value.as < val > ();
  val two = yystack_[0].value.as < val > ();

  if(one.is_num){
    gen_const_to_reg(one.bag,'b');
  }else{
    print("LOAD " + std::to_string(one.bag));
    print("SWP b");
  }

  if(two.is_num){
    gen_const_to_reg(two.bag,'a');
  }else{
    print("LOAD " + std::to_string(two.bag));
  }

  print("SUB b");
  print("RST g");
  print("JPOS " + k + 2);
  print("INC g");
}
#line 1234 "parser.cpp"
    break;

  case 45: // condition: value ">=" value
#line 574 "parser.yy"
                 {
  val one = yystack_[2].value.as < val > ();
  val two = yystack_[0].value.as < val > ();

  if(two.is_num){
    gen_const_to_reg(two.bag,'b');
  }else{
    print("LOAD " + std::to_string(two.bag));
    print("SWP b");
  }

  if(one.is_num){
    gen_const_to_reg(one.bag,'a');
  }else{
    print("LOAD " + std::to_string(one.bag));
  }

  print("SUB b");
  print("RST g");
  
  print("JPOS " + k + 2);//skok do x
  int temp=code.size();

  if(one.is_num){
    gen_const_to_reg(one.bag,'c');
  }else{
    print("LOAD " + std::to_string(one.bag));
    print("SWP c");
  }

  print("SWP b");
  print("SUB c");

  int jump=k+2;
  print("JPOS " + jump);
  code[temp]="JPOS " + std::to_string(jump);
  print("INC g");//x
}
#line 1277 "parser.cpp"
    break;

  case 46: // condition: value "<=" value
#line 612 "parser.yy"
                 {
  val one = yystack_[2].value.as < val > ();
  val two = yystack_[0].value.as < val > ();

  if(one.is_num){
    gen_const_to_reg(one.bag,'b');
  }else{
    print("LOAD " + std::to_string(one.bag));
    print("SWP b");
  }

  if(two.is_num){
    gen_const_to_reg(two.bag,'a');
  }else{
    print("LOAD " + std::to_string(two.bag));
  }

  print("SUB b");
  print("RST g");
  
  print("JPOS " + k + 2);//skok do x
  int temp=code.size();

  if(two.is_num){
    gen_const_to_reg(two.bag,'c');
  }else{
    print("LOAD " + std::to_string(two.bag));
    print("SWP c");
  }

  print("SWP b");
  print("SUB c");

  int jump=k+2;
  print("JPOS " + jump);
  code[temp]="JPOS " + std::to_string(jump);
  print("INC g");//x
}
#line 1320 "parser.cpp"
    break;

  case 47: // value: NUM
#line 654 "parser.yy"
    {
  val t;
  t.is_num=1;
  t.bag=yystack_[0].value.as < unsigned long > ();
  yylhs.value.as < val > ()=t;
}
#line 1331 "parser.cpp"
    break;

  case 48: // value: identifier
#line 660 "parser.yy"
             {
  val t;
  t.is_num=0;
  t.bag=yystack_[0].value.as < pid > ().address;
  yylhs.value.as < val > ()=t;
}
#line 1342 "parser.cpp"
    break;

  case 49: // identifier: PIDENTIFIER
#line 670 "parser.yy"
            {
  if(get_variable(yystack_[0].value.as < std::string > ())->type==O)YYERROR;
  pid t;
  t.is_t=0;
  t.var_index=0;
  t.idx=0;
  t.address=get_addr(yystack_[0].value.as < std::string > ());
  yylhs.value.as < pid > ()=t;
}
#line 1356 "parser.cpp"
    break;

  case 50: // identifier: PIDENTIFIER "[" PIDENTIFIER "]"
#line 679 "parser.yy"
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
#line 1378 "parser.cpp"
    break;

  case 51: // identifier: PIDENTIFIER "[" NUM "]"
#line 696 "parser.yy"
                                  {
  VAR* tab = get_variable(yystack_[3].value.as < std::string > ());
  if(tab->type!=T)YYERROR;
  if(yystack_[1].value.as < unsigned long > ()<tab->array_start||yystack_[1].value.as < unsigned long > ()>tab->array_end)YYERROR;
  gen_const_to_reg(yystack_[1].value.as < unsigned long > ()-tab->array_start+tab->memory_address,'e');
  //$$=$3-tab->array_start+tab->memory_address;

  pid t;
  t.is_t=1;
  t.var_index=0;
  t.idx=yystack_[1].value.as < unsigned long > ();
  t.address=get_addr(yystack_[3].value.as < std::string > ());
  yylhs.value.as < pid > ()=t;
}
#line 1397 "parser.cpp"
    break;


#line 1401 "parser.cpp"

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
        std::string msg = YY_("syntax error");
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

#if YYDEBUG || 0
  const char *
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytname_[yysymbol];
  }
#endif // #if YYDEBUG || 0









  const signed char parser::yypact_ninf_ = -31;

  const signed char parser::yytable_ninf_ = -1;

  const short
  parser::yypact_[] =
  {
     -31,    28,    -2,   -31,    27,    16,   -31,    38,    35,    -1,
     -29,     3,    58,   258,   -18,   -31,   -31,   -31,    20,    80,
     258,    22,    82,    59,    34,    40,    40,   258,    84,    85,
      40,    55,   -31,    67,    69,    92,   258,   -31,   -29,   -31,
     137,   258,    56,   -31,    95,    57,   -31,    93,    83,    53,
     -31,    81,   149,    75,    90,    91,   -31,   -31,   -31,    40,
     101,   166,   116,   -31,   178,   118,   -31,    31,   109,   110,
     258,    40,    40,    40,    40,    40,    40,   258,    40,    40,
     -31,   -31,   104,     5,   129,   -31,   -31,   -31,   114,   -31,
     130,   -31,   -31,     4,   -31,   -31,   -31,   -31,   -31,   -31,
     195,   108,    41,   -31,    40,    40,    40,    40,    40,    94,
     -31,   -31,   258,   -31,   -31,   -31,    40,    40,   -31,   -31,
     -31,   -31,   -31,   133,   212,   106,   107,   123,   -31,   258,
     258,   -31,   224,   241,   -31,   -31
  };

  const signed char
  parser::yydefact_[] =
  {
       5,     0,     0,     1,     0,     0,     2,     0,     0,     0,
      32,     0,    25,     0,     0,    29,    30,    31,     0,     0,
       0,     0,     0,     0,    49,     0,     0,     0,     0,     0,
       0,     0,     9,     0,     0,     0,     0,    21,    32,    28,
       0,     0,     0,    20,     0,     0,    47,    49,     0,     0,
      48,     0,     0,     0,     0,     0,     7,     8,    17,     0,
      23,     0,     0,     4,     0,     0,    34,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      18,    19,     0,    35,     0,     6,    27,     3,     0,    22,
       0,    51,    50,     0,    41,    42,    43,    44,    45,    46,
       0,     0,     0,    10,     0,     0,     0,     0,     0,     0,
      26,    33,     0,    12,    13,    14,     0,     0,    36,    37,
      38,    39,    40,     0,     0,     0,     0,     0,    11,     0,
       0,    24,     0,     0,    15,    16
  };

  const short
  parser::yypgoto_[] =
  {
     -31,   -31,   -31,   -31,   -20,   -30,   -31,   -31,   134,   -31,
     111,   -31,   -31,   -24,    52,     0
  };

  const signed char
  parser::yydefgoto_[] =
  {
       0,     1,     2,     6,    31,    32,     8,    33,    14,    18,
      19,    67,    82,    48,    49,    34
  };

  const unsigned char
  parser::yytable_[] =
  {
      40,    57,    51,    35,    12,    23,    36,    52,    12,    24,
      57,   104,   105,   106,   107,   108,    61,    15,    16,    17,
       4,    64,    57,    13,     5,    50,    50,    20,     3,    54,
      50,    57,     7,    25,    57,   112,   113,    26,    37,     9,
      27,    38,    28,    35,    46,    47,    41,    29,    30,    89,
      93,    44,    90,    45,   101,    10,    23,   100,    11,    50,
      24,    68,    69,    57,    71,    72,    73,    74,    75,    76,
      57,    50,    50,    50,    50,    50,    50,    22,    50,    50,
      56,   116,    55,   117,    25,    39,    42,    43,    26,    53,
      47,    27,   124,    28,    57,    58,    59,    60,    29,    30,
      66,    65,    57,    57,    50,    50,    50,    50,    50,   132,
     133,    83,    45,    70,    79,    77,    50,    50,    80,    81,
      84,    86,    88,    94,    95,    96,    97,    98,    99,    91,
      92,   102,   103,   109,   110,   111,   115,   127,    23,   123,
     129,   130,    24,   131,     0,    21,     0,     0,     0,    62,
      23,     0,     0,     0,    24,     0,   118,   119,   120,   121,
     122,     0,    63,     0,     0,     0,    25,    23,   125,   126,
      26,    24,     0,    27,     0,    28,     0,     0,    25,    23,
      29,    30,    26,    24,     0,    27,    78,    28,     0,     0,
       0,    85,    29,    30,     0,    25,    23,     0,     0,    26,
      24,     0,    27,    87,    28,     0,     0,    25,     0,    29,
      30,    26,     0,    23,    27,     0,    28,    24,     0,     0,
       0,    29,    30,     0,    25,    23,     0,     0,    26,    24,
     114,    27,     0,    28,     0,     0,     0,     0,    29,    30,
       0,    25,    23,     0,   128,    26,    24,     0,    27,     0,
      28,     0,     0,    25,     0,    29,    30,    26,     0,    23,
      27,     0,    28,    24,     0,   134,     0,    29,    30,     0,
      25,     0,     0,     0,    26,     0,     0,    27,     0,    28,
       0,     0,   135,     0,    29,    30,     0,    25,     0,     0,
       0,    26,     0,     0,    27,     0,    28,     0,     0,     0,
       0,    29,    30
  };

  const short
  parser::yycheck_[] =
  {
      20,    31,    26,    21,     5,     1,    24,    27,     5,     5,
      40,     6,     7,     8,     9,    10,    36,    46,    47,    48,
      22,    41,    52,    24,    26,    25,    26,    24,     0,    29,
      30,    61,     5,    29,    64,    31,    32,    33,    18,    23,
      36,    21,    38,    21,     4,     5,    24,    43,    44,    18,
      70,    17,    21,    19,    78,    17,     1,    77,    23,    59,
       5,     4,     5,    93,    11,    12,    13,    14,    15,    16,
     100,    71,    72,    73,    74,    75,    76,    19,    78,    79,
      25,    40,    30,    42,    29,     5,     4,    28,    33,     5,
       5,    36,   112,    38,   124,    28,    27,     5,    43,    44,
       5,    45,   132,   133,   104,   105,   106,   107,   108,   129,
     130,    59,    19,    30,    39,    34,   116,   117,    28,    28,
      19,     5,     4,    71,    72,    73,    74,    75,    76,    20,
      20,    79,    28,     4,    20,     5,    28,     4,     1,    45,
      34,    34,     5,    20,    -1,    11,    -1,    -1,    -1,    38,
       1,    -1,    -1,    -1,     5,    -1,   104,   105,   106,   107,
     108,    -1,    25,    -1,    -1,    -1,    29,     1,   116,   117,
      33,     5,    -1,    36,    -1,    38,    -1,    -1,    29,     1,
      43,    44,    33,     5,    -1,    36,    37,    38,    -1,    -1,
      -1,    25,    43,    44,    -1,    29,     1,    -1,    -1,    33,
       5,    -1,    36,    25,    38,    -1,    -1,    29,    -1,    43,
      44,    33,    -1,     1,    36,    -1,    38,     5,    -1,    -1,
      -1,    43,    44,    -1,    29,     1,    -1,    -1,    33,     5,
      35,    36,    -1,    38,    -1,    -1,    -1,    -1,    43,    44,
      -1,    29,     1,    -1,    32,    33,     5,    -1,    36,    -1,
      38,    -1,    -1,    29,    -1,    43,    44,    33,    -1,     1,
      36,    -1,    38,     5,    -1,    41,    -1,    43,    44,    -1,
      29,    -1,    -1,    -1,    33,    -1,    -1,    36,    -1,    38,
      -1,    -1,    41,    -1,    43,    44,    -1,    29,    -1,    -1,
      -1,    33,    -1,    -1,    36,    -1,    38,    -1,    -1,    -1,
      -1,    43,    44
  };

  const signed char
  parser::yystos_[] =
  {
       0,    50,    51,     0,    22,    26,    52,     5,    55,    23,
      17,    23,     5,    24,    57,    46,    47,    48,    58,    59,
      24,    57,    19,     1,     5,    29,    33,    36,    38,    43,
      44,    53,    54,    56,    64,    21,    24,    18,    21,     5,
      53,    24,     4,    28,    17,    19,     4,     5,    62,    63,
      64,    62,    53,     5,    64,    63,    25,    54,    28,    27,
       5,    53,    59,    25,    53,    45,     5,    60,     4,     5,
      30,    11,    12,    13,    14,    15,    16,    34,    37,    39,
      28,    28,    61,    63,    19,    25,     5,    25,     4,    18,
      21,    20,    20,    53,    63,    63,    63,    63,    63,    63,
      53,    62,    63,    28,     6,     7,     8,     9,    10,     4,
      20,     5,    31,    32,    35,    28,    40,    42,    63,    63,
      63,    63,    63,    45,    53,    63,    63,     4,    32,    34,
      34,    20,    53,    53,    41,    41
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    49,    50,    51,    51,    51,    52,    52,    53,    53,
      54,    54,    54,    54,    54,    54,    54,    54,    54,    54,
      54,    55,    56,    57,    57,    57,    57,    58,    58,    59,
      59,    59,    59,    60,    60,    61,    61,    61,    61,    61,
      61,    62,    62,    62,    62,    62,    62,    63,    63,    64,
      64,    64
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     2,     8,     7,     0,     6,     5,     2,     1,
       4,     7,     5,     5,     5,     9,     9,     2,     3,     3,
       2,     4,     4,     3,     8,     1,     6,     4,     2,     1,
       1,     1,     0,     3,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     1,     1,
       4,     4
  };


#if YYDEBUG
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
  "DOWNTO", "READ", "WRITE", "\":\"", "T", "I", "O", "$accept",
  "program_all", "procedures", "main", "commands", "command", "proc_head",
  "proc_call", "declarations", "args_decl", "type", "args", "expression",
  "condition", "value", "identifier", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  parser::yyrline_[] =
  {
       0,   239,   239,   248,   249,   250,   254,   255,   259,   260,
     264,   282,   283,   284,   285,   286,   287,   288,   289,   293,
     302,   306,   310,   314,   320,   330,   336,   348,   349,   358,
     359,   360,   361,   365,   366,   370,   377,   396,   415,   451,
     452,   456,   493,   530,   552,   574,   612,   654,   660,   670,
     679,   696
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
#line 1821 "parser.cpp"

#line 712 "parser.yy"


namespace yy {
  void parser::error (const std::string& msg) {
    std::cerr << "Error: " << msg << '\n';
  }
}

int main () {
  yy::parser parse;
  return parse ();
}
