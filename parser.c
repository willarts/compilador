#include "parser.h"

int main(int argc, char *argv[])
{
	init_parser(argc, argv);

	inic_tablas();
	
	unidad_traduccion(CEOF);

	match(CEOF, 9);

	last_call=1;

	error_handler(COD_IMP_ERRORES);

	return 0;
}


/********* funciones del parser ***********/

void unidad_traduccion(set folset)
{	

	pushTB();
	mostrar_tabla();
	test(CVOID | CCHAR | CINT | CFLOAT | folset, NADA ,40);
	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT)){
		declaraciones(folset | CVOID | CCHAR | CINT | CFLOAT);
		test(CVOID | CCHAR | CINT | CFLOAT | folset, NADA ,44);
	}
	pop_nivel();
}


void declaraciones(set folset)
{	
	especificador_tipo(folset | CIDENT | CPAR_ABR | CASIGNAC | CCOR_ABR | CCOMA | CPYCOMA);
	
	strcpy(inf_id->nbre, lexema());
	match(CIDENT, 17);
	
	
	especificador_declaracion(folset);
}


void especificador_tipo(set folset)
{
	test(CVOID| CCHAR | CINT | CFLOAT, folset, 41);	
	switch(lookahead())
	{
		case CVOID:
			scanner();
			inf_id->ptr_tipo = en_tabla("void");
			break;
			
		case CCHAR:
			scanner();
			inf_id->ptr_tipo = en_tabla("char");
			break;
			
		case CINT:
			scanner();
			inf_id->ptr_tipo = en_tabla("int");
			break;
			
		case CFLOAT:
			scanner();
			inf_id->ptr_tipo = en_tabla("float");
			break;
			
		default:
			error_handler(18);
	}
	test(folset, NADA, 42);	
}


void especificador_declaracion(set folset)
{	
	test(CPAR_ABR | CASIGNAC | CCOR_ABR | CCOMA | CPYCOMA, folset, 43);//TODO: se puede configurar
	switch(lookahead())
	{
		case CPAR_ABR:
			definicion_funcion(folset);
			break;
		
		case CASIGNAC:
		case CCOR_ABR:
		case CCOMA:
		case CPYCOMA:
			declaracion_variable(folset);
			break;
		
		default:
			error_handler(19);
	}
}


void definicion_funcion(set folset)
{
	match(CPAR_ABR, 20);

	pushTB();// abre bloque de funcion.
	
	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT ))
		lista_declaraciones_param(folset | CPAR_CIE | CLLA_ABR);

	match(CPAR_CIE, 21);

	proposicion_compuesta(folset);

	pop_nivel(); //cierra el nivel.
}


void lista_declaraciones_param(set folset)
{
	declaracion_parametro(folset | CCOMA | CVOID | CCHAR | CINT | CFLOAT );
	test(CCOMA | folset, CVOID | CCHAR | CINT | CFLOAT, 45);
	while(lookahead_in(CCOMA | CVOID | CCHAR | CINT | CFLOAT))
	{
		match(CCOMA, 64);
		declaracion_parametro(folset | CCOMA | CVOID | CCHAR | CINT | CFLOAT);
		test(CCOMA | folset,  CVOID | CCHAR | CINT | CFLOAT, 45);
	}
}


void declaracion_parametro(set folset)
{
	especificador_tipo(folset | CAMPER | CIDENT | CCOR_ABR | CCOR_CIE);
	//test(CAMPER|CIDENT|CCOR_ABR,folset,??);
	if(lookahead_in(CAMPER))
		scanner();
		
	match(CIDENT, 17);

	if(lookahead_in(CCOR_ABR | CCOR_CIE))
	{
		match(CCOR_ABR, 35);
		match(CCOR_CIE, 22);
	}
}


