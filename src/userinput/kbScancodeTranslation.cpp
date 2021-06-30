#include "kbScancodeTranslation.h"

namespace QWERTYKeyboard {
	const char ASCIITable[] = {
		 0 ,  0 , '1', '2',
		'3', '4', '5', '6',
		'7', '8', '9', '0',
		'-', '=',  0 ,  0 ,
		'q', 'w', 'e', 'r',
		't', 'y', 'u', 'i',
		'o', 'p', '[', ']',
		 0 ,  0 , 'a', 's',
		'd', 'f', 'g', 'h',
		'j', 'k', 'l', ';',
		'\'','`',  0 , '\\',
		'z', 'x', 'c', 'v',
		'b', 'n', 'm', ',',
		'.', '/',  0 , '*',
		 0 , ' '
	};
	char Translate(uint8_t scancode, bool uppercase) {
		if (scancode > sizeof(ASCIITable) / sizeof(char))
			return 0;
		return ASCIITable[scancode];
	}
}
namespace AZERTYKeyboard {
	const char ASCIITable[] = {
		 0 ,  0 , '1', '2',
		'3', '4', '5', '6',
		'7', '8', '9', '0',
		')', '=',  0 ,  0 ,
		'a', 'z', 'e', 'r',
		't', 'y', 'u', 'i',
		'o', 'p', '^', '$',
		 0 ,  0 , 'q', 's',
		'd', 'f', 'g', 'h',
		'j', 'k', 'l', 'm',
		'%',  0 ,  0 , '*',
		'w', 'x', 'c', 'v',
		'b', 'n', ',', ';',
		':', '!',  0 , '*',
		 0 , ' ',  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 ,  0 ,
		 0 ,  0 ,  0 , '7',
		'8', '9', '-', '4',
		'5', '6', '+', '1',
		'2', '3', '0', '.',
	};
	char Translate(uint8_t scancode, bool uppercase) {
		if (scancode > sizeof(ASCIITable) / sizeof(char))
			return 0;
		if (uppercase) {
			return ASCIITable[scancode] - 32;
		}
		return ASCIITable[scancode];
	}
}