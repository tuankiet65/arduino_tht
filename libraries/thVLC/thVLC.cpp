// ************************************************** //
//		Library for sensing light transmit value
//		Version 8.1 - 140609  
//		Use 8 sensors
//    Hiện tượng gặp phải: Khi 2 led cùng sáng thì độ sáng của led giảm,
//    nếu cathode của 2 led nối với 2 pin khác nhau thì không sao.
//    Truyền nhiều byte liên tục sử dụng rxReadyFlag
//    Khi sử dụng, tên các kênh trùng với tên các pin Arduino
//    Sửa lại một số tên hàm và thêm thư viện thAvr
//    Bỏ kết nối trở 1K và tối ưu một số lệnh điều khiển
//    Mắc lại mạch điện thành: SensorPin - R - AnodeLed -|>|- KathodeLed - ctrlPin
//    Đọc ADC và sử dụng trực tiếp kết quả đo ADC để set trạng thái
//    140609 - 14h16 - Chỉnh sửa hàm sensorRead(byte pin) để đọc được cảm biến ở kênh 10 - hoàn thiện
// ************************************************** //

#include "thVLC.h"
#include <thVLC_digitalWriteFast.h>
#include <thAVR.h>


thVLC_ thVLC;

#define NUMOFSENSOR	8		// define number of sensor used
#define CHANNEL_0	2				//	Arduino digital pin 2

#define USEC_PERTICK     1250    // microseconds per clock interrupt tick

// ***** define times for signals in millisecond  ************ //
#define TIME_START_LOW     16
#define TIME_START_HIGH    82

#define TIME_BIT1_LOW      44    
#define TIME_BIT1_HIGH     16    // State HIGH - led on   

#define TIME_BIT0_LOW      16     
#define TIME_BIT0_HIGH     44    // State HIGH - led on 

// ******************************************************//
  
#define GAP_TICKS          8		 // 8 * 10ms = 80ms - entry state stop receiving if led off over 80ms 
#define START_SIGNAL       6
#define NUMOFBITS_RECV     8

#define SKIP_START_TIME      5  // in millisecond
#define SENSOR_HISTORY_SIZE	 2	 //

// ******** define states for recording pulse ********* //
#define STATE_IDLE     2
#define CHECK_START    3
#define STATE_MARK     4
#define STATE_SPACE    5
// *********************************************************//


#define BLOCK_ID_EEPROM_ADDR  500   // EEPROM Address of Block ID


const uint8_t _ctrl_R2   = A3;			// pin Arduino control 10K Ohm register for sensing light
const uint8_t _sensorPin = A2;			// Analog pin read sensing light

typedef struct
{
  uint8_t  ctrlPin;
  
  uint8_t  txLedState;  				// used to save state leds
  uint8_t  txValueBuffer;       // bộ đệm lưu các giá trị để truyền nhiều kênh đồng thời
  
  uint8_t  rxTimeStateLow;			// record time of state low - led off
  uint8_t  rxTimer;					   	// time pulses
  uint8_t  rxState;					  	// 
  uint8_t  rxBitCounter;		  	// count the number of bits received
  uint8_t  rxDecodeResult;
  uint8_t  rxResult;
  uint8_t  rxReadyFlag;
  
  uint8_t  sensorState;				 
  uint16_t sensorHistory[SENSOR_HISTORY_SIZE];
} CHANNEL_STRUCT;

CHANNEL_STRUCT _channels[NUMOFSENSOR];
CHANNEL_STRUCT* _rxChannel;
CHANNEL_STRUCT* _txChannel;

#define SENSOR_THRESHOLD  15 
#define DEFAULT_SENSOR_VALUE  (32768) 

uint8_t  _sensorHistoryIdx; 
uint32_t txLastTime;

