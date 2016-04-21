#ifndef SUDOKU_2015_H_
#define SUDOKU_2015_H_

int insertValor(int fila, int columna, int valor);
void sudoku9x9();
void iniciarArmThumb();
int obtenerValor(int i, int j);
int esPista(int i, int j);
int esError(int i, int j);
int esCandidato(int i, int j, int candidato);

#endif /* SUDOKU_2015_H_ */
