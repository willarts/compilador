int a;

void main() {
    int i = 1, sum = 1;

    cin >> a;
    i = a;
    while (0 < a) {
        sum = sum * a; 
        a= a - 1;
    }
    cout << "factoriar de " << i << " es: " << sum ;
}