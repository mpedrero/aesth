#include <iostream>
#include <omp.h>
#include "aes_errors.hpp"
#include "aes_types.hpp"
#include "aes_msg.hpp"
#include "aes_globals.hpp"


// Macro que obtiene el producto de {02} y el argumento de entrada en modulo {1b}
#define xtime(x)   ((x<<1) ^ (((x>>7) & 1) * 0x1b))

// Tabla S-Box
int getSBoxValue(byte num)
{
	int sbox[256] = {
	// 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, //0
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, //1
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, //2
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, //3
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, //4
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, //5
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, //6
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, //7
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, //8
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, //9
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, //A
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, //B
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, //C
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, //D
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, //E
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }; //F
	return sbox[(int)num];
}


// Tabla para la constante de ronda, el array empieza en 1 en lugar de en 0
int Rcon[255] = {
	// 0     1     2     3     4     5     6     7     8     9     A     B     C     D     E     F
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
	0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
	0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
	0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
	0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
	0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
	0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
	0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
	0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
	0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
	0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
	0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
	0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
	0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
	0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
	0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb  };


// Esta funcion genera las claves de ronda a partir de la clave inicial.
void keyExpansion()
{
	int i,j;
	byte temp[4],k;

	// La primera subclave es la clave inicial
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			round_key[i][j] = key[i][j];
		}
	}

	// Generamos el resto de subclaves de ronda
	while (j<(Nr*4)){

		// Copiamos la columna anterior
		for(i=0;i<4;i++){
			temp[i]=round_key[i][j-1];
		}

		if(i % Nk == 0){
			// Rotword de la columna
			k = temp[0];
			temp[0] = temp[1];
			temp[1] = temp[2];
			temp[2] = temp[3];
			temp[3] = k;

			// SubBytes de la columna
			temp[0]=getSBoxValue(temp[0]);
			temp[1]=getSBoxValue(temp[1]);
			temp[2]=getSBoxValue(temp[2]);
			temp[3]=getSBoxValue(temp[3]);

			// Columna actual XOR Constante de ronda
			temp[0] = temp[0] ^ Rcon[i/Nk];
		}

		else if (Nk > 6 && i % Nk == 4){
			// SubBytes a la columna
			temp[0]=getSBoxValue(temp[0]);
			temp[1]=getSBoxValue(temp[1]);
			temp[2]=getSBoxValue(temp[2]);
			temp[3]=getSBoxValue(temp[3]);
		}
		round_key[0][j] = round_key[0][j-Nk] ^ temp[0];
		round_key[1][j] = round_key[1][j-Nk] ^ temp[1];
		round_key[2][j] = round_key[2][j-Nk] ^ temp[2];
		round_key[3][j] = round_key[3][j-Nk] ^ temp[3];
		j++;
	}
}

// Funcion AddRoundKey del estandar AES
void AddRoundKey(int round)
{
	int i,j;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			state[i][j] ^= round_key[i][round*4+j];
		}
	}
}

// Funcion AddRoundKey con parametro bloque del estandar AES
void AddRoundKey(int round, byte bloque[4][4])
{
	int i,j;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			bloque[i][j] ^= round_key[i][round*4+j];
		}
	}
}

// Funcion SubBytes del estandar AES
void SubBytes()
{
	int i,j;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			state[i][j] = getSBoxValue(state[i][j]);
		}
	}
}

// Funcion SubBytes con parametro bloque del estandar AES
void SubBytes(byte bloque[4][4])
{
	int i,j;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			bloque[i][j] = getSBoxValue(bloque[i][j]);
		}
	}
}

