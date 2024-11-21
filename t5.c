
int a = 4;
float b = 3.14;
char c = 'A';

void main() {
    int i = 0, sum = 0;
    while (i < a) {
        sum = sum + i; 
        i= i + 1;
    }
    cout << "float: " << b << " char: " << c << " Suma:" << sum;
}


