#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <cstdio>
#include <climits>

using namespace std;

const string PREFita = "fita";
const string EXTFita = ".txt";

void criarFitas (int numFitas){
    
    for (int i = 0; i < numFitas; i++){
        ofstream fita(PREFita + to_string(i) + EXTFita);
        fita.close();
    }
}

void limparFitas(int numFitas){
    
    for (int i = 0; i < numFitas; i++){
        ofstream fita(PREFita + to_string(i) + EXTFita, ios::trunc);
        fita.close();
    }
}

void apagarFitas(int numFitas){
    for (int i = 0; i < numFitas; i++){
        remove((PREFita + to_string(i) + EXTFita).c_str());
    }
}

void salvarOrdenado(string nomeArquivo, string fitaFinal){
    ifstream in(fitaFinal);
    ofstream out(nomeArquivo, ios::trunc);
    
    int num;
    
    while (in >> num){
        out << num << endl;
    }
    
    in.close();
    out.close();
}

void ordenacaoFitas(string nomeArquivo, int capacidadeMemoria, int numFitas){
   
    ifstream entrada(nomeArquivo);
    if (!entrada.is_open()) {
        cout << "Erro: nao foi possivel abrir o arquivo '" << nomeArquivo << "'." << endl;
        return;
    }
    
    entrada.clear();
    entrada.seekg(0, ios::beg);
    
    int numFitasEntrada = numFitas / 2;
    int numFitasSaida = numFitas - numFitasEntrada;

    criarFitas(numFitas);  // cria os arquivos de fita
    
    // etapa 1: ler o arquivo original
    int* memoria = new int[capacidadeMemoria];  // ponteiro para simular a memória
    int count = 0, fitaAtual = 0;
    string nomeFita;
    
    // Etapa 2: Ler blocos do arquivo e salvar ordenado nas fitas de entrada
    int numero;
    while (entrada >> numero) {
        *(memoria + count) = numero;
        count++;

        if (count == capacidadeMemoria) {
            // Ordenação usando bubble sort (com ponteiros)
            for (int i = 0; i < capacidadeMemoria - 1; i++) {
                for (int j = 0; j < capacidadeMemoria - i - 1; j++) {
                    if (*(memoria + j) > *(memoria + j + 1)) {
                        int temp = *(memoria + j);
                        *(memoria + j) = *(memoria + j + 1);
                        *(memoria + j + 1) = temp;
                    }
                }
            }

            nomeFita = PREFita + to_string(fitaAtual % numFitasEntrada) + EXTFita;
            ofstream fita(nomeFita, ios::app);
            for (int i = 0; i < capacidadeMemoria; i++) {
                fita << *(memoria + i) << endl;
            }
            fita.close();
            count = 0;
            fitaAtual++;
        }
    }
        // Escreve o que restou na memória (se não completou o último bloco)
    if (count > 0) {
        for (int i = 0; i < count - 1; i++) {
            for (int j = 0; j < count - i - 1; j++) {
                if (*(memoria + j) > *(memoria + j + 1)) {
                    int temp = *(memoria + j);
                    *(memoria + j) = *(memoria + j + 1);
                    *(memoria + j + 1) = temp;
                }
            }
        }
        
        nomeFita = PREFita + to_string(fitaAtual % numFitasEntrada) + EXTFita;
        ofstream fita(nomeFita, ios::app);
        for (int i = 0; i < count; i++) {
            fita << *(memoria + i) << endl;
        }
        fita.close();
    }

    entrada.close();
    delete[] memoria;  // libera a memória alocada dinamicamente
    
    // intercalacao
    bool terminou = false;
    bool* fitaTemDados = new bool[numFitas]; // para checar se a fita ainda tem dados
    
    int rodada = 0;
    while (!terminou) {
        // Limpa as fitas de saída para nova rodada
        for (int i = 0; i < numFitasSaida; i++) {
            ofstream limpa("fita" + to_string(numFitasEntrada + i) + ".txt", ios::trunc);
            limpa.close();
        }
        
     // Abre as fitas de entrada
        ifstream* leitores = new ifstream[numFitasEntrada];
        int* valoresAtuais = new int[numFitasEntrada];
        bool* ativo = new bool[numFitasEntrada];

     for (int i = 0; i < numFitasEntrada; i++) {
            leitores[i].open("fita" + to_string(i) + ".txt");
            if (leitores[i] >> valoresAtuais[i]) {
                ativo[i] = true;
                fitaTemDados[i] = true;
            } else {
                ativo[i] = false;
                fitaTemDados[i] = false;
            }
        }
        
        // Fitas de saída para essa rodada
        int fitaSaidaIndex = 0;
        ofstream saida("fita" + to_string(numFitasEntrada + fitaSaidaIndex) + ".txt", ios::app);

        // Intercala enquanto houver dados ativos
        while (true) {
            int menor = INT_MAX;
            int indiceMenor = -1;

            // Busca o menor valor entre os ativos
            for (int i = 0; i < numFitasEntrada; i++) {
                if (ativo[i] && valoresAtuais[i] < menor) {
                    menor = valoresAtuais[i];
                    indiceMenor = i;
                }
            }

            if (indiceMenor == -1) break; // acabou a rodada

            // Escreve o menor na fita de saída atual
            saida << menor << endl;

            // Avança a fita de onde veio o menor valor
            if (leitores[indiceMenor] >> valoresAtuais[indiceMenor]) {
                ativo[indiceMenor] = true;
            } else {
                ativo[indiceMenor] = false;
            }
        }

        // Fecha tudo
        for (int i = 0; i < numFitasEntrada; i++) leitores[i].close();
        saida.close();
        delete[] leitores;
        delete[] valoresAtuais;
        delete[] ativo;

        // Verifica se só uma fita tem dados
        int fitasComDados = 0;
        int fitaFinalIndex = -1;
        for (int i = 0; i < numFitas; i++) {
            ifstream teste("fita" + to_string(i) + ".txt");
            int dummy;
            if (teste >> dummy) {
                fitasComDados++;
                fitaFinalIndex = i;
            }
            teste.close();
        }

        if (fitasComDados == 1) {
            // salvar fita final no arquivo original
            salvarOrdenado(nomeArquivo, "fita" + to_string(fitaFinalIndex) + ".txt");
            terminou = true;
        } else {
            // Troca os papéis: os dados agora estão nas fitas de saída
            for (int i = 0; i < numFitasEntrada; i++) {
                ifstream in("fita" + to_string(numFitasEntrada + i) + ".txt");
                ofstream out("fita" + to_string(i) + ".txt", ios::trunc);
                string linha;
                while (getline(in, linha)) {
                    out << linha << endl;
                }
                in.close();
                out.close();
            }
        }

        rodada++;
    }

    delete[] fitaTemDados;

    // Limpeza final
    apagarFitas(numFitas);
}

int main() {
    int opcao;
    
    do {
        cout << "------------------------------" << endl;
        cout << "Menu: " << endl;
        cout << "0 - Sair" << endl;
        cout << "1 - Intercalacao Balanceada por Varios Caminhos" << endl;
        cout << "2 - Intercalacao por Segmentos Variaveis" << endl;
        cout << "Opcao: ";
        cin >> opcao;
        
        switch (opcao){
            
            case 0:
                cout << "Saindo..." << endl;
                break;
            
            case 1: {
                string nomeArquivo;
                int capacidadeMemoria, numFitas;
                
                cout << "Nome do arquivo de entrada (ex: entrada.txt): ";
                cin >> nomeArquivo;
                
                cout << "Capacidade da memoria (quantos numeros cabem de cada vez): ";
                cin >> capacidadeMemoria;
            
                cout << "Numero total de fitas (par ou impar, minimo 2): ";
                cin >> numFitas;
                
                ordenacaoFitas(nomeArquivo, capacidadeMemoria, numFitas);

                cout << "Arquivo ordenado com sucesso! Verifique o arquivo " << nomeArquivo << endl;
                
                break;
            }
            
            case 2: {
                break;
            }
        }

    } while (opcao != 0);

    return 0;
}