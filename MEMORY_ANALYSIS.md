# Analise de Memoria — CSA (STM32F042K6Tx)

**MCU:** STM32F042K6Tx (Cortex-M0, sem FPU)
**Flash:** 32,768 bytes (32 KB)
**RAM:** 6,144 bytes (6 KB)
**Compilador:** ARM Compiler V5.06 (Keil MDK-ARM)
**Otimizacao atual:** -O1 (balanco velocidade/tamanho)
**Data da analise:** 2026-03-26

---

## 1. Resumo Executivo

| Recurso | Usado | Total | Livre | Uso |
|---------|-------|-------|-------|-----|
| **Flash (ROM)** | 31,668 bytes | 32,768 bytes | **1,100 bytes** | 96.6% |
| **RAM** | 2,456 bytes | 6,144 bytes | 3,688 bytes | 40.0% |

**Problema:** O firmware atual ocupa 96.6% da Flash, restando apenas 1,100 bytes livres. Para habilitar a funcao de recepcao CAN (CAN RX callback), o build sobe para ~31,400 bytes. O objetivo e liberar pelo menos 1 KB adicional de Flash para permitir novas funcionalidades.

**Flash e o recurso critico.** RAM tem espaco suficiente.

---

## 2. Mapa de Memoria Flash — Por Modulo

### 2.1 Codigo da Aplicacao

| Modulo | Codigo (bytes) | % do Total | Descricao |
|--------|---------------|------------|-----------|
| main.o | 4,990 | 15.8% | Logica principal, LEDs, bateria, rampa |
| mpu_6050.o | 1,736 | 5.5% | Driver MPU6050 + Kalman |
| calibracao.o | 1,140 | 3.6% | Rotinas de calibracao |
| perifericos.o | 1,050 | 3.3% | Inicializacao de perifericos |
| flash.o | 280 | 0.9% | Leitura/escrita Flash |
| pid.o | 228 | 0.7% | Controlador PID |
| sae_j1939.o | 180 | 0.6% | Protocolo CAN J1939 |
| stm32f0xx_it.o | 138 | 0.4% | Handlers de interrupcao |
| stm32f0xx_hal_msp.o | 624 | 2.0% | Callbacks MSP (HAL) |
| system_stm32f0xx.o | 2 | 0.0% | SystemInit CMSIS |
| startup | 116 | 0.4% | Codigo de inicializacao |
| **Subtotal Aplicacao** | **10,484** | **33.1%** | |

### 2.2 Drivers HAL (ST)

| Modulo HAL | Codigo (bytes) | % do Total | Funcoes Usadas |
|------------|---------------|------------|----------------|
| stm32f0xx_hal_tim.o | 2,342 | 7.4% | PWM Init, Start, Config |
| stm32f0xx_hal_can.o | 2,276 | 7.2% | CAN Init, Start, AddTxMsg, IRQHandler |
| stm32f0xx_hal_rcc.o | 2,244 | 7.1% | Clock config (HSI48, PLL) |
| stm32f0xx_hal_i2c.o | 2,136 | 6.7% | I2C Mem_Read, Mem_Write |
| stm32f0xx_hal_adc.o | 1,856 | 5.9% | ADC Start, Poll, GetValue, Stop |
| stm32f0xx_hal_gpio.o | 510 | 1.6% | GPIO Init, ReadPin, WritePin |
| stm32f0xx_hal_flash.o | 480 | 1.5% | Flash Unlock, Program, Lock |
| stm32f0xx_hal_rcc_ex.o | 392 | 1.2% | Periph clock config |
| stm32f0xx_hal_tim_ex.o | 272 | 0.9% | Master config, break/deadtime |
| stm32f0xx_hal_flash_ex.o | 256 | 0.8% | Flash erase |
| stm32f0xx_hal_cortex.o | 222 | 0.7% | NVIC Set/Enable IRQ |
| stm32f0xx_hal_i2c_ex.o | 222 | 0.7% | Analog/digital filter config |
| stm32f0xx_hal.o | 196 | 0.6% | HAL_Init, HAL_Delay, IncTick |
| **Subtotal HAL** | **13,404** | **42.3%** | |

### 2.3 Bibliotecas do Compilador

