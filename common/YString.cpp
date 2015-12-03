
#include "YString.h"

  size_t String::Count() const
  {
    return (pData != NULL) ? pData->realSize : 0;
  }

  Char32 String::GetAt(size_t pos, Char32 retIfFail) const
  {
    Char32 res = retIfFail;
    if (pData && pos < pData->realSize)
    {
      const Char8 * p = AtPos(pos);
      res = ReadChar32(p);
    }
    return res;
  }

  bool String::SetAt(size_t pos, Char32 val)
  {
    bool res = false;
    if (pData && pos < pData->realSize)
    {
      const Char8 * p = AtPos(pos);
      size_t diff = p - pData->pLine;
      assert(p);

      size_t oldCharLen = Step(p);
      size_t newCharLen = Step(val);
      size_t newSize    = pData->size - oldCharLen + newCharLen;

      StrData * pNewData = CreateData(newSize);
      pNewData->size     = newSize;
      pNewData->realSize = pData->realSize;
      MemCpy(pNewData->pLine, pData->pLine, diff);
      WriteChar32(pNewData->pLine + diff, val);
      MemCpy(pNewData->pLine + diff + newCharLen, pData->pLine + diff + oldCharLen, newSize - diff - newCharLen);
      pNewData->pLine[newSize] = 0;
      ReplaceData(pNewData);

      res = true;
    }
    return res;
  }

  void String::Clear()
  {      
    if (pData)
    {
      assert(pData->refs.Count() > 0);
      ReplaceData(NULL);
    }
  }

  String::String(): pData(NULL) 
  {}

  String::String(const char * line, size_t len): pData(NULL)
  {
    FromLocale(line, len);
  }

  String::String(const wchar_t * line, size_t len): pData(NULL)
  {
    FromWChar(line, len);
  }
  
  String::~String() 
  { 
    Clear(); 
  }

  String::String(const String & copy): pData(NULL)
  { 
    if (copy.pData) copy.pData->refs.Inc();
    ReplaceData(copy.pData);
  }

  void String::operator=(const String & copy) 
  { 
    if (copy.pData) copy.pData->refs.Inc();
    ReplaceData(copy.pData);
  }

  bool String::IsSet() const
  {
    return Size() > 0;
  }

  bool String::IsEmpty() const
  {
	return !IsSet();
  }

  String & String::FromInt  (int   value)
  {
    bool isMinus = (value < 0) ? true : false;      
    if (isMinus) value = -value;

    size_t len  = GetIntLen(value);
    size_t size = len + (isMinus ? 1 : 0);

    StrData * pNewData = CreateData(size);
    pNewData->size     = size;
    pNewData->realSize = size;
    Char8 * pLine = pNewData->pLine;

    if (isMinus) 
    {
      *pLine = '-';
      ++pLine;
    }

    if (value)
    {
      int num0 = value / 10;
      int num1 = value;
      size_t i = len - 1;

      while(num1 != 0)
      {
        pLine[i--] = '0' + num1 - num0 * 10;
        num1 = num0;
        num0 /= 10;
      }
    }
    else
    {
      *pLine = '0';
    }
    
    pNewData->pLine[size] = 0;
    ReplaceData(pNewData);

    return *this;
  }

  String & String::FromFloat(float value, size_t afterPoint, Char8 delimer)
  {                         
    if (afterPoint == 0) return FromInt((int)value);

    bool isMinus = (value < 0.0f) ? true : false;      
    if (isMinus) value = -value;

    size_t len = GetIntLen((int)value);
    size_t size = len + afterPoint + 1 + (isMinus ? 1 : 0);

    StrData * pNewData = CreateData(size);
    pNewData->size     = size;
    pNewData->realSize = size;
    Char8 * pLine = pNewData->pLine;

    if (isMinus) 
    {
      *pLine = '-';
      ++pLine;
    }

    long realNumber = (long)(value * PowTen(afterPoint));

    long num0 = realNumber / 10;
    long num1 = realNumber;

    size_t pointPos = len;

    for (int i = len + afterPoint; i >= 0; --i)
    {
      if(i == pointPos) 
      { 
        pLine[i] = delimer;
      }
      else
      {
        pLine[i] = (Char8)((long)'0' + num1 - num0 * 10);
        num1 = num0;
        num0 /= 10;
      }
    }
    
    pNewData->pLine[size] = 0;
    ReplaceData(pNewData);

    return *this;
  }

  String & String::FromBool(bool value)
  {
    return value ? FromLocale("true") : FromLocale("false");
  }

  String & String::FromLatin(const Char8  * str, size_t len)
  {
    assert(str);
    if (len == 0) len = StrLen(str);

    StrData * pNewData = CreateData(len);
    pNewData->size     = len;
    pNewData->realSize = len;
    for(size_t i = 0; i < len; ++i) pNewData->pLine[i] = str[i] < 128 ? str[i] : '?';
    pNewData->pLine[len] = 0;
    ReplaceData(pNewData);
    
    return *this;
  }

  String & String::FromUTF8 (const Char8  * str, size_t len)
  {
    assert(str);
    if (len == 0) len = StrLen(str);
    size_t size  = 0;
    size_t count = 0;

    while (true)
    {
      size_t step = Step(str + size);
      if (size + step <= len)
      {
        size += step;
        ++count;
      }
      else
      {
        break;
      }
    } 

    StrData * pNewData = CreateData(size);
    pNewData->size     = size;
    pNewData->realSize = count;
    MemCpy(pNewData->pLine, str, size);
    pNewData->pLine[size] = 0;
    ReplaceData(pNewData);
    
    return *this;
  }

  String & String::FromUCS2 (const Char16 * str, size_t len)
  {
    assert(str);
    if (len == 0) len = StrLen(str);
    size_t size  = 0;
    size_t count = len;

    for (size_t i = 0; i < len; ++i) size += Step((Char32)(str[i]));

    StrData * pNewData = CreateData(size);
    pNewData->size     = size;
    pNewData->realSize = count;
    Char8 * pLine = pNewData->pLine;
    for (size_t i = 0; i < len; ++i)
    {
      pLine += WriteChar32(pLine, (Char32)(str[i]));
    }
    pNewData->pLine[size] = 0;
    ReplaceData(pNewData);
    
    return *this;
  }

  String & String::FromUCS4 (const Char32 * str, size_t len)
  {
    assert(str);
    if (len == 0) len = StrLen(str);
    size_t size  = 0;
    size_t count = len;

    for (size_t i = 0; i < len; ++i) size += Step((Char32)(str[i]));

    StrData * pNewData = CreateData(size);
    pNewData->size     = size;
    pNewData->realSize = count;
    Char8 * pLine = pNewData->pLine;
    for (size_t i = 0; i < len; ++i)
    {
      pLine += WriteChar32(pLine, (Char32)(str[i]));
    }
    pNewData->pLine[size] = 0;
    ReplaceData(pNewData);
    
    return *this;
  }

  String & String::FromLocale(const char * str, size_t len)
  {                  
#if defined(WIN32) || defined(WIN64)
    size_t count = ::MultiByteToWideChar(CP_ACP, 0, str, len == 0 ? -1 : len, NULL, 0);
    void * p = Malloc(count * sizeof(wchar_t));                        
    count = ::MultiByteToWideChar(CP_ACP, 0, str, len == 0 ? -1 : len, (LPWSTR)p, count);
    FromUCS2((const Char16 *)p, count - 1);
    Free(p);
#else // UNIX
    FromUTF8((const Char8 *)str, len);
#endif
    return *this;
  }

  String & String::FromWChar(const wchar_t * str, size_t len)
  {
#if defined(WIN32) || defined(WIN64)
    FromUCS2((const Char16 *)str, len);
#else // UNIX
    FromUCS4((const Char32 *)str, len);
#endif
    return *this;
  }
                                
  //
  const Buffer8  String::Latin() const
  {
    Buffer8 buff;
    buff.Init(PrintLatin(NULL, 0), this, &String::PrintLatin);
    return buff;
  }

  const Buffer8  String::UTF8()  const
  {
    Buffer8 buff;
    buff.Init(PrintUTF8(NULL, 0), this, &String::PrintUTF8);
    return buff;
  }

  const Buffer16 String::UCS2()  const
  {
    Buffer16 buff;
    buff.Init(PrintUCS2(NULL, 0), this, &String::PrintUCS2);
    return buff;
  }

  const Buffer32 String::UCS4()  const
  {
    Buffer32 buff;
    buff.Init(PrintUCS4(NULL, 0), this, &String::PrintUCS4);
    return buff;
  }

  const BufferChar  String::Locale() const
  {
#if defined(WIN32) || defined(WIN64)
    BufferChar buff;
    buff.Init(PrintLocale(NULL, 0), this, &String::PrintLocale);
    return buff;
#else // UNIX
    BufferChar buff;
    buff.InitFrom<Char8>(UTF8());
    return buff;
#endif
  }

  const BufferWChar String::WChar () const
  {
    BufferWChar buff;
#if defined(WIN32) || defined(WIN64)
    buff.InitFrom(UCS2());
#else // UNIX
    buff.InitFrom(UCS4());
#endif
    return buff;
  }

  int String::Int(int retifFail) const
  {                
    const Char8 * pLine = Line();
    while (*pLine == ' ') ++pLine;

    bool bNegat = (*pLine == '-');

    if(bNegat || *pLine == '+') ++pLine;
    
    int result = 0;
    bool isDigit = false;

    while(*pLine != 0) 
    {
      Char8 src = *pLine;
      if(src >= '0' && src <= '9')
      {
        isDigit = true;
        result = result * 10 + (src - '0');
        ++pLine;
      }
      else break;        
    }
    return isDigit ? ( bNegat ? -result : result ) : retifFail;
  }

  float String::Float(float retifFail) const
  {
    const Char8 * pLine = Line();

    while (*pLine == ' ') ++pLine;

    bool bNegat = (*pLine == '-');

    if (bNegat || *pLine == '+') ++pLine;
    
    float dig = 10.0f;
    bool  before = true;
    float result = 0.0f;

    bool isDigit = false;

    while (*pLine) 
    {
      Char8 src = *pLine;
      if (src >= '0' && src <= '9')
      {
        isDigit = true;
        if (before) 
        {
          result = result * 10.0f + (float)(src - '0');
        }
        else 
        { 
          result = result + dig * (src - '0'); 
          dig *= dig; 
        }
        ++pLine;
      }
      else if ((src == '.' || src == ',') && before) 
      { 
        dig = 0.1f;
        before = false; 
        ++pLine;
        continue; 
      }
      else break;
    }
    return isDigit ? ( bNegat ? -result : result ) : retifFail;
  }

  bool String::Bool(bool retifFail) const //@todo optimize
  {
    String left = *this;
    left.ToUpper().Trim();
    if (left == String("TRUE")  || left == String("1")) return true;
    if (left == String("FALSE") || left == String("0")) return false;
    return retifFail;
  }     

  size_t String::PrintLatin(Char8  * str, size_t len) const
  {
    size_t count = Count();
    if (len == 0) return count + 1;
    
    size_t i = 0;      
    const Char8 * pSrc = Line();
    Char8 * pDst = str;

    for (i = 0; i < count && i < (len - 1); ++i)
    {
      Char8 a = *pSrc;
      if (a >= 0x80) a = '?';

      if (pDst) 
      {
        *pDst = a;
        ++pDst; 
      }
      pSrc += Step(pSrc);
    }
    if (pDst) *pDst = 0;
    return pDst - str;
  }

  size_t String::PrintUTF8 (Char8  * str, size_t len) const
  {
    return PrintUTF8 (str, len, NULL);
  }

  size_t String::PrintUTF8 (Char8  * str, size_t len, size_t * pRealSize) const
  {
    if (len == 0) return Size() + 1;
    
    const Char8 * pSrc    = Line();
    Char8 * pDst    = str;
    size_t realSize = 0;

    while(*pSrc)
    {
      const Char8 * p = pSrc + Step(pSrc);
       
      if ((pDst - str) + (p - pSrc) > (int)(len - 1)) break;
      while (pSrc != p)
      {
        *pDst++ = *pSrc++;
      }
      ++realSize;
    }                 
    if (pDst) *pDst = 0;
    if (pRealSize) *pRealSize = realSize;
    return pDst - str;
  }

  size_t String::PrintUCS2 (Char16 * str, size_t len) const
  {                                 
    size_t count = Count();
    if (len == 0) return count + 1;
    
    size_t i = 0;      
    const Char8  * pSrc = Line();
    Char16 * pDst = str;

    for (i = 0; i < count && i < (len - 1); ++i)
    {
      if (pDst) 
      {
        *pDst = (Char16)ReadChar32(pSrc);
        ++pDst; 
      }
      pSrc += Step(pSrc);
    }
    if (pDst) *pDst = 0;
    return pDst - str;
  }

  size_t String::PrintUCS4 (Char32 * str, size_t len) const
  {                                 
    size_t count = Count();
    if (len == 0) return count + 1;
    
    size_t i = 0;      
    const Char8  * pSrc = Line();
    Char32 * pDst = str;

    for (i = 0; i < count && i < (len - 1); ++i)
    {
      if (pDst) 
      {
        *pDst = ReadChar32(pSrc);
        ++pDst; 
      }
      pSrc += Step(pSrc);
    }
    if (pDst) *pDst = 0;
    return pDst - str;
  }

  size_t String::PrintLocale(char * str, size_t len) const
  {
#if defined(WIN32) || defined(WIN64)
    size_t size = PrintUCS2(NULL, 0);
    Char16 * p = (Char16*)Malloc(size * sizeof(Char16));
    size = PrintUCS2(p, size);                              
    size_t res = ::WideCharToMultiByte(CP_ACP, 0, (const wchar_t *)p, -1, str, len, NULL, NULL);
    Free((void*)p);
    return res;
#else // UNIX
    return PrintUTF8((Char8*)str, len);
#endif
  }

  size_t String::PrintWChar (wchar_t * str, size_t len) const
  {
#if defined(WIN32) || defined(WIN64)
    return PrintUCS2((Char16*)str, len);
#else // UNIX
    return PrintUCS4((Char32*)str, len);
#endif
  }

  //
  bool String::Has(const String & line, bool bIgnoreCase) const
  {
    return Find(line, 0, bIgnoreCase) != -1;
  }

  int String::Find(const String & line, size_t pos, bool bIgnoreCase) const
  {
    if (pos >= Count()) return -1;

    const Char8 * pStr = pData ? AtPos(pos) : Line();

    int res = -1;

    if (!bIgnoreCase && IsLatin()) // small optimization
    {
      const Char8 * p = StrStr(pStr, line.Line());
      res = ((p == NULL) ? -1 : pos + (p - pStr));
    }
    else             
    { 
      const Char8 * pLine       = pStr;
      const Char8 * pSearchLine = line.Line();
      
      int count = 0;

      while(*pLine)
      {
        Char32 ch1 = ReadChar32(pLine);
        Char32 ch2 = ReadChar32(pSearchLine);
        if(bIgnoreCase)
        {
          ch1 = GetUpper(ch1);
          ch2 = GetUpper(ch2);
        }

        if(ch1 == ch2)
        {
          const Char8 * pLine2       = pLine;
          const Char8 * pSearchLine2 = pSearchLine;

          while(ch1 == ch2 && ch1 != 0 && ch2 != 0)
          {           
            pLine2       += Step(pLine2);
            pSearchLine2 += Step(pSearchLine2);

            ch1 = ReadChar32(pLine2);
            ch2 = ReadChar32(pSearchLine2);
            if(bIgnoreCase)
            {
              ch1 = GetUpper(ch1);
              ch2 = GetUpper(ch2);
            }
          }
           
          if(ch2 == 0) 
          {
            res = count;
            break; 
          }
        }
        pLine += Step(pLine);
        ++count;
      }
    }
    return res;
  }

  int String::RFind(const String & search, size_t pos, bool bIgnoreCase) const //@todo optimize
  {
    size_t count = Count();
    int cur = Find(search, 0, bIgnoreCase);
    while (cur >= 0 && (size_t)cur <= pos)
    {
      int pos = Find(search, cur + 1, bIgnoreCase);
      if (pos < 0) break;
      cur = pos;
    }
    return cur; 
  }

  int String::Compare(const String & line, bool bIgnoreCase) const
  {
    int res = 0;

    if (!bIgnoreCase && IsLatin()) // small optimization
    {
      res = StrCmp(Line(), line.Line());
    }
    else             
    { 
      const Char8 * pLineLeft  = Line();
      const Char8 * pLineRight = line.Line();

      Char32 left, right;
      do
      {
        left  = ReadChar32(pLineLeft); 
        right = ReadChar32(pLineRight);
        if(bIgnoreCase)
        {
          left  = GetUpper(left);
          right = GetUpper(right);
        }
        pLineLeft  += Step(pLineLeft);
        pLineRight += Step(pLineRight);
      }
      while(left == right && left && right);

      res = (left < right) ? -1 : (left > right ? 1 : 0);
    }
    return res;
  }

  String & String::Replace(const String & from, const String & to, size_t startPos, size_t maxCount) //@todo optimize
  {
    for (size_t i = 0; i < maxCount; ++i)
    {
      int pos = Find(from, startPos);
      if (pos < 0) break;
      String line1 = *this;
      String line2 = *this;
      *this = line1.SubStr(0, pos) + to + line2.SubStr(pos + from.Count());
    }
    return *this;
  }

  String & String::SubStr (size_t pos, size_t len)
  {
    if (pData && pos < pData->realSize)
    {
      size_t count = Count();
      size_t newCount = 0;

      const Char8 * pFrom = AtPos(pos);
      const Char8 * pTo = pFrom;
      for(size_t i = pos; i < len && i < count; ++i) 
      {
        pTo += Step(pTo);
        ++newCount;
      }
      size_t newSize = pTo - pFrom;

      StrData * pNewData = CreateData(newSize);
      pNewData->size     = newSize;
      pNewData->realSize = newCount;
      MemCpy(pNewData->pLine, pFrom, newSize);
      pNewData->pLine[newSize] = 0;
      ReplaceData(pNewData);
    }
    else
    {
      Clear();
    }
    return *this; 
  }    

  String & String::Trim()
  {
    return Trim(true, true);
  }

  String & String::LTrim()
  {
    return Trim(true, false);
  }

  String & String::RTrim()
  {
    return Trim(false, true);
  }

  String & String::ToUpper()
  {
    return ConvertChars(GetUpper);
  }

  String & String::ToLower()
  {
    return ConvertChars(GetLower);
  }
  
  String & String::ConvertChars(Char32(*pFunc)(Char32))
  {
    if (pData)
    {
      size_t newSize = 0;
      size_t count = Count();

      if (IsLatin())
      {
        newSize = Size();
      }
      else
      {
        const Char8 * pLine = Line();
        for(size_t i = 0; i < count; ++i) 
        {
          Char32 ch = ReadChar32(pLine);
          ch = pFunc(ch);
          newSize += Step(ch);
          pLine += Step(pLine);
        }
      }

      StrData * pNewData = CreateData(newSize);
      pNewData->size     = newSize;
      pNewData->realSize = count;

      const Char8 * pLineSrc = Line();
      Char8 * pLineDst = pNewData->pLine;
      for(size_t i = 0; i < count; ++i) 
      {
        Char32 ch = ReadChar32(pLineSrc);
        ch = pFunc(ch);
        pLineDst += WriteChar32(pLineDst, ch);
        pLineSrc += Step(pLineSrc);
      }

      pNewData->pLine[newSize] = 0;
      ReplaceData(pNewData);
    }
    return *this; 
  }

  String & String::Append(const String & line)
  {                          
    if (line.Count() > 0)
    {
      size_t newCount = Count() + line.Count();
      size_t newSize  = Size()  + line.Size();

      StrData * pNewData = CreateData(newSize);
      pNewData->size     = newSize;
      pNewData->realSize = newCount;
      MemCpy(pNewData->pLine,          Line(),      Size());
      MemCpy(pNewData->pLine + Size(), line.Line(), line.Size());
      pNewData->pLine[newSize] = 0;
      ReplaceData(pNewData);
    }
    return *this; 
  }

  String & String::Insert(size_t pos, const String & line)
  {                          
    if (line.Count() > 0 && pos <= Count())
    {
      size_t count    = Count();
      size_t newCount = count  + line.Count();
      size_t newSize  = Size() + line.Size();

      const Char8 * pLine1   = Line();
      const Char8 * pLine2   = pLine1;
      for(size_t i = 0;   i < pos;   ++i) pLine2   += Step(pLine2);
      const Char8 * pLineEnd = pLine2;
      for(size_t i = pos; i < count; ++i) pLineEnd += Step(pLineEnd);
      size_t  len1 = pLine2   - pLine1;
      size_t  len2 = pLineEnd - pLine2;

      StrData * pNewData = CreateData(newSize);
      pNewData->size     = newSize;
      pNewData->realSize = newCount;

      MemCpy(pNewData->pLine + 0,                  pLine1,      len1);
      MemCpy(pNewData->pLine + len1,               line.Line(), line.Size());
      MemCpy(pNewData->pLine + line.Size() + len1, pLine2,      len2);

      pNewData->pLine[newSize] = 0;
      ReplaceData(pNewData);
    }
    return *this; 
  }
  
  String   String::operator+ (const String & str) const
  {
    String res = *this;
    res.Append(str);
    return res;
  }

  String & String::operator+=(const String & str)
  {
    return Append(str);
  }
  
  bool String::operator==(const String & str) const
  {
    return Compare(str, false) == 0;
  }

  bool String::operator!=(const String & str) const
  {
    return Compare(str, false) != 0;
  }

  bool String::operator< (const String & str) const
  {
    return Compare(str, false) < 0;
  }

  bool String::operator<=(const String & str) const
  {
    return Compare(str, false) <= 0;
  }

  bool String::operator> (const String & str) const
  {
    return Compare(str, false) > 0;
  }

  bool String::operator>=(const String & str) const
  {
    return Compare(str, false) >= 0;
  }

  //**

  void * String::Malloc(size_t size)
  {
    return malloc(size);
  }

  void String::Free(void * pMem)
  {
    return free(pMem);
  }

  void String::ReplaceData(StrData * pNewData)
  {
    StrData * p = pData;
    pData = pNewData;
    if (p)
    {
      if (p->refs.Dec() == 0) Free((void*)p);
    }
  }

  String::StrData * String::CreateData(size_t size)
  {                        
    StrData * pData = (StrData*)Malloc(sizeof(StrData) + size + 1);
    pData->pLine = (Char8*)(pData + 1);
    pData->refs.Init(1);
    return pData;
  }

  size_t String::Step(const Char8 * line)
  {  
    Char8 wc = (Char32)(*line);
    size_t len = 0;

    if      (wc < 0x80) len = 1;
    else if (wc < 0xe0) len = 2;
    else if (wc < 0xf0) len = 3;
    else if (wc < 0xf8) len = 4;
    else if (wc < 0xfc) len = 5;
    else                len = 6;

    return len;
  }     

  size_t String::Step(Char32 ch)
  {  
    size_t len = 0;
                                
    if (ch < 0x80)           len = 1;
    else if (ch < 0x800)     len = 2;
    else if (ch < 0x10000)   len = 3;
    else if (ch < 0x200000)  len = 4;
    else if (ch < 0x4000000) len = 5;
    else                     len = 6;

    return len;
  }

  const Char8 * String::AtPos(size_t pos) const
  {
    assert(pData && pos < pData->realSize);

    const Char8 * res = NULL;

    if (IsLatin())
    {
      res = pData->pLine + pos;
    }
    else
    {
      res = pData->pLine;
      for(size_t i = 0; i < pos; ++i) res += Step(res);
    }
    return res;
  }

  Char8 * String::AtPos(size_t pos)
  {
    assert(pData && pos < pData->realSize);

    Char8 * res = NULL;

    if (IsLatin())
    {
      res = pData->pLine + pos;
    }
    else
    {
      res = pData->pLine;
      for(size_t i = 0; i < pos; ++i) res += Step(res);
    }
    return res;
  }

  Char32 String::ReadChar32(const Char8 * line)
  {
    size_t len = 0;
    Char32 wc = (Char32)(*line);

    if      (wc < 0x80) { len = 1; }
    else if (wc < 0xe0) { len = 2; wc &= 0x1f; }
    else if (wc < 0xf0) { len = 3; wc &= 0x0f; }
    else if (wc < 0xf8) { len = 4; wc &= 0x07; }
    else if (wc < 0xfc) { len = 5; wc &= 0x03; }
    else                { len = 6; wc &= 0x01; }

    for (size_t i = 1; i < len; ++i)
    {
      wc <<= 6;
      wc |= ((Char8*)line)[i] & 0x3f;
    }
    return wc;
  }

  size_t String::WriteChar32(Char8 * pOut, Char32 ch)
  {
    size_t len = 0;    
    int first;
    
    if (ch < 0x80)
    {
      first = 0;
      len = 1;
    }
    else if (ch < 0x800)
    {
      first = 0xc0;
      len = 2;
    }
    else if (ch < 0x10000)
    {
      first = 0xe0;
      len = 3;
    }
    else if (ch < 0x200000)
    {
      first = 0xf0;
      len = 4;
    }
    else if (ch < 0x4000000)
    {
      first = 0xf8;
      len = 5;
    }
    else
    {
      first = 0xfc;
      len = 6;
    }

    for (int i = (int)len - 1; i > 0; --i)
    {
      pOut[i] = (ch & 0x3f) | 0x80;
      ch >>= 6;
    }
    pOut[0] = ch | first;     
    
    return len;
  }

  const Char8 * String::StrStr(const Char8 * str, const Char8 * strSearch)
  {
    assert(str && strSearch);
    if (strSearch[0] == 0) return (const Char8*)(str + strlen((const char*)str));
    return (const Char8*)strstr((const char*)str, (const char*)strSearch);
  }

  const int String::StrCmp(const Char8 * strLeft, const Char8 * strRight)
  {
    assert(strLeft && strRight);
    return strcmp((const char*)strLeft, (const char*)strRight);
  }
    
  bool String::IsTrimChar(Char8 ch)
  {
    return ch == ' ' || ch == '\r' || ch == '\n' || ch == 32 || ch == '\t' || ch == '\v' || ch == '\f';
  }

  size_t String::GetIntLen(int value)
  {
    assert(value >= 0);

    if(value < 10) return 1;
    if(value < 100) return 2;
    if(value < 1000) return 3;
    if(value < 10000) return 4;
    if(value < 100000) return 5;
    if(value < 1000000) return 6;
    if(value < 10000000) return 7;
    if(value < 100000000) return 8;
    if(value < 1000000000) return 9;
#if defined(_MSC_VER)
    if(value < 10000000000) return 10; 
    if(value < 100000000000) return 11;
    if(value < 1000000000000) return 12;
    if(value < 10000000000000) return 13;
    if(value < 100000000000000) return 14;
    if(value < 1000000000000000) return 15;
    if(value < 10000000000000000) return 16;
    if(value < 100000000000000000) return 17;
    if(value < 1000000000000000000) return 18;
    if(value < 10000000000000000000) return 19;
#else  //_MSC_VER
    if(value < 1000000000 * 10) return 10; 
    if(value < 1000000000 * 100) return 11;
    if(value < 1000000000 * 1000) return 12;
    if(value < 1000000000 * 10000) return 13;
    if(value < 1000000000 * 100000) return 14;
    if(value < 1000000000 * 1000000) return 15;
    if(value < 1000000000 * 10000000) return 16;
    if(value < 1000000000 * 100000000) return 17;
    if(value < 1000000000 * 1000000000) return 18;
    if(value < 1000000000 * 1000000000 * 10) return 19;
#endif //_MSC_VER
    assert(0);
    return 0;
  }

  unsigned long String::PowTen(size_t value)
  {
    switch(value)
    {
    case 0:  return 1;
    case 1:  return 10;
    case 2:  return 100;
    case 3:  return 1000;
    case 4:  return 10000;
    case 5:  return 100000;
    case 6:  return 1000000;
    case 7:  return 10000000;
    case 8:  return 100000000;
    case 9:  return 1000000000;
#if defined(_MSC_VER)
    case 10: return (unsigned long)10000000000; 
    case 11: return (unsigned long)100000000000;
    case 12: return (unsigned long)1000000000000;
    case 13: return (unsigned long)10000000000000;
    case 14: return (unsigned long)100000000000000;
    case 15: return (unsigned long)1000000000000000;
    case 16: return (unsigned long)10000000000000000;
    case 17: return (unsigned long)100000000000000000;
    case 18: return (unsigned long)1000000000000000000;
    case 19: return (unsigned long)10000000000000000000;
#else
    case 10: return (unsigned long)(1000000000 * 10); 
    case 11: return (unsigned long)(1000000000 * 100);
    case 12: return (unsigned long)(1000000000 * 1000);
    case 13: return (unsigned long)(1000000000 * 10000);
    case 14: return (unsigned long)(1000000000 * 100000);
    case 15: return (unsigned long)(1000000000 * 1000000);
    case 16: return (unsigned long)(1000000000 * 10000000);
    case 17: return (unsigned long)(1000000000 * 100000000);
    case 18: return (unsigned long)(1000000000 * 1000000000);
    case 19: return (unsigned long)(1000000000 * 1000000000 * 10);                                         
#endif //_MSC_VER                   
    }
    assert(0);
    return 0;
  }
  //

  const Char8 * String::Line() const 
  {
    static Char8 end = 0;
    return (pData != NULL) ? pData->pLine : &end;
  }

  size_t String::Size() const
  {
    return (pData != NULL) ? pData->size : 0;
  }

  bool String::IsLatin() const
  {
    return (pData != NULL) ? (pData->size == pData->realSize) : true;
  }

  String & String::Trim(bool bLeft, bool bRight)
  {
    if (pData)
    {
      size_t count = Count();
      size_t newCount = 0;
      size_t tmpCount = 0;

      const Char8 * pFrom = pData->pLine;
      size_t i;
      for(i = 0; bLeft && i < count && IsTrimChar(*pFrom); ++i) pFrom += Step(pFrom);
      const Char8 * pTo    = pFrom;
      const Char8 * pToEnd = pFrom;
      for(; i < count; ++i) 
      {
        ++tmpCount;
        size_t step = Step(pTo);
        if (!IsTrimChar(*pTo) || !bRight)
        {
          pToEnd = pTo + step;
          newCount = tmpCount;
        }
        pTo += step;
      }
      size_t newSize = pToEnd - pFrom;

      if (newSize != Size()) 
      {
        StrData * pNewData = CreateData(newSize);
        pNewData->size     = newSize;
        pNewData->realSize = newCount;
        MemCpy(pNewData->pLine, pFrom, newSize);
        pNewData->pLine[newSize] = 0;
        ReplaceData(pNewData);
      }
    }
    return *this; 
  }

