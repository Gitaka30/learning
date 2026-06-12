例外処理付きシリアルLED調光システム

やりたいこと：LEDを光らせたい
文字を読み取りたい
例外文字をはじきたい

回路図↓
https://www.st.com/resource/en/schematic_pack/mb1136-default-c04_schematic.pdf

回路図からLD2を探す
PA5がLEDに接続されている

メモリマップの表を探してGPIOAのバスはどこか調べる

リファレンスマニュアル(以下RM)からRCC(Reset and Clock control)リセット＆クロック制御を探す
GPIOA_ENR GPIOAのenableを操作して0番ビットを1にする

RCC->AHB1ENR |= ( 1 << 0 );	//これでGPIOAのGPIOAのクロックがONになる

メモリマップから、GPIOAのアドレスは0x4002 0000
オフセット値は0x30
0x4002 0030

PA5よりMODER5

MODER5[11:10]

GPIOA->MODER &= ~( 3 << 10 );	//10番ビットに3(11)を入れて00にする
GPIOA->MODER |= ( 1 << 10);	//10番ビットに1を論理和で入れて01(出力)にする

GPIOAの出力タイプのデータを探す
GPIOA port output data register(ODR)
ベースアドレスは0x4002 0000
オフセット値は0x14
0x4002 0014

GPIOA->ODR |= ( 1 << 5 );

void LED_Init(void){
	RCC->AHB1ENR |= ( 1 << 0 );	//GPIOAのTIM2クロックON
	GPIOA->MODER &= ~( 3 << 10 );	//10番ビットに3(11)を入れて00にする
	GPIOA->MODER |= ( 1 << 10 );	//10番ビットに1を論理和で入れて01(出力)にする
}

int main(void){
	char rx_data;
	
	LED_Init();  //LEDを使えるようにする

	while(1){
		rx_data = UART_ReceiveChar();
	
		switch(rx_data){
		case '1':	//1で点灯
			GPIOA->ODR |= ( 1 << 5 );
			break;

		case '0':	//0で消灯 
			GPIOA->ODR &= ~( 1 << 5);
			break;

		case 't':	//排他的論理和
			GPIOA->ODR ^= ( 1 << 5);
			break;

		default:	//安全装置　上記以外の文字はシャットアウト
			UART_SendString(">> ERROR Invalid Command\n");
			break;
	}
}


