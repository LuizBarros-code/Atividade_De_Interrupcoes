# Atividade Interrupções

Este projeto demonstra o uso de interrupções e temporizadores no Raspberry Pi Pico para controlar LEDs WS2812 e um LED RGB. O código é escrito em C e utiliza a biblioteca Pico SDK.

## Descrição

O código controla uma matriz de LEDs WS2812 e um LED RGB no Raspberry Pi Pico. Ele utiliza interrupções para detectar pressionamentos de botões e um temporizador para piscar o LED RGB repetidamente.

### Funcionalidades

- **Controle de LEDs WS2812**: Liga e desliga LEDs individuais em uma matriz 5x5 com base em números predefinidos.
- **Interrupções de Botões**: Detecta pressionamentos de botões para incrementar ou decrementar o número exibido na matriz de LEDs.
- **Temporizador Repetitivo**: Pisca o LED RGB a cada 100 ms.

## Componentes

- **LEDs WS2812**: LEDs endereçáveis controlados via PIO.
- **LED RGB**: LED controlado via GPIO.
- **Botões**: Dois botões para incrementar e decrementar o número exibido na matriz de LEDs.

## Configuração do Hardware

- **WS2812_PIN**: Pino 7
- **PIN_LED_R**: Pino 13
- **BUTTON_A**: Pino 5
- **BUTTON_B**: Pino 6

## Como Funciona

1. **Inicialização dos Pinos**: Configura os pinos GPIO para os LEDs e botões.
2. **Inicialização do PIO**: Configura o PIO para controlar os LEDs WS2812.
3. **Configuração das Interrupções**: Configura interrupções para os botões.
4. **Configuração do Temporizador**: Configura um temporizador repetitivo para piscar o LED RGB.
5. **Loop Principal**: Mantém o programa em execução.

### Funções Principais

- `put_pixel(uint32_t pixel_grb)`: Envia um pixel para o WS2812.
- `urgb_u32(uint8_t r, uint8_t g, uint8_t b)`: Converte valores RGB para um valor de 32 bits.
- `set_one_led(uint8_t r, uint8_t g, uint8_t b, uint8_t numero)`: Liga um LED com base no número fornecido.
- `gpio_irq_handler(uint gpio, uint32_t events)`: Manipulador de interrupção para os botões com debouncing.
- `repeating_timer_callback(struct repeating_timer *t)`: Callback do temporizador para piscar o LED RGB.

## Como Executar

1. **Clone o Repositório**: Clone o repositório para o seu ambiente local.
2. **Compile o Código**: Compile o código utilizando o CMake e o Pico SDK.
3. **Carregue no Raspberry Pi Pico**: Carregue o binário compilado no Raspberry Pi Pico.
4. **Conecte os Componentes**: Conecte os LEDs WS2812, o LED RGB e os botões aos pinos especificados.
5. **Execute**: Execute o código no Raspberry Pi Pico.

## Vídeo Demonstrativo

Para mais detalhes sobre o funcionamento do projeto, assista ao vídeo demonstrativo no link abaixo:

[Vídeo Demonstrativo](https://youtu.be/p6xIbqXBE5M)

---

Este README fornece uma visão geral do projeto e explica como ele funciona. Siga as instruções para configurar e executar o código no seu Raspberry Pi Pico.
