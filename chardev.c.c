#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>


#define BUFFER_SIZE 8
#define DEVICE_NAME "chardev"
#define CLASS_NAME "char"

MODULE_LICENSE("GPL");


// Static variables
static int majorNum;
static int entriesInBuffer = 0;
static int buffer[BUFFER_SIZE] = {0};
static int devOpen = 0;
static int readIndex = 0;
static int writeIndex = 0;
static struct class* class = NULL;
static struct device* device = NULL;


// Prototypes 

static int open(struct inode*, struct file*);
static int release(struct inode*, struct file*);
static ssize_t read(struct file*, char*, size_t, loff_t*);
static ssize_t write(struct file*, const char*, size_t, loff_t*);

// File Operations
static struct file_operations fops = 
{
	.open = open,
	.release = release,
	.read = read,
	.write = write
};

int init_module()
{
	printk(KERN_INFO "Chardev: Instantiated - Registering Major Number...\n");
	majorNum = register_chrdev(0, DEVICE_NAME, &fops);
	
	if (majorNum < 0)
	{
		printk(KERN_ALERT "Chardev: Registration failed! Exiting Instantiation!!!\n");
		return 1;
	}
	
	printk(KERN_INFO "Chardev: Chardev successfully registered with major number: %d ...\n", majorNum);
	class = class_create(THIS_MODULE, CLASS_NAME);
	
	if (IS_ERR(class)){               
      unregister_chrdev(majorNum, DEVICE_NAME);
      printk(KERN_ALERT "Chardev: Failed to register device class... Exiting Instantiation\n");
      return PTR_ERR(class);
    }
   printk(KERN_INFO "Chardev: device class registered correctly\n");
   
   device = device_create(class, NULL, MKDEV(majorNum, 0), NULL, DEVICE_NAME);
   if (IS_ERR(device)){
      class_destroy(class);
      unregister_chrdev(majorNum, DEVICE_NAME);
      printk(KERN_ALERT "Chardev: Failed to create the device... Exiting Instantiation\n");
      return PTR_ERR(device);
   }
   printk(KERN_INFO "Chardev: instantiation suscessful\n");
   return 0;
   	
}

void cleanup_module()
{
	device_destroy(class, MKDEV(majorNum, 0));     
	class_unregister(class);                         
	class_destroy(class);                             
	unregister_chrdev(majorNum, DEVICE_NAME);             
	printk(KERN_INFO "Chardev: Signing off!\n");
}

static int open(struct inode* inode, struct file* file)
{
	devOpen++;
	try_module_get(THIS_MODULE);
	printk(KERN_INFO "Chardev: Device opened. There are currently %d opens.\n", devOpen);
	return 0;
}

static int release(struct inode* inode, struct file* file)
{
	devOpen--;
	module_put(THIS_MODULE);
	printk(KERN_INFO "Chardev: Device closed. %d opens\n", devOpen);
	return 0;
}

static ssize_t read(struct file* file, char* outputBuffer, size_t size, loff_t* loff)
{
	int charactersRead = 0;
	
	/*
		While there is room in the output buffer and while there exists entries to be read, we read. 
		Read from the buffer using the readIndex. Set that index's data to NULL (pop), then incrememnt the 
		readIndex. We've popped one value from the buffer, so decrement it's size counter (entriedInBuffer) and 
		decrement the size left in outputBuffer.
	*/
	while (entriesInBuffer > 0 && size > 0)
	{
		if (readIndex >= BUFFER_SIZE)
			readIndex = 0;
		
		put_user(buffer[readIndex++], outputBuffer++);
		entriesInBuffer--;
		size--;
		charactersRead++;
	}
	
	return charactersRead;
}

static ssize_t write(struct file* file, const char* input, size_t len, loff_t* loff)
{
	int charactersRead = 0;
	int i;
	
	for (i = 0; i < len && entriesInBuffer < BUFFER_SIZE; i++)
	{
		if (writeIndex >= BUFFER_SIZE)
			writeIndex = 0;
		
		buffer[writeIndex++] = input[i];
		entriesInBuffer++;
		charactersRead++;		
	}
	
	return charactersRead;
}













