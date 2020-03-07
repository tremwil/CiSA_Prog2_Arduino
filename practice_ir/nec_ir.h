#ifndef NEC_IR_H
#define NEC_IR_H

// Possible IrCmd error values
#define ERROR_SUCCESS 0
#define ERROR_TIMEOUT 1
#define ERROR_MISMATCH 2

// NEC IR protocol communication states
#define STATE_WAIT 0
#define STATE_LEAD_PULSE 1
#define STATE_MODE_SPACE 2
#define STATE_BYTE_READ 3
#define STATE_TERMINATOR 4

// NEC IR protocol pulse lengths
#define NEC_LEAD 9000
#define NEC_SPACE 562
#define NEC_COMMAND 4500
#define NEC_REPEAT 2250
#define NEC_ZERO 562
#define NEC_ONE 1687

// Struct for IR command
struct IrCmd
{
	byte deviceID;
	byte command;
	bool repeat : 1;
	byte error : 7;
	// Default CTOR
	IrCmd() {}
	// CTOR for error state
	IrCmd(byte err)
	{   deviceID = 0;
		command = 0;
		repeat = 0;
		error = err;
	}
	// CTOR for sucessful message read
	IrCmd(unsigned short msg, bool rep, byte err)
	{
		deviceID = msg >> 8;
		command = msg & 0xff;
		repeat = rep;
		error = err;
	}
};

// Library internals
namespace _IR_INT
{
	int S_PIN;
	int S_MASK;

	long EPS;
	
	bool unreadCmd; 
	unsigned short prvMsg;
	IrCmd cmd;
	
	int cState;
	int cError;
	
	unsigned short msg;
	unsigned short cByte;
	
	int iBit;
	int iByte;
	
	unsigned long ti;
	long dt;
	
	// Actual interrupt doing the NEC parsing
	void sInterrupt()
	{	
		long ct = micros();
		dt = ct - ti;
		ti = ct;
		
		int cSignal = PIND & S_MASK;
		
		switch (cState)
		{
			case STATE_WAIT:
				if (!cSignal) { cState = STATE_LEAD_PULSE; }
				break;
				
			case STATE_LEAD_PULSE:
				cState = abs(dt - NEC_LEAD) > EPS ? STATE_WAIT : STATE_MODE_SPACE;
				break;
				
			case STATE_MODE_SPACE:
				if (abs(dt - NEC_REPEAT) <= EPS)
				{
					cmd = IrCmd(prvMsg, true, ERROR_SUCCESS);
					cState = STATE_TERMINATOR;
				}
				else if (abs(dt - NEC_COMMAND) <= EPS) 
				{ 
					cState = STATE_BYTE_READ;
					cError = ERROR_SUCCESS;
					msg = 0;
					cByte = 0;
					iBit = 0;
					iByte = 0;
				}
				else cState = STATE_WAIT;
				break;
				
			case STATE_BYTE_READ:
				if (cSignal && abs(dt - NEC_SPACE) > EPS) { cState = STATE_WAIT; }
				if (!cSignal)
				{
					if (abs(dt - NEC_ONE) <= EPS)
					cByte = cByte | (1 << iBit); 
					else if (abs(dt - NEC_ZERO) > EPS)
					{
						cState = STATE_WAIT;
						break;
					}

					iBit++;
					if (iBit == 16)
					{	// Check if flipped byte matches actual byte
						if (((cByte >> 8) ^ (cByte & 0xff)) != 0xff)
							cError |= ERROR_MISMATCH;
						
						msg = msg << 8 | (cByte & 0xff);
						iBit = 0;
						cByte = 0;
						iByte++;
						
						if (iByte == 2) 
						{
							prvMsg = msg;
							cmd = IrCmd(msg, false, ERROR_SUCCESS);
							cState = STATE_TERMINATOR; 
						}
					}
				}
				break;
			case STATE_TERMINATOR:
				if (cSignal && abs(dt - NEC_SPACE) > EPS) { cState = STATE_WAIT; }
				else
				{
					unreadCmd = true;
					cState = STATE_WAIT;
				}
				break;
		}
	};
};

void IR_begin(int pin, int micronEps)
{
	_IR_INT::S_PIN = pin;
	_IR_INT::S_MASK = 1 << pin;
	_IR_INT::EPS = micronEps;
	_IR_INT::unreadCmd = false;
	
	_IR_INT::cState = STATE_WAIT;
	_IR_INT::cError = ERROR_SUCCESS;

	_IR_INT::ti = micros();
	
	attachInterrupt(digitalPinToInterrupt(pin), _IR_INT::sInterrupt, CHANGE);
};
	
void IR_begin(int pin)
{
	IR_begin(pin, 200);
};

void IR_stop()
{
	detachInterrupt(digitalPinToInterrupt(_IR_INT::S_PIN));
};

bool IR_available()
{
	return _IR_INT::unreadCmd;
};

IrCmd IR_read(unsigned long timeout)
{
	unsigned long tCall = millis();
	while (millis() - tCall < timeout)
	{
		if (_IR_INT::unreadCmd) 
		{
			_IR_INT::unreadCmd = false;
			return _IR_INT::cmd;
		}
		delay(5);
	}
	return IrCmd(ERROR_TIMEOUT);
};

IrCmd IR_read()
{
	IR_read(1000);
};

// Read IRCmd if present, otherwise return timeout error.
IrCmd IR_readCurrent()
{
	if (_IR_INT::unreadCmd) 
	{
		_IR_INT::unreadCmd = false;
		return _IR_INT::cmd;
	}
	return IrCmd(ERROR_TIMEOUT);
};

#endif
