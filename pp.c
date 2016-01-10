#include "pp.h"
#include "pile.h"

void usage(){
	fprintf(stderr, "Usage : pp Src_file_name Dst_file_name\n");
}

void erreur(int n, char* err){
	fprintf(stderr, "Erreur : %s\n", err);
	usage();
	exit(n);
}

void perreur(int n, char* err){
	perror(err);
	usage();
	exit(n);
}

void detect_fin_com(int *etat, char c, FILE *dst){
	if (*etat == 0){
		*etat = (c=='*')?1:0;
	}else if (*etat == 1){
		if (c == '/'){
			*etat = 2;
		}else{
			*etat = 0;
			fprintf(dst, "*");
		}
	}else{
		fprintf(stderr, "Tu ne devrais pas mettre à jour etat_fin_com.\n");
	}
}

void detect_com(int *etat_com,int *etat_fin, char c, FILE *dst){
	if (*etat_com == 0) {
		*etat_com = (c=='/')?1:0;
	}else if (*etat_com == 1) {
		if (c == '*'){
			*etat_com = 2;
		}else{
			*etat_com = 0;
			fprintf(dst, "/");
		}
	}else{
		fprintf(stderr, "Tu ne devrais pas mettre à jour etat_com.\n");
	}
	detect_fin_com(etat_fin, c, dst);
	if (!*etat_com && !*etat_fin)
		fprintf(dst,"%c", c);
}

void detect_for(int* etat_for, char c){
	if (c=='f')
		*etat_for=1;
	else
		*etat_for=0;
}

void detect_struct(int* etat_struc, char c){
	if (c=='s')
		*etat_struc=1;
	else
		*etat_struc=0;
}

void detect_do(int* etat_do, char c){
	if (c=='d')
		*etat_do=1;
	else
		*etat_do=0;
}


void search_do(int* etat_do, Stack baDo, int prof, char c){
	if (*etat_do==2){
		if (c=='{'){
			*etat_do=3;
			push(baDo, prof);
			*etat_do=0;
		}
		else if (c!=' ')
			*etat_do=0;
	}
	
	if (*etat_do==1){
		if (c=='o')
			*etat_do=2;
		else
			*etat_do=0;
	}
}

void search_for(int* etat_for, char c){
	if (*etat_for==3){
		if (c=='(')
			*etat_for=4;
		else if (c!=' ')
			*etat_for=0;
	}
	
	if (*etat_for==2){
		if (c=='r')
			*etat_for=3;
		else
			*etat_for=0;
	}
	
	if (*etat_for==1){
		if (c=='o')
			*etat_for=2;
		else
			*etat_for=0;
	}
}
	
void search_struct(int* etat_struc, char c){
	if (*etat_struc==6){
		if (c==' ')
			*etat_struc=7;
		else
			*etat_struc=0;
	}

	if (*etat_struc==5){
		if (c=='t')
			*etat_struc=6;
		else
			*etat_struc=0;
	}
	
	if (*etat_struc==4){
		if (c=='c')
			*etat_struc=5;
		else
			*etat_struc=0;
	}

	if (*etat_struc==3){
		if (c=='u')
			*etat_struc=4;
		else
			*etat_struc=0;
	}
	
	if (*etat_struc==2){
		if (c=='r')
			*etat_struc=3;
		else
			*etat_struc=0;
	}
	if (*etat_struc==1){
		if (c=='t')
			*etat_struc=2;
		else
			*etat_struc=0;
	}
}

void decNbAcc(int *nbAcc, int *nbErrB, int nbline, int nbchar){
	if (*nbAcc > 0)
		--(*nbAcc);
	else {
		fprintf(stderr, "Bloc fermé non ouvert en l%d:c%d du fichier source.\n",nbline, nbchar);
		++(*nbErrB);
	}
}

void indente (FILE *dst, int nbAcc){
	for (int i = 0; i<nbAcc; ++i)
		fprintf(dst,"\t");
}

