/*
 * TvRemote.c
 *
 * ПДУ Sony с одной кнопкой (следующий канал)
 * Сделано "just for fun"
 *
 * Created: 23.11.2013 18:48:42
 *  Author: Stanislav Zhelnio
 */ 

#define F_CPU 9600000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>

void sendSignal();							
void sendImpulse(const uint16_t usecnds);	

////// краткое описание протокола
// продолжительность полупериода несущей (мкс)
#define CARRIER_HALF_PERIOD 12

// продолжительность (мкс) префикса, битов 0 и 1, паузы после каждого бита, постфикса (мс)
#define IR_SONY_LEN_PREFIX	2400
#define IR_SONY_LEN0		600
#define IR_SONY_LEN1		1200
#define IR_SONY_PAUSE		600
#define IR_SONY_LEN_POSTFIX	26
//период повтора команды у Sony =45 мс, но т.к. у меня команда одна,
//то на динамическое вычисление продолжительности постфикса я забил

#define IR_SONY_PREFIX	sendImpulse(IR_SONY_LEN_PREFIX);
#define IR_SONY_BIT0	sendImpulse(IR_SONY_LEN0);
#define IR_SONY_BIT1	sendImpulse(IR_SONY_LEN1);	
#define IR_SONY_POSTFIX	_delay_ms(IR_SONY_LEN_POSTFIX);

//отправить импульс кодовой посылки
//тип бита (0/1/prefix) определяется продолжительностью импульса
void sendImpulse(const uint16_t usecnds)
{
	//включить несущую на время usecnds (мкс)
	const uint16_t periods = usecnds / (CARRIER_HALF_PERIOD * 2);
	for(uint16_t i = 0; i < periods; i++)
	{
		PORTB |= (1<<PORTB3);
		_delay_us(CARRIER_HALF_PERIOD);
		PORTB &= ~(1<<PORTB3);
		_delay_us(CARRIER_HALF_PERIOD);
	}
	_delay_us(IR_SONY_PAUSE);
}

//отправить кодовую посылку (команду)
void sendSignal()
{
	for(uint8_t i = 0; i < 8; i++)
	{
		//по-хорошему тут надо делать цикл со сдвигом битов
		//но мне лень, т.к. команда всего одна и все уже отлажено и работает
		IR_SONY_PREFIX
		
		//7 бит команды
		IR_SONY_BIT0
		IR_SONY_BIT0
		IR_SONY_BIT0
		IR_SONY_BIT0
		IR_SONY_BIT1
		IR_SONY_BIT0
		IR_SONY_BIT0
		
		//5 бит адреса
		IR_SONY_BIT1
		IR_SONY_BIT0
		IR_SONY_BIT0
		IR_SONY_BIT0
		IR_SONY_BIT0

		IR_SONY_POSTFIX
	}
}

int main(void)
{
	//PB4 - вход с кнопки BTN
	//PB3 - выход на ИК диод IR
	DDRB &= ~(1 << DDB4);
	PORTB |= (1 << DDB4);
	DDRB |= (1 << DDB3);
	
	//разрешаем прерывания по смене напряжения на BTN
	GIMSK |= (1<<PCIE);
	PCMSK |= (1<<PCINT4);
	
	//отключаем аналоговый компаратор (энергосбережение)
	ACSR |= (1 << ACD);
	
    //выставляем настройки сна на глубокий
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    
    sei();
    
    while(1)
		sleep_cpu();
}

//состояние портов на момент последнего изменения
volatile uint8_t portbhistory = 0xFF;

//прерывание по смене напряжения на входе
ISR(PCINT0_vect)
{
	uint8_t changedbits = PINB ^ portbhistory;
	portbhistory = PINB;

	//изменение состояния кнопки
	if(changedbits & (1 << PINB4))
	{
		//кнопка только что нажата
		if(bit_is_clear(PINB, PINB4))
		{
			//ждем пока пройдет дребезг
			_delay_ms(100);
		}
		
		//кнопка только что отпущена
		else
		{
			//ждем пока пройдет дребезг
			_delay_ms(100);
			
			//посылаем сигнал
			sendSignal();
		}
	}
}

