#include <stdio.h>
#include <stack>
#include <chrono>
#include <iostream>
#include <cstring>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

using namespace std;

mutex m;

//std::vector<std::thread> thread_vector;

int exit_found = 0;
bool no_exit = false;
int num_threads_global = 0;

// Matriz de char representnado o labirinto
char** maze; // Voce também pode representar o labirinto como um vetor de vetores de char (vector<vector<char>>)

// Numero de linhas e colunas do labirinto
int num_rows;
int num_cols;

// Representação de uma posição
struct pos_t {
	int i;
	int j;
};

// Função que le o labirinto de um arquivo texto, carrega em 
// memória e retorna a posição inicial
pos_t load_maze(const char* file_name) {
	pos_t initial_pos;
	char maze_element;

	// Abre o arquivo para leitura (fopen)
	FILE * maze_file;
	maze_file = fopen(file_name, "r");

	// Le o numero de linhas e colunas (fscanf) 
	// e salva em num_rows e num_cols
	fscanf(maze_file, "%d", &num_rows);
	fscanf(maze_file, "%d", &num_cols);

	// Aloca a matriz maze (malloc)
	maze = (char **)malloc(num_rows * sizeof(char*));
	for (int i = 0; i < num_rows; ++i)
		// Aloca cada linha da matriz
		maze[i] = (char *)malloc(num_cols * sizeof(char));
	
	for (int i = 0; i < num_rows; ++i) {
		fscanf(maze_file, "%s", maze[i]);
		for (int j = 0; j < num_cols; ++j) {
			// Le o valor da linha i+1,j do arquivo e salva na posição maze[i][j]
			// Se o valor for 'e' salvar o valor em initial_pos
			if(maze[i][j] == 'e'){
				initial_pos.i = i;
				initial_pos.j = j;
			}
		}
	}
	fclose(maze_file);
	return initial_pos;
}

// Função que imprime o labirinto
void print_maze() {
	for (int i = 0; i < num_rows; ++i) {
		for (int j = 0; j < num_cols; ++j) {
			printf("%c", maze[i][j]);
		}
		printf("\n");
	}
}


// Função responsável pela navegação.
// Recebe como entrada a posição inicial e retorna um booleano indicando se a saída foi encontrada
void walk(pos_t pos) {
	// Incrementa o número de threads existentes
	m.lock();
	num_threads_global++; 
	m.unlock();

	exit_found +=int(maze[pos.i][pos.j] == 's');

	// Posicao a ser empilhada
	pos_t valid_pos;

	// Cria uma pilha com os caminhos possiveis para ESTA thread
	stack<pos_t> valid_positions;
	valid_positions.push(pos);
	valid_positions.pop();

	/** Marcar a posição atual com o símbolo '.'
	 * Não Precisa ser travada, pois existe apenas uma thread no instante inicial
	 * e quando já está no ponto, a possibilidade de se haver mais de uma thread
	 * avaliando este mesmo ponto já foi eliminada nas avaliações subsequentes.
	*/ 
	if(maze[pos.i][pos.j] != 'e'){
		maze[pos.i][pos.j] = '.';
	
	// Avalia a posição à direita
	if (pos.j+1 < num_cols){
		m.lock();
		exit_found += int(maze[pos.i][pos.j+1] == 's');
		if (maze[pos.i][pos.j+1] == 'x'){
			maze[pos.i][pos.j+1] = 'o';
			valid_pos.i = pos.i;
			valid_pos.j = pos.j+1;
			valid_positions.push(valid_pos);
		}
		m.unlock();
	}

	// Avalia a posição à esquerda
	if (pos.j-1 >= 0){
		m.lock();
		exit_found += int(maze[pos.i][pos.j+1] == 's');
		if (maze[pos.i][pos.j-1] == 'x'){
			maze[pos.i][pos.j-1] = 'o';
			valid_pos.i = pos.i;
			valid_pos.j = pos.j-1;
			valid_positions.push(valid_pos);
		}
		m.unlock();
	}

	// Avalia a posição acima
	if (pos.i+1 < num_rows){
		m.lock();
		exit_found += int(maze[pos.i][pos.j+1] == 's');
		if (maze[pos.i+1][pos.j] == 'x'){
			maze[pos.i+1][pos.j] = 'o';
			valid_pos.i = pos.i+1;
			valid_pos.j = pos.j;
			valid_positions.push(valid_pos);
		}
		m.unlock();
	}

	// Avalia a posição abaixo
	if (pos.i-1 >= 0){
		exit_found += int(maze[pos.i][pos.j+1] == 's');
		m.lock();
		if(maze[pos.i-1][pos.j] == 'x'){
			maze[pos.i-1][pos.j] = 'o';
			valid_pos.i = pos.i-1;
			valid_pos.j = pos.j;
			valid_positions.push(valid_pos);
		}
		m.unlock();
	}

	this_thread::sleep_for(chrono::milliseconds(100));

	// Para cada caminho possível, cria uma nova tarefa
	while(!valid_positions.empty()) {
		pos_t next_pos = valid_positions.top();
		valid_positions.pop();
		std::thread t(walk,next_pos);
		t.detach();
	}

	// Decrementa o número de threads existentes
	m.lock();
	num_threads_global--;
	m.unlock();
}

int main(int argc, char* argv[]) {
	// carregar o labirinto com o nome do arquivo recebido como argumento
	system("clear");
	
	int num_threads = 0;

	pos_t initial_pos = load_maze("../data/maze2.txt");

	// chamar a função de navegação
	std::thread main_walk(walk, initial_pos);
	main_walk.detach();
	
	// Tratar o retorno (imprimir mensagem)
	while (exit_found==0 && !no_exit){
		this_thread::sleep_for(chrono::milliseconds(100));
		system("clear");
		print_maze();
		num_threads = num_threads_global; // Para Testes
		no_exit = (exit_found == 0 && num_threads == 0);
	}

	if (exit_found)
		printf("\nSaida encontrada!\n");
	else{
		printf("\nSaida não encontrada!\n");
	}

	// libera a memória da matriz
	for (int i = 0; i < num_rows; i++)
		free (maze[i]) ;
	free (maze);

	return 0;
}
