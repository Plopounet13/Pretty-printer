#include <stdio.h>/* compilation pour *//*un systeme 32-bit */
#if !(defined __LP64__ || defined __LLP64__) \
|| defined _WIN32 && !defined _WIN64
void bit(){for(int i=1; i<=32; i++){if (i%32){printf("32 bits\n");}}}
#else/* compilation \ pour un systeme 64-bit */
void bit(){for(int i=1; i<=32; i++){if (i%32){printf("64 bits\n");}}}
#endif
