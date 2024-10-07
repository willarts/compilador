#Variables#
void variable (){
    void b;
    a = 2;
    d[10] = 2;
}

#Arreglos#
void arreglo(){
    void a [23] = {1,2,3};
    int b[10];
    float f[0];
    int g[3] = {2,4,'5'};
    int h[2] = {2,4,3,4,5};
    g + 2;
} 

#Asignación#
void asig (){
    int b[10], a[10];
    int h[2] = {2,4,3,4,'3'};
    a = b;
    3 = a[2];

}
#entrada Salida#
void finOut() {
    int x; 
    y;
    cin >> y;
    cout << "Sum: " << x + y << endl;

}

#Funciones-Parámetros#
#procedimiento con return#
void fun(){
    int r;
    return 9;  
}
#funcion sin return con proposiciones#
int fun1(){
    int c ;
    c = 3+3;
}
#funcion sin return sin proposiciones#
char fun2(){
 char t;
}
#return que no coincide con el tipo de la funcion#
char fun3(){
    int r;
    return 9;  
}

#La cantidad de parametros formales debe coicidir con la cantidad de parametros reales#
int fcompare(char a, int b, char &j, float h[]) {
    int p[13]= {2,3,} ;
     return p[2];
}
void ult (){
    int a = 2;
    fcompare('a',2);
}

#No se permite <tipo> & <nombre arreglo> [] en la definición de un parámetro.#
int fcompar2(char a, int b, char &j[], float h[]) {
    int p[13]= {2,3} ;
    return p[2];
}

#Logica booleana#
int flogi(char a, int b,float h[]) {
    int p[13]= {2,3} ;
    int s;
    s = a<c;
    s = a&&c;
    return p[2];
}
# main no debe tener argumentos#
void main(int a){
    a +2;
}
#main debe ser de tipo void#
int main(){
    int a = 2;
    return a;
}
#declaro variable nombre main#
char main = '2';
int miVar(){
    int main = 2;
    return 5;
}
