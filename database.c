#include "database.h"
#include "pokemon.h"
#include <sqlite3.h>
#include <stdio.h>
#include <argon2.h> // seguranca da senha
#include <sys/random.h> // aleatorio
#include <stdint.h> 
#include <string.h>

int validacao_resultado(int resultado, sqlite3 *db, sqlite3_stmt *stmt) {
    if (resultado != SQLITE_OK){
        printf("Erro: %s\n", sqlite3_errmsg(db)); // funcao que mostra erros
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }
    return 0;
}

// guia: 0 nome disponivel, 1 nome nao disponivel, -1 problema
int usuario_existe(const char *nome) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    int resultado;

    resultado = sqlite3_open("pokedex.db", &db);
    if (resultado != SQLITE_OK){
        printf("Erro: %s\n", sqlite3_errmsg(db)); // funcao que mostra erros
        sqlite3_close(db);
        return -1;
    }

    // abrir os usuarios
    const char *sql = "SELECT id FROM usuarios WHERE nome = ?";

    resultado = sqlite3_prepare_v2( // isso aqui apenas prepara o comando
        db, // conexao com banco
        sql, // comando executado
        -1, // le o comando ate o \0
        &stmt, // endereco onde o comando vai ser armazenado
        NULL // sem resto do texto SQL
    );

    if (validacao_resultado(resultado, db, stmt)) {
        return -1;
    }

    // passando os parametros do VALUE
    // ordem de parametro: comando preparado, posicao (SQL comeca em 1), -1 indica que eh p ler ate o fim
    // SQLITE_TRANSIENT: SQL faz uma copia do texto
    resultado = sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);

    if (validacao_resultado(resultado, db, stmt)) {
        return -1;
    }

    resultado = sqlite3_step(stmt);

    int status;
    if (resultado == SQLITE_ROW) {
        printf("Nome indisponivel\n");
        status = 1;
    } else if (resultado == SQLITE_DONE) {
        printf("Nome disponivel!\n");
        status = 0;
    } else {
        printf("Erro: %s\n", sqlite3_errmsg(db));
        status = -1;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return status;
}

