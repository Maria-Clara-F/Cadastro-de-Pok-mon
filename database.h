#ifndef DATABASE_H
#define DATABASE_H
#include "pokemon.h"
#include <sqlite3.h>
#include <stddef.h>


int inicializar_database();
// conecta e preenche a tabela de usuarios
int inserir_usuario(const char *nome, const char *senha_hash);
// validar erros
int validacao_resultado(int resultado, sqlite3 *db, sqlite3_stmt *stmt);
// funcao que verifica usuarios repetidos
int usuario_existe(const char *nome);
// protecao da senha
int gerar_hash_senha(const char *senha, char *hash, size_t tamanho_hash); // retorna 0: sucesso, 1: error
int buscar_usuario(const char *nome, usuario *u);
int verificar_senha(const char *senha, const char *hash);
// preencher os atributos no banco de dados
int inserir_pokemon(const pokemon *p);

#endif