| Biblioteca | Codigo (bytes) | % do Total | Descricao |
|------------|---------------|------------|-----------|
| **fz_ps.l** (ponto flutuante) | 5,342 | 16.9% | Emulacao software de float/double |
| **m_ps.l** (matematica) | 844 | 2.7% | atan2f, sqrtf, fmaxf, fminf |
| **c_p.l** (C runtime) | 1,058 | 3.3% | memclr, divisao, heap, startup |
| **Subtotal Bibliotecas** | **7,244** | **22.9%** | |

### 2.4 Dados Constantes (RO Data)

| Origem | Bytes |
|--------|-------|
| Tabela de vetores (RESET) | 192 |
| Constantes de bibliotecas | 192 |
| Constantes do codigo | 88 |
| **Total RO Data** | **472** |

### 2.5 Visao Geral Flash

```
+--------------------------------------------------+
|  Flash Total: 32,768 bytes (32 KB)               |
+--------------------------------------------------+
| HAL Drivers       | 13,404 bytes ||||||||||| 41% |
| Biblio. Float/Math |  6,186 bytes |||||||    19% |
| main.o             |  4,990 bytes ||||||     15% |
| App (outros)       |  4,870 bytes |||||      15% |
| C Runtime          |  1,058 bytes ||          3% |
| RO Data            |    472 bytes |           1% |
| RW Data Init       |     32 bytes |           0% |
| *** LIVRE ***      |  1,100 bytes ||          3% |
+--------------------------------------------------+
```

---

## 3. Detalhamento da Biblioteca de Ponto Flutuante

O STM32F042 (Cortex-M0) **nao possui FPU**. Toda operacao float/double e feita por software. A biblioteca `fz_ps.l` consome **5,342 bytes** — e o segundo maior consumidor de Flash.

### 3.1 Operacoes Double-Precision (CRITICO)

O codigo usa **double** sem necessidade em varios pontos, puxando bibliotecas de 64 bits:

| Funcao da Biblioteca | Bytes | Causa no Codigo |
|---------------------|-------|-----------------|
| ddiv.o (divisao double) | 1,096 | `le_bateria()`: `val * 3.3 / 4095` (3.3 e double!) |
| daddsub.o (soma/sub double) | 856 | `trata_leds_carr()`: `tensao >= 25.3` (25.3 e double!) |
| dmul.o (multiplicacao double) | 584 | `le_bateria()`: `3.3 / 4095 / 0.1060` |
| d2f.o (double→float) | 124 | Conversoes implicitas |
| dcmpin.o (comparacao double) | 144 | Comparacoes em trata_leds_carr |
| dflti.o (int→double) | 88 | Conversao val_bat_ad para double |
| dfixi.o (double→int) | 108 | |
| dlef.o, drlef.o, deqf.o | 304 | Comparacoes <= >= == |
| dnan2.o | 20 | NaN handling |
| **Total Double-Precision** | **~3,324** | **10.5% da Flash!** |

**Causa raiz:** Literais como `3.3`, `25.3`, `24.8`, `0.1060` sao **double** em C. Precisam do sufixo `f`: `3.3f`, `25.3f`, etc.

### 3.2 Operacoes Single-Precision (Necessarias)

| Funcao | Bytes | Uso |
|--------|-------|-----|
| faddsub.o | 372 | Kalman filter, PID |
| fdiv.o | 352 | Divisoes float |
| fmul.o | 176 | Multiplicacoes float |
| fcmp4.o / fcmpin.o | 276 | Comparacoes float |
| ffixi.o / ffixui.o | 124 | Float→int |
| fflti.o | 94 | Int→float |
| frlef.o / feqf.o | 172 | Comparacoes float |
| fscalbn.o | 68 | Escala |
| fsqrt.o | 144 | sqrt (via sqrtf) |
| retnan.o | 94 | NaN handling |
| **Total Single-Precision** | **~1,872** | Necessario para Kalman/PID |

### 3.3 Funcoes Matematicas

| Funcao | Bytes | Uso |
|--------|-------|-----|
| atan2f.o | 668 | Calculo de angulo (MPU) |
| fmaxf.o | 38 | Limite minimo PWM |
| fminf.o | 38 | Nao usado diretamente? |
| fpclassifyf.o | 34 | Dependencia de atan2f |
| sqrtf.o | 44 | Calculo de angulo (MPU) |
| funder.o | 22 | Underflow handling |
| **Total Math** | **844** | |

