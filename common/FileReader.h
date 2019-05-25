/*
    File reader
 */

#ifndef __FILE_READER__
#define __FILE_READER__

#ifdef _WIN32
  #define NOMINMAX
  #include <Windows.h>
  #include <TCHAR.h>
#else
  #include <sys/stat.h>
#endif 
#include <stdio.h>
#include <string>
#include "YString.h"

class FileReader
{
  private: FILE* m_pFile;
  private: String m_strFileName;

  /**
   * constructor
   */
  public: FileReader()
  {
    m_pFile = NULL;
  }


  /**
   * destructor
   */
  public: ~FileReader()
  {
    Release();
  }


  /**
   * release data
   * 
   */
  public: void Release()
  {
    closeFile();
    m_pFile = NULL;
	m_strFileName.Clear();
  }


  /**
   * Open file
   * 
   */
  public: bool openFile(const String& fileName)
  {
	  m_pFile = fopen((const char*)fileName.UTF8().Data(), "rb");
	  m_strFileName = fileName;
	  return m_pFile != NULL;
  }


  /**
   * Close file
   * 
   */
  public: void closeFile()
  {
    if(m_pFile)
    {
      fclose(m_pFile);
      m_pFile = NULL;
    }
  }


  /**
   * Read data from file
   * 
   */
  public: unsigned long readData(const char* data, unsigned long sizeBuffer)
  {
    unsigned long res = 0;
    if(m_pFile)
    {
      res = (unsigned long)fread((void* )data, 1, sizeBuffer, m_pFile);
    }
    return res;
  }


 /**
  * Get file size
  *
  */
  public: unsigned long getFileSize()
  {    
    unsigned long size = -1;

#ifdef _WIN32
    if (!m_strFileName.IsEmpty())
    {
		HANDLE hFile = CreateFile(m_strFileName.WChar().Data(), GENERIC_READ, FILE_SHARE_READ, NULL,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

      if (hFile != INVALID_HANDLE_VALUE)
      {
        unsigned long sizeHigh = 0;
        unsigned long sizeLow  = GetFileSize(hFile, &sizeHigh);
        if (sizeLow != 0xFFFFFFFF || GetLastError() == NO_ERROR)
        {
          LARGE_INTEGER li;
          li.LowPart  = sizeLow;
          li.HighPart = sizeHigh;
          size = (long)li.QuadPart;
        }
        CloseHandle(hFile);
      }
    }
#else
    struct stat statbuf;

    if (stat((char *)m_strFileName.UTF8().Data(), &statbuf) != -1) 
    {
      size = statbuf.st_size;
    }

#endif
    return size;
  }
};

#endif
