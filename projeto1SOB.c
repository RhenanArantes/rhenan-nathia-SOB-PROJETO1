#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/crypto.h>
#include <linux/scatterlist.h>
#include <crypto/hash.h>
#include <crypto/sha.h>
#include <crypto/skcipher.h>
#include <crypto/aes.h>
#include <linux/random.h>
#include <linux/err.h>

#define DEVICE_NAME "PROJ1"
#define CLASS_NAME  "proj1"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grupo - Rhenan, Nathia, Gabriela ");
MODULE_DESCRIPTION("SOB - Projeto 1 ");
MODULE_VERSION("0.0");

static int majorNumber;
static char message[256] = {0};
static short size_of_message;
static int numberOpens = 0;
static struct class* proj1Class = NULL;
static struct device* proj1Device = NULL;
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static int test_hash(const unsigned char *data, unsigned int datalen, unsigned char *digest);

static char *key = "00112233445566778899aabbccddeeff"; //Chave simétrica que será usada para cifrar e decifrar

static struct file_operations fops =
{
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};


static int do_transformation(unsigned char *key, unsigned char *input, unsigned char *output, unsigned char opcao)
{//implementação da função para realizar as transformações}



static int __init proj1_init(void) {

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber<0) {
        printk(KERN_ALERT "Não foi possivel registrar Major Number\n");
        return majorNumber;
    }
    printk(KERN_INFO "Major Number alocado para proj1 :  %d\n", majorNumber);
    proj1Class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(proj1Class)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Não foi possivel registrar a device class\n");
        return PTR_ERR(proj1Class);
    }
    printk(KERN_INFO "Device class registrada com sucesso\n");
    proj1Device = device_create(proj1Class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(proj1Device)) {
        class_destroy(proj1Class);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Não foi possivel criar o dispositivo\n");
        return PTR_ERR(proj1Device);
    }
    printk(KERN_INFO "Crypto-kernel-proj1 Inicializado \n");    
    return 0;
}

static void __exit proj1_exit(void) {
    device_destroy(proj1Class, MKDEV(majorNumber, 0));
    class_unregister(proj1Class);
    class_destroy(proj1Class);
    unregister_chrdev(majorNumber, DEVICE_NAME); 
    printk(KERN_INFO "Crypto-kernel-proj1 Finalizado!\n");
}


static int dev_open(struct inode *inodep, struct file *filep) {
    numberOpens++;
    printk(KERN_INFO "Crypto-kernel-proj1 está sendo aberto pela %d vez\n", numberOpens);
    return 0;
}


static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int error_count = 0;
    error_count = copy_to_user(buffer, message, size_of_message);
    if (error_count==0) { 
    	printk(KERN_INFO "Foram enviados para o programa Teste %d caracteres\n", size_of_message);
        printk(KERN_INFO "Conteudo enviado: %s \n", message);
        return (size_of_message=0);
    }
    else {
        printk(KERN_INFO "Falha ao enviar  %d caracteres para programa teste\n", error_count);
        return -EFAULT;
    }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {

    char opcao[BUFFER_LENGTH];
    int ret=0;
    unsigned char *digest = vmalloc(16);
    unsigned char *input = vmalloc(16);
    unsigned char *output = vmalloc(16);

    strcpy( opcao, buffer );
    buffer = buffer + 1;   // capturando a opção enviada pelo programa teste

    switch(opcao[0]){
       case 'c':
          sprintf(input, "%s", buffer);

          printk(KERN_INFO "------------------------------------C------------------------------------");
          do_transformation(key, input, output, 'c');
          printk(KERN_INFO "Mensagem Cifrada é : %.16s", output);
          strcpy( message, output ); // disponibilizando para o READ
          size_of_message = strlen(message); 
          return len;
          break;
          
       case 'd':
          sprintf(input, "%s", buffer);
          
	      printk(KERN_INFO "Mensagem: %.16s", buffer);
          printk(KERN_INFO "------------------------------------D------------------------------------");
          do_transformation(key, input, output, 'd');
          printk(KERN_INFO "Mensagem Decifrada: %.16s", output);
          strcpy( message, output );
          size_of_message = strlen(message); 
          return len;
          break;
          
       case 'h':
          sprintf(message, "%s", buffer);
          size_of_message = strlen(message);
 
          //do_hash(message, size_of_message, digest);
 
          printk(KERN_INFO "------------------------------------H------------------------------------");
          printk(KERN_INFO "Resumo Cryptografico é:  %s \n", digest);
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
    printk(KERN_INFO "Crypto-kernel-projeto1 finalizado com sucesso\n");
    return 0;
}
module_init(proj1_init);
module_exit(proj1_exit);
