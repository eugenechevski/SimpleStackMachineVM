/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_ASM_TAB_H_INCLUDED
# define YY_YY_ASM_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 7 "asm.y"


 /* Including "ast.h" must be at the top, to define the AST type */
#include "ast.h"
#include "machine_types.h"
#include "parser_types.h"
#include "lexer.h"
// #include "utilities.h" // only needed for debugging

    /* Report an error to the user on stderr */
extern void yyerror(const char *filename, const char *msg);

#line 62 "asm.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    eolsym = 258,                  /* eolsym  */
    identsym = 259,                /* identsym  */
    unsignednumsym = 260,          /* unsignednumsym  */
    plussym = 261,                 /* "+"  */
    minussym = 262,                /* "-"  */
    commasym = 263,                /* ","  */
    dottextsym = 264,              /* ".text"  */
    dotdatasym = 265,              /* ".data"  */
    dotstacksym = 266,             /* ".stack"  */
    dotendsym = 267,               /* ".end"  */
    colonsym = 268,                /* ":"  */
    lbracketsym = 269,             /* "["  */
    rbracketsym = 270,             /* "]"  */
    equalsym = 271,                /* "="  */
    noopsym = 272,                 /* "NOP"  */
    addopsym = 273,                /* "ADD"  */
    subopsym = 274,                /* "SUB"  */
    cpwopsym = 275,                /* "CPW"  */
    andopsym = 276,                /* "AND"  */
    boropsym = 277,                /* "BOR"  */
    noropsym = 278,                /* "NOR"  */
    xoropsym = 279,                /* "XOR"  */
    lwropsym = 280,                /* "LWR"  */
    swropsym = 281,                /* "SWR"  */
    scaopsym = 282,                /* "SCA"  */
    lwiopsym = 283,                /* "LWI"  */
    negopsym = 284,                /* "NEG"  */
    litopsym = 285,                /* "LIT"  */
    ariopsym = 286,                /* "ARI"  */
    sriopsym = 287,                /* "SRI"  */
    mulopsym = 288,                /* "MUL"  */
    divopsym = 289,                /* "DIV"  */
    cfhiopsym = 290,               /* "CFHI"  */
    cfloopsym = 291,               /* "CFLO"  */
    sllopsym = 292,                /* "SLL"  */
    srlopsym = 293,                /* "SRL"  */
    jmpopsym = 294,                /* "JMP"  */
    jrelopsym = 295,               /* "JREL"  */
    addiopsym = 296,               /* "ADDI"  */
    andiopsym = 297,               /* "ANDI"  */
    boriopsym = 298,               /* "BORI"  */
    noriopsym = 299,               /* "NORI"  */
    xoriopsym = 300,               /* "XORI"  */
    beqopsym = 301,                /* "BEQ"  */
    bgezopsym = 302,               /* "BGEZ"  */
    blezopsym = 303,               /* "BLEZ"  */
    bgtzopsym = 304,               /* "BGTZ"  */
    bltzopsym = 305,               /* "BLTZ"  */
    bneopsym = 306,                /* "BNE"  */
    csiopsym = 307,                /* "CSI"  */
    jmpaopsym = 308,               /* "JMPA"  */
    callopsym = 309,               /* "CALL"  */
    rtnopsym = 310,                /* "RTN"  */
    exitopsym = 311,               /* "EXIT"  */
    pstropsym = 312,               /* "PSTR"  */
    pchopsym = 313,                /* "PCH"  */
    rchopsym = 314,                /* "RCH"  */
    straopsym = 315,               /* "STRA"  */
    notropsym = 316,               /* "NOTR"  */
    regsym = 317,                  /* regsym  */
    wordsym = 318,                 /* "WORD"  */
    charsym = 319,                 /* "CHAR"  */
    stringsym = 320,               /* "STRING"  */
    charliteralsym = 321,          /* charliteralsym  */
    stringliteralsym = 322         /* stringliteralsym  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (char const *file_name);


#endif /* !YY_YY_ASM_TAB_H_INCLUDED  */
