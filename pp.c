#include "pp.h"

void usage(){
	fprintf(stderr, "Usage : pp Src_file_name Dst_file_name\n");
}

void erreur(int n, char* err){
	fprintf(stderr, "Erreur : %s\n", err);
	usage();
	exit n;
}

void perreur(int n, char* err){
	perror(err);
	usage();
	exit n;
}

int main(int argc, char** argv){
	if (argc != 3){
		erreur(1, "Nombre de parametres incorrect.");
	}
	FILE* src, dst;
	if (!(src=fopen(argv[1],"r"))){
		perreur(2, "Impossible d'ouvrir le fichier source");
	}
	if (!(dst=fopen(argv[2],"w"))){
		perreur(3, "Impossible d'ouvrir le fichier destination");
	}
	
	
	
	fclose(src);
	fclose(dst);
}