void lista_declaraciones_init(set folset, int stipo)
{
	test(CIDENT, folset | CASIGNAC | CCOR_ABR | CCOMA , 46);
	
	strcpy(inf_id->nbre, lexema());
	match(CIDENT, 17);

	int isArr =declarador_init(folset | CCOMA | CIDENT | CASIGNAC | CCOR_ABR);
	if(!isArr){
		inf_id->clase = CLASVAR;
	}
	else{
		inf_id->clase = CLASVARSTRUCT;
		inf_id->ptr_tipo = en_tabla("TIPOARREGLO");
		inf_id->desc.part_var.arr.ptero_tipo_base = stipo;
	}
	inf_id->desc.nivel = get_nivel();
	insertarTS();
	mostrar_tabla();

	test(CCOMA | folset, CIDENT | CASIGNAC | CCOR_ABR, 48);
	while(lookahead_in(CCOMA | CIDENT | CASIGNAC | CCOR_ABR))
	{
		match(CCOMA, 64);
		strcpy(inf_id->nbre, lexema());
		match(CIDENT, 17);
		int isArr = declarador_init(folset | CCOMA | CIDENT | CASIGNAC | CCOR_ABR);
		if(!isArr){
			inf_id->clase = CLASVAR;
		}
		else{
			inf_id->clase = CLASVARSTRUCT;
			inf_id->ptr_tipo = en_tabla("TIPOARREGLO");
			inf_id->desc.part_var.arr.ptero_tipo_base = stipo;
		}
		inf_id->desc.nivel = get_nivel();
		insertarTS();
		mostrar_tabla();
		test(CCOMA | folset, CIDENT | CASIGNAC | CCOR_ABR, 48);
	}
}


void declaracion_variable(set folset)
{
	int stipo = inf_id->ptr_tipo;
	int isArr = declarador_init(folset | CCOMA | CIDENT | CPYCOMA );
	if(!isArr){
		inf_id->clase = CLASVAR;
	}
	else {
		inf_id->clase = CLASVARSTRUCT;
		inf_id->ptr_tipo = en_tabla("TIPOARREGLO");
		inf_id->desc.part_var.arr.ptero_tipo_base = stipo;
	}
	inf_id->desc.nivel = get_nivel();
	insertarTS();
	mostrar_tabla();
	if(lookahead_in(CCOMA | CIDENT))
	{
		match(CCOMA,64);
		lista_declaraciones_init(folset | CPYCOMA, stipo);
	}

	match(CPYCOMA, 23);
	test(folset, NADA, 51);
}


int declarador_init(set folset)
{
	int isArr=0;
	test(CASIGNAC | CCOR_ABR | CCOMA | CPYCOMA | folset, CCOR_CIE | CLLA_ABR | CLLA_CIE | CCONS_FLO | CCONS_CAR , 47);
	switch(lookahead())
	{	
		case CCONS_FLO:
		case CCONS_CAR:
		case CASIGNAC:
			match(CASIGNAC, 66);
			constante(folset);
			break;

		case CCOR_ABR:
		case CCOR_CIE:
		case CLLA_ABR:
		case CLLA_CIE:
			isArr=1;
			 match(CCOR_ABR, 35);	
			char dimension  [20];
			if(lookahead_in(CCONS_ENT))
				 strcpy(dimension, constante(folset | CCOR_CIE | CASIGNAC | CLLA_ABR | CCONS_ENT | CCONS_FLO | CCONS_CAR | CLLA_CIE));
			int entero = (int) strtol(dimension, NULL, 10);
			
			match(CCOR_CIE, 22);
			int cant = 0;
			if(lookahead_in(CASIGNAC | CLLA_ABR | CLLA_CIE | CCONS_ENT | CCONS_FLO | CCONS_CAR))
			{
				match(CASIGNAC, 66);
				match(CLLA_ABR, 24);
				cant = lista_inicializadores(folset | CLLA_CIE);
				match(CLLA_CIE, 25);
			}
			if(entero > cant){
				inf_id->desc.part_var.arr.cant_elem = entero;
			}
			else{
				inf_id->desc.part_var.arr.cant_elem = cant;
			}
			break;
	}

	test(folset, NADA, 48);
	return isArr;
}

