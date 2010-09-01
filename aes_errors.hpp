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

