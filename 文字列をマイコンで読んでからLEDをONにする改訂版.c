文字列をマイコンで読んでからLEDをONにする改訂版

#define USART_CR1_UE    (1 << 13)  // USART Enable
#define USART_CR1_TE    (1 << 3)   // Transmitter Enable
#define USART_CR1_RE    (1 << 2)   // Receiver Enable

#define GPIO_AFR_CLEAR_PA2_PA3	(0xF << 4)　 // PA2とPA3のAF領域をマスクする用
#define GPIO_AF_USART2		(0xA << 4)   // PA2(AF7) と PA3(AF7) を設定する用

char uart2_getchar(void){
	//USART2のSRレジスタの第5ビットを見る
	while (!(USART2->SR & (1 << 5)));
	return (char)(USART2->DR); //届いた1文字を返す
	}

void usart2_init(void){
	RCC->APB1ENR |= ( 1 << 17);

	GPIOA->MODER &= ~GPIO_AFR_CLEAR_PA2_PA3; //PA2,PA3の領域を0000にクリアする
	GPIOA->MODER |= GPIO_AF_USART2;  //PA2,PA3の領域に0xAを書き込む　4は16進数で1010

	GPIOA->AFR[0] &= ~(0xFF << 8); //ARFLのAFの領域をクリアする
	GPIOA->AFR[0] |= (0x77 << 8);  //ARFLのAFの領域に01110111を入れる

	//ボーレート設定(115200bps)　←STM32の内部クロックは16MHzで動くと仮定
	分周比=16000000Hz/115200bps = 8.6805
	0,6805 × 16 =10.888 →　11 (0xB) 
	0x008B	

	USART2->BRR = 0x008B;
	
	//USART2を起動し、UE,TE,REビットをONにする
	USART2->CR1 |= (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE); //13ビット目のUSART2の電源、3ビット目の送信、2ビット目の受信をON
}
//1文字送信関数の定義
void uart2_patcher(char c){
	while(!(USART2->SR & (1 << 7)));　//USART2のSRレジスタの第7ビットを見る
	USART2->DR = c;		//データレジスタに文字を書き込む
}
int my_strcmp(char *str1, char *str2){
	while(*str1 && (*str1 == *str2)){  //両方の文字が一致かつstr1が終端ではない場合の間ループ
		str1++;
		str2++;
	}
	return (*str1 == *str2) ? 1:0;
}

void uart2_putstr(char *s){
	//ポインタsが指す文字が\0になるまでループ
	while(*s != '\0'){
		uart2_patcher(*s);	//1文字ずつ送信
		s++;
	}
}

int main (void){
	usart2_init();
	// 疑似受信バッファ（PCから "led on" と打たれたと仮定）
    char input_buffer[32];
    int index = 0;

    uart2_putstr("Please input command(led on /letoff\n"); 

    while(1) {
        char c = uart2_getchar();  //getcharの役割
	uart2_patcher(c);　　//そのまま送り返す
	//エンターが押されたら文字列の終わりとみなす
        if (c == '\r' || c == '\n') {
	    input_buffer[index] = '\0';
	  
	  if (my_strcmp(input_buffer, "led on") == 1) {
            GPIOA->ODR |= (1 << 5);             // 物理LED点灯
            uart2_putstr(">> LED IS NOW ON!\n"); // PCへの報告
            input_buffer[0] = '\0';             // バッファクリア
        } 
        // もし入力が "led off" だったらLEDを消灯
        else if (my_strcmp(input_buffer, "led off") == 1) {
            GPIOA->ODR &= ~(1 << 5);             // 物理LED消灯
            uart2_putstr(">> LED IS NOW OFF!\n");// PCへの報告
            input_buffer[0] = '\0';
        } 
            else if (index > 0) {
                uart2_putstr("\n>> Unknown command...\n");
      }
	index = 0;
// 普通の文字が打たれている間は、配列にどんどん溜めていく
        else {
            // 配列のサイズ（32文字）を超えて溢れないようにガード（安全対策）
            if (index < 31) {
                input_buffer[index] = c;
                index++;
        }
    }
}
