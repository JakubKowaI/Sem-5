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
#line 15 "parser.yy"

  namespace yy {
      parser::symbol_type yylex();
  }
  int GF=1234577;

  int mod(long long x){
    if(x<0)return (GF+(x%GF))%GF;
    else return x%GF;
  }

  int mod2(long long x){
    long long m = GF - 1;
    x %= m;
    if (x < 0) x += m;
    return x;
  }
  
  int power(long long b,long long e){
    //std::cout<<std::endl<<e<<" - "<<std::endl;
    e=mod2(e);
    int c=1;
    for(int i=0;i<e;i++){
      c=(c*b)%GF;
    }
    return c;
  }

    int power2(long long b,long long e){
    //std::cout<<std::endl<<e<<" - "<<std::endl;
    e=mod2(e);
    int c=1;
    for(int i=0;i<e;i++){
      c=(c*b)%(GF-1);
    }
    return c;
  }

  int div(long long a, long long b) {
    b = mod(b);
    if (b == 0) { std::cout<<"Dzielienie przez 0!!!"<<std::endl; return 0; }
    return (a * power(b,GF-2))%GF;
  }

  int div2(long long a, long long b) {
    b = mod2(b);
    if (b == 0) { std::cout<<"Dzielienie przez 0!!!"<<std::endl; return 0; }
    return (a * power2(b,GF-3))%(GF-1);
  }

#line 97 "parser.cpp"


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
#line 170 "parser.cpp"

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
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_EXPEXPR: // EXPEXPR
      case symbol_kind::S_exp: // exp
      case symbol_kind::S_PRIMARY: // PRIMARY
        value.YY_MOVE_OR_COPY< long long > (YY_MOVE (that.value));
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
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_EXPEXPR: // EXPEXPR
      case symbol_kind::S_exp: // exp
      case symbol_kind::S_PRIMARY: // PRIMARY
        value.move< long long > (YY_MOVE (that.value));
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
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_EXPEXPR: // EXPEXPR
      case symbol_kind::S_exp: // exp
      case symbol_kind::S_PRIMARY: // PRIMARY
        value.copy< long long > (that.value);
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
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_EXPEXPR: // EXPEXPR
      case symbol_kind::S_exp: // exp
      case symbol_kind::S_PRIMARY: // PRIMARY
        value.move< long long > (that.value);
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
      case symbol_kind::S_NUMBER: // NUMBER
      case symbol_kind::S_EXPEXPR: // EXPEXPR
      case symbol_kind::S_exp: // exp
      case symbol_kind::S_PRIMARY: // PRIMARY
        yylhs.value.emplace< long long > ();
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
  case 5: // line: exp "end of line"
#line 94 "parser.yy"
          { std::cout << "\nWynik: " << mod(yystack_[1].value.as < long long > ()) << std::endl; }
#line 586 "parser.cpp"
    break;

  case 6: // line: error "end of line"
#line 95 "parser.yy"
            {std::cout<<std::endl;yyerrok;}
#line 592 "parser.cpp"
    break;

  case 7: // EXPEXPR: PRIMARY
#line 100 "parser.yy"
              {yylhs.value.as < long long > ()=yystack_[0].value.as < long long > ();std::cout<<mod2(yystack_[0].value.as < long long > ())<<" ";}
#line 598 "parser.cpp"
    break;

  case 8: // EXPEXPR: exp "+" exp
#line 101 "parser.yy"
               { yylhs.value.as < long long > () = mod2(yystack_[2].value.as < long long > () + yystack_[0].value.as < long long > ()); std::cout<<'+'<<" "; }
#line 604 "parser.cpp"
    break;

  case 9: // EXPEXPR: exp "-" exp
#line 102 "parser.yy"
                { yylhs.value.as < long long > () = mod2(yystack_[2].value.as < long long > () - yystack_[0].value.as < long long > ()); std::cout<<'-'<<" "; }
#line 610 "parser.cpp"
    break;

  case 10: // EXPEXPR: exp "*" exp
#line 103 "parser.yy"
               { yylhs.value.as < long long > () = mod2(yystack_[2].value.as < long long > () * yystack_[0].value.as < long long > ()); std::cout<<'*'<<" "; }
#line 616 "parser.cpp"
    break;

  case 11: // EXPEXPR: exp "/" exp
#line 104 "parser.yy"
                { yylhs.value.as < long long > () = div2(yystack_[2].value.as < long long > (), yystack_[0].value.as < long long > ()); std::cout<<'/'<<" "; }
#line 622 "parser.cpp"
    break;

  case 12: // EXPEXPR: exp "^" EXPEXPR
#line 106 "parser.yy"
                  { yylhs.value.as < long long > () = mod2(power2(yystack_[2].value.as < long long > (), yystack_[0].value.as < long long > ())); std::cout<<'^'<<" "; }
#line 628 "parser.cpp"
    break;

  case 13: // EXPEXPR: "(" exp ")"
#line 107 "parser.yy"
                    { yylhs.value.as < long long > () = yystack_[1].value.as < long long > (); }
#line 634 "parser.cpp"
    break;

  case 14: // exp: PRIMARY
#line 115 "parser.yy"
              {yylhs.value.as < long long > ()=yystack_[0].value.as < long long > ();std::cout<<mod(yystack_[0].value.as < long long > ())<<" ";}
#line 640 "parser.cpp"
    break;

  case 15: // exp: exp "+" exp
#line 116 "parser.yy"
               { yylhs.value.as < long long > () = mod(yystack_[2].value.as < long long > () + yystack_[0].value.as < long long > ()); std::cout<<'+'<<" "; }
#line 646 "parser.cpp"
    break;

  case 16: // exp: exp "-" exp
