// #include <stdio.h>
// #include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"
#include "ws2812.pio.h"


#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7
#define PIN_LED_R 13
#define BUTTON_A 5
#define BUTTON_B 6

// Variável global para armazenar a cor (Entre 0 e 255 para intensidade)
uint8_t led_r = 0; // Intensidade do vermelho
uint8_t led_g = 100; // Intensidade do verde
uint8_t led_b = 0; // Intensidade do azul
static volatile uint8_t incrementar_decrementar = 0;
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

// Prototipação das rotinas
static inline void put_pixel(uint32_t pixel_grb); 
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
void set_one_led(uint8_t r, uint8_t g, uint8_t b, uint8_t numero);
static void gpio_irq_handler(uint gpio, uint32_t events);
bool repeating_timer_callback(struct repeating_timer *t);


// Buffer para armazenar quais LEDs estão ligados matriz 5x5
bool numeros[10][NUM_PIXELS] = {
    {
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0
    },
    {
    0, 1, 1, 1, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 0, 1, 0, 0
    },
    {
    0, 1, 1, 1, 0, 
    0, 1, 0, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 1, 0, 1, 0, 
    0, 0, 1, 0, 0
    },
    {
    0, 0, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 0, 0, 0, 
    0, 0, 1, 0, 0, 
    0, 1, 1, 1, 0
    },
    {
    0, 1, 1, 1, 0, 
    0, 0, 1, 0, 0, 
    0, 1, 1, 1, 1, 
    0, 1, 1, 0, 0, 
    0, 0, 1, 0, 0
    },
    {
    0, 0, 1, 1, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 0, 0, 0, 
    0, 1, 1, 0, 0, 
    0, 1, 1, 1, 0
    },
    {
    0, 0, 1, 0, 0, 
    0, 1, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 1, 0, 0, 0, 
    0, 0, 1, 0, 0
    },
    {
    0, 0, 0, 1, 0, 
    0, 0, 1, 0, 0, 
    0, 1, 0, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0
    },
    {
    0, 0, 1, 0, 0, 
    0, 1, 0, 1, 0, 
    0, 0, 1, 0, 0, 
    0, 1, 0, 1, 0, 
    0, 0, 1, 0, 0
    },
    {
    0, 0, 1, 0, 0, 
    0, 0, 0, 1, 0, 
    0, 1, 1, 1, 0, 
    0, 1, 0, 1, 0, 
    0, 0, 1, 0, 0
    }
};

int main()
{   
    // Inicialização dos pinos
    gpio_init(PIN_LED_R);
    gpio_set_dir(PIN_LED_R, GPIO_OUT);
    
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    
    // Inicialização do PIO e do programa
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    // Inicialização do programa WS2812
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Declaração de uma estrutura de temporizador de repetição
    // Esta estrutura armazenará informações sobre o temporizador configurado
    struct repeating_timer timer;

    // Configura o temporizador para chamar a função de callback a cada 100 ms
    add_repeating_timer_ms(100, repeating_timer_callback, NULL, &timer);

    // Chamando uma única vez para exibir o zero quando o programa rodar na simulação e/ou no kit
    set_one_led(led_r, led_g, led_b, incrementar_decrementar);

    while (1)
    {
        // Laço principal 
    }

    return 0;
}


// Função para enviar um pixel para o WS2812
static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// Função para converter RGB para um valor de 32 bits
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

// Função para ligar um LED com base no número fornecido
void set_one_led(uint8_t r, uint8_t g, uint8_t b, uint8_t numero)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    for (uint8_t i = 0; i < NUM_PIXELS; i++)
    {
        if (numeros[numero][i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    
    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000) // 200 ms de debouncing
    {
        last_time = current_time; // Atualiza o tempo do último evento

        if (gpio == 5)
        {
            if (incrementar_decrementar < 9)
                incrementar_decrementar++;
        }
        else if (gpio == 6) 
        {
            if (incrementar_decrementar > 0)
                incrementar_decrementar--;
        }

        set_one_led(led_r, led_g, led_b, incrementar_decrementar);
    }
}

// Função de callback que será chamada repetidamente pelo temporizador
// O tipo bool indica que a função deve retornar verdadeiro ou falso para continuar ou parar o temporizador.
bool repeating_timer_callback(struct repeating_timer *t)
{
    // Liga ou desliga o led
    gpio_put(PIN_LED_R, !gpio_get(PIN_LED_R));
    
    // Retorna true para manter o temporizador repetindo. Se retornar false, o temporizador para
    return true;
}