#include "pp.h"

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

void detect_com(int *etat, char c, FILE *dst){
	if (*etat == 0) {
		*etat = (c=='/')?1:0;
	}else if (*etat == 1) {
		if (c == '*'){
			*etat = 2;
		}else{
			*etat = 0;
			fprintf(dst, "/");
		}
	}else{
		fprintf(stderr, "Tu ne devrais pas mettre à jour etat_com.\n");
	}
	if (!*etat)
		fprintf(dst,"%c", c);
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

void decNbAcc(int *nbAcc, int *nbErrB, int nbline, int nbchar){
	if (*nbAcc > 0)
		--(*nbAcc);
	else {
		fprintf(stderr, "Bloc fermé non ouvert en l%d:c%d du fichier source.\n",nbline, nbchar);
		++nbErrB;
	}
}

void indente (FILE *dst, int nbAcc){
	for (int i = 0; i<nbAcc; ++i)
		fprintf(dst,"\t");
}

void transINLINE(FILE* dst, int *etat, int *etat_com, int *nbAcc, int *nbErrB, int nbline, int nbchar, char c){
	if (c == '{'){
		*etat = NEW_BLOC;
		fprintf(dst,"\n");
		indente(dst, *nbAcc);
	}else if (c == '}'){
		*etat = END_BLOC;
		decNbAcc(nbAcc, nbErrB, nbline, nbchar);
		fprintf(dst,"\n");
		indente(dst, *nbAcc);
	}else if (c == '"'){
		*etat = IN_STR;
	}else if (c == '\n'){
		*etat = NEW_LINE;
	}
	detect_com(etat_com, c, dst);
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
		etat = NEW_LINE,
		etat_com = 0,
		etat_fin_com = 0,
		nbline=1,
		nbchar=0,
		nbErrB = 0,
		nbErrC = 0,
		etat_ppc = 0;
	char c;
	/*
	 **
	 **
	  A FAIRE :
		GERER COMMENTAIRES TROP FERMES
	 **
	 **
	*/
	while ((c = fgetc(src)) != EOF){
		if (c == '\n'){
			nbline++;
			nbchar=0;
		}else
			++nbchar;
		switch (etat) {
			case NEW_LINE:
				/* A CHOISIR : saut de ligne autorisé ? */
				if (c != ' ' && c != '\t'){
					if (c == '"'){
						etat = IN_STR;
					}else if (c == '{'){
						etat = NEW_BLOC;
					}else if (c == '}'){
						etat = END_BLOC;
						decNbAcc(&nbAcc, &nbErrB, nbline, nbchar);
					}else{
						etat = IN_LINE;
					}
					indente(dst, nbAcc);
					detect_com(&etat_com, c, dst);
				}
				break;
				
			case IN_LINE:
				transINLINE(dst, &etat, &etat_com, &nbAcc, &nbErrB, nbline, nbchar, c);
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
						etat = END_BLOC;
						decNbAcc(&nbAcc, &nbErrB, nbline, nbchar);
					}else if (c != '{'){
						etat = IN_LINE;
					}
					indente(dst, nbAcc);
					detect_com(&etat_com, c, dst);
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
					}else{
						etat = IN_LINE;
					}
					indente(dst, nbAcc);
					detect_com(&etat_com, c, dst);
				}
				break;
				
			case IN_COM:
				if (c == '\n'){
					etat = NEW_LINE_COM;
				}
				detect_fin_com(&etat_fin_com, c, dst);
				if (!etat_fin_com)
					fprintf(dst, "%c", c);
				break;
				
			case NEW_LINE_COM:
				if (c != ' ' && c != '\t'){
					indente(dst, nbAcc);
					fprintf(dst, "/*%c", c);
					etat = IN_COM;
					detect_fin_com(&etat_com, c, dst);
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
				detect_com(&etat_com, c, dst);
				break;
				
			default:
				printf("This is not a state.\n");
				break;
		}
		if (etat_com == 2){
			etat = IN_COM;
			etat_com = 0;
			etat_fin_com =0;
			fprintf(dst, "\n");
			indente(dst, nbAcc);
			fprintf(dst, "/*");
		}
		if (etat_fin_com == 2){
			if (etat != IN_COM){
				fprintf(stderr, "Commentaire fermé avant d'être ouvert en l%d:c%d du fichier source.\n", nbline, nbchar);
				++nbErrC;
			}
			etat = NEW_LINE;
			etat_com = 0;
			etat_fin_com = 0;
			fprintf(dst, "*/\n");
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
	
	fclose(src);
	fclose(dst);
}