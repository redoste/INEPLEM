// Copyright: 2019 redoste
/*
*	This file is part of INEPLEM.
*
*	INEPLEM is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	INEPLEM is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with INEPLEM.  If not, see <https://www.gnu.org/licenses/>.
*/
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