#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cmath> 
#include <typeinfo>

FILE* arquivoOut = fopen("out.txt", "w");
FILE* arquivoIn = fopen("input.txt", "r");

using namespace std;

typedef enum{DIREITA, ESQUERDA, RAIZ}DIRECAO;

typedef struct rep
{
    int repeticao;
    char word;
}Rep;

typedef struct no
{
    struct no *dir, *esq;
    struct rep *word;
    DIRECAO direcao;
}No;

typedef struct lista
{
    char caracter;
    char* path;
    struct lista* next;
}Lista;

int menu();
No* insertBeginNewRep(char word);
No* criarNo();
void updateRep(vector<No*>& R, int index);
Rep* criarRep();
void reordenarRep(vector<No*>& R);
void createNoOfTree(vector<No*>& nosVet);
void legendTree(Lista** l, string texto, No** r);

int main() {
    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #else
        system("clear");
    #endif

    int opc = menu();

    if(opc == 1)
    {
        char texto[3000], vet[9];
        int tamChar = fread(texto, sizeof(char), sizeof(texto), arquivoIn);
        texto[tamChar] = '\0';
        vector<No*> nosRepeticoes;
        string outputLeg, auxTexto, outputBin;
        int resto, restoBin, i, j;
        Lista* l = NULL;
    
        // Criar lista de nós/repetições para cada letra
        for(int i = 0; i < tamChar; i++)
        {
          char caracter = texto[i];
          size_t tamVec = nosRepeticoes.size();
          if(tamVec <= 0) nosRepeticoes.push_back(insertBeginNewRep(caracter));
          else
          {
            int auxRep = -1;
            for(int j = 0; j < tamVec; j++) if(nosRepeticoes[j]->word->word == caracter) auxRep = j;
            if(auxRep >= 0) updateRep(nosRepeticoes, auxRep);
            else nosRepeticoes.push_back(insertBeginNewRep(caracter));
          }
        }

        // Fazer arvore com todos as folhas
        while(nosRepeticoes.size() > 1) createNoOfTree(nosRepeticoes);
        
        // Cria a legenda da compactação e escreve ela no arquivo de saída.
        legendTree(&l, auxTexto, &nosRepeticoes[0]);
        fprintf(arquivoOut, "\a");

        for(i = 0; texto[i] != '\0'; i++)
        {
            for(Lista* p = l; p != NULL; p = p->next)
            {
                if(p->caracter == texto[i])
                {
                    for(j = 0; p->path[j] != '\0'; j++) outputBin.push_back(p->path[j]);
                    break;
                }
            }
        }
        outputBin.push_back('\0');

        // Converte o binario em decimal e depois em char
        size_t tamOutputBin = outputBin.size();
        for (i = 0, j = 0; i < tamOutputBin-1; i++)
        {
            vet[j] = outputBin[i];

            if (outputBin[i+1] == '\0' && j < 7)
            {   
                while (j < 7)
                {
                    vet[j] = '0';
                    j++;
                }
            }

            j++;

            if (j == 8)
            {
                vet[8] = '\0';
                j = 0;
                // printf("%c", (char)strtol(vet, NULL, 2));
                fprintf(arquivoOut, "%c", (char)strtol(vet, NULL, 2));
            }

        }

        printf("Arquivo compactado, olhe o arquivo de saida (out.txt).\n");
        fclose(arquivoIn);
        fclose(arquivoOut);
    }
    else {
       printf("oi\n"); 
    }
}

void legendTree(Lista** l, string texto, No** r) {
    if(*r != NULL)
    {
        if((*r)->direcao != RAIZ)
        {
            if((*r)->direcao == DIREITA) texto += '0';
            else texto += '1';

            if((*r)->esq != NULL || (*r)->dir != NULL)
            {
                legendTree(&(*l), texto, &(*r)->esq);
                legendTree(&(*l), texto, &(*r)->dir);
            }
            else
            {
                for(char elem : texto)
                {
                    fprintf(arquivoOut, "%c", elem);
                }
                fprintf(arquivoOut, "-%i%c", (*r)->word->word, 11);

                Lista* novo = (Lista*)malloc(sizeof(Lista));
                novo->caracter = (*r)->word->word;
                novo->path = (char*)malloc(texto.size()+1);
                int i;
                for (i = 0; i < sizeof(novo->path); i++)
                {
                    novo->path[i] = texto[i];
                }
                novo->path[i] = '\0';
                novo->next = NULL;

                if(*l != NULL)
                {
                    Lista* p;
                    for(p = *l; p->next != NULL; p = p->next);
                    p->next = novo;
                }
                else
                {
                    *l = novo;
                }
            }
        }
        else 
        {
            legendTree(&(*l), texto, &(*r)->esq);
            legendTree(&(*l), texto, &(*r)->dir);
        }
    }
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

    // Exclui as raizes  que foram juntadas
    nosVet.erase(nosVet.begin());
    nosVet.erase(nosVet.begin());

    // Inclui o no raiz criado na lista de Nós
    nosVet.push_back(r12);

    // Reordena a Lista de repetições
    reordenarRep(nosVet);
}

void reordenarRep(vector<No*>& R) {
    size_t tamR = R.size();
    for(int i = 0; i < tamR-1; i++)
        for(int j = i+1; j < tamR; j++)
            if(R[i]->word->repeticao > R[j]->word->repeticao)
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

    if(!opc) exit(0);
    return opc;
}