/**
  Name: 		aes_globals.hpp
  Author: 		Manuel Pedrero Luque
  Description: 	Define las variables globales del programa
*/

#include "aes_types.hpp"
#include "aes_msg.hpp"
#include <string>

string plain_filename;
string output_filename;
string key_filename;

// The number of columns comprising a state in AES. This is a constant in AES. Value=4
#define Nb 4

Msg plain;
// Almacena la clave
byte key[4][4];
// Almacena un bloque del archivo plano
byte block[4][4];
// Almacena las claves de ronda
byte round_key[4][60];
// Numero de rondas del cifrador. Se inicializa a cero.
int Nr = 0;
// Numero de palabras de 32 bits de la clave. Se inicializa a cero.
int Nk = 0;

// plain - array de 128 bits con el texto en claro.
// key - array de 128 bits con la clave.
// state - array de 128 bits para los estados intermedios.
byte in[16];
byte out[16];
byte state[4][4];