int lista_inicializadores(set folset)
{
	int cantidad_cosntantes=0;
	constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);
	cantidad_cosntantes++;
	test(CCOMA | folset, CCONS_ENT | CCONS_FLO | CCONS_CAR, 63);
	while(lookahead_in(CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR))
	{
		match(CCOMA, 64);
		constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);
		cantidad_cosntantes++;
		test(CCOMA | folset , CCONS_ENT | CCONS_FLO | CCONS_CAR, 63);
	}
	return cantidad_cosntantes;
}


void proposicion_compuesta(set folset)
{
	test(CLLA_ABR, folset | CVOID | CCHAR | CINT | CFLOAT  | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CLLA_CIE, 49);

	match(CLLA_ABR, 24);

	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CLLA_CIE);

	if(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		lista_proposiciones(folset | CLLA_CIE);

	match(CLLA_CIE, 25);
	test(folset, NADA, 50);
}


void lista_declaraciones(set folset)
{
	declaracion(folset | CVOID | CCHAR | CINT | CFLOAT); 
	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT)){
		declaracion(folset | CVOID | CCHAR | CINT | CFLOAT); 
	}
}


void declaracion(set folset)
{
	especificador_tipo(folset | CIDENT | CPYCOMA);

	lista_declaraciones_init(folset | CPYCOMA, inf_id->ptr_tipo);

	match(CPYCOMA, 23);

	test(folset, NADA, 51);
}


