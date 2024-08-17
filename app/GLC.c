#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define EPSILON '_'

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

void printStack(Stack *stack) {
    if (stack->top == -1) {
        printf("Pilha vazia\n");
        return;
    }

    printf("Conteudo da pilha: ");
    for (int i = 0; i <= stack->top; i++) {
        printf("%c ", stack->items[i]);
    }
    printf("\n\n");
}

//Função para atualizar a tabela
void preencheTabela(ParsingTable *table, int size) {
    char nonTerminal;
    char production[MAX];

    for (int i = 0; i < size; i++) {
        scanf(" %c", &nonTerminal);
        table[i].nonTerminal = nonTerminal;

        scanf("%s", production);
        strcpy(table[i].production, production);
    }
}

//Funções para remover recursões
void removeIndirectRecursion(ParsingTable *table, int size) {
    int a;
    for (int i = 0; i < size; i++) {
        printf("I=%d\n", i);
        for (int j = 0; j < i; j++) {
            printf("J=%d\n", j);
            scanf("%d", &a);
            if (table[i].production[0] == table[j].nonTerminal) {
                char newProductions[MAX][MAX];
                int newCount = 0;
                for (int k = 0; k < size; k++) {
                    printf("K=%d\n", k);
                    if (table[k].nonTerminal == table[j].nonTerminal) {
                        char newProduction[MAX];
                        strcpy(newProduction, table[k].production);
                        strcat(newProduction, table[i].production + 1);
                        strcpy(newProductions[newCount++], newProduction);
                        //strcpy(table[j].production, newProduction);
                        printf("%s\n", table[newCount].production);
                    }
                }
                for (int k = 0; k < newCount; k++) {
                    strcpy(table[size++].production, newProductions[k]);
                }
            }
        }
    }
}

void removeLeftRecursion(ParsingTable *table, int size) {
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
char* findProduction(ParsingTable *table, int size, char nonTerminal, char terminal) {
    char *production = "";
    for (int i = 0; i < size; i++) {
        //printf("%c = %c\t", table[i].nonTerminal, nonTerminal);
        //printf("%c = %c\n", table[i].production[0], terminal);
        if (table[i].nonTerminal == nonTerminal && table[i].production[0] == terminal) {
            return table[i].production;
        }
    }
    for (int i = 0; i < size; i++) {
        if (table[i].nonTerminal == nonTerminal) {
            //printf("p: |%s|\n", production);
            if (strcmp(production,"")==0) {
                production = table[i].production;
            } else {
                if (production[0] != EPSILON) {
                    production = "_";
                }
            }
        }
    }
    return production;
}

int LL1Parser(ParsingTable *table, int size) {
    char input[MAX];
    printf("Digite a entrada a ser analisada: ");
    scanf("%s", input); 
    strcat(input, "$");

    Stack stack;
    stack.top = -1;

    push(&stack, '$');
    push(&stack, table[0].nonTerminal);

    int i = 0, pausa;
    while (!isEmpty(&stack)) {
        char top = peek(&stack);
        char current = input[i];
        if (top == current) {
            pop(&stack);
            printStack(&stack);
            //scanf("%d", &pausa);
            i++;
        } else {
            if (top >= 'A' && top <= 'Z') {
                char* production = findProduction(table, size, top, current);
                printf("Producao: %c -> %s\n",top, production);
                if (production) {
                    pop(&stack);
                    for (int j = strlen(production) - 1; j >= 0; j--) {
                        if (production[j] != EPSILON) {
                            push(&stack, production[j]);
                        }
                    }
                    printStack(&stack);
                    //scanf("%d", &pausa);
                }
            } else {
                printf("Erro de sintaxe\n");
                return 1;
            }
        }
        //scanf("%d", &pausa);

    }

    printf("Entrada aceita\n");
    //scanf("%d", &pausa);

    return 0;
}

int main() {
    // Exemplo de tabela de análise
    ParsingTable table[] = {
        {'E', "TX"},
        {'X', "+TX"},
        {'X', EPSILON},
        {'T', "FY"},
        {'Y', "*FY"},
        {'Y', EPSILON},
        {'F', "(E)"},
        {'F', "a"}
    };
    int size = sizeof(table) / sizeof(table[0]);
    
    printf("Tabela de Analise\n");
    for (int i = 0; i < size; i++) {
        printf(" %c -> %s\n", table[i].nonTerminal, table[i].production);
    }
    printf("\n");
    
    char continua = 's';
    while (continua == 's') {
        char aux = 's';
        /*printf("Deseja utilizar a ultima tabela? (s/n)\n");
        scanf("%c", &aux);

        if (aux=='n') {
            printf("Quantas producoes tem a sua gramatica?\n");
            scanf("%d", &size);
            printf("Escreva as producoes:\n");
            preencheTabela(table, size);
            removeIndirectRecursion(table, size);
            //removeLeftRecursion(table, size);
            printf("%c -> %s\n", table[2].nonTerminal, table[2].production);
        }*/
        
        LL1Parser(table, size);

        printf("\nDeseja analisar outra entrada? (s/n)\n");
        scanf(" %c", &continua);
    }
    
    return 0;
}
/*
A Ba
B Cd
C Af
*/