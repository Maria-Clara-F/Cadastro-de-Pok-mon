#ifndef POKEMON_H
#define POKEMON_H
#define MAX_CARACTERE 50

typedef struct
{
    int hp;
    int attack;
    int defense;
    int sp_attack;
    int sp_defense;
    int speed;
    
}Stats;

typedef struct
{
    int id;
    int pokedex_number;
    char name[64]; 
    char type1[20];
    char type2[20];
    int generation;
    float height;
    float weight;
    Stats stats;

}pokemon;

typedef struct
{
    int id;
    char nome[MAX_CARACTERE];
    char senha[25];
    char senha_hash[256];

}usuario;



#endif