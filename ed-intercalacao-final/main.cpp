#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <cstdio>   // usar o remove()
#include <climits>  // para INT_MAX

using namespace std;

// Constantes
const string PREFita = "fita";
const string EXTFita = ".txt";

// gerar nome de fita baseado no indice
string nomeFita(int idx){
    return PREFita + to_string(idx) + EXTFita;
}

// apagar fitas
void apagarFitas(int numFitas) {
	for (int i = 0; i < numFitas; i++) {
		remove(nomeFita(i).c_str());
	}
}

// cria fitas temporarias vazias
void criarFitas (int numFitas){
    
    for (int i = 0; i < numFitas; i++){
        ofstream* fout = new ofstream(nomeFita(i).c_str());
        fout->close();
        delete fout;
    }
}

void distribuirBalanceada(const string& nomeArquivo, int numFitas, int capacidadeMemoria){
    
    ifstream* fin = new ifstream(nomeArquivo.c_str());
    
    if (!fin->is_open()){
        cout << "ERRO: Ao abrir o arquivo " << nomeArquivo << endl;
        delete fin;
        return;
    }
    
    criarFitas(numFitas);
    
    int numero;
    int fitaAtual = 0;
    int count = 0;  // contador de elementos na fita atual
    
    ofstream* fout = nullptr;
    string nome;
    
    while ((*fin) >> numero){
        nome = nomeFita(fitaAtual);
        fout = new ofstream(nome.c_str(), ios::app);
        (*fout) << numero << endl;
        fout->close();
        delete fout;
        
        count++;
        if (count == capacidadeMemoria){
            count = 0;
            fitaAtual++;
            
            if (fitaAtual == numFitas){
                fitaAtual = 0;
            }
        }
    }
    
    fin->close();
    delete fin;
    
}

// constante para máximo números a serem ordenados em memória na fita
const int MAX = 10000;

// função para ordenar fita lendo em um array fixo e reescrevendo
void ordenarFita(const string& nomeArquivo){
    ifstream* fin = new ifstream(nomeArquivo.c_str());
    
    if (!fin->is_open()){
        cout << "ERRO: Ao abrir a fita " << nomeArquivo << endl;
        delete fin;
        return;
    }
    
    int numeros[MAX];
    int count = 0;
    int num;
    
    while ((*fin) >> num){
        if (count < MAX){
            numeros[count++] = num;
        }   else {
            cout << "Arquivo muuito grande " << nomeArquivo << endl;
            fin->close();
            delete fin;
            return;
        }
    }
    
    fin->close();
    delete fin;
    
    sort (numeros, numeros + count);
    
    ofstream* fout = new ofstream(nomeArquivo.c_str(), ios::trunc);
    
    for (int i = 0; i < count; i++){
        (*fout) << numeros[i] << endl;
    }
    
    fout->close();
    delete fout;
}

void ordenarTodasFitas(int numFitas){
    for (int i = 0; i < numFitas; i++){
        ordenarFita(nomeFita(i));
    }
}

//verificar se arquivo esta vazio
bool arquivoVazio(const char* nomeArquivo){
    FILE* f = fopen(nomeArquivo, "r");
    if (f == nullptr) return true; // nao existe, vazio
    
    int c = fgetc(f);
    fclose(f);
    return(c == EOF);
}

