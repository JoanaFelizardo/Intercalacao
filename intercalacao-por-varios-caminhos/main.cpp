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


// Funcao para gerar runs simples e gravar em fitas
void gerarRuns (const string& nomeEntrada, int registrosMemoria, int numFitas) {

	ifstream entrada (nomeEntrada);
	if (!entrada.is_open()) {
		cout << "ERRO: Nao foi possivel abrir o arquivo de entrada" << endl;
		return;
	}

	int* buffer = new int[registrosMemoria];
	int fitaAtual = 0;

	while (true) {
		// ler registros para o buffer
		int i = 0;
		while (i < registrosMemoria && entrada >> buffer[i]) {
			i++;
		}

		if (i == 0) break;
		
		// ordenar o buffer
		sort (buffer, buffer + i);

		// escrever no final da fita atual
		string nomeFita = PREFita + to_string(fitaAtual) + EXTFita;
		ofstream fita(nomeFita, ios::app);

		if (!fita.is_open()) {
			cout << "ERRO: Nao foi possivel abrir a fita" << endl;
			delete[] buffer;
			entrada.close();
			return;
		}

		for (int j = 0; j < i; j++) {
			fita << buffer[j] << " ";
		}
		fita << endl;   // marca o fim do bloco
		fita.close();

		fitaAtual = (fitaAtual + 1) % numFitas;
	}

	delete[] buffer;
	entrada.close();

}

// apagar fitas
void apagarFitas(int numFitas) {
	for (int i = 0; i < numFitas; i++) {
		string nomeFita = PREFita + to_string(i) + EXTFita;
		remove(nomeFita.c_str());
	}
}

