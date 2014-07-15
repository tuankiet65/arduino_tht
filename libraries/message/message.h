#define BAUD 9600
#define FAIL 1
#define SUCCESS 2
#define HANDSHAKE_MESSAGE 3
#define MESSAGE_END 4
#define IGNORE 5
#define MESSAGE_BEGIN 6

#include <avr/pgmspace.h>

class _message {
public:
	static unsigned char encode(unsigned char a, unsigned char b);
	static void decode(unsigned char c, unsigned char *a, unsigned char *b);
};

extern _message message;