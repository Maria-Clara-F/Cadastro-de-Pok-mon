#include <stdio.h>
#include "pokemon.h"
#include "database.h"
#include "funcoes.h"

int pokebox_usuario(usuario *treinador);

int main() {
    if (inicializar_database() != 0) {
        return 1;
    }
    int opcao;
    int entrada = 1;
    usuario treinador = {0};
    do {
        printf("/=============================/\n");
        printf("      // // POKEDEX // //      \n");
        printf("/=============================/\n");
        printf("[0] Sair\n[1] Cadastrar usuario\n[2] Entrada de usuario\n");
        printf("O que deseja? ");
        if (!ler_int(&opcao, 0, 2)) {
            printf("\nEntrada encerrada.\n");
            return 0;
        }

        switch(opcao) {
            case 0:
                printf("Programa finalizado!\n");
                break;
            case 1:
                entrada = 1;
                cadastro_usuario(&entrada);

                if (entrada == 0) {
                    treinador = (usuario){0};

                    if (entrada_usuario(&treinador) == 0) {
                        pokebox_usuario(&treinador);
                    }
                }
                break;
            case 2:
                treinador = (usuario){0};
                if (entrada_usuario(&treinador) == 0) {
                    pokebox_usuario(&treinador);
                }
                break;
        }
    }while(opcao!=0);
}

int pokebox_usuario(usuario *treinador) {
    int opcao;
    do {
        printf("/=============================/\n");
        printf("      BEM VINDO TREINADOR!     \n");
        printf("/=============================/\n");
        printf("Seu id: %d\n", treinador->id);
        printf("Seu usuario: %s\n", treinador->nome);

        printf("[0] Logout\n[1] Cadastrar Pokemon\n[2] Consultar Pokemon\n[3] Minha Pokedex\n[4] Arena de Batalha\n[5] Remover Pokemon\n[6] Estatisticas da Pokedex\n");
        printf("O que deseja? ");
        while(!ler_int(&opcao, 0, 6));
        printf("\n");

        switch(opcao) {
            case 0: 
                printf("Saindo da conta...\n");
                return 0;
            case 1:
                cadastrar();
                break;
            case 2: 
                consultar();
                break;
            case 3:
                pokebox();
                break;
            case 4:
                arena();
                break;
            case 5:
                remover();
                break;
            case 6:
                estatisticas();
                break;
        }

    } while (opcao!=0);

    return 0;
}