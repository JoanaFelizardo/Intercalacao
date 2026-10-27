#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <cstdio>
#include <climits>

using namespace std;

const string PREFita = "fita";
const string EXTFita = ".txt";

struct segmento{
    int numero;
    int segmento;
};

string nomeFita (int index){
    return PREFita + to_string(index) + EXTFita;
}

void criarFitas(int numFitas){
    for(int i = 0; i < numFitas; i++){
        ofstream fout(nomeFita(i).c_str());
        fout.close();
    }
}

bool arquivoVazio(const char* nomeArquivo){
    FILE* fita = fopen(nomeArquivo, "r");
    if (fita == nullptr) return true;
    
    int aux = fgetc(fita);
    fclose(fita);
    return (aux == EOF);
}

void apagarFitas(int numFitas){
    for (int i = 0; i < numFitas; i++){
        remove(nomeFita(i).c_str());
    }
}

void distribuiBalanceada(const string& nomeArquivo, int capacidade, int numFitas){
    
    ifstream fin(nomeArquivo.c_str());
    
    if(!fin.is_open()){
        cout << "ERRO: Ao abrir arquivo: " << nomeArquivo << endl;
        return;
    }
    
    criarFitas(numFitas);
    
    int fitasEntrada = numFitas / 2;
    int fitaAtual = 0;
    
    int buffer[10000]; // Ajuste o tamanho máximo conforme necessidade
    if (capacidade > 10000) {
        cout << "ERRO: BUFFER!" << endl;
        fin.close();
        return;
    }
    
    while (true){
        int cont = 0;
        int valor;
        
        //  le ate a capacidade do arquivo original para o buffer
        while (cont < capacidade && (fin >> valor)){
            buffer[cont++] = valor;
        }
        
        if (cont == 0) break;
        
        sort(buffer, buffer + cont);
        
        ofstream fout(nomeFita(fitaAtual).c_str(), ios::app);
        if(!fout.is_open()){
            cout << "ERRO: Ao abrir a fita: " << nomeFita(fitaAtual) << endl;
            fin.close();
            return;
        }
        
        for (int i = 0; i < cont; i++){
            fout << buffer[i] << endl;
        }
        
        fout.close();
        
        fitaAtual = (fitaAtual + 1) % fitasEntrada;
        
      
    }
    fin.close();
}

void distribuiSegmentos(const string& nomeArquivo, int numFitas){
    
    ifstream fin(nomeArquivo.c_str());
    
    if (!fin.is_open()){
        cout << "ERRO: Ao abrir arquivo: " << nomeArquivo << endl;
        return;
    }
    
    criarFitas(numFitas);
    
    int fitasEntrada = numFitas / 2;
    int fitaAtual = 0;
    int numeroAnterior, numeroAtual;
    bool primeiraLeitura = true;
    
    ofstream fout(nomeFita(fitaAtual).c_str(), ios::app);
    if (!fout.is_open()){
        cout << "ERRO: Ao abrir fita: " << nomeFita(fitaAtual) << endl;
        fin.close();
        return;
    }
    
    while (fin >> numeroAtual){
        
        if (primeiraLeitura){
            numeroAnterior = numeroAtual;
            fout << numeroAtual << endl;
            primeiraLeitura = false;
            
        }   else {
            if (numeroAtual >= numeroAnterior){
                // grava na mesma fitaA
                fout << numeroAtual << endl;
                
            }   else {
                // mudou o segmento, fecha fita 
                fout.close();
                
                //passa para a proxima fita
                fitaAtual = (fitaAtual + 1) % fitasEntrada;
                fout.open(nomeFita(fitaAtual).c_str(), ios::app);
                
                if (!fout.is_open()){
                    cout << "ERRO: Ao abrir fita: " << nomeFita(fitaAtual) << endl;
                    fin.close();
                    return;
                }
                
                //grava o numero atual na nova fitaA
                fout << numeroAtual << endl;
            }
            numeroAnterior = numeroAtual;
        }
    }
    
    fout.close();
    fin.close();
}

void Intercalacao(const string& nomeArquivo, int numFitas){
    
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
    cout << "Intercalaçao finalizada. Arquivo Ordenado: " << nomeArquivo << endl;
}

int main(){
    int opcao;
    
    do {
        cout << "--------------------------------------" << endl;
        cout << "MENU:" << endl;
        cout << "1 - Intercalaçao Balanceada" << endl;
        cout << "2 - Intercalaçao Segmentos" << endl;
        cout << "3 - Intercalacao Polifasica" << endl;
        cout << "0 - Sair" << endl;
        cout << "Escolha uma opcao: ";
        cin >> opcao;
        
        switch (opcao){
            
            case 0:
                cout << "Saindo..." << endl;
                break;
                
            case 1:{
               
               
                string nomeArquivo;
                int capacidade, numFitas;
                
                cout << "Digite o nome do arquivo: ";
                cin >> nomeArquivo;
                
                cout << "Digite o numero de fitas: ";
                cin >> numFitas;
                
                cout << "Digite a capacidade das fitas: ";
                cin >> capacidade;
                
                distribuiBalanceada(nomeArquivo, capacidade, numFitas);
                Intercalacao(nomeArquivo, numFitas);
                
                
                
               //  cout << "Funcao nao implementada!" << endl;
                break;
            }
            
            case 2: {
                string nomeArquivo;
                int numFitas;
                
                cout << "Digite o nome do arquivo: ";
                cin >> nomeArquivo;
                
                cout << "Digite o numero das fitas (deve ser par >= 4): ";
                cin >> numFitas;
                
                
                distribuiSegmentos(nomeArquivo, numFitas);
                Intercalacao(nomeArquivo, numFitas);
            }
            
            case 3: {
                cout << "Funcao nao implementada!" << endl;
                break;
            }
        }
        
    } while (opcao != 0);
  

    return 0;
}