//funcao para intercalacao Balanceada
void intercalacaoBalanceada(const string& nomeArquivo, int numFitas, int capacidadeMemoria){
    
    distribuirBalanceada(nomeArquivo, numFitas, capacidadeMemoria);
    ordenarTodasFitas(numFitas);
    
    int numFitasEntrada = numFitas / 2;
    int numFitasSaida = numFitas - numFitasEntrada;
    
    bool terminou = false;
    
    while(!terminou){
        
        for (int i = 0; i < numFitasEntrada; i++){
            string fitasEntrada1 = nomeFita(i);
            string fitasEntrada2 = nomeFita((i + 1) % numFitasEntrada);
            string fitaSaida = nomeFita(numFitasEntrada + i);
            
            cout << "Abrindo fitas " << fitasEntrada1 << " e " << fitasEntrada2 << endl; 
            ifstream* fin1 = new ifstream(fitasEntrada1.c_str());
            ifstream* fin2 = new ifstream(fitasEntrada2.c_str());
            ofstream* fout = new ofstream(fitaSaida.c_str(), ios::trunc);
            
            if (!fin1->is_open() || !fin2->is_open()){
                cout << "ERRO: Ao abrir as fitas para a intercalacao!" << endl;
                if (fin1) { 
                    fin1->close(); 
                    delete fin1;
                }
                if (fin2) { 
                    fin2->close(); 
                    delete fin2; 
                }
                if (fout) { 
                    fout->close(); 
                    delete fout;
                }
                return;
            }
            
            int valor1, valor2;
            bool temVal1 = (fin1->good() && (*fin1) >> valor1); // Tenta ler o valor
            bool temVal2 = (fin2->good() && (*fin2) >> valor2); // Tenta ler o valor
            // intercalacao
            while(temVal1 && temVal2){
                
                if (valor1 <= valor2){
                    (*fout) << valor1 << endl;
                    temVal1 = (fin1->good() && (*fin1) >> valor1); // Tenta ler o próximo valor
                }   else {
                    (*fout) << valor2 << endl;
                    temVal2 = (fin2->good() && (*fin2) >> valor2); // Tenta ler o próximo valor
                }
            }
            
            while(temVal1){
                (*fout) << valor1 << endl;
                temVal1 = (fin1->good() && (*fin1) >> valor1); // Tenta ler o próximo valor
            }
            
            while (temVal2){
                (*fout) << valor2 << endl;
                temVal2 = (fin2->good() && (*fin2) >> valor2); // Tenta ler o próximo valor
            }
            
            fin1->close();
            fin2->close();
            fout->close();
            
            delete fin1;
            delete fin2;
            delete fout;
        }
        
        // verifica se so uma fita tem dados 
        int numFitasDados = 0;
        int indiceFita = -1;
        
        for (int i = numFitasEntrada; i < numFitas; i++){
            if (!arquivoVazio(nomeFita(i).c_str())){
                numFitasDados++;
                indiceFita = i;
            }
        }
        
        if (numFitasDados = 1){
            ifstream* fin = new ifstream(nomeFita(indiceFita).c_str());
            ofstream* fout = new ofstream(nomeArquivo.c_str(), ios::trunc);
            
            int n;
            while ((*fin) >> n){
                (*fout) << n << endl;
            }
            
            fin->close();
            fout->close();
            delete fin;
            delete fout;
            terminou = true;
        }   else {
            // copiar fitas de saida para entrada e limpar saida
            for(int i = 0; i < numFitasEntrada; i++){
                ifstream* fin = new ifstream(nomeFita(numFitasEntrada + i).c_str());
                ofstream* fout = new ofstream(nomeFita(i).c_str(), ios::trunc);
                
                int n;
                while ((*fin) >> n){
                    (*fout) << n << endl;
                }
                
                fin->close();
                fout->close();
                delete fin;
                delete fout;
            }
            
            //limpar fitas de saida
            for (int i = numFitasEntrada; i < numFitas; i++){
                ofstream* fst = new ofstream(nomeFita(i).c_str(), ios::trunc);
                fst->close();
                delete fst;
            }
        }
    }
    
    apagarFitas(numFitas);
}

