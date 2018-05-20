%{
/* C declarations and includes*/
# define LIST strcat(buf , yytext )
# define print                  { LIST; if(Opt_S) printf("%d: %s", linenum, buf); linenum++; buf[0] = '\0';}
# define token(t)               { LIST ; if ( Opt_T ) printf ("<%s>\n", (t));}
# define tokenChar(t)           { LIST ; if ( Opt_T ) printf ("<%s>\n", (t));}
# define tokenKeyword(t)        { LIST ; if ( Opt_T ) printf ("<KW%s>\n", (t));}
# define tokenVar(t, s)         { LIST ; if ( Opt_T ) printf ("<%s: %s>\n", #t, (s));}
# define MAX_LINE_LENG 256

int Opt_S = 1;
int Opt_T = 1;
int linenum = 1;
char buf[ MAX_LINE_LENG ];
%}

letter                               [a-zA-z]
digit                                [0-9]
oct                                  [0-7]
zero                                 [0]
int                                  0|([1-9][0-9]*)
float                                [0-9]+\.[0-9]+
str                                  [a-zA-Z_0-9]
space                                [ \t]
keywords                             array|begin|boolean|def|do|else|end|false|for|integer|if|of|print|read|real|string|then|to|true|return|var|while
delimiter                            [\,\;\:\(\)\[\]]
operator                             [\+\-\*\/]|mod|:=|[\<\>]|<=|>=|<>|and|or|not

%START COMMENT

%%

<INITIAL>{space}                     {LIST;}
\n                                   {print;}

<INITIAL>"/*"                        {LIST; BEGIN COMMENT;}
<COMMENT>"*/"                        {LIST; BEGIN INITIAL;}
<COMMENT>.                           {LIST;}

<INITIAL>"//&S+".*                   {Opt_S=1; LIST;}
<INITIAL>"//&S-".*                   {Opt_S=0; LIST;}
<INITIAL>"//&T+".*                   {Opt_T=1; LIST;}
<INITIAL>"//&T-".*                   {Opt_T=0; LIST;}

<INITIAL>"//".*                      {LIST;}

<INITIAL>{delimiter}                 {tokenChar(yytext);}

<INITIAL>{operator}                  {token(yytext);}

<INITIAL>{keywords}                  {tokenKeyword(yytext);}

<INITIAL>{int}                       {tokenVar(integer, yytext);}
<INITIAL>{zero}{oct}+                {tokenVar(oct_integer, yytext);}
<INITIAL>{float}                     {tokenVar(float, yytext);}
<INITIAL>({float}|{int})[eE][+-]?{int}   {tokenVar(scientific, yytext);}
<INITIAL>{letter}{str}*              {tokenVar(id, yytext);}
<INITIAL>\"([^"]|\"{2})*\"           {
                                       LIST;
                                       for(int i = 0; i < yyleng - 1; i++){
                                         if((int)(yytext[i]) == 20 || (yytext[i] == '\\' && yytext[i + 1] == 'n')){
                                           printf("error at line %d: bad character \"%s\"\n",linenum,yytext);
                                           return 0;
                                         }
                                       }

                                       if ( Opt_T ){ // printf (" <%s: %s>\n", #t, (s));
                                         int i;
                                         printf("<%s: ", "string");
                                         for(i = 1; i < yyleng - 1; i++){
                                           if(yytext[i] == '\"' && yytext[i + 1] == '\"')
                                             i++;
                                           printf("%c", yytext[i]);
                                         }
                                         printf(">\n");
                                       }
                                    }
.                                   {printf("error at line %d: bad character \"%s\"\n",linenum,yytext); return 0;}
%%

int main ( int argc , char ** argv ){

  if( argc != 2 ) {
    fprintf ( stderr , " Usage : ./ scanner [ filename ]\n" );
    exit (0);
  }
  FILE *fp = fopen ( argv [1] , "r" );
  if( fp == NULL ) {
    fprintf ( stderr , " Open file error \n" );
    exit (-1);
  }
  yyin = fp;
  yylex();
  exit (0);
}
