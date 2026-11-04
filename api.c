#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include "pokemon.h"
#include "database.h"


typedef struct {
    char *dados; // ponteiro pq nao sei o tamanho do json -> usar alocacao de memoria dinamica
    size_t tamanho; // quanrtidade de bytes recebidos ate o momento
} RespostaAPI;

size_t receber_dados(char *ptr, size_t size, size_t nmemb, void *userdata) {

    size_t bytes = size * nmemb; // tamanho da unidade * quantidade de unidades. Quem passa esses parametros eh a propria libcurl

    RespostaAPI *resposta = userdata; // ponteiro para RespostaAPI

    char *novo = realloc(resposta->dados, resposta->tamanho + bytes + 1);
    /* - resposta->dados: região de memória atual.
       - resposta->tamanho: quantidade de bytes que já recebeu.
       - bytes: quantidade que acabou de chegar.
       - + 1: espaço reservado para o caractere '\0'
       Como dados começa com NULL, a primeira chamada de realloc() funciona como uma alocação inicial.
       Usa a variável temporária novo pq, se realloc() falhar, o ponteiro original continuará válido.
    */

    if (novo == NULL) {
        return 0;
    }

    resposta->dados = novo; //passando o tamanho novo

    memcpy(resposta->dados + resposta->tamanho, ptr, bytes); //Copia os bytes recebidos para a memória que reservada
    //indicando que a cópia deve começar depois dos dados que já foram armazenados, sem sobrescrevê-los.
    resposta->tamanho += bytes;
    resposta->dados[resposta->tamanho] = '\0';

    return bytes;
}


