#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cmath> 
#include <typeinfo>

#define tam 3000

using namespace std;

typedef enum { DIREITA, ESQUERDA, RAIZ } DIRECAO;

typedef struct rep {
    int repeticao;
    char word;
} Rep;

typedef struct no {
    struct no* dir, * esq;
    struct rep* word;
    DIRECAO direcao;
} No;

typedef struct lista {
    char caracter;
    char* path;
    struct lista* next;
} Lista;

int menu();
No* insertBeginNewRep(char word);
No* criarNo();
void updateRep(vector<No*>& R, int index);
Rep* criarRep();
void reordenarRep(vector<No*>& R);
void createNoOfTree(vector<No*>& nosVet);
void legendTree(Lista** l, string texto, No** r, FILE** arquivoSaidaCompactacao);
void createSinglyLinkedList(Lista** l, char word, string texto);
char* converter_int_bin(char caractere);

int main() {
    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #else
        system("clear");
    #endif

    int opc = menu();
    string outputLeg, auxPath, outputBin;
    char texto[tam], vet[9];
    Lista* l = NULL;
    int i, j;

    if (opc == 1) {
        FILE* arquivoSaidaCompactacao = fopen("out.txt", "w");
        FILE* arquivoEntradaCompactacao = fopen("input.txt", "r");

        int tamChar = fread(texto, sizeof(char), sizeof(texto), arquivoEntradaCompactacao);
        texto[tamChar] = '\0';
        vector<No*> nosRepeticoes;

        // Criar lista de nós/repetições para cada letra
        for (int i = 0; i < tamChar; i++) {
            char caracter = texto[i];
            size_t tamVec = nosRepeticoes.size();
            if (tamVec <= 0) nosRepeticoes.push_back(insertBeginNewRep(caracter));
            else {
                int auxRep = -1;
                for (int j = 0; j < tamVec; j++) if (nosRepeticoes[j]->word->word == caracter) auxRep = j;
                if (auxRep >= 0) updateRep(nosRepeticoes, auxRep);
                else nosRepeticoes.push_back(insertBeginNewRep(caracter));
            }
        }

        // Fazer arvore com todos as folhas
        while (nosRepeticoes.size() > 1) createNoOfTree(nosRepeticoes);

        // Cria a legenda da compactação e escreve ela no arquivo de saída.
        legendTree(&l, auxPath, &nosRepeticoes[0], &arquivoSaidaCompactacao);
        fprintf(arquivoSaidaCompactacao, "\a");

        for (i = 0; texto[i] != '\0'; i++) {
            for (Lista* p = l; p != NULL; p = p->next) {
                if (p->caracter == texto[i]) {
                    for (j = 0; p->path[j] != '\0'; j++) outputBin.push_back(p->path[j]);
                    break;
                }
            }
        }
        outputBin.push_back('\0');

        // Converte o binario em decimal e depois em char
        size_t tamOutputBin = outputBin.size();
        for (i = 0, j = 0; i < tamOutputBin - 1; i++) {
            vet[j] = outputBin[i];

            if (outputBin[i + 1] == '\0' && j < 7) {
                while (j < 7) {
                    vet[j] = '0';
                    j++;
                }
            }

            j++;

            if (j == 8) {
                vet[8] = '\0';
                j = 0;
                fprintf(arquivoSaidaCompactacao, "%c", (char)strtol(vet, NULL, 2));
            }
        }

        printf("Arquivo compactado, olhe o arquivo de saida (out.txt).\n");
        fclose(arquivoEntradaCompactacao);
        fclose(arquivoSaidaCompactacao);
    }
    else {
        FILE* arquivoEntradaDescompactacao = fopen("out.txt", "r");
        FILE* arquivoSaidaDescompactacao = fopen("input.txt", "w");

        int tamChar = fread(texto, sizeof(char), sizeof(texto), arquivoEntradaDescompactacao);
        texto[tamChar] = '\0';
        int aux = 0, indexWordString = 0, tamTextoCompactado = 0;
        char wordString[30];
        string textoCompactado;

        for (i = 0; i < tamChar; i++) {
            if (texto[i] == '-') {
                aux = 1;
            }
            else if (texto[i] == '\v') {
                aux = 0;
                wordString[indexWordString] = '\0';
                indexWordString = 0;
                int wordAsc = atoi(wordString);
                createSinglyLinkedList(&l, wordAsc, auxPath);
                auxPath.clear();
            }
            else if (texto[i] == '\a') {
                aux = 2;
            }
            else {
                if (!aux) {
                    auxPath.push_back(texto[i]);
                }
                else if (aux == 1) {
                    wordString[indexWordString++] = texto[i];
                }
                else if (aux == 2) {
                    textoCompactado.push_back(texto[i]);
                    tamTextoCompactado++;
                }
            }
        }
        // textoCompactado.push_back('\0');

        for (i = 0; i < tamTextoCompactado; i++) {
            long int ascII = textoCompactado[i];
            outputBin += converter_int_bin(ascII);
        }
        outputBin.push_back('\0');

        j = 0;
        char trad[tam];
        for (i = 0; outputBin[i] != '\0'; i++) {
            trad[j++] = outputBin[i];

            for (Lista* p = l; p != NULL; p = p->next) {
                trad[j] = '\0';
                if (!strcmp(trad, p->path)) {
                    fprintf(arquivoSaidaDescompactacao, "%c", p->caracter);
                    j = 0;
                    break;
                }
            }
        }

        printf("Arquivo descompactado, olhe o arquivo de entrada (input.txt).\n");
        fclose(arquivoEntradaDescompactacao);
        fclose(arquivoSaidaDescompactacao);
    }
}