#line 117 "parser.yy"
                { yylhs.value.as < long long > () = mod(yystack_[2].value.as < long long > () - yystack_[0].value.as < long long > ()); std::cout<<'-'<<" "; }
#line 652 "parser.cpp"
    break;

  case 17: // exp: exp "*" exp
#line 118 "parser.yy"
               { yylhs.value.as < long long > () = mod(yystack_[2].value.as < long long > () * yystack_[0].value.as < long long > ()); std::cout<<'*'<<" "; }
#line 658 "parser.cpp"
    break;

  case 18: // exp: exp "/" exp
#line 119 "parser.yy"
                { yylhs.value.as < long long > () = div(yystack_[2].value.as < long long > (), yystack_[0].value.as < long long > ()); std::cout<<'/'<<" "; }
#line 664 "parser.cpp"
    break;

  case 19: // exp: exp "^" EXPEXPR
#line 121 "parser.yy"
                  { yylhs.value.as < long long > () = mod(power(yystack_[2].value.as < long long > (), yystack_[0].value.as < long long > ())); std::cout<<'^'<<" "; }
#line 670 "parser.cpp"
    break;

  case 20: // exp: "(" exp ")"
#line 122 "parser.yy"
                    { yylhs.value.as < long long > () = yystack_[1].value.as < long long > (); }
#line 676 "parser.cpp"
    break;

  case 21: // PRIMARY: NUMBER
#line 126 "parser.yy"
         { yylhs.value.as < long long > () = yystack_[0].value.as < long long > ();  }
#line 682 "parser.cpp"
    break;

  case 22: // PRIMARY: "-" PRIMARY
#line 127 "parser.yy"
                          {yylhs.value.as < long long > () = -yystack_[0].value.as < long long > ();  }
#line 688 "parser.cpp"
    break;


#line 692 "parser.cpp"

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









  const signed char parser::yypact_ninf_ = -23;

  const signed char parser::yytable_ninf_ = -1;

  const signed char
  parser::yypact_[] =
  {
     -23,    17,   -23,    16,   -23,     6,    18,   -23,   -23,    -3,
     -23,   -23,   -23,    32,    18,    18,    18,    18,    52,   -23,
     -23,    55,    55,     0,     0,    18,   -23,    46,   -23,    39,
      18,    18,    18,    18,    52,   -23,    55,    55,     0,     0,
     -23
  };

  const signed char
  parser::yydefact_[] =
  {
       2,     0,     1,     0,    21,     0,     0,     4,     3,     0,
      14,     6,    22,     0,     0,     0,     0,     0,     0,     5,
      20,    15,    16,    17,    18,     0,    19,     0,     7,     0,
       0,     0,     0,     0,     0,    13,     8,     9,    10,    11,
      12
  };

  const signed char
  parser::yypgoto_[] =
  {
     -23,   -23,   -23,   -22,    -1,     1
  };

  const signed char
  parser::yydefgoto_[] =
  {
       0,     1,     8,    26,    27,    10
  };

  const signed char
  parser::yytable_[] =
  {
       9,    14,    15,    16,    17,    13,    12,    18,    19,     4,
      18,     5,    40,    21,    22,    23,    24,     2,     3,    28,
       4,     4,     5,     5,    29,     6,     6,    11,     7,    36,
      37,    38,    39,     0,     0,    28,    14,    15,    16,    17,
       0,    20,    18,    14,    15,    16,    17,     0,    35,    18,
      30,    31,    32,    33,     0,     4,    34,     5,     0,     0,
      25,    16,    17,     0,     0,    18
  };

  const signed char
  parser::yycheck_[] =
  {
       1,     4,     5,     6,     7,     6,     5,    10,    11,     3,
      10,     5,    34,    14,    15,    16,    17,     0,     1,    18,
       3,     3,     5,     5,    25,     8,     8,    11,    11,    30,
      31,    32,    33,    -1,    -1,    34,     4,     5,     6,     7,
      -1,     9,    10,     4,     5,     6,     7,    -1,     9,    10,
       4,     5,     6,     7,    -1,     3,    10,     5,    -1,    -1,
       8,     6,     7,    -1,    -1,    10
  };

  const signed char
  parser::yystos_[] =
  {
       0,    14,     0,     1,     3,     5,     8,    11,    15,    17,
      18,    11,    18,    17,     4,     5,     6,     7,    10,    11,
       9,    17,    17,    17,    17,     8,    16,    17,    18,    17,
       4,     5,     6,     7,    10,     9,    17,    17,    17,    17,
      16
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    13,    14,    14,    15,    15,    15,    16,    16,    16,
      16,    16,    16,    16,    17,    17,    17,    17,    17,    17,
      17,    18,    18
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     0,     2,     1,     2,     2,     1,     3,     3,
       3,     3,     3,     3,     1,     3,     3,     3,     3,     3,
       3,     1,     2
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "NUMBER", "\"+\"",
  "\"-\"", "\"*\"", "\"/\"", "\"(\"", "\")\"", "\"^\"", "\"end of line\"",
  "NEG", "$accept", "input", "line", "EXPEXPR", "exp", "PRIMARY", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const signed char
  parser::yyrline_[] =
  {
       0,    88,    88,    89,    93,    94,    95,   100,   101,   102,
     103,   104,   106,   107,   115,   116,   117,   118,   119,   121,
     122,   126,   127
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
#line 1019 "parser.cpp"

#line 130 "parser.yy"


namespace yy {
  void parser::error (const std::string& msg) {
    std::cerr << "Error: " << msg << '\n';
  }
}

int main () {
  yy::parser parse;
  return parse ();
}
