
#ifndef __STRING_REFCOUNTER_H__
#define __STRING_REFCOUNTER_H__

class RefConter //@todo make thread safe
{
private:
  size_t count;
public:
  void Init(size_t startCount)
  {
    count = startCount;
  }
  size_t Count()
  {
    return count;
  }
  size_t Inc()
  {
    return ++count;
  }
  size_t Dec()
  {
    return --count;
  }
}; // class RefConter


#endif //__STRING_REFCOUNTER_H__
