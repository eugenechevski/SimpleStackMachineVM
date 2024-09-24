 /* $Id: asm.y,v 1.62 2024/08/28 21:25:00 leavens Exp $ */

%code top {
#include <stdio.h>
}

%code requires {

 /* Including "ast.h" must be at the top, to define the AST type */
#include "ast.h"
#include "machine_types.h"
#include "parser_types.h"
#include "lexer.h"
// #include "utilities.h" // only needed for debugging

    /* Report an error to the user on stderr */
extern void yyerror(const char *filename, const char *msg);
}    

%verbose
%define parse.lac full
%define parse.error detailed

%parse-param { char const *file_name }

%token eolsym
%token <ident> identsym
%token <unsignednum> unsignednumsym
%token <token> plussym    "+"
%token <token> minussym   "-"
%token commasym   ","
%token <token> dottextsym ".text"
%token <token> dotdatasym ".data"
%token <token> dotstacksym ".stack"
%token dotendsym  ".end"
%token colonsym   ":"
%token <token> lbracketsym "["
%token <token> rbracketsym "]"
%token <token> equalsym   "="
%token <token> noopsym    "NOP"
%token <token> addopsym   "ADD"
%token <token> subopsym   "SUB"
%token <token> cpwopsym   "CPW"
%token <token> andopsym   "AND"
%token <token> boropsym   "BOR"
%token <token> noropsym   "NOR"
%token <token> xoropsym   "XOR"
%token <token> lwropsym   "LWR"
%token <token> swropsym   "SWR"
%token <token> scaopsym   "SCA"
%token <token> lwiopsym   "LWI"
%token <token> negopsym   "NEG"
%token <token> litopsym   "LIT"
%token <token> ariopsym   "ARI"
%token <token> sriopsym   "SRI"
%token <token> mulopsym   "MUL"
%token <token> divopsym   "DIV"
%token <token> cfhiopsym  "CFHI"
%token <token> cfloopsym  "CFLO"
%token <token> sllopsym   "SLL"
%token <token> srlopsym   "SRL"
%token <token> jmpopsym   "JMP"
%token <token> jrelopsym  "JREL"
%token <token> addiopsym  "ADDI"
%token <token> andiopsym  "ANDI"
%token <token> boriopsym  "BORI"
%token <token> noriopsym  "NORI"
%token <token> xoriopsym  "XORI"
%token <token> beqopsym   "BEQ"
%token <token> bgezopsym  "BGEZ"
%token <token> blezopsym  "BLEZ"
%token <token> bgtzopsym  "BGTZ"
%token <token> bltzopsym  "BLTZ"
%token <token> bneopsym   "BNE"
%token <token> csiopsym   "CSI"
%token <token> jmpaopsym  "JMPA"
%token <token> callopsym  "CALL"
%token <token> rtnopsym   "RTN"
%token <token> exitopsym  "EXIT"
%token <token> pstropsym  "PSTR"
%token <token> pchopsym   "PCH"
%token <token> rchopsym   "RCH"
%token <token> straopsym  "STRA"
%token <token> notropsym  "NOTR"

%token <reg> regsym

%token <token> wordsym    "WORD"
%token <token> charsym    "CHAR"
%token <token> stringsym  "STRING"
%token <charlit> charliteralsym
%token <stringlit> stringliteralsym


%type <program> program
%type <text_section> textSection
%type <addr> entryPoint
%type <addr> addr
%type <ident> label
%type <asm_instr> asmInstr
%type <label_opt> labelOpt
%type <empty> empty
%type <asm_instrs> asmInstrs
%type <instr> instr
%type <instr> noArgInstr
%type <token> noArgOp
%type <instr> twoRegCompInstr
%type <token> twoRegCompOp
%type <number> offset
%type <number> number
%type <token> sign
%type <instr> noTargetOffsetInstr
%type <token> noTargetOffsetOp
%type <instr> noSourceOffsetInstr
%type <token> noSourceOffsetOp
%type <instr> oneRegOffsetArgInstr
%type <token> oneRegOffsetArgOp
%type <instr> oneRegArgInstr
%type <token> oneRegArgOp
%type <number> arg
%type <instr> oneRegOffsetInstr
%type <token> oneRegOffsetOp
%type <instr> shiftInstr
%type <token> shiftOp
%type <immed> shift
%type <instr> argOnlyInstr
%type <token> argOnlyOp
%type <instr> immedArithInstr
%type <token> immedArithOp
%type <immed> immed
%type <instr> immedBoolInstr
%type <token> immedBoolOp
%type <immed> uimmed
%type <instr> branchTestInstr
%type <token> branchTestOp
%type <instr> jumpInstr
%type <token> jumpOp
%type <instr> syscallInstr
%type <instr> offsetOnlySyscall
%type <token> offsetOnlySyscallOp
%type <instr> regOffsetSyscall
%type <token> regOffsetSyscallOp
%type <instr> noArgSyscall
%type <token> noArgSyscallOp
%type <data_section> dataSection
%type <unsignednum> staticStartAddr
%type <static_decls> staticDecls
%type <static_decl> staticDecl
%type <data_size> dataSize
%type <initializer> initializerOpt
%type <stack_section> stackSection
%type <unsignednum> stackBottomAddr

