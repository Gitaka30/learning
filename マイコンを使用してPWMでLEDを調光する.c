マイコンを使用してPWMでLEDを調光する

PWMを動作させるためにTIM2を使用する

TIM2を動作させるためにクロックを入れる
⇒RCC_AHB1ENR　0番ビットに1を入れる

RCC->AHB1ENR |= (1 << 0);	//クロックを入れる
RCC->APB1ENR |= (1 << 0);	//TIM2のクロックを入れる

TIPS
AHB1 Advanced High-performance Bus 1	高速通信・スピード重視 GPIO系
APB1 Advanced Peripheral Bus 1		普通通信・確実性重視   TIM系　UART系

LEDを動作させるためにMODER5を使用する
PA5を代替機能モードにする
MODER5[11:10]

クロック周波数16MHzで動作

クロック周波数16000Hz
目標周波数 1000Hz

RM0368に記載のタイマー公式　PWM周波数=クロック周波数/(PSC+1)(ARR+1)
PSC(プリスケーラ)　スピード　ARR(オートリロードレジスタ)　タイミング

PSC+1=16　ARR+1=1000 ←1～999の値を刻める
⇒PSC=15

TIM2->PSC = 15;		//16MHzを1MHzにスピードを落とす
TIM2->ARR = 999;	//0～999まで1ms周期に設定して1kHzにする
TIM2->CCR1 = 500;	//初期デューティー比50%

PA5がTIM2のチャンネル担当エリアだからOC1M
データシートでAlternate function mapping の表からPA02を探す
⇒TIM2_CH1の機能として使えるのはPA0とPA5⇒PA5がLEDに接続されているので使用

OC1M：OutputCompare　電気出力 1 チャンネル1 M モード設定
TIM2->CCMR1 &= ~(0x7 << 4);	//4～6番ビットをクリア
TIM2->CCMR1 |= (0x6 << 4);	//6(110)を書いてPWMモード設定

PA5を代替機能モードに切り替える
GPIOA->MODER &= ~(0x3 << (5 * 2));	//0x3(11)を入れて、(10,11)ビットを清掃
GPIOA->MODER |= (0x2 << (5 * 2));	//0x2(10)： 代替機能モード

Altenate Function Register表からTIM2_CH1の代替機能に変える必要がある
⇒GPIOA->AFR[0] &= ~(15　<< (5 * 4));	//PA5にAF1(TIM2_CH1)の役割を割り当てる
　GPIOA->AFR[0] |= (1 << (5 * 4));	//AF1を指定する

void PWM_LED_Init(void){
	RCC->AHB1ENR |= (0x1 << 0);	//クロックを入れる
	RCC->APB1ENR |= (0x1 << 0);	//TIM2のクロックを入れる

	GPIOA->MODER &= ~(0x3 << (5 * 2));	//0x3(11)を入れて、2ビット分を清掃
	GPIOA->MODER |= (0x2 << (5 * 2));	//0x2(10)： 代替機能モード

	GPIOA->AFR[0] &= ~(0xF << (5 * 4));	//0xF(1111)を入れて、20～23ビット分清掃
    	GPIOA->AFR[0] |=  (0x1  << (5 * 4));	//0x1(01)：AF1(TIM2_CH1)を書き込む

	TIM2->PSC = 15;		//16MHzを1MHzにスピードを落とす
	TIM2->ARR = 999;	//0～999まで1ms周期に設定して1kHzにする
	TIM2->CCR1 = 500;	//初期デューティー比50%

	TIM2->CCMR1 &= ~(0x7 << 4);	//4～6番ビットをクリア
	TIM2->CCMR1 |= (0x6 << 4);	//6(110)を書いてPWMモード設定
	TIM2->CCER  |= (0x1 << 0);	//AF1を開放

	TIM2->CR1   |= (0x1 << 0);	//カウンター有効
}




