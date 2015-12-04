#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include "timer.h"
#define BASEPID 9000

int main(int argc, char *argv[]){
	int i, j , rows, cols,x, n, pid , pipefd[2] , w, data_len, len ;
  	char buff[9000];
  	char str[9000+1] = {'\0'};
	unsigned int loadTime, processTime;
	double  start, finish, elapsed;
	int size;

	if(argc < 2) {
		printf("\nPlease, run the application with correct parameters\n\n"
			   "\tEXAMPLE:\n\t[application-name] [matrix-size]\n\n");

		return 0;
	}

	size = atoi(argv[1]);
	
	int matriz1[size][size], matriz2[size][size], matriz_result[size][size];
	//Gera as matrizes que serão usadas na multiplicação e como resultado
	for (rows = 0; rows < size; rows++) {
		for (cols = 0; cols < size; cols++) {
			matriz1[rows][cols] = 1;
			matriz2[rows][cols] = 2;
			matriz_result[rows][cols] = 0;
		}
	}
	
	//!!!!!!!!!!!!inicia o contador para o tempo de processamento!!!!!!!!!!!!!!
	GET_TIME(start);
	
	//Gera os processos filhos que farão o calculo da multiplicação de matrizes
	for ( i=0;i<size;i++ )
  	{
    		pipe(pipefd);
      		if( pipefd < 0 )
			printf("Erro de pipe\n");
    		pid = fork();
    		if( pid == -1 )
    		{
      			printf("Erro de fork\n");
    		}
    		if( pid == 0 )//child
    		{
      			for( j=0;j<size;j++ )
      			{
				for( n=0;n<size;n++ )
				{
	  				matriz_result[i][j] += matriz1[i][n]*matriz2[n][j];
				}
			len = sprintf( str, "%d ", matriz_result[i][j] );   // tamanho de valor+blank em len
			w = write( pipefd[1], str, len  );     // escrever apenas o valor e blank
			if( w != len )                         // checar o numero escrito
				printf("\nWrite error");        // sair se der erro de leitura
			}
			*str = 0;                     // Coloca um null no final para terminar a string
			write( pipefd[1], str, 1 );
			exit(0);
		}
    		if( pid > 0 )//parent
    		{
      			wait(0);
      			if( ( data_len = read( pipefd[0], buff, sizeof ( buff ) ) ) <=0 )
      			{
				fprintf( stderr,"Erro de leitura: %d\n", strerror( errno ) );	//erro de leitura do buff 
				exit(1);							
      			}
      			for(x=0;x<size;x++){
				sscanf(buff, "%d",&matriz_result[i][x]);	
			}
			//sscanf( buff, "%d %d %d %d", &matriz_result[i][0], &matriz_result[i][1], &matriz_result[i][2], &matriz_result[i][3]);
    		}
  	}
  	//!!!!!!!!!!!!encerra o contador para tempo de processamento!!!!!!!!!!!!!!!
  	GET_TIME(finish);
	elapsed = finish - start;
	printf("The code took %e seconds\n", elapsed);
	
	  	// printf("C:\n");
	  	// for( i=0;i<size;i++ )
	  	// {
	   //  		for( j=0;j<size;j++ )
	   //  		{
	   //    			printf("%d\t",matriz_result[i][j]);
	   //  		}
	   //  		printf("\n");
	  	// }
}