%start program

%code {
 /* extern declarations provided by the lexer */
extern int yylex(void);

 /* extern void yyerror(char const *msg); */

 /* The AST for the program, set by the semantic action for program. */
ast_program_t progast; 

 /* Set the program's ast to be t */
extern void setProgAST(ast_program_t t);
}

%%

program : textSection dataSection stackSection ".end"
           { setProgAST(ast_program($1, $2, $3)); } ;

textSection : ".text" entryPoint asmInstrs 
           { $$ = ast_text_section($1,$2,$3); } ;


entryPoint : addr ;

addr : label { $$ = ast_addr_label($1); }
      | unsignednumsym { $$ = ast_entry_addr($1); }
      ;

label : identsym ;


asmInstrs : asmInstr { $$ = ast_asm_instrs_singleton($1); }
      | asmInstrs asmInstr { $$ = ast_asm_instrs_add($1,$2); }
      ;

asmInstr : labelOpt instr eolsym { $$ = ast_asm_instr($1,$2); } ;

labelOpt : label ":" { $$ = ast_label_opt_label($1); }
      | empty { $$ = ast_label_opt_empty($1); }
      ;

empty : %empty { $$ = ast_empty(lexer_filename(), lexer_line()); } ;


instr : noArgInstr | twoRegCompInstr | noTargetOffsetInstr
      | noSourceOffsetInstr | oneRegOffsetArgInstr | oneRegArgInstr
      | oneRegOffsetInstr | shiftInstr | argOnlyInstr | immedArithInstr
      | immedBoolInstr | branchTestInstr | jumpInstr | syscallInstr
      ;


noArgInstr : noArgOp { $$ = ast_0arg_instr($1); } ;

noArgOp : "NOP" | "RTN" ;


twoRegCompInstr : twoRegCompOp regsym "," offset "," regsym "," offset
           {
	       $$ = ast_2reg_instr($1, $2.number, $4.value,
				   $6.number, $8.value,
				   lexer_token2func($1.toknum));
	   }
           ;

twoRegCompOp : "ADD" | "SUB" | "CPW" 
               | "AND" | "BOR" | "NOR" | "XOR" | "SCA" | "LWI" | "NEG" ;


offset : number 
           {
	       machine_types_check_fits_in_offset($1.value);
	       $$ = $1;
	   }
           ;

number : sign unsignednumsym
           {
	       word_type val = $2.value;
               if ($1.toknum == minussym) {
		   val = - val;
               }
               $$ = ast_number($1, val);
	   }
           ;

sign : "+" | "-"
     | empty { $$ = ast_token(lexer_filename(), lexer_line(), plussym); }
     ; 


noTargetOffsetInstr : noTargetOffsetOp regsym "," regsym "," offset
           {
	       $$ = ast_2reg_instr($1, $2.number, 0, $4.number, $6.value,
				   lexer_token2func($1.toknum));
	   }
           ;

noTargetOffsetOp : "LWR" ;


noSourceOffsetInstr : noSourceOffsetOp regsym "," offset "," regsym
           {
	       $$ = ast_2reg_instr($1, $2.number, $4.value, $6.number, 0,
				   lexer_token2func($1.toknum));
	   }
           ;

noSourceOffsetOp : "SWR" ;


oneRegOffsetArgInstr : oneRegOffsetArgOp regsym "," offset "," arg
           {
	       $$ = ast_1reg_instr($1, other_comp_instr_type,
				   1, $2.number, $4.value,
				   lexer_token2func($1.toknum),
				   ast_immed_number($6.value));
	   }
           ;

oneRegOffsetArgOp : "LIT" ;

arg : number 
           {   /* the number is signed */
	       machine_types_check_fits_in_arg($1.value);
	       $$ = $1;
	   }
           ;


oneRegArgInstr : oneRegArgOp regsym "," arg
           {
	       $$ = ast_1reg_instr($1, other_comp_instr_type,
				   1, $2.number, 0,
				   lexer_token2func($1.toknum),
				   ast_immed_number($4.value));
	   }
           ;

oneRegArgOp : "ARI" | "SRI" ;
                

oneRegOffsetInstr : oneRegOffsetOp regsym "," offset
           {
	       $$ = ast_1reg_instr($1, other_comp_instr_type,
				   1, $2.number, $4.value,
				   lexer_token2func($1.toknum),
				   ast_immed_none());
	   }
           ;

oneRegOffsetOp : "MUL" | "DIV" | "CFHI" | "CFLO" | "JMP" | "CSI" ;


