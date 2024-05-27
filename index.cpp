#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cmath> 

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

int menu();
No* insertBeginNewRep(char word);
No* criarNo();
void updateRep(vector<No*>& R, int index);
Rep* criarRep();
void reordenarRep(vector<No*>& R);
void createNoOfTree(vector<No*>& nosVet);
void legendTree(string* outBin, string texto, No** r);

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
        vector<No*> nosRepeticoes;
        string outputBin, auxTexto;
        int resto, restoBin, i, j;
    
        //Criar lista de nós/repetições para cada letra
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

        //Fazer arvore com todos as folhas
        while(nosRepeticoes.size() > 1) createNoOfTree(nosRepeticoes);
        
        //Cria a legenda da compactação
        legendTree(&outputBin, auxTexto, &nosRepeticoes[0]);
        fprintf(arquivoOut, "\a");

        //Inteira com zeros a saida binaria, até ser multiplo de oito
        resto = outputBin.size()/8;
        if(resto) restoBin = abs(outputBin.size()-(resto*8));
        else restoBin = outputBin.size()-8;
        char outputBinario[outputBin.size()+1+abs(restoBin)];
        
        for(i = 0; i < outputBin.size(); i++) outputBinario[i] = outputBin[i];
        for(i; i < sizeof(outputBinario); i++) outputBinario[i] = '0';
        outputBinario[sizeof(outputBinario)-1] = '\0';

        //ERRO: VOCE ESTA PEGANDO O CODIGO BINARIO DE ACORDO COM A ORDEM DA LEGENDA, MAS VOCE TEM QUE TRADUZIR O TEXTO COM A LEGENDA E ASSIM VAI TER O CODIGO BINARIO E DEPOIS VOCE USA A FUNCAO ABAIXO

        //Transforma a saída binaria em letras
        for (i = 0, j = 0; outputBinario[i] != '\0'; i++)
        {
            vet[j] = outputBinario[i];
            j++;
            if (j == 8)
            {
                vet[j] = '\0';
                j = 0;
                cout << strtol(vet, NULL, 2) << endl;
                fprintf(arquivoOut, "%c", (char)strtol(vet, NULL, 2));
            }
        }

        fclose(arquivoIn);
        fclose(arquivoOut);
    }
}

void legendTree(string* outBin, string texto, No** r) {
    if(*r != NULL)
    {
        if((*r)->direcao != RAIZ)
        {
            if((*r)->direcao == DIREITA) texto += '0';
            else texto += '1';

            if((*r)->esq != NULL || (*r)->dir != NULL)
            {
                legendTree(outBin, texto, &(*r)->esq);
                legendTree(outBin, texto, &(*r)->dir);
            }
            else
            {
                for(char elem : texto)
                {
                    fprintf(arquivoOut, "%c", elem);
                }
                fprintf(arquivoOut, "-%i%c", (*r)->word->word, 11);

                *outBin += texto;
            }
        }
        else 
        {
            legendTree(outBin, texto, &(*r)->esq);
            legendTree(outBin, texto, &(*r)->dir);
        }
    }
}

void createNoOfTree(vector<No*>& nosVet) {
    //Pegando os dois primeiros nos do vector
    No* r1 = nosVet[0];
    No* r2 = nosVet[1];

    //Criando repeticao de r12
    Rep* rep = criarRep();
    rep->repeticao = (r1->word->repeticao + r2->word->repeticao);
    rep->word = '\0'; 

    //Criando raiz r12
    No* r12 = criarNo();
    r12->direcao = RAIZ;

    r12->dir = r1;
    r1->direcao = DIREITA;

    r12->esq = r2;
    r2->direcao = ESQUERDA;
    r12->word = rep;

    //Exclui as raizes  que foram juntadas
    nosVet.erase(nosVet.begin());
    nosVet.erase(nosVet.begin());

    //Inclui o no raiz criado na lista de Nós
    nosVet.push_back(r12);

    //Reordena a Lista de repetições
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
    cout << "Voce quer codificar[01] ou decodificar[02] um teto?" << endl;
    cin >> opc;
    return opc;
}