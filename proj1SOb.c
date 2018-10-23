#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/crypto.h>
#include <linux/scatterlist.h>
#include <crypto/hash.h>
#include <crypto/sha.h>
#include <crypto/skcipher.h>
#include <crypto/aes.h>
#include <linux/err.h>

#define DEVICE_NAME "CRYPTO"
#define CLASS_NAME  "crypto"
#define BUFFER_LENGTH 256 
#define DATA_SIZE     32
#define KEY_SIZE      32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rhenan Arantes SOB");
MODULE_DESCRIPTION("Projeto 1 - Sistemas Operacionais B");
MODULE_VERSION("15.9");

static char *key = "0123456789ABCDEF"; //Chave simétrica (default) que será usada para cifrar e decifrar

static int majorNumber;
static char message[129] = {0};
static short size_of_message;
static int numberOpens = 0;
static struct class* classmodule = NULL;
static struct device* devicemodule = NULL;
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static int resumo_hash(const unsigned char *data, unsigned int datalen, unsigned char *digest);
static char word[37] = {0};
static char outword[68] = {0};
static int in;
static int len;



static struct file_operations fops =
{
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

module_param(key, charp, 0000);
MODULE_PARM_DESC(key, "Chave simétrica que será usada para cifrar e decifrar");


///////////////////FUNCAO RESPONSAVEL PELAS TRANSFORMAÇOES CRYPTOGRAFICAS//////////////////////////////////
static int do_transformation(unsigned char *key, unsigned char *input, unsigned char *output, unsigned char operacao)
{
    struct scatterlist sg_input;
    struct scatterlist sg_output;
    struct crypto_skcipher *skcipher = NULL;
    struct skcipher_request *req = NULL;
    int ret = -EFAULT;   
    /////////////////////////////////////////////////////
    
    skcipher = crypto_alloc_skcipher("ecb-aes-aesni", 0, 0);  // criando handler para solicitar as transformações
    if (IS_ERR(skcipher)) {
        pr_info("problemas ao alocar skcipher handler\n");
        return PTR_ERR(skcipher);
    }
    //////////////////////////////////////////////////////
    req = skcipher_request_alloc(skcipher, GFP_KERNEL);  // registrando o handler na estrutura de dados de solicitação
    if (!req) {
        pr_info("Falha ao alocar SKCIPHER\n");
        ret = -ENOMEM;
        goto out;
    }
    ///////////////////////////////////////////////////////
    if (crypto_skcipher_setkey(skcipher, key, KEY_SIZE)) { // definindo a a chave  (eh hexa - seu tamanho define tamanho do bloco)
        pr_info("Problema ao setar a chave de cfiragem\n");
        ret = -EAGAIN;
        goto out;
    }
    /////////////////////////////////////////////////////// 
    printk(KERN_INFO "tamanho do bloco :: << %d >> \n", crypto_skcipher_blocksize(skcipher)); // verificando tamanho do bloco da cifra  no caso 16bytes

    ///////////////////////////////////////////   
    sg_init_one(&sg_input, input, DATA_SIZE);  // iniciando a estrutura que eh passada para a chamada crypto  com os dados
    sg_init_one(&sg_output, output, DATA_SIZE);
    ///////////////////////////////////////////

    skcipher_request_set_crypt(req, &sg_input, &sg_output, DATA_SIZE, NULL); // não sera utilizado IV, desta forma ele sera null  (data_size retorno em bytes)
    
    switch(operacao) {
    case 'c':
        ret = crypto_skcipher_encrypt(req); // executa a transformação de cifra propriamente dito
        
        if (ret)
            goto out;
        pr_info("Cifragem realizada\n");
        break;
    case 'd':
        ret = crypto_skcipher_decrypt(req); // realiza a transformação de decifragem
        if (ret)
            goto out;
        pr_info("decifragem realizada\n");
        break;
    default:
        goto out;
    }    
  
out:
    if (skcipher)
        crypto_free_skcipher(skcipher);  // liberando handler
    if (req)
        skcipher_request_free(req); // liberando as estruturas
    return ret;
}

static int __init proj1_init(void) {
    printk(KERN_INFO "******Crypto Module Kernel INICIADO******\n");
    
    ///////////////////Converte 1 char (1byte)  to HEXA (apenas para chave)////////////////////////
    strcpy(word, key);
    printk(KERN_INFO "Chave inicializada [String] ::<<  %s  >>\n", word);
    len = strlen(word);
    if(word[len-1]=='\n')
        word[--len] = '\0';

    for(in = 0; in<len; in++){
        sprintf(outword+in*2, "%02X", word[in]);
    }
    printk(KERN_INFO "Chave inicializada [Hexa] ::<<  %s  >>\n", outword);
    /////////////////////////////////////////////////////////////////////////////////

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber<0) {
        printk(KERN_ALERT "Falha no registro do major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "Sucesso: Major Number utilizado :: <<%d>>\n", majorNumber);
    classmodule = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(classmodule)) { 
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Falha ao registrar o device\n");
        return PTR_ERR(classmodule); 
    }
    printk(KERN_INFO "classmodule registrada com sucesso\n");
    devicemodule = device_create(classmodule, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(devicemodule)) { 
        class_destroy(classmodule);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Erro ao criar o device\n");
        return PTR_ERR(devicemodule);
    }
    printk(KERN_INFO "Device inicializado com sucesso\n");
    return 0;
}


static void __exit proj1_exit(void) {
    device_destroy(classmodule, MKDEV(majorNumber, 0)); 
    class_unregister(classmodule);
    class_destroy(classmodule);
    unregister_chrdev(majorNumber, DEVICE_NAME); 
    printk(KERN_INFO "Encerrando modulo de Kernel crypto\n");
}

static int dev_open(struct inode *inodep, struct file *filep) {
    numberOpens++;
    printk(KERN_INFO "Abrindo /dev/crypto pela %d vez\n", numberOpens);
    return 0;
}


static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int error_count = 0;
    error_count = copy_to_user(buffer, message, size_of_message);
    if (error_count==0) { 
        printk(KERN_INFO "Enviados para programa teste %d caracteres\n", size_of_message);
        return (size_of_message=0);
    }
    else {
        printk(KERN_INFO "Erro ao enviar %d caracteres ao programa teste \n", error_count);
        return -EFAULT;
    }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {

    char opcao[BUFFER_LENGTH];
    int ret=0;
    unsigned char *digest = vmalloc(36); //retorno do hash
    unsigned char *input = vmalloc(36); // entrada para função do_transformation 32+1
    unsigned char *output = vmalloc(36); // resposta da função do_transformation
    strcpy( opcao, buffer );
    buffer = buffer + 1; 

    switch(opcao[0]){
       case 'c':
          sprintf(input, "%s", buffer);
          printk(KERN_INFO "****** Opcao Cifragem ******\n");

          do_transformation(outword, input, output, 'c');////////////////
          strcpy( message, output );

          
          printk(KERN_INFO "Mensagem Cifrada [String] :: <<  %16s  >> :: %lu \n",message, strlen(message));
          size_of_message = strlen(message); 
          return len;
          break;
       case 'd':
          sprintf(input, "%s", buffer);
          printk(KERN_INFO "****** Opcao Cifragem ******\n");
          printk(KERN_INFO "Mensagem p/ Decifrar [Hexa] :: <<  %s  >> :: %lu\n", input,strlen(input));

          do_transformation(outword, input, output, 'd');          
          strcpy( message, output );
          
          printk(KERN_INFO "Mensagem Decifrada [String] :: <<  %16s  >> :: %lu\n", message,strlen(message));
          size_of_message = strlen(message); 

          return len;
          break;
          
          
       case 'h':
          sprintf(message, "%s", buffer); 
          size_of_message = strlen(message); 
 
          ret = resumo_hash(message, size_of_message, digest);
 
          printk(KERN_INFO "****** Opcao HASH ******\n");
          printk(KERN_INFO "HASH :: << %s >> \n", digest);
          strcpy( message, digest );
          size_of_message = strlen(message);         
          return len;
          break;
       default:
          break;
    }

    return len;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Device Finalizado com sucesso\n");
    return 0;
}


struct sdesc {
   struct shash_desc shash;
   char ctx[];
};

static struct sdesc *init_sdesc(struct crypto_shash *alg)
{
   struct sdesc *sdesc;
   int size;

