/*
  * SO-B 2018 - Projeto 1 - Programa teste
  *
  * Gabriela Diamante Graca - RA: 11183399
  * Nathia de Souza Marrafon - RA: 11742251
  * Rhenan Christian do Amaral Arantes - RA: 10018505
  *
  */

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define BUFFER_LENGTH 128			//buffer length
static char stringReceived[BUFFER_LENGTH];	//string to receive the device answer

int main(){
    int condition = 0, fd, ret;
    char operation[1], data[10];
    char stringToSend[BUFFER_LENGTH];		//string to send to device
    
    printf("\n####################  SO-B * Projeto 1  ####################\n");
    printf("################ Gabriela * Nathia * Rhenan ################\n\n");

    //operations menu
    do {
	condition = 0;
	printf("Informe a operacao desejada: \n");
	printf("(c) - cifrar;\n(d) - decifrar;\n(h) - calcular o resumo criptografico;\n\n");
	
	printf("Operacao: ");
	scanf("%s", operation);
   	fflush(stdin);

        //validating the operation
	if ( ((strcmp(operation, "c"))!= 0) && ((strcmp(operation, "d"))!= 0) && ((strcmp(operation, "h"))!= 0) ){
		printf("\nOperacao invalida! Veja as opcoes e escolha novamente!\n\n");
		condition = 1;
		
	}
    }while (condition == 1);

    strcat(stringToSend, operation);

    printf("\nInforme os dados desejados para a operacao escolhida:\n\n");
    printf("Dados: ");
    scanf("%s", data);
    fflush(stdin);	
 
    strcat(stringToSend, data);

    //printf("\nString a ser enviada: %s\n", stringToSend);

    //opening file
    printf("\n..abrindo arquivo!");
    //fd = open("",O_RDWR); -----------------------incluir nome do arquivo
    if (fd<0){
	perror("\nFalha ao abrir o arquivo!");
	return errno;
    }
    printf("\nArquivo aberto com sucesso!");

    //sending the string inputted by user to device
    printf("\nString a ser enviada: %s\n..enviando string!", stringToSend);
    //ret = write(fd, stringToSend, strlen(stringToSend)); ---------------------------testar apos incluir o arquivo
    if (ret<0){
	perror("\nFalha ao enviar a string!");
	return errno;
    }
    printf("\nString enviada com sucesso!");

    //receiving the answer (string) from device
    printf("\n..recebendo string!");
   // ret = read(fd, stringReceived, BUFFER_LENGTH); ------------------------testar apos incluir o arquivo
    if (ret<0){
	perror("\nFalha ao receber a string!");
	return errno;
    }

    printf("\nString recebida com sucesso!\nConteudo da string: %s\n", stringReceived);

    //closing file
    close(fd);    
}
