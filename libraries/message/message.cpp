#include <message.h>
#include <avr/pgmspace.h>

_message message;

const unsigned char PROGMEM encodeTable[500]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,230,205,180,155,130,105,80,0,0,254,229,204,179,154,129,104,79,0,0,253,228,203,178,153,128,103,78,0,0,252,227,202,177,152,127,102,77,0,0,251,226,201,176,151,126,101,76,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,250,225,200,175,150,125,100,75,0,0,249,224,199,174,149,124,99,74,0,0,248,223,198,173,148,123,98,73,0,0,247,222,197,172,147,122,97,72,0,0,246,221,196,171,146,121,96,71,0,0,245,220,195,170,145,120,95,70,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,244,219,194,169,144,119,94,69,0,0,243,218,193,168,143,118,93,68,0,0,242,217,192,167,142,117,92,67,0,0,241,216,191,166,141,116,91,66,0,0,240,215,190,165,140,115,90,65,0,0,239,214,189,164,139,114,89,64,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,238,213,188,163,138,113,88,63,0,0,237,212,187,162,137,112,87,62,0,0,236,211,186,161,136,111,86,61,0,0,235,210,185,160,135,110,85,60,0,0,234,209,184,159,134,109,84,59,0,0,233,208,183,158,133,108,83,58,0,0,232,207,182,157,132,107,82,57,0,0,231,206,181,156,131,106,81,56};
const unsigned int PROGMEM decodeTable[256]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,499,489,479,469,459,449,439,429,379,369,359,349,339,329,279,269,259,249,239,229,169,159,149,139,129,498,488,478,468,458,448,438,428,378,368,358,348,338,328,278,268,258,248,238,228,168,158,148,138,128,497,487,477,467,457,447,437,427,377,367,357,347,337,327,277,267,257,247,237,227,167,157,147,137,127,496,486,476,466,456,446,436,426,376,366,356,346,336,326,276,266,256,246,236,226,166,156,146,136,126,495,485,475,465,455,445,435,425,375,365,355,345,335,325,275,265,255,245,235,225,165,155,145,135,125,494,484,474,464,454,444,434,424,374,364,354,344,334,324,274,264,254,244,234,224,164,154,144,134,124,493,483,473,463,453,443,433,423,373,363,353,343,333,323,273,263,253,243,233,223,163,153,143,133,123,492,482,472,462,452,442,432,422,372,362,352,342,332,322,272,262,252,242,232,222,162,152,142,132,122};

void divmod10(unsigned int in, unsigned int &div, unsigned int &mod)
{
  unsigned int q = (in >> 1) + (in >> 2);
  q = q + (q >> 4);
  q = q + (q >> 8);

  q = q >> 3;
  
  unsigned int  r = in - ((q << 3) + (q << 1));	
  div = q + (r > 9);
  if (r > 9) mod = r - 10;
  else mod = r;
}

unsigned char _message::encode(unsigned char a, unsigned char b){
	return pgm_read_byte(&encodeTable[b*10+a]);
};

void _message::decode(unsigned char c, unsigned int &a, unsigned int &b){
	divmod10(pgm_read_word(&decodeTable[c]), a, b);
};