   size = sizeof(struct shash_desc) + crypto_shash_descsize(alg);
   sdesc = kmalloc(size, GFP_KERNEL);
   if (!sdesc)
      return ERR_PTR(-ENOMEM);
   sdesc->shash.tfm = alg;
   sdesc->shash.flags = 0x0;
   return sdesc;
}

static int hash_digest(struct crypto_shash *alg, const unsigned char *data, unsigned int datalen, unsigned char *digest)
{
   struct sdesc *sdesc;
   int ret;

   sdesc = init_sdesc(alg);
   if (IS_ERR(sdesc)) {
      pr_info(" problema ao configurar Sdesc\n");
      return PTR_ERR(sdesc);
   }

   ret = crypto_shash_digest(&sdesc->shash, data, datalen, digest);
   kfree(sdesc);
   return ret;
}


static int resumo_hash(const unsigned char *data, unsigned int datalen, unsigned char *digest)
{
   struct crypto_shash *alg;
   char *hash_alg_name = "sha256"; // escolha do alg
   int ret;

   alg = crypto_alloc_shash(hash_alg_name, 0, CRYPTO_ALG_ASYNC);
   if (IS_ERR(alg)) {
      pr_info("problema ao configurar algoritmo %s\n", hash_alg_name);
      return PTR_ERR(alg);
   }
   ret = hash_digest(alg, data, datalen, digest);
   crypto_free_shash(alg);
   return ret;
}
// Rhenan Christian Do Amaral Arantes - Projeto 1 Sistemas Operacionais B  2018 (15.9)
module_init(proj1_init);
module_exit(proj1_exit);
