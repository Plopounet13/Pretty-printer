#include <stdio.h>
#if !(defined __LP64__ || defined __LLP64__) || defined _WIN32 && !defined _WIN64
void bit()
{
	for(int i=1; i<=32; i++)
	{
		if (i%32)
		{
			printf("32 bits\n");
		}
	}
}

/* compilation pour un systeme 32-bit */

#else
void bit()
{
	for(int i=1; i<=32; i++)
	{
		if (i%32)
		{
			printf("64 bits\n");
		}
	}
}

/* compilation pour un systeme 64-bit */

#endif
#define ACVJ B

int test1(int number);
int* test2(int number);
/* test de l'intendation*/

/* des déclarations de fonctions*/



struct complex
{
	int Re;int Im;
}

/*test des struct*/


;

int main (int argc, char** argv)
{
	printf("test ");
	printf("reussi\n");
	/*test de saut à la ligne standard*/
	

	int boucle=1;
	for(int i=0;i<=5;i++)
	{
		printf("test serie 1 num %i\n",i);
	}
	
	/* test*/
	/*du for*/
	
int i=0;
	

	do
	{
		printf("test serie 2 num %i\n", i);
		i++;
		if (i%2)
		{
			printf("%i est impair\n",i);
		}
		else 
		{
			printf("%i pair\n", i);
		}
	}
	while (i<10);
	

	return 0;
}