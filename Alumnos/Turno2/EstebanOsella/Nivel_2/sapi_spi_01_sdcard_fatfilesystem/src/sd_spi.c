/* Copyright 2014, ChaN
 * Copyright 2016, Matias Marando
 * Copyright 2016, Eric Pernia
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
/**
 * @file sd_spi.c
 * @brief Programa de ejemplo para escritura de archivos en sistemas FAT
 * @mainpage Descripcion del programa
 *
 * Este programa hace uso de la biblioteca fatfs, dedicada a manejo de sistema
 * de archivos tipo FAT para sistemas embebidos.
 *
 * @page estructura Estructura del programa
 *
 * La interface con el hardware se provee
 * por intermedio de sapi, la cual a su vez realiza la interface con el hardware.
 *
 * En este sentido, el acceso seria:
 *
 * Programa <-> FF <-> sapi <->board <-> hardware
 *
 * @todo Hacerlo funcionar por que no tuve tiempo de probar si finalmente escribia el modulo lector o no :P
 *
 */


/*==================[inclusions]=============================================*/

#include "board.h"
#include "sapi.h"
#include "ff.h"

/*==================[macros and definitions]=================================*/

#define FILENAME "hola.txt"

/*==================[internal data declaration]==============================*/

static FATFS fs;           /**< FatFs work area needed for each volume */
static FIL fp;             /**< File object needed for each open file */

/*==================[internal functions declaration]=========================*/

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/


/** FUNCION que se ejecuta cada vezque ocurre un Tick.
 * @param puntero nulo
 * @returns true allways
 *
 * */
bool_t diskTickHook( void *ptr ){
   disk_timerproc();   /* Disk timer process */
   return 1;
}


/** FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE RESET.
 * El programa configura la placa de modo de poder realizar la escritura del archivo
 * en la tarjeta y crea un archivo con un texto.
 * En caso exitoso, enciende el LED verde, y caso contrario, LED rojo.
 * @param sin parametros
 * @returns int
 * */
int main(void){

   /* ------------- INICIALIZACIONES ------------- */

   UINT nbytes;

   /* Inicializar la placa */
   boardConfig();

   /* Inicializar GPIOs */
   gpioConfig( 0, GPIO_ENABLE );

   /* Configuración de pines de entrada para Teclas de la CIAA-NXP */
   gpioConfig( TEC1, GPIO_INPUT );
   gpioConfig( TEC2, GPIO_INPUT );
   gpioConfig( TEC3, GPIO_INPUT );
   gpioConfig( TEC4, GPIO_INPUT );

   /* Configuración de pines de salida para Leds de la CIAA-NXP */
   gpioConfig( LEDR, GPIO_OUTPUT );
   gpioConfig( LEDG, GPIO_OUTPUT );
   gpioConfig( LEDB, GPIO_OUTPUT );
   gpioConfig( LED1, GPIO_OUTPUT );
   gpioConfig( LED2, GPIO_OUTPUT );
   gpioConfig( LED3, GPIO_OUTPUT );

   /* Inicializar el conteo de Ticks con resolución de 10ms, 
    * con tickHook diskTickHook */
   tickConfig( 10, diskTickHook );

   /* SPI configuration */
   Board_SSP_Init(LPC_SSP1);
   Chip_SSP_Init(LPC_SSP1);
   Chip_SSP_Enable(LPC_SSP1);

             
   /* ------ PROGRAMA QUE ESCRIBE EN LA SD ------- */

   /* Give a work area to the default drive */
   if( f_mount( &fs, "", 0 ) != FR_OK ){
      /* If this fails, it means that the function could
       * not register a file system object.
       * Check whether the SD card is correctly connected */
   }

   /* Create/open a file, then write a string and close it */
   if( f_open( &fp, FILENAME, FA_WRITE | FA_CREATE_ALWAYS ) == FR_OK ){
      f_write( &fp, "Hola mundo\r\n", 12, &nbytes );

      f_close(&fp);

      if( nbytes == 12 ){
         /* Turn ON LEDG if the write operation was successful */
         gpioWrite( LEDG, ON );
      }
   } else{
      /* Turn ON LEDR if the write operation was fail */
      gpioWrite( LEDR, ON );
   }

   /* ------------- REPETIR POR SIEMPRE ------------- */
   while(1) {
      sleepUntilNextInterrupt();
   }

   /* NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa no es llamado
      por ningun S.O. */
   return 0 ;
}

/*==================[end of file]============================================*/