// Funcao 1: intercalacao Balanceada
void IntercalacaoBalanceada(int numFitas, const string& nomeEntrada) {

	bool ordenado = false;  // controla quando termina
	int passada = 0;        // conta quantas vezes processa os dados

	// enquanto nao estiver totalmente ordenado
	while(!ordenado) {

		// abrir fitas de entrada
		int numFitasEntrada = numFitas/2;                       // divide fitas entre entrada e saida
		ifstream* fitasEntrada = new ifstream[numFitasEntrada]; // leitura array entrada
		int* valores = new int[numFitasEntrada];                // armazena valores atuais de cada fita
		bool* ativos = new bool[numFitasEntrada]();             // indica quais fitas tem dados
		int fitasAtivas = 0;                                    // conta quantas fitas tem dados para processar

		// abre as fitas de entrada
		for (int i = 0; i < numFitasEntrada; i++) {
			string nomeFita = PREFita + to_string(i) + EXTFita;
			fitasEntrada[i].open(nomeFita); // abre para leitura

			// le o primeiro valor de cada uma e marca se tiver dados
			if(fitasEntrada[i].is_open() && (fitasEntrada[i] >> valores[i])) {
				ativos[i] = true;   // marca como ativa
				fitasAtivas++;      // incrementa o contador
			}
		}


		if(fitasAtivas == 0) {
			//libera memoria se nao tiver
			delete[] fitasEntrada;
			delete[] valores;
			delete[] ativos;
			return;
		}

		// abrir fitas de saida
		int numFitasSaida = numFitas - numFitasEntrada;     // restante das fitas
		ofstream* fitasSaida = new ofstream[numFitasSaida]; // array de arquivos de saida


		for (int i = 0; i < numFitasSaida; i++) {
			string nomeFita = PREFita + to_string(numFitasEntrada + i) + EXTFita;
			fitasSaida[i].open(nomeFita, ios::trunc);   // abre sobrescrevendo

			if (!fitasSaida[i].is_open()) {
				cout << "ERRO: Nao foi possivel abrir fita de saida " << i << endl;
				delete[] fitasEntrada;
				delete[] valores;
				delete[] ativos;
				return;
			}
		}

		//Processar IntercalaC'C#o
		int fitaSaidaAtual = 0;
		int blocosGerados = 0;


		while(fitasAtivas > 0) {
			//Encontrar menor valor entre fitas ativas
			int menorValor = INT_MAX;
			int fitaMenor = -1;

			// encontra o menor valro entre as fitas ativas
			for (int i = 0; i < numFitasEntrada; i++) {
				if(ativos[i] && valores[i] < menorValor) {  // se a fita ta ativa e tem menor valor
					menorValor = valores[i];    // atualiza o menor valor
					fitaMenor = i;              // guarda qual fita tem esse valor
				}
			}

			// se noa encontrou valor valido
			if (fitaMenor == -1) {
				// fim do bloco atual
				fitasSaida[fitaSaidaAtual] << endl;     // finaliza bloco atual
				blocosGerados++;
				fitaSaidaAtual = (fitaSaidaAtual + 1) % numFitasSaida;  // alterna para proxima fita de saida

				// reiniciar fitas para proximo bloco
				fitasAtivas = 0;
				for(int i = 0; i < numFitasEntrada; i++) {
					if(fitasEntrada[i].is_open()) {
						if(fitasEntrada[i] >> valores[i]) { // le o proximo valor
							ativos[i] = true;
							fitasAtivas++;
						} else {        // se acabaram os dados
							ativos[i] = false;
							fitasEntrada[i].close();
						}
					}
				}
			} else {    // se encpontrou valor valido
				// escrever valor
				fitasSaida[fitaSaidaAtual] << menorValor << " ";
				// tenta ler o proximo valor da fita
				if (!(fitasEntrada[fitaMenor] >> valores[fitaMenor])) {
					ativos[fitaMenor] = false;  // se nao consegue desativa
					fitasAtivas--;
				}
			}
		} // Fecha o while(fitasAtivas > 0)

		// FECHAR TUDO
		for (int i = 0; i < numFitasEntrada; i++) {
			if (fitasEntrada[i].is_open()) {
				fitasEntrada[i].close();
				string nomeFita = PREFita + to_string(i) + EXTFita;
				remove(nomeFita.c_str());
			}
		}

		for (int i = 0; i < numFitasSaida; i++) {
			fitasSaida[i].close();
			// sempre renomear para proxim passada
			string nomeAntigo = PREFita + to_string(numFitasEntrada + i) + EXTFita;
			string nomeNovo = PREFita + to_string(i) + EXTFita;
			rename(nomeAntigo.c_str(), nomeNovo.c_str());
		}

		delete[] fitasEntrada;
		delete[] fitasSaida;
		delete[] valores;
		delete[] ativos;

		//verificar se terminou (apenas 1 bloco)
		if (blocosGerados <= 1) {
			ifstream resultado(PREFita + to_string(0) + EXTFita);   // abre

			// verifica se esta vazio
			resultado.seekg(0, ios::end);   // posiciona o ponteiro no final
			if(resultado.tellg() == 0) {    // se a posicao 0
				cout << "Arquivo de resultado vazio!" << endl;
				return;
			}
			resultado.seekg(0, ios::beg);   // volta o ponteiro para o inicio

			// copia para o arquivo original
			ofstream saida(nomeEntrada, ios::trunc);
			if (!resultado.is_open() || !saida.is_open()) {
				cout << "ERRO: Ao criar arquivo final!" << endl;
				return;
			}

			saida << resultado.rdbuf(); // copia todo o conteudo da fita
			resultado.close();
			saida.close();
			ordenado = true;

			cout << "Intercalacao Balanceada concluida em " << passada+1 << " passadas!" << endl;
		}
		passada++;
	} // Fecha o while(!ordenado)
}

int main(){
    	int opcao;

	do {

		cout << "------------------------------" << endl;
		cout << "Menu: " << endl;
		cout << "0 - Encerrar programa" << endl;
		cout << "1 - Intercalacao Balanceada de Varios Caminhos" << endl;
		cout << "2 - Intercalacao Balanceada com Segmentos de Tamanho Variável" << endl;
		cout << "3 - Intercalacao com selecao por substituicao" << endl;
		cout << "Escolha uma opcao: ";
		cin >> opcao;

		switch (opcao) {

		case 0:
			cout << "Encerrando o programa!" << endl;
			break;

		case 1: {
			string nomeArquivo;
			int registrosMemoria, numFitas;

			cout << "Insira o nome do arquivo de entrada: ";
			cin >> nomeArquivo;

			cout << "Numero de registros cabem na memoria: ";
			cin >> registrosMemoria;

			cout << "Numero de fitas: ";
			cin >> numFitas;

			gerarRuns(nomeArquivo, registrosMemoria, numFitas);
			IntercalacaoBalanceada(numFitas,nomeArquivo);
			apagarFitas(numFitas);

			break;
		}

		case 2: {
		    
            break;
		}


		case 3:{
		    
		    break;
		}
	}

	} while (opcao != 0);





	return 0;

    return 0;
}