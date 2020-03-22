#ifndef PARSER_H
#define PARSER_H

int CurrentLevel;

void open_scope();
void close_scope();
void func_decl();
void module();
void parser_init();


#endif