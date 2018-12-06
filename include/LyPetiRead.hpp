/*
  \author    Shchablo Konstantin
  \version   1.0
  \date      April 2018
  \copyright GNU Public License.
*/
#ifndef LYPETIREAD_h
#define LYPETIREAD_h 

#include <vector>
#include <string>
#include <fcntl.h>
#include <zlib.h>

#include "zmBuffer.hpp"
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
	bool read(int* pos, std::vector<zdaq::buffer*>* result);
	int size();
  //----------------------------------------------------------------------------
protected:
  int32_t _fdIn;
};
#endif 