// intercalacao por segmentos de tamanho variavwl
void intercalacaoSegmentos(const string& nomeArquivo, int numFitas){
    
    ifstream* fin = new ifstream(nomeArquivo.c_str());
    
    if(!fin->is_open()){
        cout << "ERRO: Ao abrir o arquivo!" << nomeArquivo << endl;
        delete fin;
        return;
     }
     
     criarFitas(numFitas);
     
     int segmentosAtual = 0;
     int numeroAnterior, numeroAtual;
     
     //ler o primeiro numero
     if (!(*fin) >> numeroAnterior){
         cout << "Arquivo vazio!" << endl;
         fin->close();
         delete fin;
         return;
     }
     
     //escreve o primeiro numero na fita 0
    ofstream* fout = new ofstream(nomeFita(0).c_str(), ios::app);
    (*fout) << numeroAnterior << endl;
    fout->close();
    delete fout;
    
    while((*fin) >> numeroAtual){
        if (numeroAtual < numeroAnterior){
            segmentosAtual++;
            
            if (segmentosAtual = numFitas){
                segmentosAtual = 0;
            }
        }
        
        fout = new ofstream(nomeFita(segmentosAtual).c_str(), ios::app);
        (*fout) << numeroAtual << endl;
        fout->close();
        delete fout;
        numeroAnterior = numeroAtual;
    }
    
    fin->close();
    delete fin;
    
    int numFitasEntrada = numFitas / 2;
    int numFitasSaida = numFitas - numFitasEntrada;
    bool terminou = false;
    
    while(!terminou){
        
        for (int i = 0; i < numFitasEntrada; i++){
            string fitaEntrada1 = nomeFita(i);
            string fitaEntrada2 = nomeFita((i + 1) % numFitasEntrada);
            string fitaSaida = nomeFita(numFitasEntrada + i);

            ifstream* fin1 = new ifstream(fitaEntrada1.c_str());
            ifstream* fin2 = new ifstream(fitaEntrada2.c_str());
            ofstream* fout = new ofstream(fitaSaida.c_str(), ios::trunc);
            
            if (!fin1->is_open() || !fin2->is_open()){
                cout << "ERRO: Ao abrir fitas para intercalacao" << endl;
                if (fin1){
                    fin1->close();
                    delete fin1;
                }
                if (fin2){
                    fin2->close();
                    delete fin2;
                }
                if (fout){
                    fout->close();
                    delete fout;
                }
                return;
            }
            
            int val1, val2;
            bool temVal1 = (fin1->good() && (*fin1) >> val1); // Tenta ler o valor
            bool temVal2 = (fin2->good() && (*fin2) >> val2); // Tenta ler o valor

            
            while (temVal1 && temVal2){
                
                if (val1 <= val2){
                    (*fout) << val1 << endl;
                     temVal1 = (fin1->good() && (*fin1) >> val1); // Tenta ler o próximo valor
                }   else {
                    (*fout) << val2 << endl;
                    temVal2 = (fin2->good() && (*fin2) >> val2); // Tenta ler o próximo valor
                }
            }
            
            while(temVal1) {
                (*fout) << val1 << endl;
                temVal1 = (fin1->good() && (*fin1) >> val1); // Tenta ler o próximo valor
            }
            
            while(temVal2){
                (*fout) << val2 << endl;
                temVal2 = (fin2->good() && (*fin2) >> val2); // Tenta ler o próximo valor
            }
            
            fin1->close();
            fin2->close();
            fout->close();
            
            delete fin1;
            delete fin2;
            delete fout;
        }
        
        int numFitasDados = 0;
        int indiceFita = -1;
        
        for (int i = numFitasEntrada; i < numFitas; i++){
            if (!arquivoVazio(nomeFita(i).c_str())){
                numFitasDados++;
                indiceFita = i;
            }
        }
        
        if(numFitasDados = 1){
            ifstream* fin = new ifstream(nomeFita(indiceFita).c_str());
            ofstream* fout = new ofstream(nomeArquivo.c_str(), ios::trunc);
            int n;
            
            while((*fin) >> n){
                (*fout) << n << endl;
            }
            
            fin->close();
            fout->close();
            delete fin;
            delete fout;
            terminou = true;
        }   else {
            
            for (int i = 0; i < numFitasEntrada; i++){
                ifstream* fin = new ifstream(nomeFita(numFitasEntrada + i).c_str());
                ofstream* fout = new ofstream(nomeFita(i).c_str(), ios::trunc);
                
                int n;
                while((*fin) >> n){
                    (*fout) << n << endl;
                }
                
                fin->close();
                fout->close();
                delete fin;
                delete fout;
            }
            
            for (int i = numFitasEntrada; i < numFitas; i++){
                ofstream* fst = new ofstream(nomeFita(i).c_str(), ios::trunc);
                fst->close();
                delete fst;
            }
        }
    }
    
    apagarFitas(numFitas);
}

int main(){
    
    int opcao;
    
    do {
        
        cout << "----------------------------" << endl;
        cout << "MENU: " << endl;
        cout << "0 - Sair" << endl;
        cout << "1 - Intercalacao Balanceada" << endl;
        cout << "2 - Intercalacao Segmentada" << endl;
        cout << "Escolha uma opcao: ";
        cin >> opcao;
        
        switch (opcao){
            
            case 1: {
                string nomeArquivo;
                int numFitas, capacidadeMemoria;
                
                cout << "Digite o nome do arquivo (.txt): ";
                cin >> nomeArquivo;
                
                cout << "Digite o numero de fitas: ";
                cin >> numFitas;
                
                cout << "Digite o numero de capacidade da Memoria: ";
                cin >> capacidadeMemoria;
                
                intercalacaoBalanceada(nomeArquivo, numFitas, capacidadeMemoria);
                break;
            }
            
            case 2: {
                string nomeArquivo;
                int numFitas, capacidadeMemoria;
                
                cout << "Digite o nome do arquivo (.txt): ";
                cin >> nomeArquivo;
                
                cout << "Digite o numero de fitas: ";
                cin >> numFitas;
                
                intercalacaoSegmentos(nomeArquivo, numFitas);
                
                break;
            }
            
            case 0: 
                cout << "Saindo..." << endl;
                
            break;
        }
        
        
    }   while (opcao != 0);
    
    return 0;
}
