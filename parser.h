#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codigos.h"
#include "util.h"
#include "error.h"
#include "ts.h"

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
