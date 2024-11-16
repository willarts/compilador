#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codigos.h"
#include "util.h"
#include "error.h"
#include "ts.h"
#include "sistejec.h"

int last_call=0;

/*********** prototipos *************/

void unidad_traduccion();
void declaraciones();
void especificador_tipo();
void especificador_declaracion();
void definicion_funcion();
void declaracion_variable();
void lista_declaraciones_param();
void declaracion_parametro();
int declarador_init();
void lista_declaraciones_init();
int lista_inicializadores();
void lista_proposiciones();
void lista_declaraciones();
void declaracion();
atributos_sintetizados proposicion();
void proposicion_expresion();
void proposicion_compuesta();
void proposicion_seleccion();
void proposicion_iteracion();
void proposicion_e_s();
atributos_sintetizados proposicion_retorno();
atributos_sintetizados expresion();
atributos_sintetizados expresion_simple();
atributos_sintetizados termino();
atributos_sintetizados factor();
atributos_sintetizados variable();
void llamada_funcion();
int lista_expresiones();
atributos_sintetizados constante();
void mostrarLista();

float CODE[TAM_PROG];
int lcode = 0;

char CS[TAM_CTES];
int lcs = 0;   
char CSAUX[TAM_CTES];

int desplazamiento=0;



void impr_codgen2()
{
	int i = 0;
	while(CODE[i] != PARAR)
	{
		switch((int) CODE[i])
		{
			case CRCT:
				printf("CRCT ");
				i++;
				
				if(CODE[i+1] == 0)
					printf("%c", (char) CODE[i++]);
            	else
					if(CODE[i+1] == 1)
						printf("%d", (int) CODE[i++]);
               		else
						printf("%.2f", (float) CODE[i++]);
				printf(" %d\n", (int) CODE[i++]);
				break;
				

			case CRVL:
				printf("CRVL ");
				i++;
				printf("%d, ", (int) CODE[i++]);
				printf("%d, ", (int) CODE[i++]);
				printf("%d\n", (int) CODE[i++]);
				break;

			case SUM:
				printf("SUM ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case SUB:
				printf("SUB ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case MUL:
				printf("MUL ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case DIV:
				printf("DIV ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case INV:
				printf("INV ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case AND:
				printf("AND ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case OR:
				printf("OR ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case NEG:
				printf("NEG ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case CMMA:
				printf("CMMA ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case CMME:
				printf("CMME ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case CMIG:
				printf("CMIG ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case CMAI:
				printf("CMAI ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case CMEI:
				printf("CMEI ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case CMNI:
				printf("CMNI ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case CAST:
				printf("CAST ");
				i++;
				printf("%d, ", (int) CODE[i++]);
				printf("%d\n", (int) CODE[i++]);
				break;

			case ALM:
				printf("ALM ");
				i++;
				printf("%d, ", (int) CODE[i++]);
				printf("%d, ", (int) CODE[i++]);
				printf("%d\n", (int) CODE[i++]);
				break;

			case LEER:
				printf("LEER ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case IMPR:
				printf("IMPR ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case BIFS:
				printf("BIFS ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case BIFF:
				printf("BIFF ");
				i++;
				printf("%d, ", (int) CODE[i++]);
				printf("%d\n", (int) CODE[i++]);
				break;

			case INPP:
				printf("INPP \n");
				i++;
				break;

			case PARAR:
				printf("PARAR \n");
				i++;
				break;

			case ALOC:
				printf("ALOC ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case DMEM:
				printf("DMEN ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case IMPRCS:
				printf("IMPRCS \n");
				i++;
				break;

			case ENBL:
				printf("ENBL ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case FINB:
				printf("FINB ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;

			case CRCTS:
				printf("CRCTS ");
				i++;
				printf("%d\n", (int) CODE[i++]);
				break;
		}
	}        
	printf("PARAR");
}

int tamTipo (int tipo){
	int tam=0;
	switch (tipo)
		{
			case TCHAR:
				tam = 1;
				break;
			case TINT:
				tam = sizeof(int);
				break;
			case TFLOAT:
				tam = sizeof(float);
				break;
		}
	return tam;
}
//int GEN= 1;