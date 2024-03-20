#include <stdio.h>
#include <stack>
#include <unistd.h>

#include <iostream>
#include <cstring>
using namespace std;

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

// Estrutura de dados contendo as próximas
// posicões a serem exploradas no labirinto
std::stack<pos_t> valid_positions;
/* Inserir elemento: 
	 pos_t pos;
	 pos.i = 1;
	 pos.j = 3;
	 valid_positions.push(pos)
 */
// Retornar o numero de elementos: 
//    valid_positions.size();
// 
// Retornar o elemento no topo: 
//  valid_positions.top(); 
// 
// Remover o primeiro elemento do vetor: 
//    valid_positions.pop();


// Função que le o labirinto de um arquivo texto, carrega em 
// memória e retorna a posição inicial
pos_t load_maze(const char* file_name) {
	pos_t initial_pos;
	char maze_element;

	// Abre o arquivo para leitura (fopen)
	FILE * maze_file = fopen(file_name, "r");

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
bool walk(pos_t pos) {
	sleep(1);
	if (maze[pos.i][pos.j] == 's')
		return true;
	// Marcar a posição atual com o símbolo 'o'
	maze[pos.i][pos.j] = 'o';
	// Limpa a tela
	system("clear");
	// Imprime o labirinto
	print_maze();

	/*  Dada a posição atual, verifica quais sao as próximas posições válidas
		Checar se as posições abaixo são validas (i>0, i<num_rows, j>0, j <num_cols)
		e se são posições ainda não visitadas (ou seja, caracter 'x') e inserir
		cada uma delas no vetor valid_positions
			- pos.i, pos.j+1
			- pos.i, pos.j-1
			- pos.i+1, pos.j
			- pos.i-1, pos.j
		Caso alguma das posições válidas seja igual a 's', retornar verdadeiro
	*/
	pos_t valid_pos;
	valid_pos.i = pos.i;
	valid_pos.j = pos.j;

	if (pos.j+1 < num_cols){
		if (maze[pos.i][pos.j+1] == 's')
			return true;
		if (maze[pos.i][pos.j+1] == 'x'){
			valid_pos.i = pos.i;
			valid_pos.j = pos.j+1;
			valid_positions.push(valid_pos);
		}
	}

	if (pos.j-1 >= 0){
		if (maze[pos.i][pos.j-1] == 's')
			return true;
		if (maze[pos.i][pos.j-1] == 'x'){
			valid_pos.i = pos.i;
			valid_pos.j = pos.j-1;
			valid_positions.push(valid_pos);
		}
	}

	if (pos.i+1 < num_rows){
		if (maze[pos.i+1][pos.j] == 's')
			return true;
		if (maze[pos.i+1][pos.j] == 'x'){
			valid_pos.i = pos.i+1;
			valid_pos.j = pos.j;
			valid_positions.push(valid_pos);
		}
	}

	if (pos.i-1 >= 0){
		if (maze[pos.i-1][pos.j] == 's')
			return true;
		if(maze[pos.i-1][pos.j] == 'x'){
			valid_pos.i = pos.i-1;
			valid_pos.j = pos.j;
			valid_positions.push(valid_pos);
		}
	}

	if (!valid_positions.empty()) {
		pos_t valid_position = valid_positions.top();
		valid_positions.pop();
		walk(valid_position);
	}
	else{
		return false;
	}
}

int main(int argc, char* argv[]) {
	// carregar o labirinto com o nome do arquivo recebido como argumento
	system("clear");
	
	pos_t initial_pos = load_maze("../data/maze2.txt");

	// chamar a função de navegação
	bool exit_found = walk(initial_pos);
	
	// Tratar o retorno (imprimir mensagem)
	if (exit_found){
		system("clear");
		print_maze();
		printf("\nSaida encontrada!\n");
	}
	else{
		system("clear");
		print_maze();
		printf("\nSaida não encontrada!\n");
	}

	// libera a memória da matriz
	for (int i = 0; i < num_rows; i++)
		free (maze[i]) ;
	free (maze);

	return 0;
}
