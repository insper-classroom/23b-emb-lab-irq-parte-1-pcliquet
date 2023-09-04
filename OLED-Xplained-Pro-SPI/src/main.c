

#include "asf.h"



#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"



//Declarar o LED_1
#define LED_PIO_1    PIOA
#define LED_PIO_ID_1        ID_PIOA
#define LED_PIO_IDX_1       0
#define LED_PIO_IDX_MASK_1  (1u<< LED_PIO_IDX_1)

#define LED_PIO_2    PIOC
#define LED_PIO_ID_2        ID_PIOC
#define LED_PIO_IDX_2       30
#define LED_PIO_IDX_MASK_2  (1u<< LED_PIO_IDX_2)

#define LED_PIO_3    PIOB
#define LED_PIO_ID_3        ID_PIOB
#define LED_PIO_IDX_3       2
#define LED_PIO_IDX_MASK_3  (1u<< LED_PIO_IDX_3)



#define BUT_PIO_1		PIOD
#define BUT_PIO_ID_1		ID_PIOD
#define BUT_PIO_IDX_1		28
#define BUT_PIO_IDX_MASK_1	(1u<< BUT_PIO_IDX_1)

#define BUT_PIO_2		PIOC
#define BUT_PIO_ID_2		ID_PIOC
#define BUT_PIO_IDX_2		31
#define BUT_PIO_IDX_MASK_2	(1u<< BUT_PIO_IDX_2)

#define BUT_PIO_3		PIOA
#define BUT_PIO_ID_3		ID_PIOA
#define BUT_PIO_IDX_3		19
#define BUT_PIO_IDX_MASK_3	(1u<< BUT_PIO_IDX_3)



/*  Default pin configuration (no attribute). */
#define _PIO_DEFAULT             (0u << 0)
/*  The internal pin pull-up is active. */
#define _PIO_PULLUP              (1u << 0)
/*  The internal glitch filter is active. */
#define _PIO_DEGLITCH            (1u << 1)
/*  The internal debouncing filter is active. */
#define _PIO_DEBOUNCE            (1u << 3)


volatile char but_flag;
volatile char but2_flag;
volatile char but3_flag;

char texto[20];

void io_init(void);
void pisca_led(int t);


void but_callback(void)
{
	but_flag =1;
	
}

void but2_callback(void)
{
	but2_flag =1;
	
}

void but3_callback(void)
{
	but3_flag =1;
	
}

void io_init(void)
{

	// Configura led
	pmc_enable_periph_clk(LED_PIO_ID_1);
	pio_configure(LED_PIO_1, PIO_OUTPUT_0, LED_PIO_IDX_MASK_1, PIO_DEFAULT);

	// Inicializa clock do perif?rico PIO responsavel pelo botao
	pmc_enable_periph_clk(BUT_PIO_ID_1);
	pmc_enable_periph_clk(BUT_PIO_ID_2);
	pmc_enable_periph_clk(BUT_PIO_ID_3);
	
	pio_set(LED_PIO_1, LED_PIO_IDX_MASK_1	);

	// Configura PIO para lidar com o pino do bot?o como entrada
	// com pull-up
	pio_configure(BUT_PIO_ID_1, PIO_INPUT, BUT_PIO_IDX_MASK_1, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(BUT_PIO_ID_2, PIO_INPUT, BUT_PIO_IDX_MASK_2, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(BUT_PIO_ID_3, PIO_INPUT, BUT_PIO_IDX_MASK_3, PIO_PULLUP | PIO_DEBOUNCE);

	// Configura interrup??o no pino referente ao botao e associa
	// fun??o de callback caso uma interrup??o for gerada
	// a fun??o de callback ? a: but_callback()
	pio_handler_set(BUT_PIO_1,
	BUT_PIO_ID_1,
	BUT_PIO_IDX_MASK_1,
	PIO_IT_FALL_EDGE,
	but_callback);
	
	pio_handler_set(BUT_PIO_2,
	BUT_PIO_ID_2,
	BUT_PIO_IDX_MASK_2,
	PIO_IT_FALL_EDGE,
	but2_callback);
	
	pio_handler_set(BUT_PIO_3,
	BUT_PIO_ID_3,
	BUT_PIO_IDX_MASK_3,
	PIO_IT_FALL_EDGE,
	but3_callback);

	// Ativa interrup??o e limpa primeira IRQ gerada na ativacao
	pio_enable_interrupt(BUT_PIO_1, BUT_PIO_IDX_MASK_1);
	pio_get_interrupt_status(BUT_PIO_1);
	
	pio_enable_interrupt(BUT_PIO_2, BUT_PIO_IDX_MASK_2);
	pio_get_interrupt_status(BUT_PIO_2);
	
	pio_enable_interrupt(BUT_PIO_3, BUT_PIO_IDX_MASK_3);
	pio_get_interrupt_status(BUT_PIO_3);
	
	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais pr?ximo de 0 maior)

	NVIC_EnableIRQ(BUT_PIO_ID_1);
	NVIC_SetPriority(BUT_PIO_ID_1, 4); // Prioridade 4
	
	NVIC_EnableIRQ(BUT_PIO_ID_2);
	NVIC_SetPriority(BUT_PIO_ID_2, 4); // Prioridade 4
	
	NVIC_EnableIRQ(BUT_PIO_ID_3);
	NVIC_SetPriority(BUT_PIO_ID_3, 4); // Prioridade 4
	
	
	
}


void pisca_led(int t){
	for (int i=0;i<30;i++){
		pio_clear(LED_PIO_1, LED_PIO_IDX_MASK_1);
		delay_ms(t);
		pio_set(LED_PIO_1, LED_PIO_IDX_MASK_1);
		delay_ms(t);
		if (but2_flag==1){
			but2_flag =0;
			break;
		}
	}
}


int main (void){
	io_init();
	board_init();
	sysclk_init();
	delay_init();
	

    int tempo =200;

	// Init OLED
	gfx_mono_ssd1306_init();
	
	sprintf(texto,"%.2f hertz",(1000.0/(tempo+tempo)));
	gfx_mono_draw_string(texto, 20,16, &sysfont);

	
	
	
	
	

	/* Insert application code here, after the board has been initialized. */
	while(1) {
		if(but_flag){
			delay_ms(300);
			
			if (pio_get(BUT_PIO_1, PIO_INPUT, BUT_PIO_IDX_MASK_1)) {
				if (tempo>100){
					tempo-=100;
				}
				else if(tempo < 100){
					tempo = 100;
				}
				else{
					tempo = 200;
				}
			}
			else {
				tempo+=100;
			}
			sprintf(texto,"%.2f hertz",(1000.0/(tempo+tempo)));
			gfx_mono_draw_string(texto, 20,16, &sysfont);
			
			pisca_led(tempo);
			but_flag=0;
		}
		if(but2_flag){
			sprintf(texto,"Parou     ");
			gfx_mono_draw_string(texto, 20,16, &sysfont);
			but2_flag = 0;
		}
		if(but3_flag){
			tempo +=100;
			sprintf(texto,"%.2f hertz",(1000.0/(tempo+tempo)));
			gfx_mono_draw_string(texto, 20,16, &sysfont);
			
			pisca_led(tempo);
			but3_flag =0;
		}
		
		pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
		
	}
}