---

## 4. Mapa de Memoria RAM

| Regiao | Bytes | Descricao |
|--------|-------|-----------|
| **Stack** | 1,024 | Pilha (0x400) |
| **Heap** | 512 | Heap (0x200) |
| **ZI Data (BSS)** | 572 | Variaveis globais zeradas |
| **RW Data** | 348 | Variaveis globais inicializadas |
| **Total Usado** | 2,456 | 40% |
| **Livre** | 3,688 | 60% |

### Maiores Consumidores de RAM (ZI/BSS):

| Variavel/Handle | Bytes (est.) | Modulo |
|-----------------|-------------|--------|
| perifericos.o BSS (hadc, hcan, hi2c1, htim1, htim14) | 372 | Handles HAL |
| main.o BSS (pid, struct_zeramento) | 56 | PID + calibracao |
| main.o DATA (LEDs, floats, flags) | 272 | Variaveis globais |
| mpu_6050.o DATA (Kalman states) | 36 | Estados Kalman |
| sae_j1939.o DATA | 12 | J1939 union |
| calibracao.o DATA | 12 | |
| stm32f0xx_hal.o DATA (uwTick) | 9 | Tick counter |

**Nota:** Os handles HAL (`CAN_HandleTypeDef`, `I2C_HandleTypeDef`, etc.) consomem ~372 bytes de RAM. Em bare metal, seriam eliminados.

---

## 5. Oportunidades de Otimizacao

### 5.1 PRIORIDADE ALTA — Ganho Facil

#### A) Eliminar Double-Precision (~2,500-3,300 bytes)
**Esforco: Facil | Risco: Nenhum**

Adicionar sufixo `f` em todos os literais float. Exemplos:

```c
// ANTES (puxa biblioteca double — 3,324 bytes extras!)
leituras_bateria[cont_bateria] = ((val_bat_ad * 3.3) / 4095) / 0.1060;
if(tensao >= 25.3)
if(tensao >= 24.8)

// DEPOIS (somente float — elimina ddiv, dmul, daddsub, etc.)
leituras_bateria[cont_bateria] = ((val_bat_ad * 3.3f) / 4095.0f) / 0.1060f;
if(tensao >= 25.3f)
if(tensao >= 24.8f)
```

**Arquivos a corrigir:**
- `main.c`: `le_bateria()` — literais `3.3`, `4095`, `0.1060`
- `main.c`: `trata_leds_carr()` — literais `1.0`, `25.3`, `24.8`, `24.3`, `24`, `21`
- `main.c`: `trata_PID()` — literal `1000.0` (definido como `MILIS_PARA_SEGUNDOS`)
- `main.h`: `#define MILIS_PARA_SEGUNDOS 1000.0` → `1000.0f`
- `main.h`: `#define tensao_maxima 25.0` → `25.0f`
- `main.h`: `#define angulo_acionamento_pid 1.5` → `1.5f`
- `main.h`: `#define histerese_acionamento_pid 0.5` → `0.5f`
- `mpu_6050.c`: verificar todos os literais (a maioria ja usa `f`)

**Economia estimada: ~2,500-3,300 bytes**

#### B) Mudar Otimizacao para -Os (~1,000-2,000 bytes)
**Esforco: Trivial | Risco: Baixo**

No Keil: Project → Options → C/C++ → Optimization → mudar de "Level 1" para "Optimize for Size (-Os)".

O compilador ira priorizar tamanho de codigo. Em Cortex-M0, -Os tipicamente economiza 5-10% comparado com -O1.

**Economia estimada: ~1,500-3,000 bytes**

#### C) Remover `#include <stdio.h>` e `<stdlib.h>` (~0-200 bytes)
**Esforco: Trivial | Risco: Nenhum**

Em `main.c` e `stm32f0xx_it.c`, estas bibliotecas sao incluidas mas aparentemente nao usadas (nenhum printf, sprintf, malloc). O `abs()` de `<stdlib.h>` pode ser substituido por macro:

```c
#define ABS(x) ((x) < 0 ? -(x) : (x))
```

Mesmo que o linker ja remova funcoes nao usadas, os headers podem puxar inicializacoes.

---

### 5.2 PRIORIDADE MEDIA — Requer Mais Trabalho

