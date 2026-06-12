マイコンを使用したUART操作
やりたいこと　PA2(UART2_TX)を使ってSTM32F4マイコンから文字を送信する

①ピンを特定する
データシートからPA2の情報を見る(Alternate function mapping)
PA2をUSART2_TXとして使用する場合は機能としてAF7を選択する
機能AF7はレジスタに書き込む値は7と特定⇒2進数で0111を書く

②GPIOAにクロックを供給する
リファレンスマニュアルRM0090を開いてRCCレジスタマップを見る
メモリマップからRCC_AHB1ENRのBusがAHB1であることを特定
レジスタマップからGPIOAENはビット0を特定
オフセット値は0x30であることを特定

③USART2にクロックを供給する
リファレンスマニュアルRM0090を開いてRCCレジスタマップを見る
メモリマップからRCC_APB1ENRのBusがAPB1であることを特定
レジスタマップからUSART2ENはビット17を特定
オフセット値は0x40であることを特定

④ベースアドレスを調べる
リファレンスマニュアルRM0090を開いてメモリマップを見る
RCCのベースアドレス　：0x40023800
GPIOAのベースアドレス：0x40020000
USART2のベースアドレス：0x40004400

⑤GPIOのオフセット値を調べる
リファレンスマニュアルRM0090を開いてGPIOレジスタマップを見る
MODERは2ピンなので[x:y]表記　
ex)MODER2→ピン番号×2=4　y=4←開始ピン番号
　		MODER[5:4]
AFRL：0x20 MODER：0x00
AFRLは4ピンなのでAFRL→ピン番号×4　y=8
AF7なのでGPIOA_AFRLはAFRL[11:8]

⑥USART2オフセット値を調べる
リファレンスマニュアルRM0090を開いてUSARTレジスタマップを見る
USART_DR：0x04
USART_BRR：0x08
USART_CR1：0x0C

TIPS
BRRボーレートBRRの計算方法
バスクロック16MHz
ボーレート9600bps
計算 16000000/16*9600=104.166..
整数部104(0x68)小数部0.166×16=2.66(0x3)⇒0x683
×16の意味：16進数用の数字に変換するため

#define RCC_AHB1ENR  (*(volatile uint32_t *)0x40023830)
#define RCC_APB1ENR  (*(volatile uint32_t *)0x40023840)
#define GPIOA_MODER  (*(volatile uint32_t *)0x40020000)
#define GPIOA_AFRL   (*(volatile uint32_t *)0x40020020)
#define USART2_BRR   (*(volatile uint32_t *)0x40004408)
#define USART2_CR1   (*(volatile uint32_t *)0x4000440C)
#define USART2_DR    (*(volatile uint32_t *)0x40004404)

#define USART2_SR    (*(volatile uint32_t *)0x40004400)

void uart2_init(void) {
    RCC_AHB1ENR |= (1 << 0);	// 1. GPIOAのクロック供給
    RCC_APB1ENR |= (1 << 17);	// 2. USART2のクロック供給
    
    				// 3. PA2をAFモードに設定（MODER）
    GPIOA_MODER	&= ~(3 << 4);	//ビット5:4を一旦00にクリア　3は0b11なので2ビット分まとめてクリア
    GPIOA_MODER	|= (2 << 4);	//ビット4を10にして兼用モードに設定する

    				// 4. PA2のAF番号をAF7に設定（AFRL）
　　GPIOA_AFRL	&= ~(0xF << 8);	//ビット11:8を一旦00にクリア　AFRLは1ピン当たり4ビットなのでクリアに使うマスクは0b1111なので4ビット分まとめてクリア
　　GPIOA_AFRL  |= ( 7 << 8);

    USART2_BRR = 0x683;		// 5. ボーレート設定（BRR）

    USART2_CR1 |= (1 << 3);	// 6. 送信有効化（CR1のTEビット）
    USART2_CR1 |= (1 << 13);	// 7. USART有効化（CR1のUEビット）
}
void uart_send(char c) {
	while ((USART2_SR & (1 << 7)) == 0);	//ビット7が1になるまでループ
    	USART2_DR = c;		// 8. DRレジスタに文字を書き込む
}
