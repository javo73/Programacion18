#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct elemento{
	char simbolo[3];
	int nroatom;
	float densidad;
	int codigo;
}elemento;

typedef struct nodo{
	elemento r;
	struct nodo *sig;
}nodo;

nodo *agrega_fifo(nodo *l, elemento nuev){ // Agrego al final de la lista (op. 1)
	nodo *nuevo;
	nodo *aux = l;
	
	nuevo= (nodo *) malloc(sizeof(nodo));
	strcpy(nuevo->r.simbolo , nuev.simbolo);
	nuevo->r.codigo = nuev.codigo;
	nuevo->r.densidad = nuev.densidad;
	nuevo->r.nroatom = nuev.nroatom;
	nuevo->sig = NULL; //porque va a ser el ultimo elemento
	if(l == NULL){//si la lista esta vacia retorno nuevo directamente
		return nuevo;
	}
	else{
		while(aux->sig != NULL)
		aux = aux->sig;
		//Ahora aux es el ultimo elemento de la lista
		aux->sig = nuevo;
		return l;
	}
}

nodo *agrega_e(nodo *l){
	nodo *aux;
	elemento elem;
	int ref;
	
	do{	
	printf("\nIngrese un Simbolo (i.e Fe,Ti,Na)");
	scanf("%s", elem.simbolo);
	printf("\nIngrese Nro. Atomico:");
	scanf("%d", &elem.nroatom);
	printf("\nIngrese una densidad [g/ml]:");
	scanf("%f", &elem.densidad);
	printf("\nIngrese un codigo interno:");
	scanf("%d", &elem.codigo);
	printf("Si desea añadir otro elemento ingrese 1, cualquier otro ingreso saldra del programa");
	scanf("%d", &ref);
	l = agrega_fifo(l, elem);
	}while(ref == 1);
	return l;
}

int graba_arch(nodo *l){ //graba los datos de la lista a un archivo elementos.dat 
	nodo *aux = l;
	int n;
	FILE *f;
	f= fopen("elementos.dat", "w");
	if (f == NULL){
		printf("Error al crear archivo");
		return -1;
	}
	n= fwrite(&aux->r, sizeof(elemento), 1, f);
	while(n == 1){
		aux = aux->sig;
		n = fwrite(&aux->r, sizeof(elemento), 1, f);
	}
	if(fclose(f) != 0){
		printf("Error al cerrar archivo");
		return -2;
	}
	return 0;
}	

nodo *destruir(nodo *lista){
	nodo *aux= lista;
	while(lista != NULL){
		aux = lista;
		lista = lista->sig;
		free(aux);
	}
	printf("Lista destruida");
	return NULL;
}

void imprime_elemento(elemento e){ // Imprime un elemento en pantalla
	printf("Simbolo: %s", e.simbolo);
	printf("Nro Atomico: %d", e.nroatom);
	printf("Densidad: %.2f", e.densidad);
	printf("Codigo Interno: %d", e.codigo);
}
void mostrar(nodo *lista){
	nodo *aux= lista;
	while(aux != NULL){
		imprime_elemento(aux->r);
		aux = aux->sig;
	}
}
int main(){
	nodo *lista = NULL;
	int ref;
	lista = agrega_e(lista);
	//grabo los datos que haya en la lista y verifico que se hayan guardado correctamente
	ref = graba_arch(lista);
	if(ref == 0)
	printf("Datos guardados");
	mostrar(lista);
	lista = destruir(lista);
	return 0;
	
}
