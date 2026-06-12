STM32F4マイコンを使用したボタン操作
やりたいこと　PA0を使ってボタン操作する

①ピンを特定する　回路図からピンはPA0
GPIOAENはリファレンスマニュアルから0ビット目
RCC_AHB1ENRをクロック供給レジスタ表から調べる

②レジスタマップからオフセット値を探す
レジスタ境界アドレス表からRCCのベースアドレスを探す

③GPIOAEN_MODERの設定
GPIOA_MODERのベースアドレスを探す
アドレスオフセット値を探す

#include <stdint.h> // uint32_t の型定義用

// クロック制御 (RCC)
#define REG_RCC_AHB1ENR     (*(volatile uint32_t *)0x40023830) // ベース 0x40023800 + オフセット 0x30

// GPIOA (PA0 ボタン入力用)
#define REG_GPIOA_MODER     (*(volatile uint32_t *)0x40020000) // ベース 0x40020000 + オフセット 0x00
#define REG_GPIOA_IDR       (*(volatile uint32_t *)0x40020010) // ベース 0x40020000 + オフセット 0x10

// GPIOD (PD12 緑LED出力用)
#define REG_GPIOD_MODER     (*(volatile uint32_t *)0x40020C00) // ベース 0x40020C00 + オフセット 0x00
#define REG_GPIOD_ODR       (*(volatile uint32_t *)0x40020C14) // ベース 0x40020C00 + オフセット 0x14

//待ち関数(チャタリング防止)
void simple_delay(volatile uint32_t count)
{
	while(count--){__asm("nop");}
} 


int main(void)
{
    // Bit 0 = GPIOAEN (PA0用)
    // Bit 3 = GPIODEN (PD12用)
    REG_RCC_AHB1ENR |= (1 << 0) | (1 << 3);

    // 【PA0】入力モード [1:0] = 00
    REG_GPIOA_MODER &= ~(3 << (0 * 2)); // 0ビット目と1ビット目を「00」にクリア

    // 【PD12】汎用出力モード [25:24] = 01
    REG_GPIOD_MODER &= ~(3 << (12 * 2)); // 12番ピンのビットを一度クリア
    REG_GPIOD_MODER |=  (1 << (12 * 2)); // 12番ピンに「01」を書き込み

    unit8_t previous_state = 0;

    while (1)
    {
        // 現在のボタンの状態を読み取る（0 または 1）
        uint8_t current_state = (REG_GPIOA_IDR & (1 << 0)) ? 1 : 0;

        // 【条件】「離れていて(0)」、今回は「押されている(1)」＝ 押した瞬間
        if (previous_state == 0 && current_state == 1)
        {
            // PD12（緑LED）の状態を反転させる（XOR演算）
            REG_GPIOD_ODR ^= (1 << 12);

            // チャタリング防止：接点が安定するまで少し待つ
            simple_delay(200000); 
            
            // 待機後、もう一度ボタン状態を読み直してノイズによる誤動作を防ぐ
            current_state = (REG_GPIOA_IDR & (1 << 0)) ? 1 : 0;
        }

        // 次のループのために、今の状態を「前回の状態」として記憶
        previous_state = current_state;
    }
}

#include "stm32f4xx.h"

void uarrt2_init(void){
	RCC->AHB1ENR |= (1 << 0);	//GPIOAのクロック供給
	RCC->APB1ENR |= (1 << 17);	//USART2のクロック供給

	GPIOA->MODER &= ~(3 << 4);	//ビット5:4を一旦00にクリアする
	GPIOA->MODER |= (2 << 4);	//ビット4を10にして兼用モードにする

	GPIOA->AFR[0] &= ~(0xF << 8);	//ビット11:8を一旦00にクリアする
	GPIOA->AFR[0] |= (7 << 8);	//AF7を書き込む

	USART2->BRR = 0x683;

	USART2->CR1 |= (1 << 3);
	USART2->CR1 |= (1 << 13);
}

void uart_send(char c){
	while ((USART2->SR & (1 << 7)) == 0);

	USAER2->DR = c;
}

