マイコンから繋がってるLEDを光らせる

例
STM32F4 DiscoveryボードのLD4（緑LED）をGPIOで点灯させるコードを書く


やること
①STM32F4の回路図を見に行く
https://www.st.com/resource/en/schematic_pack/mb997-f407vgt6-b02_schematic.pdf

②当該のLD4はどこに繋がっているか回路図で確認する　
⇒PD12が判明

③当該LEDはアクティブハイかアクティブローか確認する　
⇒LEDのカソード側がGNDに落ちてるのでアクティブハイが特定

④クロック供給レジスタ表を見に行く　
⇒マニュアル内をclock registerで探して表を見つける ⇒レジスタ名；RCC_AHB1ENRが判明
⇒GPIOで今回動かしたいポートはDを有効にしたい
⇒表からGPIODENを探す　
⇒3ビット目が判明
⇒アドレスオフセット：0x30

⇒　RCC_AHB1ENR |= (1 << 3);

⑤マニュアル内でMemory addresの表を探す
⇒RCCのベースアドレスを探す　←　クロック供給レジスタにアクセスするため
⇒ベースアドレス：0x40023800

オフセットは④で判明した0x30
⇒絶対アドレスは
0x40023800 + 0x0030  = 0x40023830 

⑥⑤と同様にGPIOD_MODER,GPIOD_ODRのベースアドレスを探す
オフセットはGPIOD_MODERは0x00、GPIOD_ODRは0x14
　⇑RCCのレジスタマップ一覧表を見ると⑤、⑥のオフセットアドレスは一撃で読める

GPIOレジスタマップでMODERのオフセット0x00を確認
MODERは1ピンあたり2ビットなので　ピン番号×2=開始ビット番号
PD12の設定ビットはMODER[25:24]　→ ピン番号12 × 2 = 24が開始ビット
出力モード = 01 なのでビット24を1にする
GPIOレジスタmapを探してビットを探す⇒[25:24]

#define RCC_AHB1ENR  (*(volatile uint32_t *)0x40023830)
#define GPIOD_MODER  (*(volatile uint32_t *)0x40020C00)
#define GPIOD_ODR    (*(volatile uint32_t *)0x40020C14)

void led_init(void) {
    RCC_AHB1ENR |= (1 << 3);		//GPIODへのクロック供給を有効化	ビット3(GPIODEN)を1にする
	GPIOD_MODER &= ~(3 << 24);		//ビット25:24を一旦00にクリア　3は0b11なので2ビット分まとめてクリア
	GPIOD_MODER |= (1 << 24);		//ビット24を1にして出力モードを01に設定する
	GPIOD_ODR |= (1 << 12);			//PD12をHighにしてLED4点灯
	// クロック供給：GPIODENビット（ビット3）を1にする
    // 入出力方向：PD12を出力モードに設定
}

void led_on(void) {
    GPIOD_ODR |= (1<<12);			// PD12をHighに出力
}


TIPS

RCC_AHB1ENRはSTM32マイコンの電力を供給して眠りから起こすためのメイン電源スイッチレジスタ
Reset and Clock Control クロック管理部内の
AHB1 			AHB1という名前の高速バスの通り道に繋がっている
Periheral		周辺回路たちの
ENABLE REGISTER		機能を有効化するレジスタ

