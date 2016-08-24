/**
* @search_masters.c
* @brief scan for a Link masters on the LAN
*
* @author Davide Talon (<talon.davide@gmail.com>)
* @version 1.0
* @since 1.0
*
* @copyright Copyright (c) 2016-2017
* @copyright Apache License, Version 2.0
*/


/**IDEA
* Creo un socket UDP su una porta random, invio un messaggio in broadcast e attendo la risposta del serve
* contenente ip, porta e nome. Li elenco ed aspetto che l'utente scelga il master di destinazione.
* Una volta scelto il master apro un socket TCP con il master per inviare i dati.
*/



/* ----------- INCLUDE ----------- */
#include <stdio.h>


/* ----------- END INCLUDE ----------- */

int scan_for_master () {
	

}

int main (void) {

	scan_for_master();




	return 0;
}


