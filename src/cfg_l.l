%{
#include    "cfg_y.h"
#include    <stdio.h>
#include    <stdlib.h>

static int last_key = -1;
%}
svc_begin [A-Za-z]+[\t \n]*\{
svc_end \}
value [A-Za-z0-9\/-]+
%%

{svc_begin} { yylval = yytext; return SVC_BEGIN; }
{svc_end} { yylval = yytext; return SVC_END; }
= { yylval = yytext; return EQ; }
port { yylval = yytext; return (last_key = PORT); }
cmd { yylval = yytext; return (last_key = CMD); }
socktype { yylval = yytext; return (last_key = SOCKTYPE); }
{value} {
    int val_type;
    switch(last_key)
    {
        case PORT: 
            val_type = PORT_VAL;
            break;
        case CMD: 
            val_type = CMD_VAL;
            break;
        case SOCKTYPE: 
            val_type = SOCKTYPE_VAL;
            break;
        default: 
            printf("syntax error: %s\n", yytext);
            exit(EXIT_FAILURE);
    }
    last_key = -1;
    yylval = yytext;
    return val_type;
}
%%

int yywrap()
{
    return 1;
}

