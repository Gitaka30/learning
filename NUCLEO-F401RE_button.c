#include <stdint.h>

// 疑似的なレジスタ構造体の定義（実務・仕様書に準拠）
typedef struct {
    volatile uint32_t MODER;   // GPIOポート・モード・レジスタ
    volatile uint32_t OTYPER;  // GPIOポート出力タイプ・レジスタ
    volatile uint32_t OSPEEDR; // GPIOポート出力速度・レジスタ
    volatile uint32_t PUPDR;   // GPIOポート・プルアップ/プルダウン・レジスタ
    volatile uint32_t IDR;     // GPIOポート入力データ・レジスタ
    volatile uint32_t ODR;     // GPIOポート出力データ・レジスタ
} GPIO_TypeDef;

typedef struct {
    volatile uint32_t AHB1ENR; // AHB1周辺クロック有効化レジスタ
} RCC_TypeDef;

// NUCLEO-F401RE（STM32F401）のベースアドレス定義
#define RCC   ((RCC_TypeDef *)  0x40023800)
#define GPIOA ((GPIO_TypeDef *) 0x40020000) // ユーザーLED (PA5)
#define GPIOC ((GPIO_TypeDef *) 0x40020800) // ユーザーボタン (PC13)

int main(void) {
    // 手順1：周辺クロックの有効化
    
    RCC->AHB1ENR |= (1 << 0) | (1 << 2);  // GPIOA（LED用）とGPIOC（ボタン用）のクロックを同時に有効化

    // 手順2：LEDピン（PA5）を出力モードに設定
    
    GPIOA->MODER &= ~(0x3 << (5 * 2));    // 清鎖：PA5のモードビット（10番、11番ビット）をクリア
    GPIOA->MODER |= (0x1 << (5 * 2));    // 書き込み：汎用出力モード（01）を設定

    // 手順3：ボタンピン（PC13）を入力モードに設定
    
    GPIOC->MODER &= ~(0x3 << (13 * 2));  // 清鎖：PC13のモードビット（26番、27番ビット）をクリア（00で入力モード）

    while (1) {
        // 手順4：ボタン入力状態の読み出し（ビットマスク処理）
        // NUCLEOボードのボタンは「押すとLOW（0）」になるプルアップ回路（アクティブLOW）
        if (!(GPIOC->IDR & (1 << 13))) {    
            GPIOA->ODR |= (1 << 5);       // ボタンが押されている場合：LED（PA5）を点灯（HIGHにする）
        } else {
           
            GPIOA->ODR &= ~(1 << 5);     // ボタンが離されている場合：LED（PA5）を消灯（LOWにする）
        }
    }
}
