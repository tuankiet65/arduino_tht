#include "thVLC.h"
#include <digitalWriteFast.h>


thVLCClass thVLC;

#define NUMOFSENSOR	8
#define CHANNEL_0	2

#define USEC_PERTICK     1250


#define TIME_START_LOW     16
#define TIME_START_HIGH    82

#define TIME_BIT1_LOW      44
#define TIME_BIT1_HIGH     16

#define TIME_BIT0_LOW      16
#define TIME_BIT0_HIGH     44



#define GAP_TICKS          8
#define START_SIGNAL       6
#define NUMOFBITS_RECV     8

#define SKIP_START_TIME      5
#define SENSOR_HISTORY_SIZE	 2


#define STATE_IDLE     2
#define CHECK_START    3
#define STATE_MARK     4
#define STATE_SPACE    5



#define BLOCK_ID_EEPROM_ADDR  500


const uint8_t _ctrl_R2   = A3;
const uint8_t _sensorPin = A2;

typedef struct {
    uint8_t  ctrlPin;

    uint8_t  txLedState;
    uint8_t  txValueBuffer;

    uint8_t  rxTimeStateLow;
    uint8_t  rxTimer;
    uint8_t  rxState;
    uint8_t  rxBitCounter;
    uint8_t  rxDecodeResult;
    uint8_t  rxResult;
    uint8_t  rxReadyFlag;

    uint8_t  sensorState;
    uint16_t sensorHistory[SENSOR_HISTORY_SIZE];
} CHANNEL_STRUCT;

CHANNEL_STRUCT _channels[NUMOFSENSOR];
CHANNEL_STRUCT *_rxChannel;
CHANNEL_STRUCT *_txChannel;

#define SENSOR_THRESHOLD  15
#define DEFAULT_SENSOR_VALUE  (32768)

uint8_t  _sensorHistoryIdx;
uint32_t txLastTime;


void thVLCClass::begin() {
    for(int8_t i = NUMOFSENSOR - 1; i >= 0 ; i--) {
        CHANNEL_STRUCT *channel = &_channels[i];
        channel->ctrlPin = CHANNEL_0 + i;
        channel->rxState = STATE_IDLE;
        for(int8_t idx = 0; idx < SENSOR_HISTORY_SIZE; idx++)
            channel->sensorHistory[idx] = DEFAULT_SENSOR_VALUE;
        channel->txLedState = HIGH;
    }
    PORTD |= 0xFC;
    DDRD  |= 0xFC;
    PORTB |= 0x03;
    DDRB  |= 0x03;
    PORTC |= _BV(2);
    DDRC  |= _BV(2);
    PORTC &= ~_BV(3);
    DDRC  |=  _BV(3);
    _rxChannel = &_channels[0];
    ADMUX = 0x42;
    avrPullUpDisable();
    avrADCClockDivConfig(ADC_CLK_DIV_8);
    avrTimer1ConfigNormal(AVR_TIMER1_TOP(USEC_PERTICK));
    avrTimer1OC1AInteruptEnable();
}


byte thVLCClass::getID() {
    return avrEepromRead(BLOCK_ID_EEPROM_ADDR);
}



boolean thVLCClass::receiveReady(byte pin) {
    byte channel = pin - CHANNEL_0;
    if(channel > (NUMOFSENSOR - 1))
        return false;
    CHANNEL_STRUCT *addr = &_channels[channel];
    return addr->rxReadyFlag;
}

byte thVLCClass::receiveResult(byte pin) {
    byte channel = pin - CHANNEL_0;
    if(channel > (NUMOFSENSOR - 1))
        return 0;
    CHANNEL_STRUCT *addr = &_channels[channel];
    addr->rxReadyFlag = 0;
    return (addr->rxResult);
}


void rxReadMode() {
    DDRC &= (~_BV(2));
    DDRC |= _BV(3);
    DDRD &= 0x03;
    DDRB &= 0xFC;
    digitalWriteFast(_rxChannel->ctrlPin, HIGH);
    pinModeFast(_rxChannel->ctrlPin, OUTPUT);
}

void rxLedMode() {
    digitalWriteFast(_rxChannel->ctrlPin,_rxChannel->txLedState);
    DDRD |= 0xFC;
    DDRB |= 0x03;
    DDRC &= ~(_BV(3));
    DDRC |= _BV(2);
}



uint16_t rxSensorRead() {
    rxReadMode();
    ADCSRA |= _BV(ADSC);
    rxLedMode();
    while(ADCSRA & (_BV(ADSC)));
    return ADC;
}


