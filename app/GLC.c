#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

// Estrutura para a tabela de análise
typedef struct {
    char nonTerminal;
    char production[MAX];
} ParsingTable;

// Funções para manipulação da pilha
typedef struct {
    char items[MAX];
    int top;
} Stack;

void push(Stack *s, char item) {
    s->items[++(s->top)] = item;
}

char pop(Stack *s) {
    return s->items[(s->top)--];
}

char peek(Stack *s) {
    return s->items[s->top];
}

int isEmpty(Stack *s) {
    return s->top == -1;
}

//Função para atualizar a tabela
void preencheTabela(ParsingTable *table, int size) {
    char nonTerminal;
    char production[MAX];

    for (int i = 0; i < size; i++) {
        scanf("%c", &nonTerminal);
        table[i].nonTerminal = nonTerminal;

        scanf("%s", production);
        strcpy(table[i].production, production);
    }
}

// Função para encontrar a produção na tabela de análise
char* findProduction(ParsingTable table[], int size, char nonTerminal, char terminal) {
    for (int i = 0; i < size; i++) {
        if (table[i].nonTerminal == nonTerminal && table[i].production[0] == terminal) {
            return table[i].production;
        }
    }
    return NULL;
}

int LL1Parser(ParsingTable table[], int size) {
    char input[MAX];
    printf("Digite a entrada a ser analisada (termine com $): ");
    scanf("%s", input); //id+id*id$

    Stack stack;
    stack.top = -1;

    push(&stack, '$');
    push(&stack, 'E');

    int i = 0;
    while (!isEmpty(&stack)) {
        char top = peek(&stack);
        char current = input[i];

        if (top == current) {
            pop(&stack);
            i++;
        } else if (top >= 'A' && top <= 'Z') {
            char* production = findProduction(table, size, top, current);
            if (production) {
                pop(&stack);
                for (int j = strlen(production) - 1; j >= 0; j--) {
                    if (production[j] != 'ε') {
                        push(&stack, production[j]);
                    }
                }
            } else {
                printf("Erro de sintaxe\n");
                return 1;
            }
        } else {
            printf("Erro de sintaxe\n");
            return 1;
        }
    }

    printf("Entrada aceita\n");
}

int main() {
    // Exemplo de tabela de análise
    ParsingTable table[] = {
        {'E', "TX"},
        {'X', "+TX"},
        {'X', "ε"},
        {'T', "FY"},
        {'Y', "*FY"},
        {'Y', "ε"},
        {'F', "(E)"},
        {'F', "id"}
    };
    
    char continua = 's';
    while (continua == 's') {
        char aux = 's';
        printf("Deseja utilizar a última tabela? (s/n)\n");
        scanf("%c", &aux);

        if (aux=='n') {
            preencheTabela(table, sizeof(table)/sizeof(table[0]));
        }
        
        LL1Parser(table, sizeof(table)/sizeof(table[0]));

        printf("\nDeseja analisar outra entrada? (s/n)\n");
        scanf("%c", &continua);
    }
    
    return 0;
}
