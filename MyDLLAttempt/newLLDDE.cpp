//#include "chico.h"
#include "lldde.h"

// Atualizado em 16-08-2013
// para uso de C++ e classes
// Uso de NEW e DELETE

// Lista linear duplamente encadeada
// 16-07-2012
//	Correção da rotina de exclusao de nodo,
//	corrigido exclusão quando o nodo a ser excluido for unico, estava com problemas

// 05 maio 2012
// Suporte a exclusão por ponteiro

newLLDDE::newLLDDE()
{
	nItens=0;
	lastIndice=0;
	primeiro=NULL;
	ultimo=NULL;
	atual=NULL;
	return;
}

int newLLDDE::delNodo(NODO *pnodo)
{
	if(!pnodo) return 1;

	if(nItens==0) return 1;

	// reduz contador
	nItens--;
	// se for o unico
	if((primeiro==pnodo) && (ultimo==pnodo))
	{
		primeiro=NULL;
		ultimo=NULL;
		//free(pnodo);
		delete[] pnodo;
		return 0;
	}
	// se for o primeiro da lista sendo deletado
	if(pnodo == primeiro){
		pnodo->pro->ant=pnodo->ant;
		primeiro=pnodo->pro;
		//free(pnodo);
		delete[] pnodo;
		return 0;
	}
	// se for o ultimo da lista sendo deletado
	if(pnodo == ultimo){
		pnodo->ant->pro = pnodo->pro;
		ultimo = pnodo->ant;
		//free(pnodo);
		delete[] pnodo;
		return 0;
	}
	// caso padrao, esta no meio da lista
	pnodo->ant->pro = pnodo->pro;
	pnodo->pro->ant = pnodo->ant;
	//free(pnodo);
	delete[] pnodo;
	return 0;
}

int newLLDDE::delNodoById(int b)
{
	NODO *pnodo;

	if(nItens==0) return 1;

	pnodo=primeiro;
	while(pnodo)
	{
		if(pnodo->id == b)
		{
			// nodo a deletar
			if(delNodo(pnodo))
			{
				printf("EXCLUSAO: um erro ocorreu!\n");
				return 1;
			}
			return 0;
		}
		pnodo=pnodo->pro;
	}
	return 1;
}

int newLLDDE::delNodoByPtr(void *b)
{
	NODO *pnodo;
	//if(!b) return 1;

	if(nItens==0) return 1;

	//printf("DELETANDO NODO %d\n", b);

	pnodo=primeiro;
	while(pnodo){
		if(pnodo->dado == b){
			// nodo a deletar
			if(delNodo(pnodo)){
				printf("EXCLUSAO: um erro ocorreu!\n");
				return 1;
			}
			return 0;
		}
		pnodo=pnodo->pro;
	}
	return 1;
}

int newLLDDE::addNodo(void *b)
{
	
	NODO *pnodo;
	
	// "a" e "b" devem chegar como um ponteiro valido
	if(!b) return 1;
	
	pnodo = new NODO;	//(NODO *)malloc(sizeof(NODO));
	
	pnodo->dado = b;
	pnodo->ant = ultimo;
	pnodo->pro = NULL;
	pnodo->id = lastIndice;
	
	nItens++;
	lastIndice++;

	if((primeiro == NULL) && (ultimo == NULL)){
		primeiro=pnodo;
		ultimo=pnodo;
	}else{
		ultimo->pro=pnodo;
		ultimo=pnodo;
	}
	
	return 0;
}

void newLLDDE::resetLLDDE()
{
	atual = primeiro;
	return;
}

void *newLLDDE::getNext()
{
	NODO *pnodo;

	pnodo = atual;

	// avança pro proximo nodo
	if(pnodo == NULL)
	{
		//pnodo=primeiro;
		atual = primeiro;
		return NULL;
	}else{
		atual = pnodo->pro;
	}
	return pnodo->dado;
}

NODO *newLLDDE::getNextNodo()
{
	NODO *pnodo;
	
	pnodo=atual;
	// avança pro proximo nodo
	if(pnodo==NULL){
		atual = primeiro;
		return NULL;
	}else{
		atual=pnodo->pro;
	}
	return pnodo;
}

// Apaga todos os nodos da LLDDE
// através de uma rotina q captura sempre o primeiro
void newLLDDE::apagaNodosLLDDE()
{
	NODO *pnodo;
	int i=0;
	resetLLDDE();

	while(pnodo = primeiro)
	{
		//apaga(pnodo->dado);
		delNodo(pnodo);
		resetLLDDE();
		i++;
	}
}

newLLDDE *newLLDDE::NewHandleLLDDE(newLLDDE *l)
{
	if(l==NULL)
		l = new newLLDDE; //(LLDDE *) malloc(sizeof(LLDDE));
	memcpy(l, this, sizeof(newLLDDE));
	return l;
}

//void newLLDDE::NewHandleLLDDE(newLLDDE *l)
//{
//	//newLLDDE *b;
//	// b = new newLLDDE; //(LLDDE *) malloc(sizeof(LLDDE));
//	memcpy(b, this, sizeof(newLLDDE));
//	return b;
//}