#### D) Substituir HAL_GPIO_WritePin por Acesso Direto ao BSRR (~400-500 bytes)
**Esforco: Facil | Risco: Nenhum**

O `HAL_GPIO_WritePin` e chamado dezenas de vezes (LEDs, profets, rele, buzzer). O acesso direto ao registrador BSRR e uma unica instrucao:

```c
// ANTES (HAL — funcao com validacao de parametros)
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

// DEPOIS (bare metal — 1 instrucao ARM)
GPIOA->BSRR = GPIO_PIN_8;           // SET
GPIOA->BRR  = GPIO_PIN_8;           // RESET (ou BSRR = PIN << 16)
```

Atualizar todos os `#define` em `perifericos.h`:
```c
#define buzzer_high()  (GPIOA->BSRR = GPIO_PIN_8)
#define buzzer_low()   (GPIOA->BRR  = GPIO_PIN_8)
// ... repetir para todos os pinos
```

Se `HAL_GPIO_WritePin` nao for mais referenciado, o linker remove `stm32f0xx_hal_gpio.o` parcialmente.

**Economia estimada: ~300-500 bytes**

#### E) Substituir HAL ADC por Acesso Direto (~1,500-1,800 bytes)
**Esforco: Medio | Risco: Baixo**

A funcao `le_adc_canal()` ja acessa `ADC1->CHSELR` diretamente mas ainda chama HAL_ADC_Start/Poll/GetValue/Stop. Em bare metal:

```c
uint16_t le_adc_canal(uint32_t canal)
{
    ADC1->CHSELR = (1 << canal);
    ADC1->CR |= ADC_CR_ADSTART;
    while (!(ADC1->ISR & ADC_ISR_EOC));
    return (uint16_t)ADC1->DR;
}
```

Tambem elimina a necessidade de `HAL_ADC_Init` — a inicializacao pode ser feita diretamente nos registradores.

**Economia estimada: ~1,500-1,800 bytes** (remove stm32f0xx_hal_adc.o inteiro)

#### F) Substituir HAL TIM por Acesso Direto (~2,000-2,300 bytes)
**Esforco: Medio | Risco: Baixo**

O timer e usado apenas para PWM (TIM1 CH2/CH3). A inicializacao e simples:

```c
void tim1_pwm_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
    TIM1->PSC = 0;
    TIM1->ARR = 3199;
    TIM1->CCMR1 = (6 << 12);  // CH2 PWM mode 1
    TIM1->CCMR2 = (6 << 4);   // CH3 PWM mode 1
    TIM1->CCER = TIM_CCER_CC2E | TIM_CCER_CC3E;
    TIM1->BDTR = TIM_BDTR_MOE;
    TIM1->CR1 = TIM_CR1_CEN;
    TIM1->CCR2 = 0;
    TIM1->CCR3 = 0;
}
```

**Economia estimada: ~2,000-2,300 bytes** (remove hal_tim.o + hal_tim_ex.o)

#### G) Substituir HAL I2C por Acesso Direto (~1,500-2,000 bytes)
**Esforco: Medio | Risco: Medio**

O I2C e usado apenas em modo polling (Mem_Read, Mem_Write) para o MPU6050. Uma implementacao bare metal e ~150 linhas:

```c
uint8_t i2c_mem_write(uint8_t addr, uint8_t reg, uint8_t data)
{
    I2C1->CR2 = (addr & 0xFE) | (2 << 16) | I2C_CR2_AUTOEND | I2C_CR2_START;
    while (!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = reg;
    while (!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = data;
    while (!(I2C1->ISR & I2C_ISR_STOPF));
    I2C1->ICR = I2C_ICR_STOPCF;
    return 0;
}
```

**Economia estimada: ~1,500-2,000 bytes** (remove hal_i2c.o + hal_i2c_ex.o)

#### H) Substituir HAL CAN por Acesso Direto (~2,000-2,200 bytes)
**Esforco: Medio | Risco: Medio**

O CAN usa Init, Start, ConfigFilter, AddTxMessage, e IRQHandler. Em bare metal:

