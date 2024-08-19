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
void preencheTabela(ParsingTable *table, int *size) {
    char nonTerminal;
    char production[MAX];

    for (int i = 0; i < *size; i++) {
        scanf(" %c", &nonTerminal);
        table[i].nonTerminal = nonTerminal;

        scanf("%s", production);
        strcpy(table[i].production, production);
    }
}

//Funções para remover recursões
void printProductions(ParsingTable *table, int size) {
    for (int i = 0; i < size; i++) {
        printf("%c -> %s\n", table[i].nonTerminal, table[i].production);
    }
}

void inverterString(char *str) {
    int i, j;
    char temp;
    int len = strlen(str);

    for (i = 0, j = len - 1; i < j; i++, j--) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

void removeDirectRecursion(ParsingTable *table, int *size) {
    //Essa função vai criar uma nova produção ε e inverter a produção de todo não-terminal que tiver uma recursão direta 
    for (int i = 0; i < *size; i++) {
        if (table[i].nonTerminal == table[i].production[0]) {
            ParsingTable currentNonTerminal;
            currentNonTerminal.nonTerminal = table[i].nonTerminal;
            strcpy(currentNonTerminal.production, "_");
            inverterString(table[i].production);
            printf("%c\t%s\t%s\n", currentNonTerminal.nonTerminal,currentNonTerminal.production,table[i].production);
            table[*size]=currentNonTerminal;
            printf("%c\t%s", table[*size].nonTerminal, table[i].production);
            (*size)++;
        }
    }
}

// Função para buscar o índice na tabela onde a produção começa com um caractere específico
int busca_indice_comeco(ParsingTable tabela[], int tamanho, char inicio) {
    for (int i = 0; i < tamanho; i++) {
        if (tabela[i].production[0] == inicio) {
            return i;
        }
    }
    return -1; // Se não encontrar, retorna -1
}

void sortProductions(ParsingTable *productions, int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            if (productions[i].nonTerminal > productions[j].nonTerminal) {
                ParsingTable temp = productions[i];
                productions[i] = productions[j];
                productions[j] = temp;
            }
        }
    }
}

void removeIndirectRecursion(ParsingTable *table, int *size) {
    ParsingTable auxTable[MAX];
    int tamanho = *size, isRecursive=0;
    int NonTermComMaisDeUmaProducao[MAX] = {0};
    memcpy(auxTable, table, (tamanho) * sizeof(ParsingTable));
    
    //Verifica se possui recursão indireta
    for (int i = 0; i < *size; i++) {
        for (int j = 0; j < i; j++) {
            if (table[i].production[0] == table[j].nonTerminal) {
                isRecursive = 1;
            }
        }
    }

    //Subtitui os não-terminais nas produções que estão na recursão
    if (isRecursive==1) {
        for (int i = 0; i < tamanho; i++) {
            char termo_atual = auxTable[i].nonTerminal;
            int indice_substituicao = busca_indice_comeco(auxTable, tamanho, termo_atual);
            NonTermComMaisDeUmaProducao[indice_substituicao] = 1;
            if (indice_substituicao != -1) {
                auxTable[i].nonTerminal = table[indice_substituicao].nonTerminal;
                strcpy(auxTable[i].production, table[i].production);
                strcat(auxTable[i].production, table[indice_substituicao].production + 1);
            }
        }
    }
    sortProductions(auxTable, *size);
    memcpy(table, auxTable, *size * sizeof(ParsingTable));
}

// Função para encontrar a produção na tabela de análise
char* findProduction(ParsingTable *table, int size, char nonTerminal, char terminal) {
    char *production = "";
    //Percorre a tabela procurando uma produção que corresponda ao não-terminal e ao terminal fornecidos
    for (int i = 0; i < size; i++) {
        if (table[i].nonTerminal == nonTerminal && table[i].production[0] == terminal) {
            return table[i].production;
        }
    }
    //Se não encontrou, significa que a não existe uma produção que corresponda ao não-terminal e ao símbolo atual da entrada
    for (int i = 0; i < size; i++) {
        //Para cada produção que o não terminal seja igual ao topo
        if (table[i].nonTerminal == nonTerminal) {
            //Se a produção está vazia, recebe a produção atual
            if (strcmp(production,"")==0) {
                production = table[i].production;
            } else {
                //Se não, dá preferencia pelo ε
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

        //Se o topo e o símbolo atual da entrada forem iguais, remove o topo da pilha e avança na entrada
        if (top == current) {
            pop(&stack);
            printStack(&stack);
            i++;
        } else {
            if (top >= 'A' && top <= 'Z') {
                char* production = findProduction(table, size, top, current);
                printf("Producao: %c -> %s\n",top, production);
                if (production) {
                    //Se encontrou a produção remove o não-terminal do topo da pilha
                    pop(&stack);
                    for (int j = strlen(production) - 1; j >= 0; j--) {
                        if (production[j] != EPSILON) {
                            // e empilha em ordem inversa, se não for ε
                            push(&stack, production[j]);
                        }
                    }
                    printStack(&stack);
                }
            } else {
                printf("Erro de sintaxe\n");
                return 1;
            }
        }
    }
    printf("Entrada aceita\n");
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
    printProductions(table, size);
    
    char continua = 's';
    while (continua == 's') {
        char aux = 's';
        printf("Deseja utilizar a ultima tabela? (s/n)\n");
        scanf(" %c", &aux);

        if (aux=='n') {
            printf("Quantas producoes tem a sua gramatica?\n");
            scanf("%d", &size);
            printf("Escreva as producoes:\n");
            preencheTabela(table, &size);
            removeIndirectRecursion(table, &size);
            removeDirectRecursion(table, &size);

            printf("\nSua nova tabela de analise:\n");
            printProductions(table, size);
        }
        
        LL1Parser(table, size);

        printf("\nDeseja analisar outra entrada? (s/n)\n");
        scanf(" %c", &continua);
    }
    
    return 0;
}