// 0: achou, 1: nao encontrou, -1: erro
int buscar_usuario(const char *nome, usuario *u) {
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    int status = -1;

    int resultado = sqlite3_open("pokedex.db", &db);

    if (resultado != SQLITE_OK) {
        printf("Erro: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    const char *sql =
        "SELECT id, nome, senha_hash FROM usuarios WHERE nome = ?;";

    resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (resultado != SQLITE_OK) {
        printf("Erro: %s\n", sqlite3_errmsg(db));
        goto limpeza;
    }

    resultado = sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);

    if (resultado != SQLITE_OK) {
        printf("Erro: %s\n", sqlite3_errmsg(db));
        goto limpeza;
    }

    resultado = sqlite3_step(stmt);

    if (resultado == SQLITE_ROW) {
        const unsigned char *nome_db = sqlite3_column_text(stmt, 1);
        const unsigned char *senha_db = sqlite3_column_text(stmt, 2);

        // Verificar se os ponteiros sao validos
        if (nome_db == NULL || senha_db == NULL) {
            printf("Erro ao recuperar dados do usuario!\n");
            goto limpeza;
        }

        // Verificar se os textos cabem nos vetores
        if (strlen((const char *)nome_db) >= sizeof(u->nome) ||
            strlen((const char *)senha_db) >= sizeof(u->senha_hash)) {
            printf("Erro: dados excedem o tamanho permitido!\n");
            goto limpeza;
        }

        // Copiar os dados para a struct
        u->id = sqlite3_column_int(stmt, 0);

        snprintf(u->nome, sizeof(u->nome), "%s",
                 (const char *)nome_db);

        snprintf(u->senha_hash, sizeof(u->senha_hash), "%s",
                 (const char *)senha_db);

        printf("Usuario encontrado!\n");
        status = 0;

    } else if (resultado == SQLITE_DONE) {
        printf("Usuario inexistente!\n");
        status = 1;

    } else {
        printf("Erro: %s\n", sqlite3_errmsg(db));
        status = -1;
    }

limpeza: // rotulo que o "goto" chama
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return status;
}

// 0 correta, 1 incorreta, -1 problema
int verificar_senha(const char *senha, const char *hash) {

    int resultado = argon2id_verify(hash, senha, strlen(senha)); // hash do usuario, senha digitada
    if (resultado == ARGON2_OK) {
        printf("Senha correta!\n");
        return 0;
    } else if (resultado == ARGON2_VERIFY_MISMATCH) {
        printf("Senha incorreta!\n");
        return 1;
    } else {
        printf("Erro ao verificar senha: %s\n", argon2_error_message(resultado));
        return -1;
    }

}

int inicializar_database() {

    sqlite3 *db;
    int resultado = sqlite3_open("pokedex.db", &db);
    if (resultado != SQLITE_OK){
        printf("Erro: %s\n", sqlite3_errmsg(db)); // funcao que mostra erros
        sqlite3_close(db);
        return 1;
    }

    const char *sql =
    "CREATE TABLE IF NOT EXISTS usuarios ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "nome TEXT NOT NULL UNIQUE,"
    "senha_hash TEXT NOT NULL"
    ");";

    // nome, tipo, PRIMARY KEY: identificador unico, AUTOINCREMENT: incrementa sozinho
    // NOT NULL: n pode ficar vazio
    // UNIQUE: só pode um unico para cada um

    resultado = sqlite3_exec(db, sql, NULL, NULL, NULL); // executar: passamos o ponteiro do banco, e o ponteiro da tabela
    if (resultado != SQLITE_OK){
        printf("Erro: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////
    ////////////////////////////////////
    //////////////////////////
    // tabela dos pokemons
    const char *sql_pokemons = 
    "CREATE TABLE IF NOT EXISTS pokemons ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT," // so pode ter uma unica primary key
    "POKEDEX_NUMBER INTEGER NOT NULL UNIQUE,"
    "NAME TEXT NOT NULL,"
    "TYPE_1 TEXT NOT NULL,"
    "TYPE_2 TEXT,"
    "GENERATION INTEGER,"
    "HEIGHT REAL,"
    "WEIGHT REAL,"
    "HP INTEGER,"
    "ATTACK INTEGER,"
    "DEFENSE INTEGER,"
    "SP_ATTACK INTEGER,"
    "SP_DEFENSE INTEGER,"
    "SPEED INTEGER"
    ");";

    resultado = sqlite3_exec(db, sql_pokemons, NULL, NULL, NULL);
    if (resultado != SQLITE_OK){
        printf("Erro: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    printf("Tabela criada com sucesso!\n");
    sqlite3_close(db);
    return 0;

}

int inserir_usuario(const char *nome, const char *senha_hash) {

    sqlite3 *db;
    int resultado = sqlite3_open("pokedex.db", &db); // abertura do banco de dados

    //
    if (resultado != SQLITE_OK){
        printf("Erro: %s\n", sqlite3_errmsg(db)); // funcao que mostra erros
        sqlite3_close(db);
        return 1;
    }
    //

    const char *sql =
    "INSERT INTO usuarios (nome, senha_hash) VALUES (?, ?);";

    sqlite3_stmt *stmt = NULL; // ponteiro para um comando

    resultado = sqlite3_prepare_v2( // isso aqui apenas prepara o comando
        db, // conexao com banco
        sql, // comando executado
        -1, // le o comando ate o \0
        &stmt, // endereco onde o comando vai ser armazenado
        NULL // sem resto do texto SQL
    );

    if (validacao_resultado(resultado, db, stmt)) {
        return 1;
    }

    // passando os parametros do VALUE
    // ordem de parametro: comando preparado, posicao (SQL comeca em 1), -1 indica que eh p ler ate o fim
    // SQLITE_TRANSIENT: SQL faz uma copia do texto
    resultado = sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_TRANSIENT);
    if (validacao_resultado(resultado, db, stmt)) {
        return 1;
    }
    resultado = sqlite3_bind_text(stmt, 2, senha_hash, -1, SQLITE_TRANSIENT);
    if (validacao_resultado(resultado, db, stmt)) {
        return 1;
    }

    // executando o comando preparado
    resultado = sqlite3_step(stmt);
    if (resultado != SQLITE_DONE) {
        printf("Erro: %s\n", sqlite3_errmsg(db)); // funcao que mostra erros
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1;
    }
    //

    sqlite3_finalize(stmt);
    sqlite3_close(db);


    return 0;
}


// protecao de senha
// gerar um salt aleatório
// configurar os parâmetros do Argon2id
// gerar o hash
// verificar se funcionou
// salt eh conjunto de bytes aleatorios, com salts diferentes, mesmo se a senha for igual, eles vao ter hash diferentes
int gerar_hash_senha(const char *senha, char *hash, size_t tamanho_hash) {
    uint8_t salt[16]; // inteiro sem sinal de 8 bits, vetor
    ssize_t bytes = getrandom(salt, sizeof(salt), 0); // preenchendo o vetor
    // salt: onde armazena
    // tamanho, comportamento padrao
    if (bytes != (ssize_t)sizeof(salt)) { // verificar se o tamanho ta certo
    printf("Erro ao gerar salt!\n");
    return 1;
    }

    uint32_t t_cost = 3;
    uint32_t m_cost = 65536;
    uint32_t parallelism = 1;
    uint32_t hash_len = 32;
    size_t tamanho_senha = strlen(senha);
    size_t tamanho_salt = sizeof(salt);

    // verificar o tamanho
    if (tamanho_hash < argon2_encodedlen(t_cost, m_cost, parallelism, sizeof(salt), hash_len, Argon2_id)) {
    printf("Espaco insuficiente para armazenar o hash!\n");
    return 1;
}

    int resultado = argon2id_hash_encoded(
    t_cost, // quantidade de interacoes
    m_cost, // memoria
    parallelism, // grau de paralelismo
    senha, // senha
    tamanho_senha, 
    salt,
    tamanho_salt,
    hash_len, // tamanho total do hash
    hash, // onde sera armazenado
    tamanho_hash
    );

    if(resultado != ARGON2_OK) {
        printf("Erro ao gerar o hash: %s\n", argon2_error_message(resultado));
        return 1;
    }

    return 0;

}