// ************* define pins connected ********************* //
void thVLC_::begin()
{	
  for (int8_t i = NUMOFSENSOR - 1; i >= 0 ; i--)
  {
    CHANNEL_STRUCT* channel = &_channels[i];
    channel->ctrlPin = CHANNEL_0 + i;    
    //pinModeFast(i + CHANNEL_0, OUTPUT);
    channel->rxState = STATE_IDLE;
    for (int8_t idx = SENSOR_HISTORY_SIZE - 1; idx >= 0; idx--)
    {
      channel->sensorHistory[idx] = DEFAULT_SENSOR_VALUE;
      channel->txLedState = HIGH;  // txLedState = HIGH then the led off
    }
  }
  PORTD |= 0xfc;
  PORTB |= 0x03;
  DDRD  |= 0xfc; // set bits 2,3,4,5,6,7 of DDRD - set D2-D7 as OUTPUT
  DDRB  |= 0x03; // set bits 0,1 of DDRB - set PB0 and PB1 as OUTPUT
  
  //digitalWriteFast(_sensorPin, HIGH);  // unique set pin sensor to LOW
  //pinModeFast(_sensorPin, OUTPUT);     // set rxLedMode as default 
  PORTC |= _BV(PORTC2);
  DDRC  |= _BV(DDC2);
  
  //digitalWriteFast(_ctrl_R2, LOW);     // unique set pin control 10k ohm resistor to LOW
  //pinModeFast(_ctrl_R2, INPUT);	  	   // always connect resistor 10K ohm 
  PORTC &= ~_BV(PORTC3); 
  DDRC |= _BV(DDC3);

  avrADCClockDivConfig(ADC_CLK_DIV_4);
  avrTimer1ConfigNormal(AVR_TIMER1_TOP(USEC_PERTICK));
  avrTimer1InteruptEnable();
  avrPullUpDisable();
  _rxChannel = &_channels[0];
  
  ADMUX = 0x42;    // 0100 0010 - AVCC with external capacitor at AREF pin & analog input channel 2
}


byte thVLC_::getID()
{
  return EEPROM.read(BLOCK_ID_EEPROM_ADDR);
}


// ******************************************************** // 
char thVLC_::receiveReady(byte pin)   // channel - channeless of receiver (channel = 0 or 1)
{
  byte channel = pin - CHANNEL_0;
  if (channel > (NUMOFSENSOR - 1))
  {
    return 0;
  }
  CHANNEL_STRUCT* addr = &_channels[channel];
  return addr->rxReadyFlag;
}

byte thVLC_::receiveResult(byte pin)
{
  byte channel = pin - CHANNEL_0;
  if (channel > (NUMOFSENSOR - 1))
  {
    return 0;
  }
  CHANNEL_STRUCT* addr = &_channels[channel]; 
  addr->rxReadyFlag = 0;  // reset ready flag
  return (addr->rxResult);
}
// ******************************************************* //

void rxReadMode() // configure the sensor channels 
{	
  /*
  //sets the sensor pin (A2 -PIN C2) as input 
  //pinModeFast(_sensorPin, INPUT);
  DDRC &= (~_BV(DDC2));
    
  //connect resistor 10K ohm 
  //pinModeFast(_ctrl_R2, OUTPUT); // _ctrl_R2 - connect to A3
  DDRC |= _BV(DDC3);
  */
  DDRC = ((DDRC | _BV(DDC3)) & (~_BV(DDC2)));  
  
  DDRD &= 0x03; // clear bits 2,3,4,5,6,7 of DDRD - set D2-D7 as INPUT 
  DDRB &= 0xfc; // clear bits 0,1 of DDRB - set PB0 and PB1 as INPUT
  // sets the pin "ctrlPin" as output to read the sensing light on the "channel" 
  digitalWriteFast(_rxChannel->ctrlPin, HIGH);
  pinModeFast(_rxChannel->ctrlPin, OUTPUT);	 
}

void rxLedMode()
{ 		
  // recover last state's leds
  digitalWriteFast(_rxChannel->ctrlPin,_rxChannel->txLedState);
  DDRD |= 0xfc; // set bits 2,3,4,5,6,7 of DDRD - set D2-D7 as OUTPUT
  DDRB |= 0x03; // set bits 0,1,2 of DDRB - set PB0 and PB1 as OUTPUT
  // disconnect 10Kohm resistor to save power
  //pinModeFast(_ctrl_R2, INPUT);	
  DDRC &= ~(_BV(DDC3));
  // sets the common led pin as output
  //pinModeFast(_sensorPin, OUTPUT);
  DDRC |= _BV(DDC2);
}
// **************************************************** //