void transINLINE(FILE* dst, int *etat, int *etat_com, int *etat_fin_com, int *nbAcc, int *nbErrB, int nbline, int nbchar, char c, int* struc, Stack biDo, int endDo){
	if (c == '{'){
		*etat = NEW_BLOC;
		fprintf(dst,"\n");
		indente(dst, *nbAcc);
	}else if (c == '}'){
		decNbAcc(nbAcc, nbErrB, nbline, nbchar);
	}else if (c == '"'){
		*etat = IN_STR;
	}else if (c == '\n' && !endDo){
		*etat = NEW_LINE;
	}
	if (!endDo || (endDo && c!='\n'))
		detect_com(etat_com, etat_fin_com, c, dst);
}

int main(int argc, char** argv){
	if (argc != 3){
		erreur(1, "Nombre de parametres incorrect.");
	}
	FILE *src, *dst;
	if (!(src=fopen(argv[1],"r"))){
		perreur(2, "Impossible d'ouvrir le fichier source");
	}
	if (!(dst=fopen(argv[2],"w"))){
		perreur(3, "Impossible d'ouvrir le fichier destination");
	}
	int nbAcc = 0,
		struc = 0,
		etat = NEW_LINE,
		etat_com = 0,
		etat_fin_com = 0,
		nbline = 1,
		nbchar = 0,
		nbErrB = 0,
		nbErrC = 0,
		biDo=0,
		inFor = 0,
		endDo = 0,
		etat_ppc = 0;
		
	char c;
	Stack nbDo=new_stack();
	
	
	while ((c = fgetc(src)) != EOF){
		if (c == '\n'){
			nbline++;
			nbchar=0;
		}else
			++nbchar;
		
		
		switch (etat) {
			case NEW_LINE:
				if (c != ' ' && c != '\t' && c != '\n'){
					if (c == '"'){
						etat = IN_STR;
					}else if (c == '{'){
						etat = NEW_BLOC;
					}else if (c == '}'){
						decNbAcc(&nbAcc, &nbErrB, nbline, nbchar);
						if (struc<7 && top(nbDo)!=nbAcc)
							etat=END_BLOC;
						else {
							endDo=1;
							etat=IN_LINE;
							if (struc>=7)
								struc=8;
							else
								pop(nbDo);
						}
					}else if (c == '#'){
						etat = IN_PPC;
					}else{
						etat = IN_LINE;
					}
					indente(dst, nbAcc);
					
					if (struc!=7)
						detect_struct(&struc, c);
					detect_do(&biDo, c);
					detect_for(&inFor, c);
					detect_com(&etat_com, &etat_fin_com, c, dst);
				}
				break;
				
			case IN_LINE:
				transINLINE(dst, &etat, &etat_com, &etat_fin_com, &nbAcc, &nbErrB, nbline, nbchar, c, &struc, nbDo, endDo);
				search_do(&biDo, nbDo, nbAcc, c);
				search_for(&inFor, c);
				search_struct(&struc, c);
				
				if (inFor!=4 && c == ';'){
					endDo=0;
					if (struc==8)
						struc=0;
					fprintf(dst, "\n");
					etat=NEW_LINE;
				}
				break;
				
			case NEW_BLOC:
				++nbAcc;
				fprintf(dst, "\n");
				if (c == '\n' || c == ' ' || c == '\t'){
					etat = NEW_LINE;
				}else{
					if (c == '"'){
						etat = IN_STR;
					}else if (c == '}'){
						decNbAcc(&nbAcc, &nbErrB, nbline, nbchar);
						if (struc<7 && top(nbDo)!=nbAcc)
							etat = END_BLOC;
						else {
							etat=IN_LINE;
							if (struc>=7)
								struc=8;
							else{
								pop(nbDo);
								endDo=1;
							}
						}
					}else if (c != '{'){
						etat = IN_LINE;
					}
					indente(dst, nbAcc);
					detect_do(&biDo, c);
					detect_for(&inFor, c);
					if (struc<7)
						detect_struct(&struc, c);
					detect_com(&etat_com, &etat_fin_com, c, dst);
				}
				break;
				
			case END_BLOC:
				fprintf(dst, "\n");
				if (c == '\n' || c == ' ' || c == '\t'){
					etat = NEW_LINE;
				}else{
					if (c == '"'){
						etat = IN_STR;
					}else if (c == '{'){
						etat = NEW_BLOC;
					}else if (c == '}'){
						decNbAcc(&nbAcc, &nbErrB, nbline, nbchar);
						if (struc<7 && top(nbDo)!=nbAcc)
							etat = END_BLOC;
						else {
							etat=IN_LINE;
							if (struc>=7)
								struc=8;
							else{
								pop(nbDo);
								endDo=1;
							}
						}
					}else{
						etat = IN_LINE;
					}
					indente(dst, nbAcc);
					detect_do(&biDo, c);
					detect_for(&inFor, c);
					detect_com(&etat_com, &etat_fin_com, c, dst);
				}
				break;
				
			case IN_COM:
				detect_fin_com(&etat_fin_com, c, dst);
				if (c == '\n'){
					fprintf(dst, "*/\n");
					indente(dst, nbAcc);
					fprintf(dst, "/*");
				}else if (!etat_fin_com)
					fprintf(dst, "%c", c);
				
				break;
				
			case NEW_LINE_COM:
				if (c != ' ' && c != '\t'){
					fprintf(dst, "/*%c", c);
					etat = IN_COM;
					detect_fin_com(&etat_fin_com, c, dst);
				}
				break;
				
			case IN_STR:
				if (c =='"'){
					etat = IN_LINE;
				}else if (c == '\n'){
					fprintf(dst, "\"");
					etat = NEW_LINE_STR;
				}
				fprintf(dst, "%c", c);
				break;
				
			case NEW_LINE_STR:
				if (c != ' ' && c != '\n' && c != '\t'){
					indente(dst, nbAcc);
					fprintf(dst, "\"%c", c);
					if (c == '"')
						etat = IN_LINE;
					else
						etat = IN_STR;
				}
				break;
				
				
			case IN_PPC:
				if (c == '\\'){
					if (etat_ppc == 0)
						etat_ppc = 1;
					else if (etat_ppc==1)
						etat_ppc=0;//si jamais le "\" était échappé un autre "\"
				}
				if (c=='\n'){
					if (etat_ppc==1){//si le "\" est suivi d'une vrai entrée
						etat_ppc = 0;
					}else//si il y a une entrée seule
						etat = NEW_LINE;
				}
				detect_com(&etat_com, &etat_fin_com, c, dst);
				if (etat_com==2){
					fprintf(dst, "\n");
					indente(dst, nbAcc);
				}
				break;
				
			default:
				printf("This is not a state.\n");
				break;
		}
		if (etat_com == 2){
			etat = IN_COM;
			etat_com = 0;
			etat_fin_com =0;
			fprintf(dst, "/*");
		}
		if (etat_fin_com == 2){
			if (etat != IN_COM){
				fprintf(stderr, "Commentaire fermé avant d'être ouvert en l%d:%d du fichier source.\n", nbline, nbchar);
				++nbErrC;
			}
			fprintf(dst, "*/\n");
			etat = NEW_LINE;
			etat_com = 0;
			etat_fin_com = 0;
		}
	}
	
	if (feof(src)){
		printf("Fin lecture.\n");
		fprintf(stderr, "Il y a eu %d blocs fermés sans être ouverts, %d blocs non refermés, et %d commentaires fermés sans être ouverts.\n",nbErrB, nbAcc, nbErrC);
		if (etat == IN_COM || etat == NEW_LINE_COM){
			fprintf(stderr, "Le fichier source termine par un commentaire non fermé.\n");
		}
	}else{
		perreur(4, "Erreur lecture fichier source");
	}
	delete(nbDo);
	fclose(src);
	fclose(dst);
}