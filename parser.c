#include "parser.h"


int main(int argc, char *argv[])
{
	init_parser(argc, argv);
	
	unidad_traduccion();

	match(CEOF, 9);

	last_call=1;

	error_handler(COD_IMP_ERRORES);

	return 0;
}


/********* funciones del parser ***********/

void unidad_traduccion()
{
	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaraciones();
}


void declaraciones()
{	
	especificador_tipo();
	
	match(CIDENT, 17);
	
	especificador_declaracion();
}


void especificador_tipo()
{	
	switch(lookahead())
	{
		case CVOID:
			scanner();
			break;
			
		case CCHAR:
			scanner();
			break;
			
		case CINT:
			scanner();
			break;
			
		case CFLOAT:
			scanner();
			break;
			
		default:
			error_handler(18);
	}
}


void especificador_declaracion()
{	
	switch(lookahead())
	{
		case CPAR_ABR:
			definicion_funcion();
			break;
		
		case CASIGNAC:
		case CCOR_ABR:
		case CCOMA:
		case CPYCOMA:
			declaracion_variable();
			break;
		
		default:
			error_handler(19);
	}
}


void definicion_funcion()
{
	match(CPAR_ABR, 20);

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones_param();

	match(CPAR_CIE, 21);

	proposicion_compuesta();
}


void lista_declaraciones_param()
{
	declaracion_parametro();

	while(lookahead_in(CCOMA))
	{
		scanner();
		declaracion_parametro();
	}
}


void declaracion_parametro()
{
	especificador_tipo();

	if(lookahead_in(CAMPER))
		scanner();

	match(CIDENT, 17);

	if(lookahead_in(CCOR_ABR))
	{
		scanner();
		match(CCOR_CIE, 22);
	}
}


void lista_declaraciones_init()
{
	match(CIDENT, 17);

	declarador_init();

	while(lookahead_in(CCOMA))
	{
		scanner();
		match(CIDENT, 17);
		declarador_init();
	}
}


void declaracion_variable()
{
	declarador_init();

	if(lookahead_in(CCOMA))
	{
		scanner();
		lista_declaraciones_init();
	}

	match(CPYCOMA, 23);
}


void declarador_init()
{
	switch(lookahead())
	{
		case CASIGNAC:
			scanner();
			constante();
			break;

		case CCOR_ABR:
			scanner();
			
			if(lookahead_in(CCONS_ENT))
				constante();

			match(CCOR_CIE, 22);

			if(lookahead_in(CASIGNAC))
			{
				scanner();
				match(CLLA_ABR, 24);
				lista_inicializadores();
				match(CLLA_CIE, 25);
			}
			break;
	}
}


void lista_inicializadores()
{
	constante();

	while(lookahead_in(CCOMA))
	{
		scanner();
		constante();
	}
}


void proposicion_compuesta()
{
	match(CLLA_ABR, 24);

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones();

	if(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		lista_proposiciones();

	match(CLLA_CIE, 25);
}


void lista_declaraciones()
{
	declaracion(); 

	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		declaracion(); 
}


void declaracion()
{
	especificador_tipo();

	lista_declaraciones_init();

	match(CPYCOMA, 23);
}


void lista_proposiciones()
{
	proposicion();

	while(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		proposicion();
}


void proposicion()
{
	switch(lookahead())
	{
		case CLLA_ABR:
			proposicion_compuesta();
			break;
		
		case CWHILE:
			proposicion_iteracion();
			break;
		
		case CIF:
			proposicion_seleccion();
			break;
		
		case CIN:
		case COUT:
			proposicion_e_s();
			break;
		
		case CMAS:
		case CMENOS:
		case CIDENT:
		case CPAR_ABR:
		case CNEG:
		case CCONS_ENT:
		case CCONS_FLO:
		case CCONS_CAR:
		case CCONS_STR:
		case CPYCOMA:
			proposicion_expresion();
			break;
		
		case CRETURN:
			proposicion_retorno();
			break;
		
		default:
			error_handler(26);
	}
}


void proposicion_iteracion()
{
	match(CWHILE, 27);

	match(CPAR_ABR, 20);

	expresion();

	match(CPAR_CIE, 21);

	proposicion();
}


void proposicion_seleccion()
{
	match(CIF, 28);

	match(CPAR_ABR, 20);

	expresion();

	match(CPAR_CIE, 21);

	proposicion();

	if(lookahead_in(CELSE))
	{
		scanner();
		proposicion();
	}
}


void proposicion_e_s()
{
	switch(lookahead())
	{
		case CIN:
			scanner();
			
			match(CSHR, 30);
			
			variable();
			
			while(lookahead_in(CSHR))
			{
				scanner();
				variable();
			}

			match(CPYCOMA, 23);
			
			break;
		
		case COUT:
			scanner();

			match(CSHL, 31);
			
			expresion();

			while(lookahead_in(CSHL))
			{
				scanner();
				expresion();
			}

			match(CPYCOMA, 23);
			
			break;
		
		default:
			error_handler(26);
	}
}


void proposicion_retorno()
{
	scanner();
	
	expresion();
	
	match(CPYCOMA, 23);
}


void proposicion_expresion()
{
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		expresion();

	match(CPYCOMA, 23);
}


void expresion()
{	
	expresion_simple();

	while(lookahead_in(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG))
	{
		switch(lookahead())
		{
			case CASIGNAC:
				scanner();
				expresion_simple();
				break;
				
			case CDISTINTO:
			case CIGUAL:
			case CMENOR:
			case CMEIG:
			case CMAYOR:
			case CMAIG:
				scanner();
				expresion_simple();
				break;
		}
	}
}


void expresion_simple()
{
	if(lookahead_in(CMAS | CMENOS))
		scanner();

	termino();

	while(lookahead_in(CMAS | CMENOS | COR))
	{
		scanner();
		termino();
	}
}


void termino()
{	
	factor();

	while(lookahead_in(CMULT | CDIV | CAND))
	{
		scanner();
		factor();
	}
}


void factor()
{
	switch(lookahead())
	{
		case CIDENT:
			/***************** Re-hacer *****************/
			if(sbol->lexema[0] == 'f')
				llamada_funcion();
			else
				variable();
			/********************************************/
			/* El alumno debera evaluar con consulta a TS
			si bifurca a variable o llamada a funcion */
			break;
		
		case CCONS_ENT:
		case CCONS_FLO:
		case CCONS_CAR:
			constante();
			break;
		
		case CCONS_STR:
			scanner();
			break;
		
		case CPAR_ABR:
			scanner();
			expresion();
			match(CPAR_CIE, 21);
			break;
			
		case CNEG:
			scanner();
			expresion();
			break;
			
		default:
			error_handler(32);
	}
}


void variable()
{
	match(CIDENT, 17);

	/* El alumno debera verificar con una consulta a TS
	si, siendo la variable un arreglo, corresponde o no
	verificar la presencia del subindice */

	if(lookahead_in(CCOR_ABR))
	{
		scanner();
		expresion();
		match(CCOR_CIE, 22);
	}
}


void llamada_funcion()
{
	match(CIDENT, 17);
	
	match(CPAR_ABR, 20);
	
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		lista_expresiones();

	match(CPAR_CIE, 21);
}


void lista_expresiones()
{
	expresion();

	while(lookahead_in(CCOMA))
	{
		scanner();
		expresion();
	}
}


void constante()
{
	switch(lookahead())
	{
		case CCONS_ENT:
			scanner();
			break;
		
		case CCONS_FLO:
			scanner();
			break;
		
		case CCONS_CAR:
			scanner();
			break;
		
		default:
			error_handler(33);
	}
}
