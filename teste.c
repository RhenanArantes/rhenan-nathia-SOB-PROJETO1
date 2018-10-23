#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 128
static unsigned char receive[BUFFER_LENGTH]; // resposta do modulo de kernel
static unsigned char receive1[BUFFER_LENGTH];
static unsigned char receive2[BUFFER_LENGTH];
char tohexresult[68];
char tohexresultcifra[130];
char tooriginalresultdecifra[130];
char tooriginalresult[100];
char tooriginalresult2[100];
static unsigned char strA[35]; // variaveis responsaveis por criação de substrings (entradas menores que 16 caracteres)
static unsigned char strB[35];// variaveis responsaveis por criação de substrings (entradas maiores que 16 caracteres)
static unsigned char strAD[68];// variaveis responsaveis por criação de substrings (64 caracteres)
static unsigned char strBD[68];// variaveis responsaveis por criação de substrings (128 caracteres)
static int subop =0;

int main(int argc, char * argv[]){
	
	  void split_input (char *x) // pega entrada do usuário e separa em 2 substrings
	  {
	    if (strlen (x) <= 32)
	      {
		if (strlen (x) % 2 == 0)
		  {			// se for par
		    if (strlen (x) > 16)
		      {
			char str2[strlen (x) / 2 + 1];
			char str3[strlen (x) / 2 + 1];
			strncpy (str2, x, sizeof (char) * (strlen (x) / 2));
			str2[strlen (x) / 2] = '\0';
			strcpy (strA, str2);
			strncpy (str3, x + (strlen (x) / 2),sizeof (char) * (strlen (x) / 2));
			str3[strlen (x) / 2] = '\0';
			strcpy (strB, str3);
		      }
		    else
		      {
			char str2[16];
			strncpy (str2, x, sizeof (char) * (strlen (x)));
			str2[strlen (x)] = '\0';
			strcpy (strA, str2);
		      }
		  }
		else
		  {			// se for impar
		    if (strlen (x) > 16)
		      {
			char str2[17];
			char str3[17];
			strncpy (str2, x, sizeof (char) * (strlen (x) / 2 + 1));
			str2[strlen (x) / 2 + 1] = '\0';
			strcpy (strA, str2);
			strncpy (str3, x + (strlen (x) / 2 + 1),sizeof (char) * ((strlen (x) / 2)));
			str3[strlen (x) / 2] = '\0';
			strcpy (strB, str3);
		      }
		    else
		      {
			char str2[16];
			strncpy (str2, x, sizeof (char) * (strlen (x)));
			str2[strlen (x)] = '\0';
			strcpy (strA, str2);
		      }
		  }
	      }else if (strlen (x) == 128){
	          if (strlen (x) % 2 == 0)
	  	    {			// se for par
	  		    char str2[strlen (x) / 2 + 1];
	  		    char str3[strlen (x) / 2 + 1];
	  		    strncpy (str2, x, sizeof (char) * (strlen (x) / 2));
	  		    str2[strlen (x) / 2] = '\0';
	  		    strcpy (strAD, str2);
	  		    strncpy (str3, x + (strlen (x) / 2),sizeof (char) * (strlen (x) / 2));
	  		    str3[strlen (x) / 2] = '\0';
	  		    strcpy (strBD, str3);
	  	   
	  	  }
	      }
	  }
	
	  void tohex (char *x)
	  {
	    unsigned char word[33], outword[65];	//17:16+1, 33:16*2+1
	    int i, len;
	    strcpy (word, x);

	    len = strlen (word);
	    if (word[len - 1] == '\n')
	      word[--len] = '\0';

	    for (i = 0; i < len; i++)
	      {
		sprintf (outword + i * 2, "%02X", word[i]);
	      }
	    printf ("<<%s >> %lu\n", outword, strlen (outword));
	    strcpy (tohexresult, outword); // disponibilizando informação
	  }
	  
	  void tooriginal(char * x){
		char hexstring[65] =  "teste", *pos = hexstring;
			strcpy(hexstring, x);
		int tam = strlen(x);
		unsigned char val[38];
		memset(val,0,sizeof(val));
			size_t count = 0;
		int a =0;
		
		   for(count = 0; count < sizeof(val)/sizeof(val[0]); count++) {
		a++;
			sscanf(pos, "%2hhx", &val[count]);
		pos += 2;
			}
		val[a] = '\0';	
		strcpy(tooriginalresult,val); // disponibilizando informação
	  }
		  
   int ret, fd, a = 0, menu=0;
   char paraCifrar[BUFFER_LENGTH];
   char paraCifrar2[BUFFER_LENGTH];
   char opcao[BUFFER_LENGTH];
   char opcao2[BUFFER_LENGTH];
   strcpy( opcao, argv[1] );
   strcpy( opcao2, argv[1] );
	printf("\n\n");
   printf("Opcao Escolhida :: << %s >> \n", opcao);

   if (argc != 3){
	   printf("Quatidade de Argumentos Errada\n");
      exit(0);
   }
   switch(opcao[0]){
      case 'c':
    	  menu = 1;
    	  printf("*************Cifrar ECB(AES)*************\n");
          printf("\nEntrada do usuario [String] ::<< %s >> :: %lu\n",argv[2],strlen(argv[2]));
          
          if(strlen(argv[2])<=32){
			  split_input (argv[2]);
			 // printf ("strA = %s\n", strA);
			 // printf ("strB = %s\n", strB);
	
			  if (strlen (argv[2]) <= 16)
				{
				  subop = 16;
				  tohex (strA);
				  strcat(opcao, tohexresult);
				  strcpy(paraCifrar, opcao);
				}
			  else
				{
				  subop = 32;
				  tohex (strA);
				  strcat(opcao, tohexresult);
				  strcpy(paraCifrar, opcao);              
				  tohex(strB);
				  strcat(opcao2, tohexresult);
				  strcpy(paraCifrar2, opcao2); 
				}
          }else{
        	  system("clear");
        	  printf("\n\n<< CIFRAR >> :: Entrada deve ser no maximo de (32) caracteres\n");
        	  exit(0);
          }
   	  
         break;
         
         
      case 'd':
    	  menu = 2;
    	  printf("*************Decifrar ECB(AES)*************\n");
    	  printf("\nEntrada do usuario [ Hexa] ::<< %s >> :: %lu\n",argv[2],strlen(argv[2]));
    	  
    	  split_input (argv[2]);
    	 // printf ("strAD = %s :: %lu\n", strAD,strlen(strAD));
    	 // printf ("strBD = %s :: %lu\n", strBD,strlen(strBD));
    	  
    	  if(strlen (argv[2]) <= 128){
    	  
			  if(strlen (argv[2]) <= 64){
				  subop = 16;
				  tooriginal(argv[2]);
				  strcat(opcao, tooriginalresult);
				  strcpy(paraCifrar, opcao);
				  
			  }else{
				  subop =32;
				  tooriginal(strAD);
				  strcat(opcao, tooriginalresult);
				  strcpy(paraCifrar, opcao);
				  tooriginal(strBD);
				  strcat(opcao2, tooriginalresult);
				  strcpy(paraCifrar2, opcao2);
				  
			  }
    	  }else{
    		  system("clear");
    		  printf("\n\n<< DECIFRAR >> :: Entrada  deve ser no maximo de (128) caracteres\n\n");
    		  exit(0);
    	  }
         break;
         
         
      case 'h':
    	  menu = 3;
    	  subop = 16;
         printf("*************Resumo Cryptografico (HASH)*************\n");
         
         
         ///////////////////Converte char(byte) para 2 digito hexa////////////////////////
         printf("\nEntrada do usuario [String] ::<< %s >>",argv[2]);
         tohex(argv[2]);
         /////////////////////////////////////////////////////////////////////////////////
      
         strcat(opcao, tohexresult);
         strcpy(paraCifrar, opcao);
         break;
      default:
         printf("Opcao invalida\n");
         exit(0);
   }
//---------------------------------------------------------------------------------

          //////////////////ABRINDO O DEV CRYPTO ///////////////////////////////////////
	   printf("\nEnviando dados[hexa] para /dev/crypto...\n");
	   fd = open("/dev/CRYPTO", O_RDWR);             // Open the device with read/write access
	   if (fd < 0){
	      perror("Falha ao abrir o Arquivo do dispositivo...");
	      return errno;
	   }
	   ////////////////////////////////////////////////////////////////////////////////////////

	  if(subop ==16)
	  {  /////////// SE FOR APENAS 1 BLOCO
		  //printf("SUBOP16");
		   ret = write(fd, paraCifrar, strlen(paraCifrar)); // Send the string to the LKM
		   if (ret < 0){
		      perror("Falha ao escrever a mensagem no dispositivo.");
		      return errno;
		   }

		   sleep(2);

		   printf("\n[ENTER] para receber resposta do modulo...\n");
		   getchar();

		   ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
		   if (ret < 0){
		      perror("Falha ao receber a mensagem do dispositivo.\n");
		      return errno;
		   }
		   close(fd);
		   
		   if(menu == 3){ // HASH
				printf("Resumo Cryptografico SHA256 ::<< ");

				for(a = 0; a < strlen(receive); a++){
			            printf("%X", receive[a]);
			   }
			   printf(" >>\n");
			   printf("\n*************Resumo Crypt(HASH) Concluido*************\n\n\n");
			   close(fd);
			   return 0;
			   
		   } else if(menu == 1){  // CIFRAR
			  // printf("Msg Cifrada ecb(aes) [String] ::<< %16s >> :: %lu\n", receive, strlen(receive));
			   
		         ///////////////////Converte char(byte) para 2 digito hexa////////////////////////
			   	   tohex(receive);
		         printf("\n*************Cifrar ECB(AES) Concluido*************\n\n\n");
		         /////////////////////////////////////////////////////////////////////////////////
			   return 0;
			   
		   } else if(menu == 2){ // DECIFRAR
			  // printf("Msg Decifrada ecb(aes) [Hexa] ::<< %s >> :: %lu\n", receive, strlen(receive));
			   
			   
			   tooriginal(receive);
	   	        printf("mensagem Original Decifrada << %s >> :: %lu", tooriginalresult, strlen(tooriginalresult));
	   	        printf("\n");
	   	        printf("\n*************Decifrar ECB(AES) Concluido*************\n\n\n");
	   	     
			   return 0;
			   
		   }
		   

	   return 0;
	  }else if (subop == 32){  ///   SUBOP = 32
		 // printf("SUBOP32");
		  
		   ret = write(fd, paraCifrar, strlen(paraCifrar)); // Send the string to the LKM
		   if (ret < 0){
		      perror("Falha ao escrever a mensagem no dispositivo.");
		      return errno;
		   }

		   sleep(1);
		   
		   ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
		   if (ret < 0){
		      perror("Falha ao receber a mensagem do dispositivo.\n");
		      return errno;
		   }
		   
		   strcpy(receive1,receive);/////////////////////////////
		   strncpy (receive, " ", sizeof (char) * (strlen (receive)));
		   
		   
		   ret = write(fd, paraCifrar2, strlen(paraCifrar2)); // Send the string to the LKM
		   if (ret < 0){
		      perror("Falha ao escrever a mensagem no dispositivo.");
		      return errno;
		   }

		   sleep(1);
		   
		   ret = read(fd, receive, BUFFER_LENGTH);        // Read the response from the LKM
		   if (ret < 0){
		      perror("Falha ao receber a mensagem do dispositivo.\n");
		      return errno;
		   }
		   
		   strcpy(receive2,receive);/////////////////////////////
		   close(fd);

		   printf("\n[ENTER] para receber resposta do modulo...\n");
		   getchar();


		   
		   if(menu == 3){ // HASH
				printf("Resumo Cryptografico SHA256 ::<< ");

				for(a = 0; a < strlen(receive); a++){
			            printf("%X", receive[a]);
			   }
			   printf(" >>\n");
			   printf("\n*************Resumo Crypt(HASH) Concluido*************\n\n\n");
			   return 0;
			   
		   } else if(menu == 1){  // CIFRAR
			   //printf("Msg Cifrada ecb(aes) [String1] ::<< %16s >> :: %lu", receive1, strlen(receive1));
			   //printf("Msg Cifrada ecb(aes) [String2] ::<< %16s >> :: %lu", receive2, strlen(receive2));
			   
		         ///////////////////Converte char(byte) para 2 digito hexa////////////////////////
			   	   tohex(receive1);
			   	   strcpy(tohexresultcifra,tohexresult);
			   	   strncpy (tohexresult, " ", sizeof (char) * (strlen (tohexresult)));
			   	   tohex(receive2);
			   	strcat(tohexresultcifra,tohexresult);
			   	printf("Msg Cifrada ecb(aes) [HEXA ] ::<< %s >> :: %lu\n", tohexresultcifra, strlen(tohexresultcifra));
		         printf("\n*************Cifrar ECB(AES) Concluido*************\n\n\n");
		         /////////////////////////////////////////////////////////////////////////////////
			   return 0;
			   
		   } else if(menu == 2){ // DECIFRAR
			   //printf("Msg Decifrada ecb(aes) [Hexa] ::<< %s >> :: %lu\n", receive1, strlen(receive1));
			   //printf("Msg Decifrada ecb(aes) [Hexa] ::<< %s >> :: %lu\n", receive2, strlen(receive2));
			   
			   
			   	tooriginal(receive1);
			   	strcpy(tooriginalresultdecifra,tooriginalresult);
			   	strncpy (tooriginalresult, " ", sizeof (char) * (strlen (tooriginalresult)));
			   	tooriginal(receive2);
			   	strcat(tooriginalresultdecifra,tooriginalresult);
	   	        printf("mensagem Original Decifrada  << %s >> :: %lu", tooriginalresultdecifra, strlen(tooriginalresultdecifra));
	   	        printf("\n");
	   	        printf("\n*************Decifrar ECB(AES) Concluido*************\n\n\n");
			   return 0;
			   
		   }
		   

	   return 0;
		  
	   // Rhenan Christian Do Amaral Arantes - Projeto 1 Sistemas Operacionais B  2018 (15.9)		  
		  
	  }

	   

}

