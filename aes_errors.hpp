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
  Name: 		aes_errors.hpp
  Author: 		Manuel Pedrero Luque
  Description: 	Este metodo de deteccion de errores no usa manejo de excepciones,
				simplemente informa de la causa del error y termina o no
				la ejecucion del programa
*/


#ifndef AES_ERRORS
#define AES_ERRORS

#include <string>
#include <cstdlib>
#include <iostream>

using namespace std;


enum{
	NO_ERRORS = 0,
	CANNOT_SAVE,
	FILE_NOT_FOUND
};


/** Esta clase tiene un unico método (Throw), que recibe tres parametros:
    un entero con el id del error (el enum de arriba hace las veces de error_id)
    un bool que indica si el programa debe terminar una vez mostrado el error
    una descripcion del error (si se quiere mostrar, error_id debe valer -2)
    */

class Error
{
	public:
		static int Throw(int error_id = -2, bool fatal = true, string error_desc = "")
		{
			switch(error_id){
                case FILE_NOT_FOUND:
                    cout<<"No se encuentra el archivo"<<endl;
                    break;

                case CANNOT_SAVE:
                    cout<<"No se puede guardar el archivo. Compruebe que no este en uso ni protegido"<<endl;
                    break;

				default:
					cout<<"Error no definido"<<endl;
					break;
			}
			if(fatal == true) exit(error_id);
			else return(error_id);
		}
};


#endif

