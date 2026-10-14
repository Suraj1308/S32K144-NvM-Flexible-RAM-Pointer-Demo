
#include "sdk_project_config.h"
#include "nvm.h"
#include <stdio.h>
#include <stdint.h>


#define CLOCK_CONFIG       clockMan1_InitConfig0

#define UART_INSTANCE      INST_LPUART_1
#define UART_STATE         lpUartState0
#define UART_CONFIG        lpuart_0_InitConfig0

#define ADC_PAL_INSTANCE   adc_pal_1_instance
#define ADC_PAL_CONFIG     adc_pal_1_config
#define ADC_PAL_RESULTS    adc_pal_1_results0
#define ADC_MAX_RAW        4095u
#define BTN_GPIO           PTC
#define BTN_SW1_PIN        12U
#define BTN_SW2_PIN        13U
#define BTN_DEBUG          0
#define NVM_BLOCK_POT      1U
#define POT_HYSTERESIS     2U
#define DEBOUNCE_SAMPLES   3U
#define LOOPS_PER_MS       4000UL

static uint8_t ramBuf[NVM_BLOCK_SIZE];
static uint8_t savedValue;
static void Delay_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0u; i < (ms * LOOPS_PER_MS); i++) { }
}


static void UART_SendString(const char *str)
{
    uint32_t len = 0u;

    while (str[len] != '\0')
    {
        len++;
    }
    if (len == 0u)
    {
        return;
    }

    if (LPUART_DRV_SendData(UART_INSTANCE, (const uint8_t *)str, len) == STATUS_SUCCESS)
    {
        while (LPUART_DRV_GetTransmitStatus(UART_INSTANCE, NULL) == STATUS_BUSY)
        {

        }
    }
}

static void PrintLabelValue(const char *label, uint32_t value)
{
    char buf[48];
    (void)snprintf(buf, sizeof(buf), "%s%lu\r\n", label, (unsigned long)value);
    UART_SendString(buf);
}


static uint8_t ReadPotPercent(void)
{
    uint32_t raw;

    (void)ADC_StartGroupConversion(&ADC_PAL_INSTANCE, 0u);
    Delay_ms(2u);
    raw = *(volatile uint16_t *)&ADC_PAL_RESULTS[0];

    if (raw > ADC_MAX_RAW) { raw = ADC_MAX_RAW; }
    return (uint8_t)((raw * 100u + (ADC_MAX_RAW / 2u)) / ADC_MAX_RAW);
}


static uint8_t pressedLevel[2] = {1u, 1u};

static uint8_t ReadLevel(uint8_t pin)
{
    return (uint8_t)((PINS_DRV_ReadPins(BTN_GPIO) >> pin) & 1u);
}

static void DetectButtonPolarity(void)
{
    pressedLevel[0] = (uint8_t)(ReadLevel(BTN_SW1_PIN) ^ 1u);   
    pressedLevel[1] = (uint8_t)(ReadLevel(BTN_SW2_PIN) ^ 1u);
}

static uint8_t RawPressed(uint8_t index, uint8_t pin)
{
    return (uint8_t)((ReadLevel(pin) == pressedLevel[index]) ? 1u : 0u);
}

static uint8_t PressEvent(uint8_t index, uint8_t pin)
{
    static uint8_t cnt[2]     = {0u, 0u};
    static uint8_t latched[2] = {0u, 0u};

    if (RawPressed(index, pin) != 0u)
    {
        if (cnt[index] < 255u) { cnt[index]++; }
        if ((cnt[index] >= DEBOUNCE_SAMPLES) && (latched[index] == 0u))
        {
            latched[index] = 1u;
            return 1u;
        }
    }
    else
    {
        cnt[index] = 0u;
        latched[index] = 0u;
    }
    return 0u;
}


static uint8_t IsSaved(void)
{
    return (uint8_t)((ramBuf[0] == savedValue) ? 1u : 0u);
}

static void PrintStatus(void)
{
    UART_SendString("Status: ");
    UART_SendString(IsSaved() ? "SAVED\r\n" : "UNSAVED\r\n");
}

