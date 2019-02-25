#include <iostream>
#include <cstdint>
#include <string.h>

#include "serviceMain.h"
#include "ui.h"

int main(int argc, char* argv[]){
	if(argc == 1){
		// Aucun paramètre => On démare INEPLEM en mode Service
		serviceRegister();
		return 0;
	}
	
	else if(argc == 2){
		// Un paramètre indique un type de lancement particulier
		if(strcmp(argv[1], "-debug") == 0){
			// Lancement hors service pour le developpement
			serviceMain(argc, argv);
			return 0;
		}
	}

	else if(argc == 3){
		// Deux paramètre indique un type de lancement et un port de connection
		if(strcmp(argv[1], "-ui") == 0){
			Ui *ineplemUi = new Ui(strtol(argv[2], NULL, 10));
			while(1){
				ineplemUi->processMessage();
			}
			return 0;
		}
	}

	std::cerr << "Syntaxe des arguments invalide" << std::endl;
	return 1;
}