void legendTree(Lista** l, string texto, No** r, FILE** arquivoSaidaCompactacao) {
    if (*r != NULL) {
        if ((*r)->direcao != RAIZ) {
            if ((*r)->direcao == DIREITA) texto += '0';
            else texto += '1';

            if ((*r)->esq != NULL || (*r)->dir != NULL) {
                legendTree(&(*l), texto, &(*r)->esq, &(*arquivoSaidaCompactacao));
                legendTree(&(*l), texto, &(*r)->dir, &(*arquivoSaidaCompactacao));
            }
            else {
                for (char elem : texto) {
                    fprintf(*arquivoSaidaCompactacao, "%c", elem);
                }
                fprintf(*arquivoSaidaCompactacao, "-%i%c", (*r)->word->word, 11);
                createSinglyLinkedList(&(*l), (*r)->word->word, texto);
            }
        }
        else {
            legendTree(&(*l), texto, &(*r)->esq, &(*arquivoSaidaCompactacao));
            legendTree(&(*l), texto, &(*r)->dir, &(*arquivoSaidaCompactacao));
        }
    }
}

void createSinglyLinkedList(Lista** l, char word, string texto) {
    Lista* novo = (Lista*)malloc(sizeof(Lista));
    novo->caracter = word;
    novo->path = (char*)malloc(texto.size() + 1);
    strcpy(novo->path, texto.c_str());
    novo->next = NULL;

    if (*l != NULL) {
        Lista* p;
        for (p = *l; p->next != NULL; p = p->next);
        p->next = novo;
    }
    else {
        *l = novo;
    }
}

char* converter_int_bin(char caractere) {
    char* binario = (char*)malloc((sizeof(char) * 8 + 1));

    memset(binario, 0, (sizeof(char) * 8 + 1));

    for (int i = 0; i < sizeof(char) * 8; ++i) {
        char bit = (caractere & (1 << i)) ? '1' : '0';
        binario[sizeof(char) * 8 - i - 1] = bit;
    }

    return binario;
}

void createNoOfTree(vector<No*>& nosVet) {
    // Pegando os dois primeiros nos do vector
    No* r1 = nosVet[0];
    No* r2 = nosVet[1];

    // Criando repeticao de r12
    Rep* rep = criarRep();
    rep->repeticao = (r1->word->repeticao + r2->word->repeticao);
    rep->word = '\0';

    // Criando raiz r12
    No* r12 = criarNo();
    r12->direcao = RAIZ;

    r12->dir = r1;
    r1->direcao = DIREITA;

    r12->esq = r2;
    r2->direcao = ESQUERDA;
    r12->word = rep;

    // Exclui as raizes que foram juntadas
    nosVet.erase(nosVet.begin());
    nosVet.erase(nosVet.begin());

    // Inclui o no raiz criado na lista de Nós
    nosVet.push_back(r12);

    // Reordena a Lista de repetições
    reordenarRep(nosVet);
}

void reordenarRep(vector<No*>& R) {
    size_t tamR = R.size();
    for (int i = 0; i < tamR - 1; i++)
        for (int j = i + 1; j < tamR; j++)
            if (R[i]->word->repeticao > R[j]->word->repeticao)
                swap(R[i], R[j]);
}

No* insertBeginNewRep(char word) {
    Rep* novoRep = criarRep();
    novoRep->word = word;

    No* novoNo = criarNo();
    novoNo->word = novoRep;

    return novoNo;
}

No* criarNo() {
    No* novoNo = (No*)malloc(sizeof(No));
    novoNo->dir = NULL;
    novoNo->esq = NULL;
    novoNo->word = NULL;

    return novoNo;
}

Rep* criarRep() {
    Rep* novoRep = (Rep*)malloc(sizeof(Rep));
    novoRep->repeticao = 1;
    novoRep->word = '\0';

    return novoRep;
}

void updateRep(vector<No*>& R, int index) {
    R[index]->word->repeticao++;
}

int menu() {
    int opc;
    cout << "[00] Sair\n[01] Compactar\n[02] Descompactar\n" << endl;
    cin >> opc;

    if (!opc) exit(0);
    return opc;
}
