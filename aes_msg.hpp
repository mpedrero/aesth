/**
  Name: 		Msg.hpp
  Author: 		Manuel Pedrero Luque
  Description: 	Clase Msg. Almacena y proporciona diversas operaciones sobre un
				archivo binario.
				El stream de datos se almacena en forma de vector<byte>, donde byte es un
				tipo de datos propio y equivalente a unsigned char que almacena un byte de
				datos
*/


#ifndef AES_MSG
#define AES_MSG

#include <string>
#include <cstring>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>

#include "aes_errors.hpp"
#include "aes_types.hpp"


using namespace std;


class Msg
{
	public:
		Msg();
		~Msg();

		int LoadFromFile(string,bool);
		int SaveToFile(bool);
		int SaveToFile(string,bool);
		void SetMessage(byte*,int);
		int GetSize();
		byte Getbyte(int);
		int GetBit(int);

	private:
		byte* msg_datastream;
		string msg_filename;
		int msg_size;
};




Msg::Msg()
{
	msg_size = -1;
	msg_filename = "";
	msg_datastream = NULL;
}


Msg::~Msg()
{
	if(msg_datastream != NULL){
		delete[] msg_datastream;
	}
}


int
Msg::LoadFromFile(string filename, bool critical = true)
{
	// Abrimos el fichero
	ifstream file(filename.c_str(),ios::binary);
	if(!file.is_open()){
		Error::Throw(FILE_NOT_FOUND,critical);
		return -1;
	}

	// Guardamos el nombre del fichero
	msg_filename = filename;
	file.seekg(0, ios::end); 	//Ir al final del fichero
	msg_size = file.tellg();	//Ver posicion para saber tamaño
	file.seekg(0, ios::beg);

	//Dimensionamos el msg_datastream para guardar el stream de datos
	if(msg_datastream != NULL)
		delete[] msg_datastream;

	msg_datastream = new byte[msg_size+msg_final_mark.length()];
	//Leemos el fichero y lo guardamos en msg_datastream
	file.read((char*)msg_datastream,msg_size);

	//Añadimos la marca de fin de archivo
	for(int i=0;i<(int)msg_final_mark.length();i++){
		msg_datastream[msg_size+i] = msg_final_mark[i];
	}
	//Actualizamos el tamaño del mensaje
	msg_size +=msg_final_mark.length();

	file.close();

	cout<<"Fichero "<<msg_filename<<" leido. "<<msg_size<<" bytes cargados"<<endl;
	return 0;
}


int
Msg::SaveToFile(bool critical = false)
{
	if(msg_filename == "") msg_filename = "ciphered.txt";
	ofstream file(msg_filename.c_str(),ios::binary);
	if(file == NULL){
	    Error::Throw(CANNOT_SAVE,critical);
	    return -1;
	}
	file.write((char*)msg_datastream,msg_size);
	file.close();

	cout<<"Fichero "<<msg_filename<<" guardado. "<<msg_size<<" bytes guardados"<<endl;

	return 0;
}



int
Msg::SaveToFile(string filename, bool critical = false)
{
	msg_filename = filename;

	ofstream file(msg_filename.c_str(),ios::binary);
	if(file == NULL){
	    Error::Throw(CANNOT_SAVE,critical);
	    return -1;
	}
	file.write((char*)msg_datastream,msg_size);
	file.close();

	cout<<"Fichero "<<msg_filename<<" guardado. "<<msg_size<<" bytes guardados"<<endl;
	return 0;
}


int
Msg::GetBit(int pos)
{
	int byte_pos;
	byte byte_value;
	int bit_pos;
	int bit_value[8];
	int i,exp;

	//Nos posicionamos en el byte correcto y en el bit correcto dentro de ese byte
	byte_pos = (int)pos/8;
	bit_pos = (int)pos%8;
	byte_value = msg_datastream[byte_pos];

	//Convertimos el byte correspondiente a una secuencia de 8 bits y los almacenamos en el vector bitvalue
	for(i=7;i>=0;i--){
		exp = (int)pow(2.0,i);
		if(byte_value >= exp){
			bit_value[i] = 1;
			byte_value -= exp;
		}else{
			bit_value[i] = 0;
		}
	}
	return bit_value[bit_pos];
}


byte
Msg::Getbyte(int pos)
{
	return msg_datastream[pos];
}


int
Msg::GetSize()
{
	return msg_size;
}


void
Msg::SetMessage(byte* data, int tam)
{
	//Si el stream estaba ocupado lo borramos
	if(msg_datastream != NULL)
		delete[] msg_datastream;

	//Copiamos los bytes al stream
	msg_datastream = new byte[tam];
	memcpy(msg_datastream,data,tam);
	//Actualizamos el msg_size
	msg_size = tam;
}


#endif
