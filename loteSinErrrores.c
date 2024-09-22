# declaraciones #
int a;
char b = 'c';
float arr[10] = {1.0, 2.0, 3.5};

# Definición de funciones simples y con parámetros #
void foo() {
    int x;
}

int fsum(int a, int b) {
    return a + b;
}

float fmulti(float &a, float b[]) {
    float result = 0;
    int i = 0;
    while (i < 10) {
        result = result + a * b[i];
        i = i + 1;
    }
    return result;
}
# Estructuras de control (proposiciones de selección e iteración) #
void fcheck(int a) {
    if (a > 0) {
        cout << "Positive" << endl;
    } else if (a == 0) {
        cout << "Zero" << endl;
    } else {
        cout << "Negative" << endl;
    }
}

void floop() {
    int i = 0;
    while (i < 5) {
        cout << i << endl;
        i = i + 1;
    }
}

#Expresiones aritméticas y relacionales#

int fcalcula(int x, int y) {
    return (x + 2) * (y / 3) - 5;
}

int fcompare(int a, int b) {
    return a != b && (a > b || b <= 10);
}

# Entrada y salida #

void finOut() {
    int x, y;
    cin >> x;
    cout << "Sum: " << x + y << endl;

}

#Llamadas a funciones (con y sin parámetros)#
void testFunc() {
    float values[10] = {1.0, 2.0, 3.0};
    a = fsum(5, 3);
    cout << "Sum result: " << a << endl;
    result = fmulti(a, values);
    cout << "Multiplication result: " << result << endl;
}


