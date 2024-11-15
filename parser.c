#include "parser.h"
int mainDeclarado = 0;
int GEN = 1;
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
	if(GEN)
		CODE[lcode++] = INPP;

	pushTB();
	if(GEN){
		CODE[lcode++] = ENBL;
		CODE[lcode++] = get_nivel();
	}
	desplazamiento = 0;
	

	test(CVOID | CCHAR | CINT | CFLOAT | folset, NADA ,40);
	while(lookahead_in(CVOID | CCHAR | CINT | CFLOAT)){
		declaraciones(folset | CVOID | CCHAR | CINT | CFLOAT);
		test(CVOID | CCHAR | CINT | CFLOAT | folset, NADA ,44);
	}
	if(en_tabla("main") == NIL)
		error_handler(84);
	if(GEN){
		CODE[lcode++] = FINB;
		CODE[lcode++] = get_nivel();
	}

	pop_nivel();

		
	if (GEN) {
		CODE[lcode++] = PARAR;
		impr_codgen2();
	}
	
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
			inf_id->ptr_tipo = TVOID;
			break;
			
		case CCHAR:
			scanner();
			inf_id->ptr_tipo = TCHAR;
			break;
			
		case CINT:
			scanner();
			inf_id->ptr_tipo = TINT;
			break;
			
		case CFLOAT:
			scanner();
			inf_id->ptr_tipo = TFLOAT;
			break;
			
		default:
			error_handler(18);
	}
	test(folset, NADA, 42);	
}


void especificador_declaracion(set folset)
{	
	int isMain = strcmp(inf_id->nbre,"main") == 0;
	test(CPAR_ABR | CASIGNAC | CCOR_ABR | CCOMA | CPYCOMA, folset, 43);
	switch(lookahead())
	{
		case CPAR_ABR:
			if( isMain && inf_id->ptr_tipo != 0)
				error_handler(85);
			if(mainDeclarado && isMain)
				error_handler(106);
			if(isMain && !mainDeclarado)
				mainDeclarado = isMain;

			definicion_funcion(folset);
			break;
		
		case CASIGNAC:
		case CCOR_ABR:
		case CCOMA:
		case CPYCOMA:
			if(isMain)
				error_handler(105);
			declaracion_variable(folset);
			break;
		
		default:
			error_handler(19);
	}
}


void definicion_funcion(set folset)
{
	inf_id->clase = CLASFUNC;
	char nombreFuncion[8];
	strcpy(nombreFuncion, inf_id->nbre);
	stc_TS *descriptor = &(inf_id->desc);
	int tipoFuncion = inf_id->ptr_tipo;
	insertarTS();
	match(CPAR_ABR, 20);

	pushTB();// abre bloque de funcion.
	if(GEN){
		CODE[lcode++] = ENBL;
		CODE[lcode++] = get_nivel();
	}
	desplazamiento = 0;
	
	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT )){		
		if(strcmp(nombreFuncion,"main") == 0)
			error_handler(86);
		lista_declaraciones_param(folset | CPAR_CIE | CLLA_ABR, descriptor);
	}
	match(CPAR_CIE, 21);

	//int isFuncion = 1;
	proposicion_compuesta(folset, 1, tipoFuncion); 
	if(GEN){
		CODE[lcode++] = FINB;
		CODE[lcode++] = get_nivel();
	}
	pop_nivel(); //cierra el nivel.
}


void lista_declaraciones_param(set folset, stc_TS * desc)
{
	declaracion_parametro(folset | CCOMA | CVOID | CCHAR | CINT | CFLOAT );
	int cantParam = 1;
	tipo_inf_res  * aux = crearNodo();
	desc->part_var.sub.ptr_inf_res = aux;
	insertarTS();
	test(CCOMA | folset, CVOID | CCHAR | CINT | CFLOAT, 45);
	while(lookahead_in(CCOMA | CVOID | CCHAR | CINT | CFLOAT))
	{
		match(CCOMA, 64);
		declaracion_parametro(folset | CCOMA | CVOID | CCHAR | CINT | CFLOAT);
		cantParam++;
		aux->ptr_sig = crearNodo();
		aux = aux->ptr_sig;
		insertarTS();
		test(CCOMA | folset,  CVOID | CCHAR | CINT | CFLOAT, 45);
	}
	desc->part_var.sub.cant_par = cantParam;
	//mostrarLista(desc);
}

