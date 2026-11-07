#include "funcoes.h"
#include <stdio.h>
#include <ctype.h>
#include "pokemon.h"
#include <string.h>
#include "database.h"

int ler_int(int *opcao, int min, int max) {
    int resultado;
    int c;

    while (1) {
        resultado = scanf("%d", opcao);

        if (resultado != 1) {
            if (resultado == EOF) {
                return 0;
            }

            printf("Digite apenas numeros!\n");

            while ((c = getchar()) != '\n' && c != EOF);

            printf("Tente novamente: ");
            continue;
        }

        while ((c = getchar()) != '\n' && c != EOF);

        if (*opcao < min || *opcao > max) {
            printf("Digite um numero entre %d e %d!\n", min, max);
            printf("Tente novamente: ");
            continue;
        }

        return 1;
    }
}

void normalizar_espacos(char *str) {
    int leitura = 0;
    int escrita = 0;
    int espaco_pendente = 0;

    while (str[leitura] != '\0') {
        unsigned char c = (unsigned char)str[leitura++];

        if (isspace(c)) {
            if (escrita > 0) {
                espaco_pendente = 1;
            }
        } else {
            if (espaco_pendente) {
                str[escrita++] = ' ';
                espaco_pendente = 0;
            }

            str[escrita++] = c;
        }
    }

    str[escrita] = '\0';
}

void exibir_pokemon(const pokemon *p) {
    if(p == NULL) {
        return;
    }

    printf("\nNumero: %d", p->pokedex_number);
    printf("\nNome: %s", p->name);
    printf("\nTipo 1: %s", p->type1);
    if(p->type2[0] != '\0') {
        printf("\nTipo 2: %s", p->type2);
    }
    printf("\nGeneracao %d", p->generation);
    printf("\nAltura: %.2f", p->height);
    printf("\nPeso: %.2f", p->weight);
    printf("\nVida: %d", p->stats.hp);
    printf("\nAtaque: %d", p->stats.attack);
    printf("\nDefesa: %d", p->stats.defense);
    printf("\nAtaque especial: %d", p->stats.sp_attack);
    printf("\nDefesa especial: %d", p->stats.sp_defense);
    printf("\nVelocidade: %d", p->stats.speed);
    printf("\n");

}

//

int cadastrar() {
    return 0;
}

int consultar() {
    printf("\n=//===//===//===//====//===//===//=");
    printf("\n   CONSULTAR O BANCO DE POKEMONS   ");
    printf("\n=//===//===//===//====//===//===//=");
    int opcao;
    printf("[0] Voltar\n[1] Listar todos os Pokemons\n[2] Buscar por numero\n[3] Buscar por nome\n[4] Filtrar por tipo\n");
    printf("O que deseja? ");
    while(!ler_int(&opcao, 0, 4));
    printf("\n");

    switch (opcao) {
        case 0:
            return 0;
        case 1:
            break;
        case 2:
            int numero;
            pokemon p = {0};
            while(!ler_int(&numero, 1, 151));
            buscar_pokemon_por_numero(numero, &p);
            exibir_pokemon(&p);
            break;
        case 3:
            break;
        case 4:
            break;
    }
    return 0;
}

int pokebox() {
    return 0;
}

int arena() {
    return 0;
}

int remover () {
    return 0;
}

int estatisticas() {
    return 0;
}

//

int validar_usuario(usuario *u) {
    if (fgets(u->nome, sizeof(u->nome), stdin) == NULL) {
        return 1;
    }

    // Verifica se a entrada coube no vetor
    if (strchr(u->nome, '\n') == NULL && !feof(stdin)) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        printf("Nome muito longo!\n");
        return 1;
    }

    // Remove espaços extras e a quebra de linha
    normalizar_espacos(u->nome);

    if (u->nome[0] == '\0') {
        printf("O nome nao pode estar vazio!\n");
        return 1;
    }

    int a = usuario_existe(u->nome);
    if (a == 0) {
        return 0;
    } else {
        return 1;
    }

}

