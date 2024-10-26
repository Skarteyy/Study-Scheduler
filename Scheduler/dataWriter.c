#include <stdio.h>

int main(void){
    int read = 0;
    int test = 0;
    int sum = 0;
    int write = 0;
    float playback = 0;
    int day = 0;
    int month = 0;
    int year = 0;
    FILE *fp = fopen("UserData","w");
    if (fp == NULL){
        printf("Error opening file\n");
        return 1;
    }
    fprintf(fp,"read: %d\n",read);
    fprintf(fp,"test: %d\n",test);
    fprintf(fp,"sum: %d\n",sum);
    fprintf(fp,"write: %d\n",write);
    fprintf(fp,"playback: %f\n",playback);
    fprintf(fp,"day: %d\n",day);
    fprintf(fp,"month: %d\n",month);
    fprintf(fp,"year: %d\n",year);
    fclose(fp);
    return 0;
}