// **************************************************** // 
uint16_t rxSensorRead()   
{ 
  rxReadMode();	// entry read mode, configure the sensor channels to read  
  ADCSRA |= _BV(ADSC);          // start conversion
  rxLedMode();
  // Thoi gian thuc hien ham rxLedMode() du de ADC chuyen doi xong
  // while (ADCSRA & (_BV(ADSC)));  // wait for conversion complete  
  uint16_t ADC_value = ADC;
  return ADC_value;
}
// ******************************************************** // 

int thVLC_::sensorRead(byte pin)
{ 
  uint16_t readValue;
  byte channel = pin - CHANNEL_0;
  if (channel >= NUMOFSENSOR + 1)  // channel = NUMOFSENSOR + 1 = 8 --> pin = 10 -- pin 10 connect to a light sensor 
  {   
    return 0;
  }
  
  if (pin != 10)
  {
    CHANNEL_STRUCT* _readChannel = &_channels[channel]; 
    readValue = _readChannel->sensorHistory[_sensorHistoryIdx];
  }
  else
  {
    avrTimer1InteruptDisable();
    
    DDRC = ((DDRC | _BV(DDC3)) & (~_BV(DDC2)));      
    digitalWriteFast(10, HIGH);  // write HIGH to Arduino pin 10
    DDRD &= 0x03;                // clear bits 2,3,4,5,6,7 of DDRD - set D2-D7 as INPUT 
    DDRB = (DDRB & 0xfc) | 0x04; // clear bits 0,1 of DDRB - set PB0 and PB1 as INPUT ; set PB2 as output
    
    ADCSRA |= _BV(ADSC);
    while (ADCSRA & (_BV(ADSC)));  // wait for conversion complete  
    readValue = ADC;
    
    DDRB &= 0xfb;     // clear bit 2 of DDRB - set PB2 (Arduino digital pin 10) as INPUT
    avrTimer1InteruptEnable();  
  }
  return readValue;
}


// ************--> Detect changes in light <--*****************//   
void rxDecodeSensor() 
{  
  // *** check sensor on ctrlPin[channel] *** //
  uint16_t maxValue;
  uint16_t minValue;
  if (_rxChannel->sensorHistory[0] > _rxChannel->sensorHistory[1])
  {
     maxValue = _rxChannel->sensorHistory[0];
     minValue = _rxChannel->sensorHistory[1];
  }
  else
  {
    maxValue = _rxChannel->sensorHistory[1];
    minValue = _rxChannel->sensorHistory[0];
  }
  
  uint16_t buf = rxSensorRead();
  _rxChannel->sensorHistory[_sensorHistoryIdx] = buf; 
  
  if (buf >= minValue + SENSOR_THRESHOLD)
  {
    _rxChannel->sensorState = 1;
  }      
  if (buf <= maxValue - SENSOR_THRESHOLD)
  {
    _rxChannel->sensorState = 0;
  }  
}


// **************** in MARK state *********************** //
void rxCheckStartAndDecode()
{   
  if (_rxChannel->rxTimer < START_SIGNAL)     // if it is not start pulse     
  {                     			      // get bits
    _rxChannel->rxDecodeResult = _rxChannel->rxDecodeResult << 1;
    if (_rxChannel->rxTimeStateLow > _rxChannel->rxTimer )
    {          
      _rxChannel->rxDecodeResult |= 0x01;
    }  
    _rxChannel->rxBitCounter++;
  }
  else                    	     	 // if it is start pulse
  {
    _rxChannel->rxDecodeResult = 0;    // reset decode result
    _rxChannel->rxBitCounter = 0;
  }    
}


// *************** Check start pulse in CHECK_START state ****************************/
void rxCheckStartSignal()
{
  if (_rxChannel->rxTimer < START_SIGNAL)  // Start signal false -> entry IDLE 
  {  
    _rxChannel->rxState = STATE_IDLE;
  }
  else		// if it is start signal
  {
    _rxChannel->rxState = STATE_SPACE;
  }
}