shiftInstr : shiftOp regsym "," offset "," shift
           {
	       $$ = ast_1reg_instr($1, other_comp_instr_type,
				   1, $2.number, $4.value,
				   lexer_token2func($1.toknum),
				   $6);
	   }
           ;

shiftOp : "SLL" | "SRL" ;

shift : unsignednumsym
           {
	       machine_types_check_fits_in_shift($1.value);
	       $$ = ast_immed_unsigned($1.value);
	   }
           ;


argOnlyInstr : argOnlyOp arg
           {
	       $$ = ast_1reg_instr($1, other_comp_instr_type,
				   0, 0, 0, lexer_token2func($1.toknum),
				   ast_immed_number($2.value));
	   }
           ;

argOnlyOp : "JREL" ;


immedArithInstr : immedArithOp regsym "," offset "," immed
           {
	       $$ = ast_1reg_instr($1, immed_instr_type,
				   1, $2.number, $4.value,
				   0, $6);
	   }
           ;

immedArithOp : "ADDI" ;

immed : number
       {
	   machine_types_check_fits_in_immed($1.value);
           $$ = ast_immed_number($1.value);
       }
       ;


immedBoolInstr : immedBoolOp regsym "," offset "," uimmed
       {
	   $$ = ast_1reg_instr($1, immed_instr_type,
			       1, $2.number, $4.value,
			       0, $6);
       }
       ;

immedBoolOp : "ANDI" | "BORI" | "XORI" | "NORI" ;

uimmed : unsignednumsym
       {
	   machine_types_check_fits_in_uimmed($1.value);
           $$ = ast_immed_unsigned($1.value);
       }
       ;


branchTestInstr : branchTestOp regsym "," offset "," immed
       {
	   $$ = ast_1reg_instr($1, immed_instr_type,
			       1, $2.number, $4.value,
			       0, $6);
       }
       ;

branchTestOp : "BEQ" | "BGEZ" | "BGTZ" | "BLEZ" | "BLTZ" | "BNE" ;


jumpInstr : jumpOp addr
            {
		if ($2.address_defined) {
		    machine_types_check_fits_in_addr($2.addr);
		}
		$$ = ast_1reg_instr($1, jump_instr_type,
				    0, 0, 0,
				    0, ast_immed_addr($2));
	    }
            ;

jumpOp : "JMPA" | "CALL" ;


syscallInstr : offsetOnlySyscall | regOffsetSyscall | noArgSyscall

offsetOnlySyscall : offsetOnlySyscallOp offset
            {
		$$ = ast_1reg_instr($1, syscall_instr_type,
				    1, 0, $2.value, 
				    SYS_F, ast_syscall_code_for($1.toknum));
	    }
            ;

offsetOnlySyscallOp : "EXIT" ;


regOffsetSyscall : regOffsetSyscallOp regsym "," offset
            {
		$$ = ast_1reg_instr($1, syscall_instr_type,
				    1, $2.number, $4.value, 
				    SYS_F, ast_syscall_code_for($1.toknum));
	    }
            ;

regOffsetSyscallOp : "PSTR" | "PCH" | "RCH" ;


noArgSyscall : noArgSyscallOp 
            {
		$$ = ast_1reg_instr($1, syscall_instr_type,
				    0, 0, 0,
				    SYS_F, ast_syscall_code_for($1.toknum));
	    }
            ;

noArgSyscallOp : "STRA" | "NOTR" ;



dataSection : ".data" staticStartAddr staticDecls
              { $$ = ast_data_section($1, $2.value, $3); }
              ;

staticStartAddr : unsignednumsym ;


staticDecls : empty { $$ = ast_static_decls_empty($1); }
            | staticDecls staticDecl { $$ = ast_static_decls_add($1,$2); }
            ;

staticDecl : dataSize identsym initializerOpt eolsym
            { $$ = ast_static_decl($1, $2, $3); } ;

dataSize : "WORD" { $$ = ast_data_size($1, ds_word, 1); }
         | "CHAR" { $$ = ast_data_size($1, ds_char, 1); }
         | "STRING" "[" unsignednumsym "]"
   	          { $$ = ast_data_size($1, ds_string,
				       /* declared size is in words! */
				       $3.value); } ;

initializerOpt : "=" number { $$ = ast_initializer_number($1, $2.value); }
               | "=" charliteralsym
	          { $$ = ast_initializer_char($1, $2.value); }
               | "=" stringliteralsym
	          { $$ = ast_initializer_string($1, $2.pointer); }
               | empty { $$ = ast_initializer_empty($1); }
               ;


stackSection : ".stack" stackBottomAddr
              { $$ = ast_stack_section($1, $2.value); } ;

stackBottomAddr : unsignednumsym ;

%%

// Set the program's ast to be t
void setProgAST(ast_program_t t) { progast = t; }
