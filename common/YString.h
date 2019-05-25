
#ifndef __STRING_STRING_H__
#define __STRING_STRING_H__

#include <assert.h>
#ifndef __APPLE__
    #include <malloc.h>
#endif
#include <string.h>
   
#if defined(WIN32) || defined(WIN64)
  #define NOMINMAX
  #include <Windows.h>
#endif

#include "RefCounter.h"
#include "Buffer.h"
#include "CaseFunctions.h"

  class String
  {
  public:
    size_t Count() const;

    Char32 GetAt(size_t pos, Char32 retIfFail = 0) const;
    bool SetAt(size_t pos, Char32 val);

    void Clear();

    String();
    String(const char    * line, size_t len = 0);
    String(const wchar_t * line, size_t len = 0);    
    ~String();

    String(const String & copy);
    void operator=(const String & copy);                                        

    // Oleg fix: 2010-05-24
    bool IsSet() const;
    bool IsEmpty() const;

    String & FromInt  (int   value);
    String & FromFloat(float value,  size_t afterPoint = 2, Char8 delimer = '.');
    String & FromBool (bool  value);

    String & FromLatin (const Char8   * str, size_t len = 0);
    String & FromUTF8  (const Char8   * str, size_t len = 0);
    String & FromUCS2  (const Char16  * str, size_t len = 0);
    String & FromUCS4  (const Char32  * str, size_t len = 0);
    String & FromLocale(const char    * str, size_t len = 0);
    String & FromWChar (const wchar_t * str, size_t len = 0);
                                  
    //
    const Buffer8  Latin() const;
    const Buffer8  UTF8()  const;
    const Buffer16 UCS2()  const;
    const Buffer32 UCS4()  const;
    const BufferChar  Locale() const;
    const BufferWChar WChar () const;

    int   Int  (int   retifFail = 0)     const;
    float Float(float retifFail = 0.0f)  const;
    bool  Bool (bool  retifFail = false) const; //@todo optimize

    //
    size_t PrintLatin (Char8   * str, size_t len) const;
    size_t PrintUTF8  (Char8   * str, size_t len) const;
    size_t PrintUTF8  (Char8   * str, size_t len, size_t * pRealSize) const;
    size_t PrintUCS2  (Char16  * str, size_t len) const;
    size_t PrintUCS4  (Char32  * str, size_t len) const;
    size_t PrintLocale(char    * str, size_t len) const;
    size_t PrintWChar (wchar_t * str, size_t len) const;

    //
    bool    Has(const String & line,                            bool bIgnoreCase = false) const;
    int    Find(const String & line,   size_t pos = 0,          bool bIgnoreCase = false) const;
    int   RFind(const String & search, size_t pos = (size_t)-1, bool bIgnoreCase = false) const; //@todo optimize
    int Compare(const String & line,                            bool bIgnoreCase = false) const;

    String & Replace(const String & from, const String & to, size_t startPos = 0, size_t maxCount = (size_t)-1); //@todo optimize
    String & SubStr (size_t pos, size_t len = (size_t)-1);

    String & Trim();
    String & LTrim();
    String & RTrim();
    
    String & ToUpper();
    String & ToLower();
    
    String & ConvertChars(Char32(*pFunc)(Char32));

    String & Append(const String & line);
    String & Insert(size_t pos, const String & line);

    String   operator+ (const String & str) const;
    inline String & operator+=(const String & str);

    bool operator==(const String & str) const;
    bool operator!=(const String & str) const;
    bool operator< (const String & str) const;
    bool operator<=(const String & str) const;
    bool operator> (const String & str) const;
    bool operator>=(const String & str) const;

  private:
    static void * Malloc(size_t size);
    static void   Free(void * pMem);                     
    static void * MemCpy(Char8 * pDest, const Char8 * pSrc, size_t count)
    {
      return memcpy((void *)pDest, (void *)pSrc, count);
    }

    struct StrData
    {
      RefConter refs;
      size_t    size;
      size_t    realSize;
      Char8   * pLine; // utf8
    };
    StrData * pData;

    void ReplaceData(StrData * pNewData);                                     
    static StrData * CreateData(size_t size);

    static size_t  Step(const Char8 * line);
    static size_t  Step(Char32 ch);

    const  Char8 * AtPos(size_t pos) const;
    Char8 * AtPos(size_t pos);

    static Char32 ReadChar32(const Char8 * line);
    static size_t WriteChar32(Char8 * pOut, Char32 ch);

    static const Char8 * StrStr(const Char8 * str,     const Char8 * strSearch);
    static const int     StrCmp(const Char8 * strLeft, const Char8 * strRight);

    template<class T>
    static size_t StrLen(const T * pLine)
    {
      const T * pLineOrig = pLine;
      while (*pLine) ++pLine;
      return (size_t)(pLine - pLineOrig);
    }

    static bool IsTrimChar(Char8 ch);

    static size_t GetIntLen(int value);
    static unsigned long PowTen(size_t value);

    const Char8 * Line() const;
    size_t Size() const;
    bool IsLatin() const;

    String & Trim(bool bLeft, bool bRight);

  }; // class String

#endif //__STRING_STRING_H__