void declaracion_parametro(set folset)
{
	int flagDireccion = 0;
	especificador_tipo(folset | CAMPER | CIDENT | CCOR_ABR | CCOR_CIE);
	if(lookahead_in(CAMPER)){
		scanner();
		flagDireccion = 1;
	}
		
	strcpy(inf_id->nbre, lexema());
	inf_id->clase = CLASPAR;	
	match(CIDENT, 17);
	if(lookahead_in(CCOR_ABR | CCOR_CIE))
	{
		if(flagDireccion == 1)
			error_handler(92);
		flagDireccion = 1;
		match(CCOR_ABR, 35);
		match(CCOR_CIE, 22);
		inf_id->desc.part_var.param.ptero_tipo_base = inf_id->ptr_tipo;
		inf_id->ptr_tipo = TARREGLO;
	}
	if(flagDireccion){
		inf_id->desc.part_var.param.tipo_pje = 'R';
	}
	else {
		inf_id->desc.part_var.param.tipo_pje = 'V';
	}
}


void lista_declaraciones_init(set folset, int stipo)
{
	test(CIDENT, folset | CASIGNAC | CCOR_ABR | CCOMA , 46);
	
	strcpy(inf_id->nbre, lexema());	
	
	if(strcmp(inf_id->nbre,"main") == 0)
		error_handler(105);
	match(CIDENT, 17);

	int isArr =declarador_init(folset | CCOMA | CIDENT | CASIGNAC | CCOR_ABR, stipo);
	if(!isArr){
		inf_id->clase = CLASVAR;
		inf_id->ptr_tipo = stipo;
		int tamVar= tamTipo(stipo);
		if(GEN){
			CODE[lcode++] = ALOC;
			CODE[lcode++] = tamVar;
		}
		inf_id->desc.despl = desplazamiento;	
		desplazamiento += tamVar;

	}
	else{
		inf_id->clase = CLASVARSTRUCT;
		inf_id->ptr_tipo = en_tabla("TIPOARREGLO");
		inf_id->desc.part_var.arr.ptero_tipo_base = stipo;
	}
	inf_id->desc.nivel = get_nivel();
	insertarTS();


	test(CCOMA | folset, CIDENT | CASIGNAC | CCOR_ABR, 48);
	while(lookahead_in(CCOMA | CIDENT | CASIGNAC | CCOR_ABR))
	{
		match(CCOMA, 64);
		strcpy(inf_id->nbre, lexema());
		match(CIDENT, 17);
		int isArr = declarador_init(folset | CCOMA | CIDENT | CASIGNAC | CCOR_ABR, stipo);
		if(!isArr){
			inf_id->clase = CLASVAR;
			inf_id->ptr_tipo = stipo;

			int tamVar= tamTipo(stipo);
			if(GEN){
				CODE[lcode++] = ALOC;
				CODE[lcode++] = tamVar;
			}
			inf_id->desc.despl = desplazamiento;	
			desplazamiento += tamVar;
		}
		else{
			inf_id->clase = CLASVARSTRUCT;
			inf_id->ptr_tipo = en_tabla("TIPOARREGLO");
			inf_id->desc.part_var.arr.ptero_tipo_base = stipo;
		}
		inf_id->desc.nivel = get_nivel();
		insertarTS();
		test(CCOMA | folset, CIDENT | CASIGNAC | CCOR_ABR, 48);
	}
}


void declaracion_variable(set folset)
{
	int stipo = inf_id->ptr_tipo;
	int isArr = declarador_init(folset | CCOMA | CIDENT | CPYCOMA , stipo);
	if(!isArr){
		inf_id->clase = CLASVAR;
		int tamVar= tamTipo(stipo);
		if(GEN){
			CODE[lcode++] = ALOC;
			CODE[lcode++] = tamVar;
		}
		inf_id->desc.despl = desplazamiento;	
		desplazamiento += tamVar;
	}
	else {
		inf_id->clase = CLASVARSTRUCT;
		inf_id->ptr_tipo = en_tabla("TIPOARREGLO");
		inf_id->desc.part_var.arr.ptero_tipo_base = stipo;
	}
	inf_id->desc.nivel = get_nivel();


	insertarTS();
	if(lookahead_in(CCOMA | CIDENT))
	{
		match(CCOMA,64);
		lista_declaraciones_init(folset | CPYCOMA, stipo);
	}

	match(CPYCOMA, 23);
	test(folset, NADA, 51);
}


