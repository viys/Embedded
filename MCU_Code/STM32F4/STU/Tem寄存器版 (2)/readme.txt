void delay(u32 t)
{
	while(t--);
}
int main()
{

u8 key_val;
	KEY_Init ();
	LED_Init();
	USART_Init(84, 115200, 0);
	u16 temp;
	float Vx;
	ADC1_CH5_Init();
	
	while(1)
	{
		key_val = key_Scan();	
		if(key_val )
		{
		//USART_Send_str("hello world");
		printf("hello world");
			}
		GPIOC->ODR &= ~(1 << 4);
		delay(5000000);
		GPIOC->ODR |= (1 << 4);
		delay(5000000);
		if(flag == 1)
		{
			flag = 0;//标志位清零
			
			if(strcmp(rec,"1#") == 0)
			{
				printf ("指令正确，点亮LED\r\n");
				GPIOC->ODR &= ~(1 << 4);//灯亮
			}
			else if(strcmp(rec, "2#") == 0)
			{
				printf ("指令正确，关闭LED\r\n");
				GPIOC->ODR |= (1 << 4);//灯灭
			}
			else
			{
				printf ("指令错误，请重新输入\r\n");
			}
			memset(rec ,0 ,sizeof (rec ));
		}
		
	temp = ADC_GETD(); 
	Vx = (3.3 * temp)/4096;
	printf ("数字量：%d\n",temp);
	printf ("电压值：%.lf\r\n",Vx);
	delay(5000000);
	}
}
		
	