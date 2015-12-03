
#ifndef __STRING_CASEFUNCTIONS_H__
#define __STRING_CASEFUNCTIONS_H__

typedef unsigned int CharUnicode;

struct Case
{
  CharUnicode charSmall;
  CharUnicode charCapital;
};

Case GetUnicodeCase(CharUnicode ch);

bool IsLower(CharUnicode ch); 

bool IsUpper(CharUnicode ch);

bool IsLetter(CharUnicode ch);

CharUnicode GetLower(CharUnicode ch);

CharUnicode GetUpper(CharUnicode ch);
  

#endif //__STRING_CASEFUNCTIONS_H__