int declarador_init(set folset, int hTipo)
{
	int isArr=0;
	int entero = NIL;
	atributos_sintetizados as = {NIL,NIL,NULL, NIL};
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

			if(lookahead_in(CCONS_ENT)){
				as = constante(folset | CCOR_CIE | CASIGNAC | CLLA_ABR | CCONS_ENT | CCONS_FLO | CCONS_CAR | CLLA_CIE);
				strcpy(dimension, as.literalEntero);
				entero = (int) strtol(dimension, NULL, 10);
			}else{
				if(!lookahead_in(CCOR_CIE))
					error_handler(75);
			}
			 
			if(entero == 0)
				error_handler(75);
			
			match(CCOR_CIE, 22);
			int cant = 0;
			if(lookahead_in(CASIGNAC | CLLA_ABR | CLLA_CIE | CCONS_ENT | CCONS_FLO | CCONS_CAR))
			{
				match(CASIGNAC, 66);
				match(CLLA_ABR, 24);
				cant = lista_inicializadores(folset | CLLA_CIE, hTipo);
				match(CLLA_CIE, 25);
			}

			if(entero != NIL & entero >= cant){
				inf_id->desc.part_var.arr.cant_elem = entero;
			}
			else{
				if(entero != NIL)
					error_handler(76);
				inf_id->desc.part_var.arr.cant_elem = cant;
			}
			break;
	}

	test(folset, NADA, 48);
	return isArr;
}

int lista_inicializadores(set folset, int hTipo)
{
	int cantidad_cosntantes=0;
	atributos_sintetizados asC = constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);
	if(asC.tipo != hTipo)
		error_handler(77);
	cantidad_cosntantes++;
	test(CCOMA | folset, CCONS_ENT | CCONS_FLO | CCONS_CAR, 63);
	while(lookahead_in(CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR))
	{
		match(CCOMA, 64);
		asC = constante(folset | CCOMA | CCONS_ENT | CCONS_FLO | CCONS_CAR);
		if(asC.tipo != hTipo)
			error_handler(77);
		cantidad_cosntantes++;
		test(CCOMA | folset , CCONS_ENT | CCONS_FLO | CCONS_CAR, 63);
	}
	return cantidad_cosntantes;
}


