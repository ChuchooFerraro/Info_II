#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>

#define COM 0x3f8
#define TARJETA_ADMIN // PONER EL NUMERO PONER EL NUMERO PONER EL NUMERO PONER EL NUMERO PONER EL NUMERO PONER EL NUMERO PONER EL NUMERO PONER EL NUMERO
#define CLAVE_ADMIN 291												//Es el "0123" en el short int
#define Senal_LED_Buzzer_L 0x00										//Las señales son un 0
#define Senal_LED_Buzzer_H 0x01										//Las señales son un 1
#define loop 1

/* LAS PARTES DONDE ESTAN LOS * HAY QUE VER QUE LE MANDO AL ARDUINO PARA QUE HAGA DISTINTAS SECUENCIAS CON LOS LEDS */

void INICIAR_UART();

struct Socio
{
	char nombre[20];
	char apellido[20];
	int legajo;
	int tarjeta;
	unsigned short int password;
};

union Codigo
{
  unsigned int rfid;
  unsigned char RFID[4];
};

union Pass
{
	unsigned int contrasena;
	unsigned char con_parte[4];
};

#include "ncurses.h"

int main()
{
	
	FILE * BASE;
	struct Socio Datos;

	union Codigo C;
	
	union Pass P;
	
	int nueva_tarjeta;
	int SELECT;
	int i;
	
	/************************
	 APERTURA DE ARCHIVO Y
	 PERMISO DE PUERTO SERIE
	************************/
	
	if( (BASE = fopen("Base de Datos", "rb")) == NULL ){
		printf( "ERROR EN LA APERTURA DEL ARCHIVO" );
		return 1;
	}
	
	if( (ioperm (COM,8,1) ) == 0 ){
		printf( "ERROR DE PERMISOS" );
		return 1;
	}
	
	INICIAR_UART();
	
	/****************************************************************
	 LOOP DEL PROGRAMA.
	 RECEPCION DEL CODIGO DE LA TARJETA.
	 RECONOCIMIENTO DE LA TARJETA.
	 SI LA TARJETA NO PERTENECE A LA BASE DE DATOS, VUELVE A PEDIR
	 EL INGRESO DE UNA TARJETA.
	 SE PIDE LA CONTRASEÑA Y SI ESTA ES INCORRECTA SE VUELVE A PEDIR
	 UNA TARJETA.
	****************************************************************/
	
	while( loop ){
		
		while( (inb(COM) & 0x01) == 0 )                				//Espero el dato	
		{
			printf("APOYE LA TARJETA\n");
		}
		
		while( (inb(COM+5) & 0x01 ) == 0 || i == 4)
    	{
			i++;
    		switch (i)
    		{
    			case 1: C.RFID[3] = inb( COM ); break;
    			case 2: C.RFID[2] = inb( COM ); break;
    			case 3: C.RFID[1] = inb( COM ); break;
    			case 4: C.RFID[0] = inb( COM ); break;
    		}
    	}
    	
		printf("%x\n", c.rfid);
    	i=0;
  	
		/***************************************
		 CASO EN DONDE INGRESA EL ADMINISTRADOR
		***************************************/
		
		if( C.rfid == TARJETA_ADMIN )
		{
			outb( 0x00, COM );																				/*************************/
			while( (inb(COM+5) & 0x01) == 0 )
			{
				printf("\nINGRESE LA CLAVE DE ADMIN\n");
			}
			while( (inb(COM+5) & 0x01 ) == 0 || i == 4)
    		{
				i++;
    			switch (i)
    			{
    				case 1: P.con_parte[3] = inb( COM ); break;
    				case 2: P.con_parte[2] = inb( COM ); break;
    				case 3: P.con_parte[1] = inb( COM ); break;
    				case 4: P.con_parte[0] = inb( COM ); break;
    			}
    		}
		
			if( P.contrasena != CLAVE_ADMIN )
			{	
				outb( 0xff, COM );																			/*************************/
				printf("\nCLAVE INCORRECTA\n");
				continue;
			}
			
			MENU();
		}
		
		/***********************************
		 CASO EN DONDE INGRESA OTRA PERSONA
		***********************************/
		
		fread( &Datos , sizeof(Datos) , 1 , BASE );
		
		while( (Datos.tarjeta != C.rfid) && !feof(BASE) ){
			fread( &Datos , sizeof(Datos) , 1 , BASE );
		}
		
		if( !feof(BASE) )
		{
			outb( 0xff, COM );																				/*************************/
			printf( "NO ESTÁ INGRESADO" );							
			usleep(1500);
			outb( Senal_LED_Buzzer_L, COM );
			
			continue;
		}
		
		outb( 0x00, COM );																					/*************************/
		while( (inb(COM+5) & 0x01) == 0 )
		{
			printf("\nINGRESE LA CLAVE DE ADMIN\n");
		}
		while( (inb(COM+5) & 0x01 ) == 0 || i == 4)
    	{
			i++;
    		switch (i)
    		{
    			case 1: P.con_parte[3] = inb( COM ); break;
    			case 2: P.con_parte[2] = inb( COM ); break;
    			case 3: P.con_parte[1] = inb( COM ); break;
    			case 4: P.con_parte[0] = inb( COM ); break;
    		}
    	}
		
		if( P.contrasena != Datos.password )			//Si contraseña es incorrecta
		{					
			outb( 0xff, COM );																				/*************************/
			printf( "\nCONTRASEÑA INCORRECTA\n" );
			usleep(1500);
			outb( Senal_LED_Buzzer_L, COM );
		
			continue;
		}
		
		printf("HOLA %s" , Datos.nombre);
		outb( 0x00, COM );																					/*************************/
	}
	
	return 0;
}

void INICIAR_UART(){
	/**************************************************
	 SE CONFIGURA EL DIVISOR PARA OBTENER 9600 BAUDIOS.
	 SE CONFIGURA LA COMUNICACIÓN, Y SE HACEN LECTURAS
	 FALSAS PARA COMENZAR EL PROGRAMA.
	***************************************************/
	outb( 0x80, COM+3 );

	outb( 0x0c, COM );
	outb( 0x00, COM+1 );

	outb( 0x03, COM+3 );

	inb( COM );
	inb( COM +5 );
}
