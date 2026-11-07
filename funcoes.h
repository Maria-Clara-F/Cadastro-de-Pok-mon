#ifndef FUNCOES_H
#define FUNCOES_H
#include "pokemon.h"

int ler_int(int *opcao, int min, int max);
void normalizar_espacos(char *str);
void exibir_pokemon(const pokemon *p);

///
int cadastrar();
int consultar();
int pokebox();
int arena();
int remover();
int estatisticas();
///

///
int cadastro_usuario(int *entrada);
int entrada_usuario(usuario *u);
///

/// 
int validar_usuario(usuario *u);
int validar_senha();
void animar_pokebola();
///

#endif