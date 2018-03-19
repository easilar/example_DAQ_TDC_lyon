#ifndef LYPETIREAD_h
#define LYPETIREAD_h 

#include <vector>
#include <string>
#include <fcntl.h>
#include <zlib.h>

#include "zmBuffer.hpp"
/* How to use ------------------------------------------------------------------
  	LyPetiRead stream;
    stream.open(pathToFile);
    int codeRead = 1;
    int pos = 0;
    zdaq::buffer buf(0x100000); // raw data buf
    while(codeRead > 0) {
      codeRead = stream.read(&pos, &buf);
			// Do your Analysis here! 
		}
    stream.close();
------------------------------------------------------------------------------*/
class LyPetiRead
{
public:
  /* Constructor, destructor. */ 
  LyPetiRead();
  virtual ~LyPetiRead();
  //----------------------------------------------------------------------------
  
/* Functions. */ 
  bool open(std::string filename);
  bool close();
	int read(int* pos, std::vector<zdaq::buffer*>* result);
	int size();
  //----------------------------------------------------------------------------
  
protected:
  
  int32_t _fdIn;

};
#endif 
