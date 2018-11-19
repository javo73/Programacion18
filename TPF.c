#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/io.h>

#define P 0x70
# define PRL_DATA 0x378
# define PRL_CTL 0x37A

unsigned char in (unsigned char reg) { //Funcion para Leer Registros
        outb(reg, P); 
        return inb(P + 1);
}

void out(unsigned char valor, unsigned char reg) { //Funcion para Escribir Registros
        outb(reg, P);
        outb(valor, P + 1);
}

void perm(unsigned long puerto, unsigned long cantidad, int permiso) { //Funcion para otorgar/quitar permisos
        if (ioperm(puerto, cantidad, permiso) != 0) {
                perror("ioperm");
                exit(1);
        }
}

void check_uip(void) { //Funcion que checkea el bit UIP
        in (0x0A); // Accede y lee el Registro A
        if (( in (0x0A) & 0x80) != 0) // Verifica si el bit UIP esta arriba
                usleep(1984);
}

void setrtc_clk(void) { //Funcion para setear el clock del RTC en default y SQW en 2Hz
        unsigned char reg_a;

        perm(P, 2, 1); //Doy Permisos puertos 70h y 71h
        check_uip();
        reg_a = in(0x0A); //Leo Registro A
        reg_a = reg_a & 0xAF; // xxxx.xxx & 1010.1111 => x0x0.xxxx
        reg_a = reg_a | 0x20; //xxxx.xxx | 0010.0000 => xx1x.xxxx 
        reg_a = reg_a | 0x0F; // xx1x.xxxx | 0000.1111 => reg_a: x010.1111
        out(reg_a,0x0A); 
}

int bcd2dec(unsigned char num){
	int unidad,decena,decimal;
	unidad = num & 0x0F; // xxxx.xxxx & 0000.1111 => 0000.xxxx
	decena = num & 0xF0; // xxxx.xxxx & 1111.0000 => xxxx.0000
	decena = decena >> 4; //Desplazo los bits 4 lugares a la izq
	decimal = (decena*10 + unidad); //Armo el numero
	return decimal;
}




void rtc_sqw() {

        unsigned char reg_c;
        int i;

        perm(P, 2, 1); //Otorgo permisos port 70h y 71h
        printf("\n\nTren de Pulsos con f= 2Hz:\n\n");
        printf("0---+---1---+---2---+---3---+---4---+---5---+---6---+---7---+---8---+---9---+---10");
        printf("\n");
		setrtc_clk();
		reg_c = in(0x0C);
        for (i = 0; i < 81; i++) { //Imprimo cantidad de simbolos en la escala   
                reg_c = in(0x0C); // Limpio flags
                if (reg_c & 0x40) { // Chequeo si el bit PF esta arriba     
                        printf("|");
                } else { // si PF no esta arriba  
                        printf("_");
                }
				fflush(stdout);
                usleep(125000); // Espero 1/8 de segundo
        }
        printf("\n\n\n\n");
        perm(P, 2, 0); //Quito permisos port 70h y 71h
}

int char2bin(unsigned char d) { //funcion para convertir a binario                 
        int r = 0, b = 0, i = 1;

        while (d > 0) {
                r = d % 2;
                d = d / 2;
                b = b + (r * i);
                i = i * 10;
        }
        return b;
}

int char2oct(int d) { //Funcion para convertir a octal
        int r = 0, o = 0, i = 1;

        while (d > 0) {
                r = d % 8;
                d = d / 8;
                o = o + (r * i);
                i = i * 10;
        }
        return o;
}

void nombre_reg(int i) { //Descripcion de registros
        switch (i) {
        case 0:
                printf(" Segundos        ");
                break;
        case 1:
                printf(" Segundos Alarma ");
                break;
        case 2:
                printf(" Minutos         ");
                break;
        case 3:
                printf(" Minutos Alarma  ");
                break;
        case 4:
                printf(" Horas           ");
                break;
        case 5:
                printf(" Horas Alarma    ");
                break;
        case 6:
                printf(" Dia de la Semana");
                break;
        case 7:
                printf(" Dia del Mes     ");
                break;
        case 8:
                printf(" Mes             ");
                break;
        case 9:
                printf(" Anio            ");
                break;
        case 10:
                printf(" Registro A      ");
                break;
        case 11:
                printf(" Registro B      ");
                break;
        case 12:
                printf(" Registro C      ");
                break;
        case 13:
                printf(" Registro D      ");
                break;
                other: break;
        }
}

