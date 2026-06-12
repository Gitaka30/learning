NUCLEO-F401REマイコンを用いたLEDボタン制御
ボタンを操作する
ボタンが押されている間はLEDが付く　離すと消える

リファレンスマニュアルからGPIOCのクロックを動かす表を探す
クロック表　RCC->AHB1ENR
⇒AHB1ENRの2番ビット

GPIOC->MODERの入力モード00を使う
MODER13[27:26]

PC13をボタンとして使用する
GPIOは必ずAHB1のバスを使う

PA5をLEDとして使用する
GPIOAの0番ビットに1をいれてクロックを入れる

void button_Init(void){
	RCC->AHB1ENR |= (1 << 2);	//GPIOCEN 2番ビットに1を入れてクロックを入れる
	GPIOC->MODER &= ~(3 << (13 * 2));	//26番ビットに3(11)を入れてクリアし、入力モードにする
}

void LED_Init(void){
	RCC->AHB1ENR |= (1 << 0);
						//PS5を出力モードにする
	GPIOA->MODER &= ~(3 << (5 * 2));	//5番ビットに3(11)を入れてクリア
	GPIOA->MODER |= (1 << (5 * 2));		//01を書き込む
}
int main(void){
	button_Init();
	LED_Init();

	int prev_state = 1;	//1つ前のボタン状態を記録する

	while(1){

	int current_state;	//今のボタン状態を読み込み

	if((GPIOC->IDR & (1 << 13)) == 0){
			current_state = 0;	//押されている
	}else{
			current_state = 1;	//離れている
	}
	if(current_state == 0 && prev_state == 1){
		GPIOA->ODR ^= (1 << 5);
	}
	prev_state = current_state;
   }
}