void lista_proposiciones(set folset)
{
	proposicion(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN);
	test(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN, NADA, 68);
	while(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
    {
		proposicion(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN);
		test(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN, NADA, 68);
	}
}


void proposicion(set folset)
{
	test(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN, folset, 52);
	switch(lookahead())
	{
		case CLLA_ABR:
			proposicion_compuesta(folset);
			break;
		
		case CWHILE:
			proposicion_iteracion(folset);
			break;
		
		case CIF:
			proposicion_seleccion(folset);
			break;
		
		case CIN:
		case COUT:
			proposicion_e_s(folset);
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
			proposicion_expresion(folset);
			break;
		
		case CRETURN:
			proposicion_retorno(folset);
			break;
		
		default:
			error_handler(29);
	}
}


void proposicion_iteracion(set folset)
{
	match(CWHILE, 27);

	match(CPAR_ABR, 20);

	expresion(folset | CPAR_CIE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN );

	match(CPAR_CIE, 21);
	
	pushTB();// abre el bloque de while

	proposicion(folset);

	pop_nivel();// cierra el bloque del While
}


void proposicion_seleccion(set folset)
{
	match(CIF, 28);

	match(CPAR_ABR, 20);

	expresion(folset | CPAR_CIE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CELSE);

	match(CPAR_CIE, 21);

	pushTB();// abre bloque de if
	
	proposicion(folset | CELSE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN );

	pop_nivel(); // Cierra el bloque del if
	if(lookahead_in(CELSE))
	{
		scanner();
		pushTB(); // TODO PReguntar si es asi 
		proposicion(folset);
		pop_nivel(); 
	}

	
}


void proposicion_e_s(set folset)
{
	switch(lookahead())
	{
		case CIN:
			scanner();
			
			match(CSHR, 30);
			
			variable(folset | CSHR | CIDENT | CPYCOMA);

			while(lookahead_in(CSHR))
			{
				scanner();
				variable(folset |CSHR | CIDENT | CPYCOMA);
			}

			match(CPYCOMA, 23);
			
			break;
		
		case COUT:
			scanner();

			match(CSHL, 31);
			
			expresion(folset | CSHL | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPYCOMA);

			while(lookahead_in(CSHL))
			{
				scanner();
				expresion(CSHL |CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPYCOMA);
			}

			match(CPYCOMA, 23);
			
			break;
		
		default:
			error_handler(29);
	}
	test(folset , NADA, 53);
}


void proposicion_retorno(set folset)
{
	scanner();
	
	expresion(folset | CPYCOMA);
	
	match(CPYCOMA, 23);

	test(folset, NADA, 54);
}


void proposicion_expresion(set folset)
{

	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		expresion(folset | CPYCOMA);

	match(CPYCOMA, 23);

	test(folset, NADA, 55);
}


void expresion(set folset)
{	
	expresion_simple(folset | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
	test(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | folset, NADA, 69);
	while(lookahead_in(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG))
	{
		switch(lookahead())
		{
			case CASIGNAC:
				scanner();
				expresion_simple(folset | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
				break;
				
			case CDISTINTO:
			case CIGUAL:
			case CMENOR:
			case CMEIG:
			case CMAYOR:
			case CMAIG:
				scanner();
				expresion_simple(folset | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
				break;
		}
		test(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | folset, NADA, 69);
	}
}


void expresion_simple(set folset)
{
	test(CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR, folset | COR, 56);
	if(lookahead_in(CMAS | CMENOS))
		scanner();
		
	termino(folset | CMAS | CMENOS |COR | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
	test(CMAS | CMENOS | COR | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR, 65);
	while(lookahead_in(CMAS | CMENOS | COR | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR))
	{
		switch (lookahead())
		{
		case CMAS:
		case CMENOS:
		case COR:
			scanner();
			break;
		default:
			error_handler(65);
			break;
		}
		termino(folset | CMAS | CMENOS | COR | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
		test(CMAS | CMENOS | COR | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR, 65);
	}
}


void termino(set folset)
{	
	factor(folset | CMULT | CDIV | CAND | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG);
	test(CMULT | CDIV | CAND | folset , CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG , 65 );
	while(lookahead_in(CMULT | CDIV | CAND | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG))
	{
		switch (lookahead())
		{
		case CMULT:
		case CDIV:
		case CAND:
			scanner();
			break;
		default:
			error_handler(65);
			break;
		}
		factor(folset | CMULT | CDIV | CAND | CMULT | CDIV | CAND | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG);
		//factor hace test final
	}
}


void factor(set folset)
{
	test(CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG, folset, 57);
	switch(lookahead())
	{
		case CIDENT:
			/***************** Re-hacer *****************/
			if(sbol->lexema[0] == 'f')
				llamada_funcion(folset);
			else
				variable(folset);
			/********************************************/
			/* El alumno debera evaluar con consulta a TS
			si bifurca a variable o llamada a funcion */
			break;
		
		case CCONS_ENT:
		case CCONS_FLO:
		case CCONS_CAR:
			constante(folset);
			break;
		
		case CCONS_STR:
			scanner();
			break;
		
		case CPAR_ABR:
			match(CPAR_ABR, 20);
			expresion(folset | CPAR_CIE);
			match(CPAR_CIE, 21);
			break;
			
		case CNEG:
			scanner();
			expresion(folset);
			break;
			
		default:
			error_handler(32);
	}
	test(folset, NADA, 58);
}


void variable(set folset)
{
	test(CIDENT, folset | CCOR_ABR | CCOR_CIE | CMAS | CMENOS | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR, 59);
	match(CIDENT, 17);

	/* El alumno debera verificar con una consulta a TS
	si, siendo la variable un arreglo, corresponde o no
	verificar la presencia del subindice */

	if(lookahead_in(CCOR_ABR))
	{
		match(CCOR_ABR, 35);
		expresion(folset | CCOR_CIE);
		match(CCOR_CIE, 22);
	}
	test(folset,NADA, 60);
}


void llamada_funcion(set folset)
{
	match(CIDENT, 17);
	
	match(CPAR_ABR, 20);
	
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		lista_expresiones(folset | CPAR_CIE);

	match(CPAR_CIE, 21);
	test(folset, NADA, 61);
}


void lista_expresiones(set folset)
{
	expresion(folset| CCOMA | CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
	//test(CCOMA | folset,NADA,??);
	while(lookahead_in(CCOMA | CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR))
	{
		match(CCOMA,64);
		expresion(folset| CCOMA | CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
		//test(CCOMA | folset,NADA,??);
	}
}


char * constante(set folset)
{
	char res[10];
	
	
	test(CCONS_ENT | CCONS_FLO | CCONS_CAR, folset, 62);
	char *re = (char *) calloc(20, sizeof(char));
	strcpy(re, lexema());
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
	test(folset, NADA, 63);
	//re= res;
	
	return re;
}