// Funcion ShiftRows del estandar AES
void ShiftRows()
{
	byte temp;

	// Rotate first row 1 columns to left
	temp=state[1][0];
	state[1][0]=state[1][1];
	state[1][1]=state[1][2];
	state[1][2]=state[1][3];
	state[1][3]=temp;

	// Rotate second row 2 columns to left
	temp=state[2][0];
	state[2][0]=state[2][2];
	state[2][2]=temp;

	temp=state[2][1];
	state[2][1]=state[2][3];
	state[2][3]=temp;

	// Rotate third row 3 columns to left
	temp=state[3][0];
	state[3][0]=state[3][3];
	state[3][3]=state[3][2];
	state[3][2]=state[3][1];
	state[3][1]=temp;
}

// Funcion ShiftRows con parametro bloque del estandar AES
void ShiftRows(byte bloque[4][4])
{
	byte temp;

	// Rotate first row 1 columns to left
	temp=bloque[1][0];
	bloque[1][0]=bloque[1][1];
	bloque[1][1]=bloque[1][2];
	bloque[1][2]=bloque[1][3];
	bloque[1][3]=temp;

	// Rotate second row 2 columns to left
	temp=bloque[2][0];
	bloque[2][0]=bloque[2][2];
	bloque[2][2]=temp;

	temp=bloque[2][1];
	bloque[2][1]=bloque[2][3];
	bloque[2][3]=temp;

	// Rotate third row 3 columns to left
	temp=bloque[3][0];
	bloque[3][0]=bloque[3][3];
	bloque[3][3]=bloque[3][2];
	bloque[3][2]=bloque[3][1];
	bloque[3][1]=temp;
}

// Funcion MixColumns del estandar AES
void MixColumns()
{
	int i;
	byte Tmp,Tm,t;
	for(i=0;i<4;i++){
		t=state[0][i];
		Tmp = state[0][i] ^ state[1][i] ^ state[2][i] ^ state[3][i] ;
		Tm = state[0][i] ^ state[1][i] ; Tm = xtime(Tm); state[0][i] ^= Tm ^ Tmp ;
		Tm = state[1][i] ^ state[2][i] ; Tm = xtime(Tm); state[1][i] ^= Tm ^ Tmp ;
		Tm = state[2][i] ^ state[3][i] ; Tm = xtime(Tm); state[2][i] ^= Tm ^ Tmp ;
		Tm = state[3][i] ^ t ; Tm = xtime(Tm); state[3][i] ^= Tm ^ Tmp ;
	}
}

// Funcion MixColumns con parametro bloque del estandar AES
void MixColumns(byte bloque[4][4])
{
	int i;
	byte Tmp,Tm,t;
	for(i=0;i<4;i++){
		t=bloque[0][i];
		Tmp = bloque[0][i] ^ bloque[1][i] ^ bloque[2][i] ^ bloque[3][i] ;
		Tm = bloque[0][i] ^ bloque[1][i] ; Tm = xtime(Tm); bloque[0][i] ^= Tm ^ Tmp ;
		Tm = bloque[1][i] ^ bloque[2][i] ; Tm = xtime(Tm); bloque[1][i] ^= Tm ^ Tmp ;
		Tm = bloque[2][i] ^ bloque[3][i] ; Tm = xtime(Tm); bloque[2][i] ^= Tm ^ Tmp ;
		Tm = bloque[3][i] ^ t ; Tm = xtime(Tm); bloque[3][i] ^= Tm ^ Tmp ;
	}
}

void cifrar(byte bloque[4][4])
{
	// Suponemos longitud de clave de 128 bits
	Nr = 128;
	// Calculamos el numero de rondas y las palabras de 32 bits de la clave.
	Nk = Nr / 32;
	Nr = Nk + 6;

	// Primer AddRoundKey, antes de las rondas
	AddRoundKey(0,bloque);

	// Las primeras Nr-1 rondas son identicas
	for(int i=1;i<Nr;i++){
		SubBytes(bloque);
		ShiftRows(bloque);
		MixColumns(bloque);
		AddRoundKey(i,bloque);
	}

	// La ultima ronda no tiene la funcion MixColumns
	SubBytes(bloque);
	ShiftRows(bloque);
	AddRoundKey(Nr,bloque);
}

