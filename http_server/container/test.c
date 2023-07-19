#include<stdio.h>
#include<string.h>

/* void func_2(int *a){
    *a = 10000;
}

void func_1(int *a) {
    func_2(a);
}

int main() {
    int a;
    func_1(&a);
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

/* int main(){
    char a[] = "12:24";
    char *buf;

    buf = strtok(a, ":");
    printf("%s\n", buf);
    buf = strtok(NULL, " ");
    printf("%s\n", buf);
} */

/* int main(){
    char a[5][5];
    strcpy(a[0], "wow");
    strcpy(a[1], "funtastic");

    printf("%s", a[1]);
} */


int main(){
    char text[] = "12 34";
    int a[3][2] ={0};
    int b, c;

    sscanf(text, "%d %d", &b, &c);

    a[1][0] = b;
    a[1][1] = c;

    printf("%d\n", a[1][0]);
    printf("%d\n", a[1][1]);

}