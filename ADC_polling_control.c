#include <stdint.h>

typedef struct{
  volatile unit32_t SR;  //ステータスレジスタ
  volatile uint32_t CR1;   // コントロールレジスタ1
  volatile uint32_t CR2;   // コントロールレジスタ2
  volatile uint32_t SMPR1; // サンプリング時間レジスタ1
  volatile uint32_t SMPR2; // サンプリング時間レジスタ2
  volatile uint32_t SQR1;  // レギュラーシーケンスレジスタ1
  volatile uint32_t SQR3;  // レギュラーシーケンスレジスタ3
  volatile uint32_t DR;    // データレジスタ
} ADC_TypeDef;

typedef struct {
  volatile uint32_t MODER;   // モードレジスタ
  volatile uint32_t OTYPER;  // 出力タイプ
  volatile uint32_t OSPEEDR; // 出力速度
  volatile uint32_t PUPDR;   // プルアップ/プルダウン
} GPIO_TypeDef;

typedef struct {
  volatile uint32_t AHB1ENR; // AHB1周辺クロック有効化
  volatile uint32_t APB2ENR; // APB2周辺クロック有効化
} RCC_TypeDef;

//ベースアドレスの定義(ダミーアドレス)
#define RCC   ((RCC_TypeDef *) 0x40023800)
#define GPIOA ((GPIO_TypeDef *) 0x40020000)
#define ADC1  ((ADC_TypeDef *) 0x40012000)

//周辺クロックの供給
int main(VOid){
  RCC->AHB1ENR |= (1 << 0);  //GPIOAのクロック有効化
  RCC->APB2ENR |= (1 << 8);  //ADC1のクロック有効化

//対象のピンPA1をアナログモードに設定
  GPIOA->MODER &= ~(0x3 << (1 * 2));  //2,3番ビットを0にクリア
  GPIOA->MODER |= (0x3 << (1 * 2));   //アナログモード11を設定

//測定対象のチャンネルを選択
  ADC1->SQR3 &= ~(0x1F);  //SQ1の5ビットを0にクリア
  ADC1->SQR3 |= 1;        //チャンネル1(PA1)を1番目の測定対象に指定する

//ADCの有効化
ADC1->CR2 |= (1 << 2);
  while (1){
    //ソフトウェアによる変換(SWSTART)
    ADC1-> (1 << 30);  //SWSTARTを立てて変換開始

    //ポーリング制御  
  while ( !(ADC1->SR & (1 << 1))){
  }
  //データの読み出しとフラグクリア
  unit16_t adc_data = ADC->DR;  //データレジスタから値を読み出す
  }
}



