ボタン操作でSTM32マイコンを割り込み操作でLEDを点灯消灯をする

やりたいこと：ボタン操作で割り込みをしてLEDを操作する

①回路図を確認して使用したいボタンとLEDのピンを確認する
LED→PA5　ボタン→PC13　⇒　GPIOA、GPIOCのbusが確定する

②リファレンスマニュアルRM0390からレジスタの値を見る
GPIOA、GPIOCはともにAHB1
GPIOA:0x40020000   GPIOC:0x40020800

③AHB1の周辺レジスタの供給条件を見る
RCC_AHB1 GPIOA_ENR 0ビット目　GPIOC_ENR 3ビット目

GPIOA　0:ON 1:OFF  GPIOC　0:ON 1:OFF

⇒0ビット目と3ビット目を1にすれば両方にクロック供給できる

④GPIOの端子のMODERを確認する
今回のピン番号はLEDの5番,ボタンの13番
⇒[11:10],[27:26]ビットをみる

⑤割り込みをするピンを設定するためSYSCFG_EXTICRレジスタを見る
TIPS SYSCFG System configration controller
⇒割り込みをさせたいのはボタン
⇒ボタン番号は13ピン
⇒SYSCFG_EXTICR4が特定される

レジスタ内のどのビットを見るかを探す
13番ピン用はEXTI13なので[7:4]ビットを使用している

⑥外部割込みの設定をするためEXTI_IMRレジスタを見る
回路を見るとプルアップがされているのでボタンが押されていないときは1、押されていると0になるのでエッジ検出をする

TIPS：立ち上がりエッジ0→1 立ち下がりエッジ1→0になるとき

EXTI_INR　レジスタの13ビット目を1にする
EXTI_FTSR　レジスタの13ビット目を1にする

#include "stm32f446xx.h"

void init_gpio(void){
void EXTI15_10_IRQHandler(void);

int main(void) {
	init_gpio();
	while (1) {
	}
	return 0;
}

	//stm32f446xx.h などのヘッダーファイルで定義されているマクロ（アドレス）をセット
    RCC_TypeDef     *pRCC    = RCC;	//指している部屋： クロック制御（RCC）のレジスタ一式
    GPIO_TypeDef    *pGPIOA  = GPIOA;	//指している部屋： ポートA（GPIOA）のレジスタ一式
    GPIO_TypeDef    *pGPIOC  = GPIOC;	//指している部屋： ポートC（GPIOC）のレジスタ一式
    SYSCFG_TypeDef  *pSYSCFG = SYSCFG;	//指している部屋： システム構成制御（SYSCFG）のレジスタ一式
    EXTI_TypeDef    *pEXTI   = EXTI;	//指している部屋： 外部割り込み（EXTI）のレジスタ一式

　　pRCC->AHB1ENR |= (1 << 0);	// GPIOAにクロック供給
    pRCC->AHB1ENR |= (1 << 2);	// GPIOCにクロック供給

    //LED(PA5)[11:10]を一旦00にクリアして01を入れる
    pGPIOA->MODER &= ~(0x3 << 10); // 10,11ビット目をクリア
    pGPIOA->MODER |=  (0x1 << 10); // 10ビット目に 1 をセット

    // ボタン (PC13): [27:26]ビットを 00 (入力) にする。クリアするだけでOK
    pGPIOC->MODER &= ~(0x3 << 26); // 26,27ビット目をクリア

　　// EXTICR4レジスタ（配列では[3]番目）の [7:4]ビットを一度クリアして、0x2 (GPIOC) を書き込む
    pSYSCFG->EXTICR[3] &= ~(0xF << 4); // 7:4ビット目をクリア
    pSYSCFG->EXTICR[3] |=  (0x2 << 4); // 7:4ビット目に 0010 (0x2) をセット

　　// ⑥で判断した通り、13番ラインを許可し、立ち下がりエッジ(1→0)をONにする
    pEXTI->IMR  |= (1 << 13); // IMRレジスタの13ビット目を1（割り込み許可）
    pEXTI->FTSR |= (1 << 13); // FTSRレジスタの13ビット目を1（立ち下がり検出ON）

　　NVIC_EnableIRQ(EXTI15_10_IRQn);
}