```c
void can_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_CANEN;
    CAN->MCR |= CAN_MCR_INRQ;
    while (!(CAN->MSR & CAN_MSR_INAK));
    CAN->BTR = (11 << 16) | (1 << 20) | (0 << 24) | 11;  // 250kbps
    CAN->MCR &= ~CAN_MCR_INRQ;
    while (CAN->MSR & CAN_MSR_INAK);
    // Config filter...
}

void can_send(uint32_t ext_id, uint8_t* data, uint8_t len)
{
    CAN->sTxMailBox[0].TIR = (ext_id << 3) | CAN_TI0R_IDE;
    CAN->sTxMailBox[0].TDTR = len;
    CAN->sTxMailBox[0].TDLR = *(uint32_t*)data;
    CAN->sTxMailBox[0].TDHR = *(uint32_t*)(data + 4);
    CAN->sTxMailBox[0].TIR |= CAN_TI0R_TXRQ;
}
```

**Economia estimada: ~2,000-2,200 bytes** (remove hal_can.o)

---

### 5.3 PRIORIDADE BAIXA — Grande Esforco

#### I) Substituir Ponto Flutuante por Ponto Fixo (~4,000-5,000 bytes)
**Esforco: Alto | Risco: Alto**

Eliminar completamente `float` do projeto usando aritmetica de ponto fixo (Q16.16 ou Q8.8). Isso removeria toda a biblioteca `fz_ps.l` (5,342 bytes) e `m_ps.l` (844 bytes).

O Kalman filter e PID podem funcionar em ponto fixo, mas `atan2f` e `sqrtf` precisariam de implementacoes CORDIC ou lookup table.

**Economia estimada: ~4,000-5,000 bytes** (mas requer reescrita significativa)

#### J) Simplificar LED Handling (~100-200 bytes)
**Esforco: Facil | Risco: Nenhum**

As 16 variaveis `led_1` a `led_16` (16 bytes RAM, cada `unsigned char`) poderiam ser substituidas por acesso direto ao `union_leds_my` que ja existe:

```c
// ANTES
led_3 = 1;  // unsigned char global separado
// Na funcao envia_leds_my(), reconstroi o bitfield manualmente

// DEPOIS — usar diretamente
led_my_3 = 1;  // bit no union, que ja vira byte_leds_my automaticamente
// Remove toda a reconstrucao no envia_leds_my()
```

**Economia: ~100-200 bytes Flash + 16 bytes RAM**

---

## 6. Comparacao HAL vs Bare Metal

### 6.1 Tamanho por Periferico

| Periferico | HAL (bytes) | Bare Metal Est. (bytes) | Economia (bytes) |
|------------|------------|------------------------|-----------------|
| Timer (TIM1 PWM) | 2,614 | ~150 | ~2,464 |
| CAN | 2,276 | ~300 | ~1,976 |
| RCC (Clock) | 2,636 | ~200 | ~2,436 |
| I2C | 2,358 | ~250 | ~2,108 |
| ADC | 1,856 | ~100 | ~1,756 |
| GPIO | 510 | ~50 | ~460 |
| Flash | 736 | ~100 | ~636 |
| Cortex (NVIC) | 222 | ~30 | ~192 |
| HAL Core | 196 | 0 | ~196 |
| MSP Callbacks | 624 | 0 | ~624 |
| **Total** | **14,028** | **~1,180** | **~12,848** |

### 6.2 Resumo

| Metrica | HAL Atual | Bare Metal | Diferenca |
|---------|-----------|------------|-----------|
| **Codigo HAL/periferico** | 14,028 bytes | ~1,180 bytes | **-12,848 bytes** |
| **RAM (handles HAL)** | ~372 bytes | ~0 bytes | **-372 bytes** |
| **Flash total estimado** | 31,668 bytes | ~18,820 bytes | **-12,848 bytes** |
| **Flash livre** | 1,100 bytes | ~13,948 bytes | **+12,848 bytes** |

### 6.3 Por que o HAL e tao Grande?

O HAL da ST foi projetado para ser **generico e robusto**, nao compacto:

1. **Validacao de parametros** — Cada funcao valida todos os inputs (assert_param)
2. **Estado completo** — Handles armazenam estado, locks, callbacks, error codes
3. **Suporte a todos os modos** — DMA, IT, polling — mesmo que voce use so polling
4. **Abstracoes de callback** — MSP Init/DeInit, callbacks configurados mas nunca chamados
5. **Funcoes monoliticas** — `HAL_TIM_Base_Init` configura tudo, mesmo que voce precise de 3 registradores

