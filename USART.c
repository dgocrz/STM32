
#include <stdint.h>


//Vamos a definir las direcciones base de los registros 

#define RCC_BASE 0x40021000
#define GPIOA_BASE 0x40010800
#define GPIOB_BASE 0x40010C00
#define GPIOC_BASE 0x40011000
#define USART1_BASE 0x40013800

//Definimos la base del registro del reloj de los pines (ya que esta en bajo consumo para los arm)
#define RCC_APB2ENR (*(volatile uint32_t*)(RCC_BASE+0x18))


#define GPIOA_CRL (*(volatile uint32_t*)(GPIOA_BASE+0x00))
#define GPIOA_CRH (*(volatile uint32_t*)(RCC_BASE+0x04))
#define GPIOA_ODR (*(volatile uint32_t*)(RCC_BASE+0x0C))


#define GPIOB_CRL (*(volatile uint32_t*)(GPIOB_BASE+0x00))
#define GPIOB_ODR (*(volatile uint32_t*)(GPIOB_BASE + 0x0C))
	
#define GPIOC_CRH (*(volatile uint32_t*)(GPIOC_BASE +0x04))
#define GPIOC_IDR (*(volatile uint32_t*)(GPIOC_BASE + 0x08))

#define GPIOC 
#define USART1_SR (*(volatile uint32_t*)(USART1_BASE +0x00))
#define USART1_DR (*(volatile uint32_t*)(USART1_BASE + 0x04))
#define USART1_BRR (*(volatile uint32_t*)(USART1_BASE +0x08))
#define USART1_CR1 (*(volatile uint32_t*)(USART1_BASE +0x0C))
	
void transmitir(char c);
char leer (void);
int main (void){
	//Se enciende el reloj de los puertos 
	RCC_APB2ENR |= 0x4005;
	//Vamos a configurar los pines A9 como salida y A10 como entrada (para empezar la comunicación TX y RX)
	//PA9 como Salida push pull
	GPIOA_CRH = GPIOA_CRH&(~0x000000FF0u);
	//PA10 como entrada pull up/down
	GPIOA_CRH |= 0x08B;
	
	
	GPIOA_CRH &= ~(0xF << ((8 - 8) * 4));     // limpiar bits
    GPIOA_CRH |=  (0x2 << ((8 - 8) * 4));     // salida 2 MHz push-pull

    // --- Configurar PB0 como salida push-pull ---
    GPIOB_CRL &= ~(0xF << (0 * 4));
    GPIOB_CRL |=  (0x2 << (0 * 4));

	//PC13 para conectar el boton como entrada flotante
	
	 GPIOC_CRH &= ~(0xF << ((13-8)*4));
   GPIOC_CRH |=  (0x4 << ((13-8)*4));
	
	//Vamos a configurar el USART1
	//
	USART1_CR1 = 0x0000; //Se apaga el USART1 status con un bit de inicio
	USART1_BRR |= 0x45; //se calcula los baudios para la comunicación 
	
	USART1_CR1 |= 0x2000;
	
	
	//Dentro del while detectaremos si nos llega una señal o nosotros enviamos una 

	while(1){
	//Al presionar un boton queremos encender el LED
		//si el bit 13 de GPIOC_IDR es igual a 0 quiere decir que el boton se apreto
		if (!(GPIOC_IDR & (1 << 13))) 
			{ 
        GPIOA_ODR |= (1 << 8);  // Enciende LED local
        transmitir('1');  // Envía señal al otro micro
    }

	//Al recibir un un dato se enciende un led 
		
	if(USART1_SR &(1 << 5) ){
		
		
		char dato = USART1_DR;
		if(dato=='1'){
			GPIOB_ODR ^= (1<<0);
		
		}
	
	}
	
}

}
	//Vamos a definir las funcionas para enviar y leer datos con USART
void transmitir(char c)
		{
		while (!(USART1_SR & (1 << 7)));
		USART1_DR = c;
	}

char leer(void){
		while (!(USART1_SR & (1 << 5)));
  return USART1_DR;
}

