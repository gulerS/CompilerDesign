#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "common.h"
#include "error.h"
//#include "scanner.h"
#include "symtab.h"
//#include "vglobales.h"
//#include "prototipos.h"
#include <string.h>



/*------------------------------------------------------*/
/*   Globals											*/
/*------------------------------------------------------*/

SYMTAB_NODE_PTR symtab_root = NULL;




/*------------------------------------------------------*/
/*   search_symtab										*/
/*------------------------------------------------------*/
SYMTAB_NODE_PTR search_symtab(char *name, SYMTAB_NODE_PTR np)
{
	//char *name;
	//SYMTAB_NODE_PTR np;

	int cmp;

	while (np != (SYMTAB_NODE_PTR)NULL) {
		cmp = strcmp(name, np->name);
		if (cmp == 0) return(np);
		np = cmp < 0 ? np->left : np->right;
	}

	return (NULL);
}

/*------------------------------------------------------*/
/*   enter_symtab										*/
/*------------------------------------------------------*/
SYMTAB_NODE_PTR enter_symtab(char *name, SYMTAB_NODE_PTR *npp)
{
	//char *name;
	//SYMTAB_NODE_PTR *npp;

	int cmp;
	SYMTAB_NODE_PTR new_nodep;
	SYMTAB_NODE_PTR np;


	new_nodep = (SYMTAB_NODE_PTR)alloc_struct(SYMTAB_NODE);
	new_nodep->name = alloc_bytes(strlen(name) + 1);
	strcpy(new_nodep->name, name);
	new_nodep->left = new_nodep->right = new_nodep->next = (SYMTAB_NODE_PTR)NULL;
	new_nodep->info = NULL;
	new_nodep->defn.key = UNDEFINED;
	new_nodep->level = new_nodep->label_index = 0;
	while ((np = *npp) != NULL)
	{
		cmp = strcmp(name, np->name);
		npp = cmp < 0 ? &(np->left) : &(np->right);
	}
	*npp = new_nodep;
	return(new_nodep);
}


//void enter_data(char *data, DATA_NODE_PTR *ndp, LITERAL_TYPE type)
//{
//	DATA_NODE_PTR new_ndp, pnodo_ant;
//
//	pnodo_ant = *ndp;
//	if (pnodo_ant == (DATA_NODE_PTR)NULL) {
//		new_ndp = alloc_struct(DATA_NODE);
//		new_ndp->elemento = alloc_bytes(strlen(data) + 1);
//		strcpy(new_ndp->elemento, data);
//		new_ndp->tipo = type;
//		new_ndp->siguiente = (DATA_NODE_PTR)NULL;
//		*ndp = new_ndp;
//	}
//	else {
//		enter_data(data, &(pnodo_ant->siguiente), type);
//	}
//}