Exemplo concreto — `HAL_GPIO_WritePin`:
```c
// HAL: ~20 instrucoes (validacao, branch, write)
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    assert_param(IS_GPIO_PIN(GPIO_Pin));
    assert_param(IS_GPIO_PIN_ACTION(PinState));
    if (PinState != GPIO_PIN_RESET) {
        GPIOx->BSRR = GPIO_Pin;
    } else {
        GPIOx->BSRR = (uint32_t)GPIO_Pin << 16U;
    }
}

// Bare metal: 1 instrucao ARM
GPIOA->BSRR = GPIO_PIN_8;  // SET
GPIOA->BRR  = GPIO_PIN_8;  // RESET
```

### 6.4 Riscos do Bare Metal

| Risco | Mitigacao |
|-------|-----------|
| Bugs de inicializacao (clock, GPIO) | Copiar sequencia exata do HAL, validar com datasheet |
| I2C timing errado | Manter o valor de TIMINGR do HAL (0x00201D2B) |
| CAN bit timing errado | Manter prescaler/BTR do HAL |
| Mais dificil de manter | Comentar bem, referencia ao Reference Manual |
| CubeMX nao regenera | Aceitar — o projeto ja nao depende de regeneracao |

---

## 7. Plano de Acao Priorizado

### Fase 1 — Ganhos Imediatos (sem mudar arquitetura)

| # | Acao | Economia Est. | Esforco |
|---|------|--------------|---------|
| 1 | Mudar otimizacao para -Os | 1,500-3,000 bytes | 1 minuto |
| 2 | Adicionar sufixo `f` em todos os literais float | 2,500-3,300 bytes | 30 minutos |
| 3 | Remover includes desnecessarios (stdio.h, stdlib.h) | 0-200 bytes | 5 minutos |
| | **Total Fase 1** | **~4,000-6,500 bytes** | **< 1 hora** |

### Fase 2 — Substituicao Seletiva do HAL

| # | Acao | Economia Est. | Esforco |
|---|------|--------------|---------|
| 4 | GPIO → BSRR/BRR direto | 300-500 bytes | 1 hora |
| 5 | ADC → registradores diretos | 1,500-1,800 bytes | 2 horas |
| 6 | TIM1 → registradores diretos | 2,000-2,300 bytes | 2 horas |
| | **Total Fase 2** | **~3,800-4,600 bytes** | **~5 horas** |

### Fase 3 — Bare Metal Completo

| # | Acao | Economia Est. | Esforco |
|---|------|--------------|---------|
| 7 | I2C → registradores diretos | 1,500-2,000 bytes | 3 horas |
| 8 | CAN → registradores diretos | 2,000-2,200 bytes | 3 horas |
| 9 | RCC → registradores diretos | 2,000-2,400 bytes | 2 horas |
| 10 | Remover HAL completamente | 200-400 bytes | 1 hora |
| | **Total Fase 3** | **~5,700-7,000 bytes** | **~9 horas** |

### Economia Total Possivel

| Fase | Economia Minima | Economia Maxima |
|------|----------------|-----------------|
| Fase 1 (facil) | 4,000 bytes | 6,500 bytes |
| Fase 1 + 2 | 7,800 bytes | 11,100 bytes |
| Fase 1 + 2 + 3 | 13,500 bytes | 18,100 bytes |

**Com apenas a Fase 1, o firmware ficaria com ~25,000-27,000 bytes, liberando 5-7 KB para novas funcionalidades.**

---

## 8. Nota sobre a Funcao CAN RX

Para habilitar o `HAL_CAN_RxFifo0MsgPendingCallback` (atualmente comentado em `stm32f0xx_it.c`), sera necessario:
- `HAL_CAN_GetRxMessage` — ja linkado (~200-300 bytes adicionais)
- `HAL_CAN_ActivateNotification` — para habilitar interrupcao RX
- Variaveis para armazenar RxHeader e RxData — ~40 bytes RAM

Com a Fase 1 de otimizacao, havera espaco de sobra para esta funcionalidade.

---

*Analise gerada a partir do arquivo de mapa: `MDK-ARM/teste_csa/teste_csa.map`*
*Build analisado: ARM Compiler 5.06 update 7, Otimizacao -O1*
