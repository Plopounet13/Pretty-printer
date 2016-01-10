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
	int nbAcc = 0, etat = NEW_LINE, etat_com = 0;
	char c;
	
	while ((c = fgetc(src)) != EOF){
		switch (etat) {
			case NEW_LINE:
				if (c != ' ' && c != '\t'){
					for (int i = 0; i<nbAcc; ++i)
						fprintf(dst,"\t");
					detect_com(&etat_com, c, dst);
					if (c == '"'){
						etat = IN_STR;
					}else if (c == '{'){
						etat = NEW_BLOC;
					}else{
						etat = IN_LINE;
					}
				}
				break;
				
			case IN_LINE:
				break;
				
			case NEW_BLOC:
				break;
				
			case END_BLOC:
				break;
				
			case NEW_COM:
				break;
				
			case IN_COM:
				break;
				
			case NEW_LINE_COM:
				break;
				
			case END_COM:
				break;
				
			case IN_STR:
				break;
				
			case NEW_LINE_STR:
				break;
				
			default:
				printf("This is not a state.\n");
				break;
		}
		if (etat_com == 2){
			etat = NEW_COM;
			etat_com = 0;
			fprintf(dst, "\n/*");
		}
	}
	
	if (feof(src)){
		printf("Fin lecture");
	}else{
		perreur(4, "Erreur lecture fichier source");
	}
	
	fclose(src);
	fclose(dst);
}