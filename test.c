#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
 
#define BUFFER_LENGTH 1028           ///< The buffer length (crude but fine)
static char receive[BUFFER_LENGTH];     ///< The receive buffer from the LKM
 
int main(){
   int ret, fd;
   int command = 0;
   int numBytes;
   char stringToSend[BUFFER_LENGTH];
   printf("Starting device test code example...\n");
   fd = open("/dev/chardev", O_RDWR);             // Open the device with read/write access
   if (fd < 0){
      perror("Failed to open the device...");
      return errno;
   }
   
   while (1)
   {
	   printf("1. Read\n2. Write 3. Exit\n");
	   scanf("%d", &command);
	   
	   if (command == 1)
	   {
	           receive[0] = 0;
		   printf("How many bytes would you like to read?\n");
		   scanf(" %d", &numBytes);
		   printf("Reading from the device...\n");
			ret = read(fd, receive, numBytes);
			printf("%s\n", receive); 
			strcpy(receive, "!!!!!!!!!!!");
		   
	   }
	   else if (command == 2)
	   {
		printf("Type in a short string to send to the kernel module:\n");
		scanf(" %s", stringToSend);                // Read in a string (with spaces)
		ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
		if (ret < 0)
		{
			perror("Failed to write the message to the device.");
			return errno;
		}

	   }
	   else if (command == 3)
	   {
		   return 0;
	   }
	   else 
	   {
		   printf("Invalid option!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	   }
   }
   return 0;
}
