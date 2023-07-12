#include<stdio.h>
#include<string.h>

/* void func(int *a) {
    *a = 10;
}

int main() {
    int a;
    func(&a);
    printf("%d\n", a);
    return 0;
} */

/* int main() {
    char a[3][10];
    for (int i = 0; i < 3; i++) {
        if (strcmp(a[i], "") == 0) {
            printf("NULL\n");
        }
    }
    return 0;
} */

int main(){
    char a[] = "12:24";
    char *buf;

    buf = strtok(a, ":");
    printf("%s\n", buf);
    buf = strtok(NULL, " ");
    printf("%s\n", buf);
}

/* int main(){
    char a[5][5];
    strcpy(a[0], "wow");
    strcpy(a[1], "funtastic");

    printf("%s", a[1]);
} */