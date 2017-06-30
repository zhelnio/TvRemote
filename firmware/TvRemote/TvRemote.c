/*
 * TvRemote.c
 *
 * ��� Sony � ����� ������� (��������� �����)
 * ������� "just for fun"
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

////// ������� �������� ���������
// ����������������� ����������� ������� (���)
#define CARRIER_HALF_PERIOD 12

// ����������������� (���) ��������, ����� 0 � 1, ����� ����� ������� ����, ��������� (��)
#define IR_SONY_LEN_PREFIX	2400
#define IR_SONY_LEN0		600
#define IR_SONY_LEN1		1200
#define IR_SONY_PAUSE		600
#define IR_SONY_LEN_POSTFIX	26
//������ ������� ������� � Sony =45 ��, �� �.�. � ���� ������� ����,
//�� �� ������������ ���������� ����������������� ��������� � �����

#define IR_SONY_PREFIX	sendImpulse(IR_SONY_LEN_PREFIX);
#define IR_SONY_BIT0	sendImpulse(IR_SONY_LEN0);
#define IR_SONY_BIT1	sendImpulse(IR_SONY_LEN1);	
#define IR_SONY_POSTFIX	_delay_ms(IR_SONY_LEN_POSTFIX);

//��������� ������� ������� �������
//��� ���� (0/1/prefix) ������������ ������������������ ��������
void sendImpulse(const uint16_t usecnds)
{
	//�������� ������� �� ����� usecnds (���)
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

//��������� ������� ������� (�������)
void sendSignal()
{
	for(uint8_t i = 0; i < 8; i++)
	{
		//��-�������� ��� ���� ������ ���� �� ������� �����
		//�� ��� ����, �.�. ������� ����� ���� � ��� ��� �������� � ��������
		IR_SONY_PREFIX
		
		//7 ��� �������
		IR_SONY_BIT0
		IR_SONY_BIT0
		IR_SONY_BIT0
		IR_SONY_BIT0
		IR_SONY_BIT1
		IR_SONY_BIT0
		IR_SONY_BIT0
		
		//5 ��� ������
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
	//PB4 - ���� � ������ BTN
	//PB3 - ����� �� �� ���� IR
	DDRB &= ~(1 << DDB4);
	PORTB |= (1 << DDB4);
	DDRB |= (1 << DDB3);
	
	//��������� ���������� �� ����� ���������� �� BTN
	GIMSK |= (1<<PCIE);
	PCMSK |= (1<<PCINT4);
	
	//��������� ���������� ���������� (����������������)
	ACSR |= (1 << ACD);
	
    //���������� ��������� ��� �� ��������
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    
    sei();
    
    while(1)
		sleep_cpu();
}

//��������� ������ �� ������ ���������� ���������
volatile uint8_t portbhistory = 0xFF;

//���������� �� ����� ���������� �� �����
ISR(PCINT0_vect)
{
	uint8_t changedbits = PINB ^ portbhistory;
	portbhistory = PINB;

	//��������� ��������� ������
	if(changedbits & (1 << PINB4))
	{
		//������ ������ ��� ������
		if(bit_is_clear(PINB, PINB4))
		{
			//���� ���� ������� �������
			_delay_ms(100);
		}
		
		//������ ������ ��� ��������
		else
		{
			//���� ���� ������� �������
			_delay_ms(100);
			
			//�������� ������
			sendSignal();
		}
	}
}

