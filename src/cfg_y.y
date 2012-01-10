%{
typedef char* string;
#define YYSTYPE string
#include    "cfg.h"
#include    <strings.h>
#include    <string.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <limits.h>
#define MAX_SVC 10

static Config cfg[MAX_SVC];
static int cfg_index = 0;
static Config current_cfg;

int yyerror(char *msg);

extern FILE *yyin;
%}
%token SVC_BEGIN SVC_END EQ PORT PORT_VAL CMD CMD_VAL SOCKTYPE SOCKTYPE_VAL
%%

file: record file | record;
record: SVC_BEGIN svc_attr SVC_END {
    if(cfg_index == MAX_SVC-1)
    {
        // 报错
        yyerror("max cfg count overceed\n");
    }
    else
        cfg[cfg_index++] = current_cfg;
};
svc_attr: key_value svc_attr | key_value;

key_value: port_def | cmd_def | socktype_def;

port_def: PORT EQ PORT_VAL{
    current_cfg.port = strdup($3);
};
cmd_def: CMD EQ CMD_VAL {
    current_cfg.cmd = strdup($3);
};
socktype_def: SOCKTYPE EQ SOCKTYPE_VAL {
    current_cfg.socktype = (int)strtol($3, NULL, 10);
    if(current_cfg.socktype == LONG_MAX)
    {
        // 报错
        yyerror("socktype is not a valid integer\n");
    }
};
%%

Config* parse_config(const char *path)
{
    bzero(cfg, sizeof(cfg));
    FILE *file = NULL;
    if((file = fopen(path, "r")) == NULL)
    {
        printf("error while reading config file\n");
        exit(EXIT_FAILURE);
    }
    yyin = file;
    yyparse();
    fclose(file);

    return cfg;
}

int yyerror(char *msg)
{
    printf("error while parsing config file: %s\n", msg);
    exit(EXIT_FAILURE);
}

//int main()
//{
//    Config *c = parse_config();
//    print_config(c);
//    return 0;
//}

