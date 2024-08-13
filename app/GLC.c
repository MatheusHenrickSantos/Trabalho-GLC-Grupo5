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

//Funções para remover recursões
void removeIndirectRecursion(ParsingTable table[], int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < i; j++) {
            if (table[i].production[0] == table[j].nonTerminal) {
                char newProductions[MAX][MAX];
                int newCount = 0;
                for (int k = 0; k < size; k++) {
                    if (table[k].nonTerminal == table[j].nonTerminal) {
                        char newProduction[MAX];
                        strcpy(newProduction, table[k].production);
                        strcat(newProduction, table[i].production + 1);
                        strcpy(newProductions[newCount++], newProduction);
                    }
                }
                for (int k = 0; k < newCount; k++) {
                    strcpy(table[size++].production, newProductions[k]);
                }
            }
        }
    }
}

void removeLeftRecursion(ParsingTable table[], int size) {
    for (int i = 0; i < size; i++) {
        char alpha[MAX][MAX], beta[MAX][MAX];
        int alphaCount = 0, betaCount = 0;

        for (int j = 0; j < size; j++) {
            if (table[j].nonTerminal == table[i].nonTerminal) {
                if (table[j].production[0] == table[i].nonTerminal) {
                    strcpy(alpha[alphaCount++], table[j].production + 1);
                } else {
                    strcpy(beta[betaCount++], table[j].production);
                }
            }
        }

        if (alphaCount > 0) {
            char newNonTerminal = table[i].nonTerminal + 1;
            printf("%c -> ", table[i].nonTerminal);
            for (int j = 0; j < betaCount; j++) {
                printf("%s%c'", beta[j], newNonTerminal);
                if (j < betaCount - 1) printf(" | ");
            }
            printf("\n");

            printf("%c' -> ", newNonTerminal);
            for (int j = 0; j < alphaCount; j++) {
                printf("%s%c' | ", alpha[j], newNonTerminal);
            }
            printf("ε\n");
        }
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
    int size = sizeof(table) / sizeof(table[0]);
    
    char continua = 's';
    while (continua == 's') {
        char aux = 's';
        printf("Deseja utilizar a última tabela? (s/n)\n");
        scanf("%c", &aux);

        if (aux=='n') {
            preencheTabela(table, size);
            size = sizeof(table) / sizeof(table[0]);
        }

        removeIndirectRecursion(table, size);
        removeLeftRecursion(table, size);
        
        LL1Parser(table, size);

        printf("\nDeseja analisar outra entrada? (s/n)\n");
        scanf("%c", &continua);
    }
    
    return 0;
}
