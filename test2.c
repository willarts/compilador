#programa sin main#
void fcheck(int a) {
    if (a > 0) {
        cout << "Positive";
    }
}

void floop() {
    int i = 0;
    while (i < 5) {
        cout << i;
        i = i + 1;
    }
}

int fcalcula(int x, int y) {
    return (x + 2) * (y / 3) - 5;
}