int validar_senha(usuario *u) {
    char entrada[30];

    if (fgets(entrada, sizeof(entrada), stdin) == NULL) {
        return 1;
    }

    if (strchr(entrada, '\n') == NULL && !feof(stdin)) { // nao encontrou uma quabra de linha e nem cheogu ao fim
        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        printf("Senha muito longa!\n");
        return 1;
    }

    entrada[strcspn(entrada, "\n")] = '\0';

    if (entrada[0] == '\0') {
        printf("A senha nao pode estar vazia!\n");
        return 1;
    }

    strcpy(u->senha, entrada);

    return 0;
}

// 0: login realizado
// 1: login nao autorizado
// -1: erro

int entrada_usuario(usuario *u) {
    printf("/=============================/\n");
    printf("  // //      LOGIN       // // \n");
    printf("/=============================/\n");

    printf("Digite seu nome de usuario e senha:\n");

    printf("Usuario: ");

    char entrada_nome[sizeof(u->nome) + 1];

    if (fgets(entrada_nome, sizeof(entrada_nome), stdin) == NULL) {
        return -1;
    }

    // Verificar se a linha foi lida completamente
    if (strchr(entrada_nome, '\n') == NULL && !feof(stdin)) {
        int c = getchar();

        if (c != '\n' && c != EOF) {
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Nome muito longo!\n");
            return 1;
        }
    }

    normalizar_espacos(entrada_nome);

    if (strlen(entrada_nome) == 0 || strlen(entrada_nome) >= sizeof(u->nome)) {
        printf("Nome invalido!\n");
        return 1;
    }

    strcpy(u->nome, entrada_nome);

    int status = buscar_usuario(u->nome, u);
    if (status != 0) {
        return status;
    }

    printf("Senha: ");
    char entrada_senha[sizeof(u->senha) + 2];

    if (fgets(entrada_senha, sizeof(entrada_senha), stdin) == NULL) {
        return -1;
    }

    if (strchr(entrada_senha, '\n') == NULL && !feof(stdin)) {
        int c = getchar();

        if (c != '\n' && c != EOF) {
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Senha muito longa!\n");
            return 1;
        }
    }

    entrada_senha[strcspn(entrada_senha, "\n")] = '\0';

    if (strlen(entrada_senha) == 0 || strlen(entrada_senha) >= sizeof(u->senha)) {
        printf("Senha invalida!\n");
        return 1;
    }

    strcpy(u->senha, entrada_senha);

    status = verificar_senha(u->senha, u->senha_hash);

    if (status == 0) {
        printf("Bem-vindo, %s!!\n", u->nome);
    }

    return status;
}

int cadastro_usuario(int *entrada) {
    printf("/=============================/\n");
    printf("  // // AREA DE CADASTRO // // \n");
    printf("/=============================/\n");
    printf("Digite seu usuario: ");
    usuario u = {0}; // declarar
    int controle = validar_usuario(&u);
    if (!controle) {
        printf("Digite sua senha: ");
        controle = validar_senha(&u);
    }

    if(!controle) {
        controle = gerar_hash_senha(u.senha, u.senha_hash, sizeof(u.senha_hash));

        if (controle != 0) {
            printf("Nao foi possivel proteger a senha!\n");
            return 1;
        }

        controle = inserir_usuario(u.nome, u.senha_hash);

        if (controle != 0) {
            printf("Erro ao cadastrar usuario!\n");
            return 1;
        }
        
        printf("Cadastro realizado com sucesso!\n");
        int ok = 1;
        do {
            printf("Deseja entrar?[S/N]\n");
            char resposta;
            scanf(" %c", &resposta);
            getchar();
            if(toupper(resposta) == 'S') {
                ok = 0;
                *entrada = 0;
                return 0;
            } else if (toupper(resposta) == 'N') {
                ok = 0;
                printf("Voltando ao menu principal\n");
                return 0;
            } else {
                printf("Resposta inválida");
            }
        }while(ok);

    }

    return 0;
}

