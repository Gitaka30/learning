ADC(アナログデジタルコンバータ)入力端子をマイコンで操作する

やりたいこと：電圧変化値をマイコンに読み取らせる

①リファレンスマニュアルからGPIO port mode registerを検索する
　アナログモードの条件を確認する　⇒11(2進数)を書き込む
　MODER[1:0]に11を入れる

②メモリマップのGPIOAからBUSを確認する　⇒AHB1
  RCC_AHB1ENRレジスタのビット0(GPIOAEN)を1にする
　TIPS データシートのpin and ball definionsの表からpin nameの使いたい番号からadditional functionを確認
  
  ADC123⇒ADC1,2,3のどのピンでも使用可　IN0⇒アナログ入力の0番目のチャンネル
　リファレンスマニュアルからADC1ENのビット番号を特定する　⇒ACD1RN
  RCC_APB2ENRレジスタのビット8(ADC1EN)を1にする

③アドレスを調べる
　レジスタのベースアドレスを調べる
  RCCのベースアドレス：0x40023800
  GPIOAのベースアドレス：0x40020000
  ADC1のベースアドレス：0x40012000

#include "stm32f4xx.h"

void abc1_init(void){
	RCC->AHB1ENR |= (1 << 0);  //GPIOAのクロックON(Bit 0 = GPIOAN)
	RCC->APB2ENR |= (1 << 8);  //GPIOAのクロックON(Bit 8 = ADC1EN)

	GPIOA->MODER |= (3 << (0 * 2)); //①PA0をアナログモードに設定する
					//GPIOAのMODERのレジスタビット1:0に3(11)を書き込む
	ADC1->CR2 |= (1 << 0);	 //Bit0 = ADON(ADC ON)
	}
uint16_t adc1_read(void){
	ADC1->CR2 |= (1 << 30);  //Bit 30 = SoftWare START
	
	//変換が終わるまで待つ
	//bit1=EOC
	while ((ADC1->SR & (1 << 1)) ==0);  
	
	//データレジスタからデジタル変換された値を読みだして返す
	return (uint16_t)ADC1->DR;
	}

int main(void){
	adc1_init();
	uint16_t analog_value = 0;
	while(1){
	analog_value = adc1_read();	//PA0の電圧変化を読み取る
	}
}

TIPS
CR1とCR2の違い
CR1:複雑な特殊設定が入る
CR2:基本的な運転スイッチが入る
