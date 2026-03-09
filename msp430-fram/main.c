#include <msp430.h>
#include <msp430fr4133.h>

#define LASTTEMP_ADDR   ((unsigned int*)0x1800)
unsigned int *lastTempPtr = (unsigned int *)0x1800;  // Data FRAM 起始位址

// #pragma PERSISTENT(lastTemp) //FRAM 變數
// unsigned int lastTemp; 

unsigned int currentRead = 0;
#define TEMP_THRESHOLD 648

void initTemperatureSensor() {
    PMMCTL0_H = PMMPW_H;          // 解鎖 PMM 電源管理 
    PMMCTL2 |= INTREFEN | TSENSOREN; // [重點] 必須同時開啟 INTREFEN (內部參考電壓) 和 TSENSOREN (溫度感測器開關)
    __delay_cycles(10000); // 給它一點時間穩定
    PMMCTL0_H = 0;                // 鎖定 PMM

    // --- ADC 設定區 ---
    ADCCTL0 &= ~ADCENC;           // 設定前先 Disable
    ADCCTL0 = ADCSHT_8 | ADCON;   // ADCSHT_8: 256 cycles，長取樣時間確保讀數準確
    ADCCTL1 = ADCSHP;             // 使用採樣計時器
    ADCCTL2 = ADCRES_1;           // 10-bit 解析度 (0~1023)
    ADCMCTL0 = ADCINCH_12 | ADCSREF_1; // Channel 12 是溫度感測器
    ADCCTL0 |= ADCENC;
}

unsigned int readTemperature() {
    ADCCTL0 |= ADCENC | ADCSC;    // 開始轉換
    while (ADCCTL1 & ADCBUSY);    // 等待忙碌標誌結束
    return ADCMEM0;               // 回傳結果
}

void initGPIO() { // 紅燈 P1.0
    P1DIR |= BIT0; // Set P1.0 to output directionOUT
    P1OUT &= ~BIT0; // Clear P1.0 output latch for a defined power-on state
}

void writeLastTemp(unsigned int v) {
    SYSCFG0 &= ~DFWP;        // unlock Data FRAM
    *lastTempPtr = v;
    SYSCFG0 |= DFWP;         // lock again
}

unsigned int readLastTemp() {
    return *lastTempPtr;
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;       // Disable the GPIO power-on default high-impedance mode

    initGPIO();
    initTemperatureSensor();

    // 開機先讀 FRAM（power-loss recovery）
    unsigned int recovered = readLastTemp();

    // 如果 FRAM 裡存的是過熱紀錄，一開機就先閃爍警告
    if (recovered > TEMP_THRESHOLD) {
        int i;
        for(i = 0; i < 10; i++) { // 閃 10 下
            P1OUT |= BIT0; __delay_cycles(100000);
            P1OUT &= ~BIT0; __delay_cycles(100000);
        }
    }

    while(1) {
        currentRead = readTemperature();
        writeLastTemp(currentRead);

        if (currentRead > TEMP_THRESHOLD) {
            P1OUT |= BIT0; 
        } else {
            P1OUT &= ~BIT0;
        }

        __delay_cycles(100000); // 100ms
    }
}
