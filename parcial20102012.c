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

nodo *agrega_lifo(nodo *l, elemento nuev){ //añade al principio
	nodo *nuevo;
	nuevo= (nodo *) malloc(sizeof(nodo));
	strcpy(nuevo->r.simbolo , nuev.simbolo);
	nuevo->r.codigo = nuev.codigo;
	nuevo->r.densidad = nuev.densidad;
	nuevo->r.nroatom = nuev.nroatom;
	nuevo->sig = l;
	return nuevo;
	
}

nodo *lee_arch(nodo *l){ //Lee archivo y carga datos en lista
	int n;
	elemento nuevo;
	FILE *f;
	
	f= fopen("elementos.dat", "r");
	if (f == NULL){
	printf("Error al abrir");
	return l;
	}
	else{
	n = fread( &nuevo, sizeof(elemento), 1, f);
	while (n == 1){
		l = agrega_lifo(l, nuevo);
		n = fread(&nuevo, sizeof(elemento), 1, f);
	}
	printf("Datos cargados correctamente en lista");
	if(fclose(f) != 0){
	printf("Error al cerrar archivo");
	}
	mostrar(l);
	return l;
	}
}

int menu(void){//imprime menu
	int op;
	printf("==============MENU==========");
	printf("\n1. Nuevo Elemento\n2.Elimina");
	printf("\n3. Densidad promedio\n4.Busca elemento\n5.Salir\n");
	scanf("%d", &op);
	return op;	
}

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

void elimina_codint(nodo *l){ //Elimino buscando por codigo (op. 2)
	int d;
	nodo *ant;
	nodo *aux =l;
	printf("Ingrese codigo interno para eliminar:");
	scanf("%d", &d);
	if(l->r.codigo == d){ //si es el primero de la lista
		l = l->sig;
		free(aux);
		}
		else{
			while(aux->sig != NULL && aux->sig->r.codigo != d){
				aux = aux->sig;
			}
			if(aux->sig != NULL){ //verifico que haya salido del while porque encontro el dato
			 ant = aux->sig;
			 aux->sig = ant->sig;
			 free(ant);
		}else{ //Osea que aux->sig ES IGUAL A NULL
			printf("Codigo no encontrado");
		}	
		
}
}

void promedio(nodo *l){ //Calcula promedio e informa los mayores al promedio (op. 3)
	int i=0;
	float suma =0, prom= 0;
	nodo *aux=l;
	nodo *p= l;
	while(aux != NULL){
		suma+= aux->r.densidad;
		i++;
		aux= aux->sig;
	}
	prom = suma / i;
	aux = l;
	while(aux != NULL){
		if(aux->r.densidad >= prom){
			printf("El elemento %s con codigo %d posee densidad mayor igual al promedio", aux->r.simbolo, aux->r.codigo);
		}
		aux = aux->sig;
	}
	
	}

void listar(nodo *l){ //Busca por simbolo y lista los que encuentra (op.4)
	char nuevo[3];
	nodo *aux= l;
	int flag = 0;
	printf("Ingrese el simbolo a buscar:");
	scanf("%s", nuevo);
	while(aux != NULL){
		if(strcmp(nuevo, aux->r.simbolo) == 0){
			printf("Elemento encontrado:");
			imprime_elemento(aux->r);
			flag++;
		}
		
		aux = aux->sig;
	}
	if(flag == 0)
	printf("Elemento no encontrado");
}

int graba_arch(nodo *l){ //graba los datos de la lista a un archivo elementos2.dat 
	nodo *aux = l;
	int n;
	FILE *f;
	f= fopen("elementos2.dat", "w");
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

int main(){
	nodo *lista = NULL;
	elemento elem;
	int op,ref;
	lista = lee_arch(lista);
	if(lista == NULL)
	printf("lista esta en NULL");
	do{
		op = menu();
		switch(op){
			case 1:
				printf("\nIngrese un Simbolo (i.e Fe,Ti,Na)");
				scanf("%s", elem.simbolo);
				printf("\nIngrese Nro. Atomico:");
				scanf("%d", &elem.nroatom);
				printf("\nIngrese una densidad [g/ml]:");
				scanf("%f", &elem.densidad);
				printf("\nIngrese un codigo interno:");
				scanf("%d", &elem.codigo);
				lista = agrega_fifo(lista, elem);
				break;
			case 2:
				elimina_codint(lista);
				break;
			case 3:
				promedio(lista);
				break;
			case 4:
				listar(lista);
				break;	
			case 5:
				break;
			default:
				printf("\n Opcion Invalida");				
		} //termina el switch
	}while(op!= 5); //termino el do-while
	
	//guardo el archivo
	ref = graba_arch(lista);
	if(ref == 0)
	printf("Datos grabados en elementos2.dat");
	lista = destruir(lista);
	return 0;
	
	
}

/*Parcial 20/10/2012 - En un laboratorio de nanotecnología se tiene el archivo elementos.dat conformado por una cantidad indeterminada de registros cuya estructura es la siguiente:
*Símbolo (ejemplos: "Si" para el silicio, "Fe" para hierro)
*Nro. Atómico: (ejemplos: 8 para el oxígeno, 14 para el silicio)
*Densidad en g/ml (ejemplos: 0,071 para el hidrógeno, 16,6 para el mercurio)
*Código de identificación interno (ejemplo 123 para algún elemento)
Puesto que periódicamente se descubren o sintetizan nuevos elementos o isotopos su cantidad no puede estar limitada.
 
Se pide hacer un programa que realice las siguientes tareas:
Leer el archivo conformando una lista simple. Los nodos de la lista deben quedar en el orden inverso del que fueron leídos.
Presentar un menú con las siguientes opciones:
1-Agregar un nuevo elemento al final de la lista.
2-Eliminar un elemento conociendo su código interno.
3- Calcular la densidad promedio e informar código y símbolo de los elementos con densidad igual o superior al promedio.
4-Listar todos los elementos cuyo símbolo coincida con uno ingresado por teclado.
5- Fin del menú.
Grabar los nuevos datos en el archivo elementos2.dat y finalizar.
 
Nota: Para cada opción del menú (excepto 5) se deberá usar al menos una función.*/

