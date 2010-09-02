/*
aesth

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

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