static void PrintState(uint8_t pot)
{
    PrintLabelValue("Potentiometer: ", pot);
    PrintLabelValue("RAM value: ", ramBuf[0]);
    PrintStatus();
    UART_SendString("\r\n");
}

static void DoSave(void)
{
    UART_SendString("SW1 pressed\r\n");
    if (NvM_WriteBlock(NVM_BLOCK_POT, ramBuf) == NVM_OK)
    {
        savedValue = ramBuf[0];
        (void)NvM_SetRamBlockStatus(NVM_BLOCK_POT, 0u);
        PrintLabelValue("Saved value: ", savedValue);
    }
    else
    {
        UART_SendString("Save FAILED\r\n");
    }
    PrintStatus();
    UART_SendString("\r\n");
}

static void DoDiscard(void)
{
    UART_SendString("SW2 pressed\r\n");
    if (IsSaved() != 0u)
    {
        UART_SendString("No unsaved changes\r\n");
    }
    else if (NvM_ReadBlock(NVM_BLOCK_POT, ramBuf) == NVM_OK)
    {
        (void)NvM_SetRamBlockStatus(NVM_BLOCK_POT, 0u);
        UART_SendString("Unsaved changes discarded\r\n");
        PrintLabelValue("RAM value restored: ", ramBuf[0]);
    }
    else
    {
        UART_SendString("Restore FAILED\r\n");
    }
    PrintStatus();
    UART_SendString("\r\n");
}

int main(void)
{
    uint8_t pot, potRef, diff;

    
    CLOCK_DRV_Init(&CLOCK_CONFIG);
    PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0, g_pin_mux_InitConfigArr0);
    LPUART_DRV_Init(UART_INSTANCE, &UART_STATE, &UART_CONFIG);
    (void)ADC_Init(&ADC_PAL_INSTANCE, &ADC_PAL_CONFIG);

    Delay_ms(1500); /* wait ~1.5 s so the terminal is ready; buttons must be released now */
    DetectButtonPolarity();
    UART_SendString("S32K144 NvM Potentiometer Demo\r\n");

    
    if (NvM_ReadBlock(NVM_BLOCK_POT, ramBuf) != NVM_OK)
    {
        UART_SendString("NvM read FAILED\r\n");
        for (;;) { }
    }
    savedValue = ramBuf[0];

   
    pot       = ReadPotPercent();
    potRef    = pot;
    ramBuf[0] = pot;
    (void)NvM_SetRamBlockStatus(NVM_BLOCK_POT, (uint8_t)(IsSaved() != 0u ? 0u : 1u));
    PrintState(pot);

    for (;;)
    {
        Delay_ms(10);
        pot = ReadPotPercent();

#if (BTN_DEBUG == 1)
        {
            static uint8_t lastState = 0xFFu;
            uint8_t st = (uint8_t)((RawPressed(0u, BTN_SW1_PIN) << 1) | RawPressed(1u, BTN_SW2_PIN));
            if (st != lastState)
            {
                char dbg[64];
                lastState = st;
                (void)snprintf(dbg, sizeof(dbg), "[button pins] PTC12=%u PTC13=%u (raw levels)\r\n",
                               (unsigned int)ReadLevel(BTN_SW1_PIN), (unsigned int)ReadLevel(BTN_SW2_PIN));
                UART_SendString(dbg);
            }
        }
#endif

        if (PressEvent(0u, BTN_SW1_PIN) != 0u)
        {
            DoSave();
            potRef = pot;                
        }
        else if (PressEvent(1u, BTN_SW2_PIN) != 0u)
        {
            DoDiscard();
            potRef = pot;                
        }
        else
        {
            diff = (pot > potRef) ? (uint8_t)(pot - potRef) : (uint8_t)(potRef - pot);
            if ((diff >= POT_HYSTERESIS) || ((pot != potRef) && ((pot == 0u) || (pot == 100u))))
            {
                potRef    = pot;
                ramBuf[0] = pot;
                (void)NvM_SetRamBlockStatus(NVM_BLOCK_POT, (uint8_t)(IsSaved() != 0u ? 0u : 1u));
                PrintState(pot);
            }
        }
    }
}