int thVLCClass::sensorRead(byte pin) {
    byte channel = pin - CHANNEL_0;
    if(channel >= NUMOFSENSOR + 1)
        return 0;
    if(pin != 10) {
        CHANNEL_STRUCT *_readChannel = &_channels[channel];
        return _readChannel->sensorHistory[_sensorHistoryIdx];
    } else {
        avrTimer1OC1AInteruptDisable();
        DDRC |= _BV(3);
        DDRC &= ~_BV(2);
        digitalWriteFast(10, HIGH);
        DDRD &= 0x03;
        DDRB = (DDRB & 0xFC) | _BV(2);
        ADCSRA |= _BV(ADSC);
        while(ADCSRA & (_BV(ADSC)));
        DDRB &= ~_BV(2);
        avrTimer1OC1AInteruptEnable();
        return ADC;
    }
}



void rxDecodeSensor() {
    uint16_t maxValue = _rxChannel->sensorHistory[0];
    uint16_t minValue = _rxChannel->sensorHistory[1];
    if(minValue > maxValue) {
        uint16_t temp = maxValue;
        maxValue = minValue;
        minValue = temp;
    }
    uint16_t buf = rxSensorRead();
    _rxChannel->sensorHistory[_sensorHistoryIdx] = buf;
    if(buf >= minValue + SENSOR_THRESHOLD)
        _rxChannel->sensorState = 1;
    if(buf <= maxValue - SENSOR_THRESHOLD)
        _rxChannel->sensorState = 0;
}



void rxCheckStartAndDecode() {
    if(_rxChannel->rxTimer < START_SIGNAL) {
        _rxChannel->rxDecodeResult = _rxChannel->rxDecodeResult << 1;
        if(_rxChannel->rxTimeStateLow > _rxChannel->rxTimer)
            _rxChannel->rxDecodeResult |= 0x01;
        _rxChannel->rxBitCounter++;
    } else {
        _rxChannel->rxDecodeResult = 0;
        _rxChannel->rxBitCounter = 0;
    }
}



void rxCheckStartSignal() {
    if(_rxChannel->rxTimer < START_SIGNAL)
        _rxChannel->rxState = STATE_IDLE;
    else
        _rxChannel->rxState = STATE_SPACE;
}


void rxRecordPulse() {
    _rxChannel->rxTimer++;
    switch(_rxChannel->rxState) {
    case STATE_IDLE:
        if(_rxChannel->sensorState == 1) {
            _rxChannel->rxDecodeResult = 0;
            _rxChannel->rxBitCounter	= 0;
            _rxChannel->rxTimer = 0;
            _rxChannel->rxState = CHECK_START;
        }
        break;
    case CHECK_START:
        if(_rxChannel->sensorState == 0) {
            rxCheckStartSignal();
            _rxChannel->rxTimer = 0;
        }
        break;
    case STATE_MARK:
        if(_rxChannel->sensorState == 0) {
            rxCheckStartAndDecode();
            _rxChannel->rxState = STATE_SPACE;
            _rxChannel->rxTimer = 0;
            if(_rxChannel->rxBitCounter >= NUMOFBITS_RECV) {
                _rxChannel->rxResult = _rxChannel->rxDecodeResult;
                _rxChannel->rxReadyFlag = 1;
                _rxChannel->rxDecodeResult = 0;
                _rxChannel->rxBitCounter = 0;
            }
        }
        break;
    case STATE_SPACE:
        if(_rxChannel->sensorState == 1) {
            _rxChannel->rxTimeStateLow = _rxChannel->rxTimer;
            _rxChannel->rxState = STATE_MARK;
            _rxChannel->rxTimer = 0;
        } else if(_rxChannel->rxTimer > GAP_TICKS)
            _rxChannel->rxState = STATE_IDLE;
        break;
    }
}




void txLedWrite(byte value) {
    byte valueBuff = value ^ 0x01;
    _txChannel->txLedState = valueBuff;
    digitalWriteFast(_txChannel->ctrlPin, valueBuff);
}

void thVLCClass::ledWrite(byte pin, byte value) {
    byte channel = pin - CHANNEL_0;
    if(channel > (NUMOFSENSOR - 1))
        return;
    _txChannel = &_channels[channel];
    txLedWrite(value);
}

void txSendStart() {
    txLedWrite(HIGH);
    delay(TIME_START_HIGH);
}

void txSendBit1() {
    txLedWrite(LOW);
    delay(TIME_BIT1_LOW);
    txLedWrite(HIGH);
    delay(TIME_BIT1_HIGH);
}

void txSendBit0() {
    txLedWrite(LOW);
    delay(TIME_BIT0_LOW);
    txLedWrite(HIGH);
    delay(TIME_BIT0_HIGH);
}

