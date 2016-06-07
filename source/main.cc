#include <stdio.h>

#include "process/SONEEventProcess.h"

int main(){
    printf("## Start Listen \r\n");

    soneEventServer_init(21688,500);

    return 0;
}


void add(){

}