int buscar_pokemon_api(int numero) {
    RespostaAPI resposta = {0}; // inicializa dados com NULL e tamanho 0
    pokemon p = {0}; // comecar com os atributos zerados


    // ponteiro especial da biblioteca
    CURL *curl = curl_easy_init(); // cria um identificador que eh feito para executar a requisicao
    if (curl == NULL) {
    printf("Erro ao inicializar CURL.\n");
    return 1;
    }

    char url[100]; // acessar os pokemons

    snprintf(url, sizeof(url), "https://pokeapi.co/api/v2/pokemon/%d/", numero);

    // Configura a URL que acabamos de montar
    curl_easy_setopt(curl, CURLOPT_URL, url);
    
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, receber_dados); //chama a funcao que recebe os dados
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resposta);//endereco da struct para essa funcao
    
    
    CURLcode resultado = curl_easy_perform(curl); // executa
    if(resultado != CURLE_OK) { // CURLE_OK indica que a transferência ocorreu sem erro da libcurl, mas não garante que o servidor respondeu HTTP 200
        printf("Erro: %s\n", curl_easy_strerror(resultado));
        curl_easy_cleanup(curl);
        free(resposta.dados); // liberando memoria
        return 1;
    }

    long http_code = 0;

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code); //verifca a resposta

    if (http_code != 200) {
        printf("Erro HTTP: %ld\n", http_code);
        free(resposta.dados);
        curl_easy_cleanup(curl);
        return 1;
    }

    // busca com o json
    cJSON *json = cJSON_Parse(resposta.dados); // cJSON_Parse() interpreta a string e cria uma estrutura de dados para consulta

    if (json == NULL) {
        printf("Erro ao interpretar JSON.\n");
        free(resposta.dados);
        curl_easy_cleanup(curl);
        return 1;
    }

    // pegar o nome, id, altura e peso
    cJSON *nome = cJSON_GetObjectItemCaseSensitive(json, "name");
    cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
    cJSON *altura = cJSON_GetObjectItemCaseSensitive(json, "height");
    cJSON *peso = cJSON_GetObjectItemCaseSensitive(json, "weight");

    // verifica o nome
    if (cJSON_IsString(nome) && nome->valuestring != NULL) {
        snprintf(p.name, sizeof(p.name), "%s", nome->valuestring);
        printf("Pokemon encontrado: %s\n", p.name);
    } else {
        printf("Nome do Pokemon nao encontrado.\n");
    }

    // verifica o numero da Pokedex
    if (cJSON_IsNumber(id)) {
        p.pokedex_number = id->valueint;
        printf("Numero: %d\n", p.pokedex_number);
        if (p.pokedex_number >= 1 && p.pokedex_number <= 151) {
            p.generation = 1;
        }
        printf("Geracao: %d\n", p.generation); // por enquanto serao apenas os pokemons da 1 geracao
    } else {
        printf("Numero do Pokemon nao encontrado.\n");
    }

    // verifica a altura
    if (cJSON_IsNumber(altura)) {
        p.height = (float)altura->valuedouble / 10.0f;
        printf("Altura: %.2f m\n", p.height);
    } else {
        printf("Altura do Pokemon nao encontrada.\n");
    }

    // verifica o peso
    if (cJSON_IsNumber(peso)) {
        p.weight = (float)peso->valuedouble / 10.0f;
        printf("Peso: %.2f kg\n", p.weight);
    } else {
        printf("Peso do Pokemon nao encontrado.\n");
    }

    // tipos

    cJSON *tipos = cJSON_GetObjectItemCaseSensitive(json, "types");

    if (cJSON_IsArray(tipos)) {
        int quantidade_tipos = cJSON_GetArraySize(tipos);
        printf("Quantidade de tipos: %d\n", quantidade_tipos);
        for (int i = 0; i < quantidade_tipos; i++) {
            cJSON *elemento = cJSON_GetArrayItem(tipos, i);
            cJSON *tipo = cJSON_GetObjectItemCaseSensitive(elemento, "type");
            cJSON *nome_tipo = cJSON_GetObjectItemCaseSensitive(tipo, "name");
            cJSON *slot = cJSON_GetObjectItemCaseSensitive(elemento, "slot");
            if(cJSON_IsString(nome_tipo) && nome_tipo->valuestring != NULL && cJSON_IsNumber(slot)) {
                printf("Tipo: %s\n", nome_tipo->valuestring);
                if (slot->valueint == 1) {
                    snprintf(p.type1, sizeof(p.type1), "%s", nome_tipo->valuestring);
                } else if (slot->valueint == 2) {
                    snprintf(p.type2, sizeof(p.type2), "%s", nome_tipo->valuestring);
                }
            }
        }

        printf("\nTipos armazenados:\n");
        printf("Tipo 1: %s\n", p.type1);

        if (p.type2[0] != '\0') {
            printf("Tipo 2: %s\n", p.type2);
        }
    } else {
        printf("Tipos do Pokemon nao encontrados.\n");
    }




    // acessa o stats
    cJSON *stats = cJSON_GetObjectItemCaseSensitive(json, "stats");

    if (!cJSON_IsArray(stats)) {
        printf("Erro: stats nao eh um array.\n");

        cJSON_Delete(json);
        free(resposta.dados);
        curl_easy_cleanup(curl);
        return 1;
    } else {
        int quantidade = cJSON_GetArraySize(stats); // quantidade esperada: 6
        printf("Quantidade de atributos: %d\n", quantidade);

        // atributos do stat
        int atributos_encontrados = 0;
        for(int i = 0; i < quantidade; i++) {

            cJSON *elemento = cJSON_GetArrayItem(stats, i);
            cJSON *stat = cJSON_GetObjectItemCaseSensitive(elemento, "stat"); // abre o elemento

            cJSON *valor = cJSON_GetObjectItemCaseSensitive(elemento, "base_stat");
            cJSON *nome_stat = cJSON_GetObjectItemCaseSensitive(stat, "name"); // pega o nome do elemento

            if (cJSON_IsNumber(valor) && cJSON_IsString(nome_stat) && nome_stat->valuestring != NULL) {
                printf("%s: %d\n", nome_stat->valuestring, valor->valueint);

                // copiando os dados para a struct
                if (strcmp(nome_stat->valuestring, "hp") == 0) {
                    p.stats.hp = valor->valueint;
                    atributos_encontrados++;
                } else if (strcmp(nome_stat->valuestring, "attack")== 0) {
                    p.stats.attack = valor->valueint;
                    atributos_encontrados++;
                } else if (strcmp(nome_stat->valuestring, "defense")==0) {
                    p.stats.defense = valor->valueint;
                    atributos_encontrados++;
                } else if (strcmp(nome_stat->valuestring, "special-attack")==0) {
                    p.stats.sp_attack = valor->valueint;
                    atributos_encontrados++;
                } else if (strcmp(nome_stat->valuestring, "special-defense")==0) {
                    p.stats.sp_defense = valor->valueint;
                    atributos_encontrados++;
                } else if (strcmp(nome_stat->valuestring, "speed") == 0) {
                    p.stats.speed = valor->valueint;
                    atributos_encontrados++;
                }
            }
        }

        if (atributos_encontrados != 6) {
            printf("Erro: atributos incompletos do Pokemon.\n");

            cJSON_Delete(json);
            free(resposta.dados);
            curl_easy_cleanup(curl);

            return 1;
        }

        // imprimindo
        printf("\nAtributos armazenados:\n");
        printf("HP: %d\n", p.stats.hp);
        printf("Ataque: %d\n", p.stats.attack);
        printf("Defesa: %d\n", p.stats.defense);
        printf("Ataque especial: %d\n", p.stats.sp_attack);
        printf("Defesa especial: %d\n", p.stats.sp_defense);
        printf("Velocidade: %d\n", p.stats.speed);
        
    }

    if (p.pokedex_number <= 0 || p.name[0] == '\0' || p.type1[0] == '\0' || p.height <= 0 || p.weight <= 0 ||p.generation <= 0) {
        printf("Erro: dados incompletos do Pokemon.\n");
        cJSON_Delete(json);
        free(resposta.dados);
        curl_easy_cleanup(curl);
        return 1;
    }

    if (inserir_pokemon(&p) != 0) {
        printf("Erro ao salvar Pokemon no banco de dados.\n");

        cJSON_Delete(json);
        free(resposta.dados);
        curl_easy_cleanup(curl);

        return 1;
    }

    printf("Pokemon salvo no banco de dados!\n"); // tirar dps

    cJSON_Delete(json); // libera a memoria do json
    free(resposta.dados); // liberando memoria
    curl_easy_cleanup(curl);
    return 0;
}

int importar_primeira_geracao(void) {
    int sucessos = 0;
    int falhas = 0;

    for (int numero = 1; numero <= 151; numero++) {
        printf("\nImportando Pokemon #%d...\n", numero);

        if (buscar_pokemon_api(numero) == 0) {
            sucessos++;
        } else {
            falhas++;
        }
    }

    printf("\nImportacao finalizada!\n");
    printf("Sucessos: %d\n", sucessos);
    printf("Falhas: %d\n", falhas);

    return falhas == 0 ? 0 : 1; // se falhas == 0, return 0, senao 1

}