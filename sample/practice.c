#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	pid_t p;
    //parent
	printf("A\n\n");

    //create the child process
    /*マルチプロセスの特徴は、ここで親と子がそれぞれpという変数を持つということ。*/
	p = fork();

    //parent and child
    //まず親がここを通過し、親専用のpを表示。次に子がここを通過し、子専用のpを表示
	printf("B %d\n",p);

	return 0;
}