void mostrar_RTC(void) { //Funcion que muestra los registros 0 a 14 del RTC
        int dato, datbin, datoct;
        int i;
        char nom;

        perm(0x70, 2, 1); // pido permisos para los puertos 70 y 71 con privilegios
        check_uip(); // verifico bit UIP del REGISTRO A

        printf("\n\n\t\t   REGISTROS DEL RTC\n");
        printf("   --------------------------------------------------");
        printf("\n    Binario  | Hexa |       Descripcion         |  Octal\n");
        printf("   --------------------------------------------------");

        for (i = 0; i < 14; i++) {
                outb(0x00 + i, 0x70); // Recorro cada registro con "i", accediendo por puerto 70
                dato = inb(0x71); // Cada dato del registro recorrido lo guardo en "dato"

                datbin = char2bin(bcd2dec(dato)); // Convierte el "dato" a n° binario
                datoct = char2oct(bcd2dec(dato)); // Convierte el "dato" a n° octal

                printf("\n    %08d |  %02x  | %2d  ", datbin, dato, i);
                nombre_reg(i); // Imprime el nombre de cada registro
                if (i < 10) {
                        printf(" |   %03d", datoct);
                } else {
                        printf(" |");
                }
        }

        printf("\n\n\n");
}

void set_alarm() { //Funcion que setea la alarma del RTC

        unsigned char h, m, s, reg_c, i;

        printf("Ingrese la hora de alarma (HH:MM:SS):");
        scanf("%hhx:%hhx:%hhx", & h, & m, & s);

        perm(P, 2, 1); //Doy Permisos Ports 70h y 71h

        out(s, 0x01); /*Setear segundos alarma*/
        out(m, 0x03); /*Setear minutos alarma*/
        out(h, 0x05); /*Setear hora alarma*/

        reg_c = in (0x0C); //Limpio flags anteriores
        printf("\nEsperando las alarmas...");
        fflush(stdout);
        for (i = 0; i < 100; i++) { //Polling del Registro C en busca del bit AF
                reg_c = in (0x0C); //Leo el Registro C y limpio flags
                printf(".");
                fflush(stdout);
                if (reg_c & 0x20) { //Si el bit AF esta arriba
                        printf("\nAlarma!!!!!!!!!\n");
                        i = 100;
                }
                usleep(600000); //Espera .6 seg
        }

        perm(P, 2, 0); //Quito Permisos ports 70h y 71h

}

void paralelo() { //Funcion Imprime Registro minutos del RTC en puerto paralelo
        unsigned char prl_control, rtc_min;
        int bin, nbin, i;

        perm(P, 2, 1); //Doy permisos ports 70h y 71h
        perm(PRL_DATA, 1, 1); //Doy permisos port 378h
        perm(PRL_CTL, 1, 1); //Doy permisos port 37Ah

        rtc_min = in (0x02); //Leo el registro minutos del RTC
        bin = char2bin(rtc_min); //Convierto el byte a binario	
        printf("\n\nDato a enviar: %08d\n", bin); //Imprimo lo que deseo enviar
        rtc_min = ~rtc_min; //Niego el byte minutos por propiedades del pto paralelo
        nbin = char2bin(rtc_min); //Convierto el byte a binario
        printf("Dato a recibir:%08d\n", nbin); //Imprimo lo que deseo recibir

        outb(rtc_min, PRL_DATA); // Envio datos a la impresora
        prl_control = inb(PRL_CTL); //Leo el registro que contiene el bit STROBE
        prl_control = prl_control & 0xFE; //xxxx xxxx & 1111 1110 => par_control es xxxx xxx0
        outb(prl_control, PRL_CTL);
        // el bit STROBE esta en 0, voy a esperar 1us
        usleep(1);
        prl_control = prl_control | 0x01; //xxxx xxx0 & 0000 0001 => xxxx xxx1
        outb(prl_control, PRL_CTL); //Pongo arriba el bit STROBE
	printf("\n\n\n");	

        perm(P, 2, 0); //Quito permisos ports 70h y 71h
        perm(PRL_DATA, 1, 0); //Quito permisos port 378h
        perm(PRL_CTL, 1, 0); //Quito permisos port 37Ah

}

int main() {
        int op;
		printf("--------------------------------------\n");
		printf("Trabajo Practico Programacion 2C 2018\n");
		printf("Alumno: Javier Luis Moro\n Legajo: 5924\n");
		printf("--------------------------------------\n");
        do{
		printf("Ingrese una opcion\n");
                printf("1.Muestra Registros RTC\n2.Tren de Pulsos de 2Hz\n3.Programa Alarma\n");
                printf("4.Envia Registro a Puerto Paralelo\n5.Salir\n");
                fflush(stdout);
                scanf("%d", & op);
                switch (op) {
                case 1:
                        mostrar_RTC();
                        break;
                case 2:
                        rtc_sqw();
                        break;
                case 3:
                        set_alarm();
                        break;
                case 4:
                        paralelo();
                        break;
                case 5:
                        break;
                default:
                        printf("Opcion Invalida");
                        break;
                }
        } while (op != 5);
        return 0;
}