// *************** record and decode receive pulse *************************** // 
void rxRecordPulse()
{
  _rxChannel->rxTimer++;
  switch(_rxChannel->rxState) {
  case STATE_IDLE:
    if (_rxChannel->sensorState == 1) 
    {
      _rxChannel->rxDecodeResult = 0;   
      _rxChannel->rxBitCounter	= 0;  
      _rxChannel->rxTimer = 0;
      _rxChannel->rxState = CHECK_START; 
    }
    break;
    
  case CHECK_START:
    if (_rxChannel->sensorState == 0)      // state HIGH end, check start signal
    {
      rxCheckStartSignal();                // check start signal, if it is start signal then recode value, else come back IDLE state
      _rxChannel->rxTimer = 0;
    }   
    break;
    
  case STATE_MARK:                         // timing MARK - state HIGH - led on     
    if (_rxChannel->sensorState == 0)      // state HIGH ended, record time   
    {                
      rxCheckStartAndDecode();             // get bits if it's not start signal else come back STATE_IDLE
      _rxChannel->rxState = STATE_SPACE;
      _rxChannel->rxTimer = 0;             // reset rxTimer	  
      if(_rxChannel->rxBitCounter >= NUMOFBITS_RECV) 
      {	
        _rxChannel->rxResult = _rxChannel->rxDecodeResult;	// update new result
        _rxChannel->rxReadyFlag = 1;			// set ready flag
        _rxChannel->rxDecodeResult = 0;		// reset buffer decode result 
        _rxChannel->rxBitCounter = 0;			// reset rxBitCounter
      }
    } 
    break;     
    
  case STATE_SPACE:                     	// timing SPACE - state LOW - led off
    if (_rxChannel->sensorState == 1)     // state LOW just ended, record it
    {   
      _rxChannel->rxTimeStateLow = _rxChannel->rxTimer;
      _rxChannel->rxState = STATE_MARK;
      _rxChannel->rxTimer = 0;
    } 
    else if (_rxChannel->rxTimer > GAP_TICKS)
    {       // come back STATE_IDLE if led is turned off over GAP_TICKS time   
        _rxChannel->rxState = STATE_IDLE;
    } 
    break;
  }  
}



// **************** control transfer led************************** //
void txLedWrite(byte value)	
{     
  byte valueBuff = value ^ 0x01;
  _txChannel->txLedState = valueBuff;
  digitalWriteFast(_txChannel->ctrlPin, valueBuff);
}

void thVLC_::ledWrite(byte pin, byte value)	
{       
  byte channel = pin - CHANNEL_0;
  if (channel > (NUMOFSENSOR - 1))
  {   
    return;
  }
  _txChannel = &_channels[channel]; 
  txLedWrite(value);
}

void txSendStart()
{
  txLedWrite(HIGH);        // Led on
  delay(TIME_START_HIGH);
}

void txSendBit1()
{
  txLedWrite(LOW);         // Led off
  delay(TIME_BIT1_LOW);        
  txLedWrite(HIGH);        // Led on
  delay(TIME_BIT1_HIGH);         
}

void txSendBit0()
{
  txLedWrite(LOW);         // Led off
  delay(TIME_BIT0_LOW);
  txLedWrite(HIGH);        // Led on
  delay(TIME_BIT0_HIGH);
}

void txSendStop()
{
  txLedWrite(LOW);         // Led off
}


// *********************************************** //
uint8_t txMultiChannels, txLastChannels;

void thVLC_::txSetByte(byte pin, byte value)
{
  byte channel = pin - CHANNEL_0;
  if (channel > (NUMOFSENSOR - 1))
  {   
    return;
  }
  
  _txChannel = &_channels[channel];   
  _txChannel->txValueBuffer = value;
  txMultiChannels |= _BV(channel);
}

void txSendStartMultiChannels()
{  
  if (((millis() - txLastTime) > SKIP_START_TIME) || (txMultiChannels != txLastChannels)) 
  {
    uint8_t multiChannel = txMultiChannels;
    uint32_t time = millis();
    for(int8_t i = 7; i >= 0 ; i--)
    {
      if (multiChannel & 0x80)
      {
        _txChannel = &_channels[i]; 
        txLedWrite(LOW);
      }
      multiChannel = multiChannel << 1;
    }
    while((millis() - time) < TIME_START_LOW);
    
    multiChannel = txMultiChannels;
    time = millis();
    for(int8_t i = 7; i >=0 ; i--)
    {
      if (multiChannel & 0x80)
      {
        _txChannel = &_channels[i]; 
        txLedWrite(HIGH);
      }
      multiChannel = multiChannel << 1;
    }
    while((millis() - time) < TIME_START_HIGH);
  }
}

