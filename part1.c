#include <unistd.h>

int main(void)
{
    getpid();                  
	getppid();                 
	getuid();                  
	getgid();                  
	write(1, "hi\n", 3);       
	return 0;
}