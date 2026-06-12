マイコンの割込み動作でLEDをトグルする

使用するボタンは回路図からPC13を使用する(プルダウン抵抗がある)
LEDのPA5をボタンを押すたびに反転

EXTI13の割込み機能を使用する
割込み機能を使用する際にクロックを入れる
SYSCFG->EXTICR[3]

クロック
GPIOA：ビット0　GPIOC：ビット2　SYSCFG：ビット14

void Button_Interput_Init(void){
	RCC->AHB1ENR |= (0x1 << 0);	//GPIOAクロックを入れる(0番ビット)
	RCC->AHB1ENR |= (0x1 << 2);	//GPIOCクロックを入れる(2番ビット)
	RCC->APB2ENR |= (0x1 << 14);	//SYSCFGクロックを入れる(14番ビット)

//PC13を入力モードにする
1ピン当たり2ビットなので13*2=26番ビット

	GPIOC->MODER &= ~(0x3 << (13 * 2));	//0x3(11)を入れて26,27番ビット目を清掃
	GPIOC->MODER |= (0x0 << (13 * 2));	//0x0(00)入力モード

割り込み交差点を切り替える
4ビットずつ分かれている
	PC13をEXTI13に接続EXTICR[3]
13番ラインの部屋がレジスタのどこにあるか13(mod4))=1あまりが1
1*4 4番ビット　

//割込み交差点をPC13にする
	SYSCFG->EXTICR[3] &= ~(0xF << 4);	//4～7番ビットを清掃
	SYSCFG->EXTICR[3] |= (0x2 << 4);	//0x2を書いてPORTCを割り当てる

	EXTI->FTSR |= (0x1 << 13);	//立ち下がりエッジ検知
	EXTI->IMR |= (0x1 << 13);	//割込みマスク解除

//NVICでCPUへ通知ロックを解除
	NVIC_EnabeleIRQ(EXTI15_10_IRQn);

}