void txSendStopMultiChannels()
{
  for(int8_t i = 0; i < 8; i++)
  {
    if (txMultiChannels & (_BV(i)))
    {
      _txChannel = &_channels[i]; 
      txLedWrite(LOW);
      _txChannel->rxReadyFlag = 0;
      _txChannel->rxState = STATE_IDLE;  
    }
  }
}

void thVLC_::txSend()
{
  uint32_t time;
  txSendStartMultiChannels(); 
  
  for(int8_t bits = 0; bits < 8; bits++)
  { 
    uint8_t multiChannels = txMultiChannels;
    // write LOW to the all of tx channels -- 
    time = millis();
    for(int8_t i = 7; i >= 0; i--)
    {
      if (multiChannels & 0x80)
      {
        _txChannel = &_channels[i]; 
        txLedWrite(LOW);
      }
      multiChannels = multiChannels << 1;
    }
    while ((millis() - time) < TIME_BIT0_LOW);
 
    // check bits to send
    multiChannels = txMultiChannels;
    time = millis();
    for(int8_t i = 7; i >= 0; i--)
    {
      if (multiChannels & 0x80)
      {
        _txChannel = &_channels[i]; 
        uint8_t sendValue = _txChannel->txValueBuffer;        
        if ((sendValue & 0x80) == 0)
        {
          txLedWrite(HIGH);
        }       
        _txChannel->txValueBuffer = (_txChannel->txValueBuffer << 1);   
      } 
      multiChannels = multiChannels << 1;
    }
    while ((millis() - time) < (TIME_BIT1_LOW - TIME_BIT0_LOW));
    
    // write HIGH to the all of tx channels  
    multiChannels = txMultiChannels;
    time = millis();
    for(int8_t i = 7; i >= 0; i--)
    {
      if (multiChannels & 0x80)
      {
          _txChannel = &_channels[i]; 
          txLedWrite(HIGH);
      }
      multiChannels = multiChannels << 1;
    }
    while ((millis() - time) < TIME_BIT1_HIGH);
  } 
  
  txSendStopMultiChannels();
  
  txLastChannels = txMultiChannels;
  txMultiChannels = 0;
  txLastTime = millis();
}
// ********************************************** //

//unsigned long timeExecute;
#if defined(__AVR_ATmega8P__) || defined(__AVR_ATmega8__)
#endif

void thVLC_::sendByte(byte pin, byte value)
{
  byte channel = pin - CHANNEL_0;
  if (channel > (NUMOFSENSOR - 1))
  {   
    return;
  }
  
  if (((millis() - txLastTime) > SKIP_START_TIME) || (_txChannel->ctrlPin != channel + CHANNEL_0)) 
  {
    _txChannel = &_channels[channel];
    txSendStart(); 	
  }
  
  for (int8_t i = 7; i >= 0 ; i--)
  {
    if (value & 0x80) txSendBit1();
    else             txSendBit0();
      value = value << 1;
  }
  txSendStop();
  _txChannel->rxReadyFlag = 0;	// reset rxReadyFlag of the _txChannel channel
  _txChannel->rxState = STATE_IDLE;
  
  txLastTime = millis();
  //Serial.print("Time execute: ");
  //Serial.println(timeExecute);
}

ISR(TIMER1_COMPA_vect)
{
  //unsigned long time = micros(); 
  rxDecodeSensor();  	
  rxRecordPulse();  
  if (_rxChannel->ctrlPin == (NUMOFSENSOR - 1 + CHANNEL_0))
  {
    _rxChannel = &_channels[0];   
	  //_sensorHistoryIdx++; 
	  //_sensorHistoryIdx %= SENSOR_HISTORY_SIZE; 
    _sensorHistoryIdx = 1 - _sensorHistoryIdx;
   
    #if defined(__AVR_ATmega8P__) || defined(__AVR_ATmega8__)
      static uint8_t  DVcc_Idx = 16;
      //DVcc_Idx--;
      if (--DVcc_Idx == 0)
      {
        DVcc_Idx = 16;
        avrConfigFreq();
      }
    #endif
  }  
  else 
  {
    _rxChannel++;
  }
  //timeExecute = micros() - time;
}