// Carga el bloque nbloq en la matriz de estado y devuelve true si se
// ha llegado al final del fichero o false en caso contrario
bool cargar_bloque(Msg &archivo, byte bloque[4][4], int nbloq)
{
	int bytes_count = nbloq*16;
	bool end = false;

	// Cargamos los bits en la matriz
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			if(bytes_count < archivo.GetSize()){
				bloque[i][j] = archivo.Getbyte(bytes_count);
				bytes_count++;
			}
			else{
				bloque[i][j] = 0x00;
				end = true;
			}
		}
	}
	return end;
}

// Guarda el bloque nbloq en el array de cifrado
void guardar_bloque(byte* dest, byte bloque[4][4], int nbloq)
{
	int byte_inic = nbloq*16;

	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			dest[byte_inic] = bloque[i][j];
			byte_inic++;
		}
	}
}

void mostrar_hex(byte* salida, int tam)
{
	for(int i=0;i<tam;i++){
		printf("%x ",salida[i]);
	}
}

int main(int argc, char** argv)
{
	Msg key_file;
	Msg plain_file;
	Msg output_file;
	string output;
	byte* salida;
	int i,j,k,tid,tids,tam,bloq_count,bloq_max,nthreads;
	cout<<"Numero de argumentos: "<<argc<<endl;
	if(argc != 5){
		cout<<"Introduce el nombre del archivo a cifrar: ";
		cin>>plain_filename;
		cout<<"Introduce el nombre del archivo de clave: ";
		cin>>key_filename;
		cout<<"Introduce el nombre del archivo de salida: ";
		cin>>output_filename;
		cout<<"Introduce el numero de hilos de ejecucion: (<= 0 -> Automatico): ";
		cin>>nthreads;
	}
	else{
		plain_filename = argv[1];
		key_filename = argv[2];
		output_filename = argv[3];
		nthreads = atoi(argv[4]);
	}

	// Suponemos longitud de clave de 128 bits
	Nr = 128;
	// Calculamos el numero de rondas y las palabras de 32 bits de la clave.
	Nk = Nr / 32;
	Nr = Nk + 6;

	// Abrimos el fichero a cifrar
	plain_file.LoadFromFile(plain_filename);
	// Abrimos la clave
	key_file.LoadFromFile(key_filename);
    // Copiamos el fichero a cifrar como salida
    output_file.LoadFromFile(plain_filename);

	// Establecemos el numero de hilos
	if(nthreads > 0){
		omp_set_num_threads(nthreads);
	}

	// Cargamos la clave en la matriz
	k = 0;
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			key[i][j] = key_file.Getbyte(k);
			k++;
		}
	}

	// Reservamos espacio para el stream de salida
	tam = ((plain_file.GetSize()/16)+1)*16;
	salida = new byte[tam];

	// Expandimos la clave para obtener las subclaves de cada ronda
	keyExpansion();

	// Contador de bloques
	bloq_max = tam/16;

	#pragma omp parallel private(state,bloq_count,tid,tids) shared(salida,plain_file,bloq_max)
	{
		bloq_count = tid = omp_get_thread_num();
		tids = omp_get_num_threads();

		if(tid == 0){
			cout<<"Ejecutando el cifrador con "<<tids<<" hilos..."<<endl;
		}
		while(bloq_count < bloq_max){
			cargar_bloque(plain_file,state,bloq_count);
			cifrar(state);
			guardar_bloque(salida,state,bloq_count);
			bloq_count+=tids;
		}
	}

	cout<<"Guardando archivo de "<<tam<<" bytes..."<<endl;
	// Si se descomenta la linea de abajo, se puede ver por pantalla el resultado del cifrado
	//~ mostrar_hex(salida,tam);
	output_file.SetMessage(salida,tam);
	output_file.SaveToFile(output_filename);
	return 0;
}