void txSendStop() {
    txLedWrite(LOW);
}



uint8_t txMultiChannels, txLastChannels;

void thVLCClass::txSetByte(byte pin, byte value) {
    byte channel = pin - CHANNEL_0;
    if(channel > (NUMOFSENSOR - 1))
        return;
    _txChannel = &_channels[channel];
    _txChannel->txValueBuffer = value;
    txMultiChannels |= _BV(channel);
}

void txSendStartMultiChannels() {
    if(((millis() - txLastTime) > SKIP_START_TIME) || (txMultiChannels != txLastChannels)) {
        uint8_t multiChannel = txMultiChannels;
        uint32_t time = millis();
        for(int8_t i = 7; i >= 0 ; i--) {
            if(multiChannel & 0x80) {
                _txChannel = &_channels[i];
                txLedWrite(LOW);
            }
            multiChannel = multiChannel << 1;
        }
        while((millis() - time) < TIME_START_LOW);
        multiChannel = txMultiChannels;
        time = millis();
        for(int8_t i = 7; i >=0 ; i--) {
            if(multiChannel & 0x80) {
                _txChannel = &_channels[i];
                txLedWrite(HIGH);
            }
            multiChannel = multiChannel << 1;
        }
        while((millis() - time) < TIME_START_HIGH);
    }
}

void txSendStopMultiChannels() {
    for(int8_t i = 0; i < 8; i++) {
        if(txMultiChannels & (_BV(i))) {
            _txChannel = &_channels[i];
            txLedWrite(LOW);
            _txChannel->rxReadyFlag = 0;
            _txChannel->rxState = STATE_IDLE;
        }
    }
}

void thVLCClass::txSend() {
    uint32_t time;
    txSendStartMultiChannels();
    for(int8_t bits = 0; bits < 8; bits++) {
        uint8_t multiChannels = txMultiChannels;
        time = millis();
        for(int8_t i = 7; i >= 0; i--) {
            if(multiChannels & 0x80) {
                _txChannel = &_channels[i];
                txLedWrite(LOW);
            }
            multiChannels = multiChannels << 1;
        }
        while((millis() - time) < TIME_BIT0_LOW);
        multiChannels = txMultiChannels;
        time = millis();
        for(int8_t i = 7; i >= 0; i--) {
            if(multiChannels & 0x80) {
                _txChannel = &_channels[i];
                uint8_t sendValue = _txChannel->txValueBuffer;
                if((sendValue & 0x80) == 0)
                    txLedWrite(HIGH);
                _txChannel->txValueBuffer = (_txChannel->txValueBuffer << 1);
            }
            multiChannels = multiChannels << 1;
        }
        while((millis() - time) < (TIME_BIT1_LOW - TIME_BIT0_LOW));
        multiChannels = txMultiChannels;
        time = millis();
        for(int8_t i = 7; i >= 0; i--) {
            if(multiChannels & 0x80) {
                _txChannel = &_channels[i];
                txLedWrite(HIGH);
            }
            multiChannels = multiChannels << 1;
        }
        while((millis() - time) < TIME_BIT1_HIGH);
    }
    txSendStopMultiChannels();
    txLastChannels = txMultiChannels;
    txMultiChannels = 0;
    txLastTime = millis();
}




void thVLCClass::sendByte(byte pin, byte value) {
    byte channel = pin - CHANNEL_0;
    if(channel > (NUMOFSENSOR - 1))
        return;
    if(((millis() - txLastTime) > SKIP_START_TIME) || (_txChannel->ctrlPin != channel + CHANNEL_0)) {
        _txChannel = &_channels[channel];
        txSendStart();
    }
    for(int8_t i = 7; i >= 0 ; i--) {
        if(value & 0x80)
            txSendBit1();
        else
            txSendBit0();
        value = value << 1;
    }
    txSendStop();
    _txChannel->rxReadyFlag = 0;
    _txChannel->rxState = STATE_IDLE;
    txLastTime = millis();
}

ISR(TIMER1_COMPA_vect) {
    rxDecodeSensor();
    rxRecordPulse();
    if(_rxChannel->ctrlPin == (NUMOFSENSOR - 1 + CHANNEL_0)) {
        _rxChannel = &_channels[0];
        _sensorHistoryIdx = 1 - _sensorHistoryIdx;
#if defined(__AVR_ATmega8P__) || defined(__AVR_ATmega8__)
        static int8_t  DVcc_Idx = 16;
        if(--DVcc_Idx <= 0) {
            DVcc_Idx = 16;
            avrConfigFreq();
            ADMUX = 0x42;
        }
#endif
    } else
        _rxChannel++;
}
