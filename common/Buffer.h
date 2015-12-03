
#ifndef __STRING_BUFFER_H__
#define __STRING_BUFFER_H__

#include <assert.h>
#ifndef __APPLE__
    #include <malloc.h>
#endif

typedef unsigned char  Char8;
typedef unsigned short Char16;
typedef unsigned int   Char32;

template<typename T> struct StrDataTemplate
  {
    size_t refs;
    size_t size;
    T   * pLine;
  };


template<typename T> 
class BaseBuffer
{
friend class BaseBuffer;
public:
  typedef StrDataTemplate<T> StrData;

  StrData* pData;

protected:

  static StrData * CreateData(size_t size)
  {                        
    StrData * pData = (StrData*)malloc(sizeof(StrData) + size * sizeof(T));
    pData->pLine = (T*)(pData + 1);
    pData->refs  = 1;
    pData->size  = size;
    return pData;
  }

  void Clear()
  {
    if (pData)
    {
      assert(pData->refs > 0);
      if (--(pData->refs) == 0) 
      {
        free((void*)pData);
      }
      pData = NULL;
    }
  }

  BaseBuffer(): pData(NULL) {}

  BaseBuffer(const BaseBuffer & copy): pData(NULL)
  {
    if (copy.pData) 
    {
      pData = copy.pData;
      ++(pData->refs);
    }
  }
public:

  ~BaseBuffer() 
  { 
    Clear(); 
  }

  const T * Data() const
  {        
    static T deflt = 0;
    return (pData != NULL) ? pData->pLine : &deflt;
  }

  size_t Size() const
  {
    return (pData != NULL) ? (pData->size - 1) : 0;
  }
}; // class BaseBuffer

template<typename T>
class Buffer: public BaseBuffer<T>
{
friend class String;
friend class Buffer;
private:

  template<class TT>
  void InitFrom(const Buffer<TT> & copyBuffer) 
  {
    BaseBuffer<T>::Clear();
    if (sizeof(T) == sizeof(TT))
    {
      this->pData = (StrDataTemplate<T>*)(copyBuffer.pData);
      if (BaseBuffer<T>::pData) ++(BaseBuffer<T>::pData->refs);
    }
    else
    {
      assert(0);
    }
  }

  template<class TT>
  void Init(size_t size, const TT * pSrc, size_t (TT::*Print)(T*, size_t) const)
  {
    BaseBuffer<T>::Clear();
    BaseBuffer<T>::pData = BaseBuffer<T>::CreateData(size);
    (pSrc->*Print)(BaseBuffer<T>::pData->pLine, BaseBuffer<T>::pData->size);
  }
}; // class Buffer


typedef Buffer<Char8>  Buffer8;
typedef Buffer<Char16> Buffer16;
typedef Buffer<Char32> Buffer32;

typedef Buffer<char>    BufferChar;
typedef Buffer<wchar_t> BufferWChar;


#endif //__STRING_BUFFER_H__