void proposicion_compuesta(set folset, int isFuncion, int tipoFuncion)
{
	test(CLLA_ABR, folset | CVOID | CCHAR | CINT | CFLOAT  | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CLLA_CIE, 49);

	match(CLLA_ABR, 24);

	if (!isFuncion) {
		pushTB();
		if(GEN){
			CODE[lcode++] = ENBL;
			CODE[lcode++] = get_nivel();
			desplazamiento = 0;
		}
	} // si es una función ya creo el nivel
	
	if(lookahead_in(CVOID | CCHAR | CINT | CFLOAT))
		lista_declaraciones(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CLLA_CIE);

	if(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
						 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
						 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
		lista_proposiciones(folset | CLLA_CIE, tipoFuncion);
	else{
		if(tipoFuncion != TVOID ){
			error_handler(88);
			//mostrar_tabla();
		}
	}
	match(CLLA_CIE, 25);
	if (!isFuncion) { 
		if(GEN){
			CODE[lcode++] = FINB;
			CODE[lcode++] = get_nivel();
		}
		pop_nivel(); // si es una función le da la baja al nivel en el procedimiento definicion de funcion
	}
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


void lista_proposiciones(set folset, int tipoFuncion)
{
	atributos_sintetizados asP = proposicion(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN);

	test(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN, NADA, 68);
	while(lookahead_in(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN))
    {
		atributos_sintetizados asP2 = proposicion(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN);
		if(asP.is_return != 1)
			asP = asP2;
		test(folset | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN, NADA, 68);
	}
	if(tipoFuncion == TINT || tipoFuncion == TCHAR || tipoFuncion == TFLOAT){
		if(asP.is_return != 1)
			error_handler(88);
			
		else if(tipoFuncion != asP.tipo)
			error_handler(87);
	} else if(asP.is_return == 1)
		error_handler(89);
}

atributos_sintetizados proposicion(set folset)
{
	atributos_sintetizados as = {NIL,NIL, NULL, NIL, NIL};
	test(CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN, folset, 52);
	switch(lookahead())
	{
		case CLLA_ABR:
			proposicion_compuesta(folset, 0 , 0);
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
			as = proposicion_retorno(folset);
			break;
		
		default:
			error_handler(29);
	}
	return as;
}


void proposicion_iteracion(set folset)
{
	atributos_sintetizados asE;
	match(CWHILE, 27);

	match(CPAR_ABR, 20);

	int labelExp = lcode;
	
	asE = expresion(folset | CPAR_CIE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN );
	match(CPAR_CIE, 21);
	if(GEN){
		CODE[lcode++] = BIFF;
		CODE[lcode++] = asE.tipo;
	}
	int salidaWhile = lcode;
	lcode++;
	
	
	proposicion(folset);

	if(GEN){
		CODE[lcode++] = BIFS;
		CODE[lcode++] = labelExp;
		CODE[salidaWhile]= lcode;
	}
}


void proposicion_seleccion(set folset)
{
	match(CIF, 28);

	match(CPAR_ABR, 20);

	expresion(folset | CPAR_CIE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN | CELSE);

	match(CPAR_CIE, 21);
	
	proposicion(folset | CELSE | CLLA_ABR | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG |
							 CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR |
							 CIF | CWHILE | CIN | COUT | CPYCOMA | CRETURN );

	if(lookahead_in(CELSE))
	{
		scanner();
		proposicion(folset);

	}

	
}


void proposicion_e_s(set folset)
{
	switch(lookahead())
	{
		case CIN:
			scanner();
			
			match(CSHR, 30);
			int posEnTabla;
			if(GEN)
			  posEnTabla = en_tabla(lexema());
			atributos_sintetizados asE = variable(folset | CSHR | CIDENT | CPYCOMA, NULL);			
			int tipoE = asE.tipo != TARREGLO? asE.tipo : asE.tipo_base;

			if(tipoE == TERROR || tipoE == TVOID)
				error_handler(95);
			if(GEN){
				CODE[lcode++] = LEER;
				CODE[lcode++] = tipoE -1;
				CODE[lcode++] = ALM;
				CODE[lcode++] = ts[posEnTabla].ets->desc.nivel;
				CODE[lcode++] = ts[posEnTabla].ets->desc.despl;
				CODE[lcode++] = tipoE -1;
			}
			
			while(lookahead_in(CSHR))
			{
				scanner();
				if(GEN)
			 		posEnTabla = en_tabla(lexema());
				asE = variable(folset |CSHR | CIDENT | CPYCOMA, NULL);
				tipoE = asE.tipo != TARREGLO? asE.tipo : asE.tipo_base;
				if(tipoE == TERROR || tipoE == TVOID)
					error_handler(95);
							if(GEN){
				CODE[lcode++] = LEER;
				CODE[lcode++] = tipoE -1;
				CODE[lcode++] = ALM;
				CODE[lcode++] = ts[posEnTabla].ets->desc.nivel;
				CODE[lcode++] = ts[posEnTabla].ets->desc.despl;
				CODE[lcode++] = tipoE -1;
			}
			}

			match(CPYCOMA, 23);
			
			break;
		
		case COUT:
			scanner();

			match(CSHL, 31);
			int stringFlag = 0;
			if (GEN){
				if (lookahead() == CCONS_STR){
					stringFlag = 1;
					strcpy(CSAUX, lexema());
				}
			}
			
			asE = expresion(folset | CSHL | CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPYCOMA);
			tipoE = asE.tipo != TARREGLO? asE.tipo : asE.tipo_base;
			if(tipoE == TERROR || tipoE == TVOID)
				error_handler(95);

			if(GEN){
				if(stringFlag){
					CODE[lcode++] = CRCTS;									
					CODE[lcode++] = lcs;
					for (int i = 0; CSAUX[i] != '\0'; i++){								
						CS[lcs]= CSAUX[i];
						lcs++;
					}
					CODE[lcode++] = IMPRCS;	
					stringFlag = 0;
				}
				else{
					CODE[lcode++] = IMPR;									
					CODE[lcode++] = tipoE - 1;
				}
				
			}
			
			while(lookahead_in(CSHL))
			{
				scanner();

				if (GEN){
					if (lookahead() == CCONS_STR){
						stringFlag = 1;
						strcpy(CSAUX, lexema());
					}
				}
					

				asE = expresion(CSHL |CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPYCOMA);
				tipoE = asE.tipo != TARREGLO? asE.tipo : asE.tipo_base;
				if(tipoE == TERROR || tipoE == TVOID)
					error_handler(95);
				
				if(GEN){
					if(stringFlag){
						CODE[lcode++] = CRCTS;									
						CODE[lcode++] = lcs;
						for (int i = 0; CSAUX[i] != '\0'; i++){								
							CS[lcs]= CSAUX[i];
							lcs++;
						}
						CODE[lcode++] = IMPRCS;	
						stringFlag = 0;
					}
					else{
						CODE[lcode++] = IMPR;									
						CODE[lcode++] = tipoE - 1;
					}
					
				}

			}

			match(CPYCOMA, 23);
			break;
		
		default:
			error_handler(29);
	}
	test(folset , NADA, 53);
}


atributos_sintetizados proposicion_retorno(set folset)
{
	atributos_sintetizados as;
	scanner();
	as = expresion(folset | CPYCOMA);
	as.is_return = 1;
	match(CPYCOMA, 23);

	test(folset, NADA, 54);
	return as;
}


void proposicion_expresion(set folset)
{

	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		expresion(folset | CPYCOMA);

	match(CPYCOMA, 23);

	test(folset, NADA, 55);
}


atributos_sintetizados expresion(set folset)
{
	char  miLexema[10];
	strcpy(miLexema, lexema());
	atributos_sintetizados asE1 = expresion_simple(folset | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR), asEN;
	asEN = asE1;
	int error = 0, esAsign = 0,op = 0;
	test(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | folset, NADA, 69);
	while(lookahead_in(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG))
	{
		atributos_sintetizados asE2 = {NIL,NIL,NULL,NIL};
		switch(lookahead())
		{
			case CASIGNAC:
				esAsign = 1;
				scanner();
				if(asE1.variable == NIL)
					error_handler(82);
				asEN = expresion_simple(folset | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
				if(asE1.variable == 2 || asEN.variable == 2)
					error_handler(81);
				break;
				
			case CDISTINTO:
			case CIGUAL:
			case CMENOR:
			case CMEIG:
			case CMAYOR:
			case CMAIG:
				op = lookahead();
				scanner();
				asE2 = expresion_simple(folset | CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
				int tipoE2 = asE2.tipo != TARREGLO? asE2.tipo : asE2.tipo_base;
				int tipoEN = asEN.tipo != TARREGLO? asEN.tipo : asEN.tipo_base;
				if((tipoEN == TVOID || tipoEN == TERROR || tipoE2 == TVOID || tipoE2 == TERROR) && error == 0 ) {
					tipoEN = TERROR;
					error = 1;
					error_handler(96);
				}else{
					asEN.tipo =  tipoE2 > tipoEN? tipoE2 : tipoEN;
				}
				if(GEN){
					switch (op){
						case CDISTINTO:
							CODE[lcode++] = CMIG;
							CODE[lcode++] = NEG;
							break;
						case CIGUAL:
							CODE[lcode++] = CMIG;
							break;
							
						case CMENOR:
							CODE[lcode++] = CMME;
							break;
						case CMEIG:
							CODE[lcode++] = CMNI;
						break;
						case CMAYOR:
							CODE[lcode++] = CMMA;
						break;
						case CMAIG:
							CODE[lcode++] = CMAI;
						break;					
					}			
					CODE[lcode++] = asEN.tipo -1;
				}
				break;
		}
    		test(CASIGNAC | CDISTINTO | CIGUAL | CMENOR | CMEIG | CMAYOR | CMAIG | folset, NADA, 69);
	}
	int tipoE1 = asE1.tipo != TARREGLO? asE1.tipo : asE1.tipo_base;
	int tipoEN = asEN.tipo != TARREGLO? asEN.tipo : asEN.tipo_base;

	if(tipoE1 == TVOID || tipoE1 == TERROR || tipoEN == TVOID || tipoEN == TERROR) {
		asEN.tipo = TERROR;
	//	error_handler(96);
	}else
		if(esAsign && tipoE1 < tipoEN ){
			error_handler(104);
			asEN.tipo = TERROR;
		}
		if(GEN && esAsign){
			int pos = en_tabla(miLexema);
			CODE[lcode++] = ALM;
			CODE[lcode++] = ts[pos].ets->desc.nivel;
			CODE[lcode++] = ts[pos].ets->desc.despl;
			CODE[lcode++] = tipoE1 - 1;
		}
	return asEN;
}


atributos_sintetizados expresion_simple(set folset)
{
	test(CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR, folset | COR, 56);
	if(lookahead_in(CMAS | CMENOS))
		scanner();
		
	atributos_sintetizados asT1 = termino(folset | CMAS | CMENOS |COR | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR, -1);
	int tipoT1 = asT1.tipo != TARREGLO? asT1.tipo : asT1.tipo_base;
	test(CMAS | CMENOS | COR | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR, 65);
	while(lookahead_in(CMAS | CMENOS | COR | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR))
	{
		int op = NIL;
		switch (lookahead())
		{
		case CMAS:
		case CMENOS:
		case COR:
			op = lookahead();
			scanner();
			break;
		default:
			error_handler(65);
			break;
		}
		atributos_sintetizados asT2 = termino(folset | CMAS | CMENOS | COR | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR, tipoT1);
		int tipoT2 = asT2.tipo != TARREGLO? asT2.tipo : asT2.tipo_base;
		if((tipoT1 == TVOID || tipoT1 == TERROR || tipoT2 == TVOID || tipoT2 == TERROR )&&(op == COR)) {
			tipoT1 = TERROR;
			error_handler(96);
		}
		else{
			tipoT1 = tipoT2 > tipoT1? tipoT2 : tipoT1;
		}
		if(GEN){
			switch (op){
				case CMAS:
					CODE[lcode++] = SUM;
					break;
				case CMENOS:
					CODE[lcode++] = SUB;
					break;
					
				case COR:
					CODE[lcode++] = OR;
					break;
			}			
			CODE[lcode++] = tipoT1 -1;
		}
		test(CMAS | CMENOS | COR | folset, CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR, 65);
	}
	asT1.tipo = tipoT1;
	return asT1;
}


atributos_sintetizados termino(set folset, int tipoh)
{	
	atributos_sintetizados as = factor(folset | CMULT | CDIV | CAND | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG, tipoh);
	atributos_sintetizados as2 = {NIL,NIL,NULL, NIL};
	test(CMULT | CDIV | CAND | folset , CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG , 65 );
	while(lookahead_in(CMULT | CDIV | CAND | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG))
	{
		set op = NIL;
		switch (lookahead())
		{
			case CMULT:
			case CDIV:
			case CAND:
				op = lookahead();
				scanner();
				break;
			default:
				error_handler(65);
				break;
		}
		as2 = factor(folset | CMULT | CDIV | CAND | CMULT | CDIV | CAND | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG, tipoh);

		// Control semantico
		int tipoAS, tipoAS2;
		if (as.tipo == TARREGLO)
			tipoAS = as.tipo_base;
		else 
			tipoAS = as.tipo;

		if (as2.tipo == TARREGLO)
			tipoAS2 = as2.tipo_base;
		else 
			tipoAS2 = as2.tipo;

		// por coercion
		if (tipoAS <= tipoAS2)
			as.tipo = tipoAS2;
		else 
			as.tipo = tipoAS; 
		switch (op){
			case CMULT:
			case CDIV:
				
				break;
			case CAND:
				if (tipoAS == TVOID || tipoAS == TERROR || tipoAS2 == TVOID || tipoAS2 == TERROR) {
					as.tipo = TERROR;
					error_handler(96);
				}
				else
					as.tipo = tipoAS > tipoAS2? tipoAS : tipoAS2;				
				break;
			default:
				break;
		}
		if(GEN){
			switch (op){
				case CMULT:
					CODE[lcode++] = MUL;
					break;
				case CDIV:
					CODE[lcode++] = DIV;
					break;
					
				case CAND:
					CODE[lcode++] = AND;
					break;
			}			
			CODE[lcode++] = as.tipo -1;
		}
	}

	return as;
}


atributos_sintetizados factor(set folset, int tipoh)
{
	atributos_sintetizados as = {NIL, NIL, NULL, NIL};

	test(CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR | CPAR_ABR | CNEG, folset, 57);
	int posicionEnT = 0;
	switch(lookahead())
	{
		case CIDENT:
			posicionEnT = en_tabla(lexema());
			if(posicionEnT != NIL){
				switch(ts[posicionEnT].ets->clase){
					 case CLASFUNC:
					 	//TODO el tipo de la funcion no puede ser arreglo
					 	as.tipo = ts[posicionEnT].ets->ptr_tipo;
					 	llamada_funcion(folset, NULL);
						break;
					case CLASPAR:
					case CLASVAR:
					case CLASVARSTRUCT:{
						as = variable(folset, NULL);
						
						if(GEN & ts[posicionEnT].ets->clase == CLASVAR){
							if(tipoh == -1 || tipoh == as.tipo ){
								CODE[lcode++] = CRVL;
								CODE[lcode++] = ts[posicionEnT].ets->desc.nivel;
								CODE[lcode++] = ts[posicionEnT].ets->desc.despl;
								CODE[lcode++] = as.tipo -1;
							}else if(tipoh < as.tipo){
								CODE[lcode++] = CAST;
								CODE[lcode++] = tipoh -1;
								CODE[lcode++] = as.tipo -1;
								CODE[lcode++] = CRVL;
								CODE[lcode++] = ts[posicionEnT].ets->desc.nivel;
								CODE[lcode++] = ts[posicionEnT].ets->desc.despl;
								CODE[lcode++] = as.tipo -1;
							}else{
								CODE[lcode++] = CRVL;
								CODE[lcode++] = ts[posicionEnT].ets->desc.nivel;
								CODE[lcode++] = ts[posicionEnT].ets->desc.despl;
								CODE[lcode++] = as.tipo -1;
								CODE[lcode++] = CAST;
								CODE[lcode++] = as.tipo -1;
								CODE[lcode++] = tipoh -1;
							}
						}
					}
				}
			}else {
				// TODO 
				//para poder decidir por que camino teniamos que ir consumimos el ciden asi poder ver si sigue o no parentecis 
				char ciden[8];
				strcpy(ciden, lexema());
				match(CIDENT,17);
				as.tipo = TERROR;
				if(lookahead() == CPAR_ABR)
					llamada_funcion(folset, ciden);
				else
					 as = variable(folset, ciden);
			}
			break;
		
		case CCONS_ENT:
		case CCONS_FLO:
		case CCONS_CAR:
			
			as = constante(folset);
			
			if(GEN){ 
					if(tipoh == -1 || tipoh == as.tipo ){
						CODE[lcode++] = CRCT;
						if(as.tipo==1)CODE[lcode++] = as.literalEntero[1];
						else CODE[lcode++] = atof(as.literalEntero);
						CODE[lcode++] = as.tipo -1;
					}else{ 
						if(tipoh < as.tipo){
							CODE[lcode++] = CAST;
							CODE[lcode++] = tipoh -1;
							CODE[lcode++] = as.tipo -1;
							CODE[lcode++] = CRCT;
							if(as.tipo==1)CODE[lcode++] = as.literalEntero[1];
							else CODE[lcode++] = atof(as.literalEntero);
							CODE[lcode++] = as.tipo -1;
						}
						else{
							CODE[lcode++] = CRCT;
							if(as.tipo==1)CODE[lcode++] = as.literalEntero[1];
							else CODE[lcode++] = atof(as.literalEntero);
							CODE[lcode++] = as.tipo -1;
							CODE[lcode++] = CAST;
							CODE[lcode++] = as.tipo -1;
							CODE[lcode++] = tipoh -1;
						}
					}	
			}
			break;
		
		case CCONS_STR:
			as.tipo = TARREGLO;
			as.tipo_base = TCHAR;
			scanner();
			break;
		
		case CPAR_ABR:
			match(CPAR_ABR, 20);
			//TODO CUANDO RETORNE EXPRESION
			as = expresion(folset | CPAR_CIE);
			match(CPAR_CIE, 21);
			break;
			
		case CNEG:
			scanner();
			//CUANDO RETORNE EXPRESION
			as = expresion(folset);
			if(GEN){
				CODE[lcode++] = NEG;
				CODE[lcode++] = as.tipo -1;
			}
			break;
			
		default:
			error_handler(32);
	}
	test(folset, NADA, 58);
	return as;
}


atributos_sintetizados variable(set folset, char * name)
{	
	atributos_sintetizados as ={NIL, NIL, NULL, 1};
	int posicion;
	if(name != NULL){
		strcpy(inf_id->nbre,name);
		inf_id->ptr_tipo =TERROR;
		inf_id->clase = CLASVAR;
		as.tipo = TERROR;
			//si no encuentro en TS asigno tipoerror con el identificador		
	}else{		
		test(CIDENT, folset | CCOR_ABR | CCOR_CIE | CMAS | CMENOS | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR, 59);
		posicion = en_tabla(lexema());
		match(CIDENT, 17);
		if(ts[posicion].ets->ptr_tipo ==  TARREGLO && !lookahead_in(CCOR_ABR)){
			//TODO aclarar porque usamos el 2 como flag de que no encontre el subindice
			as.variable = 2;
			error_handler(101);
		}
		
		// conttrolo para sintetizar el tipo
		as.tipo =  ts[posicion].ets->ptr_tipo;
		if(as.tipo ==  TARREGLO){
			as.tipo_base = ts[posicion].ets->desc.part_var.arr.ptero_tipo_base;
		} 

	}
	/* El alumno debera verificar con una consulta a TS
	si, siendo la variable un arreglo, corresponde o no
	verificar la presencia del subindice 
	se decide que al reconocer por la tabla de simbolos que la variable es de tipo arreglo debe llevar subindice
	*/
	
	if(lookahead_in(CCOR_ABR))
	{
		match(CCOR_ABR, 35);
		expresion(folset | CCOR_CIE);
		match(CCOR_CIE, 22);
		if(name != NULL){
			inf_id->clase = CLASVARSTRUCT;
			inf_id->ptr_tipo = TARREGLO;
			inf_id->desc.part_var.arr.ptero_tipo_base = TERROR;
			as.tipo = TARREGLO;
			as.tipo_base = TERROR;
		}
	}
	if(name != NULL){
		insertarTS();
		error_handler(71);
	}
	test(folset,NADA, 60);
	return as;
}


void llamada_funcion(set folset, char * name)
{	
	int pos = NIL, cantEnLLamada;
	if(name != NULL){
		strcpy(inf_id->nbre,name);
		inf_id->ptr_tipo =TERROR;
		inf_id->clase = CLASFUNC;
		//si no encuentro en TS asigno tipoerror con el identificador
		insertarTS();
		error_handler(71);
	}else{
		pos = en_tabla(lexema());
		match(CIDENT, 17);
	}
	
	match(CPAR_ABR, 20);
	
	if(lookahead_in(CMAS | CMENOS | CIDENT | CPAR_ABR | CNEG | CCONS_ENT | CCONS_FLO | CCONS_CAR | CCONS_STR))
		cantEnLLamada = lista_expresiones(folset | CPAR_CIE);

	match(CPAR_CIE, 21);
	if(pos != NIL){
		 int cantEnTabla=ts[pos].ets->desc.part_var.sub.cant_par;
		 if(cantEnLLamada != cantEnTabla)
		 	error_handler(90);
	}
	test(folset, NADA, 61);
}


int lista_expresiones(set folset)
{
	expresion(folset| CCOMA | CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
	int cantParam = 1;
	//test(CCOMA | folset,NADA,??);
	while(lookahead_in(CCOMA | CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR))
	{
		match(CCOMA,64);
		expresion(folset| CCOMA | CMAS | CMENOS | CIDENT | CCONS_ENT | CCONS_FLO | CCONS_CAR | CNEG | CPAR_ABR | CCONS_STR);
		cantParam++;
	}
	return cantParam;
}


atributos_sintetizados constante(set folset)
{
	atributos_sintetizados as = {NIL, NIL, NULL, NIL};

	test(CCONS_ENT | CCONS_FLO | CCONS_CAR, folset, 62);
	as.literalEntero = (char *) calloc(20, sizeof(char));
	strcpy(as.literalEntero, lexema());
	switch(lookahead())
	{ 
		case CCONS_ENT:
			scanner();
			as.tipo = TINT;
			break;
		
		case CCONS_FLO:
			scanner();
			as.tipo = TFLOAT;
			break;
		
		case CCONS_CAR:
			scanner();
			as.tipo = TCHAR;
			break;
		
		default:
			error_handler(33);
	}
	test(folset, NADA, 63);
	return as;
}
void mostrarLista(stc_TS * desc){
	printf("cantidad parametros: %d \n",desc->part_var.sub.cant_par);
	tipo_inf_res * cur = desc->part_var.sub.ptr_inf_res;
	int i = 0;
	while(cur != NULL){
		printf("nodo %d--> tipo: %d, pje: %c, tbase: %d \n",++i,cur->ptero_tipo, cur->tipo_pje,cur->ptero_tipo_base);
		cur = cur->ptr_sig;
	}
}