#include "LyPetiRead.hpp"

/* Constructor, destructor. */ 
LyPetiRead::LyPetiRead() {
	_fdIn = -1;
}
LyPetiRead::~LyPetiRead() {
}
//------------------------------------------------------------------------------

/* Functions. */ 
bool LyPetiRead::open(std::string filename) {
  _fdIn = ::open(filename.c_str(), O_RDONLY | O_NONBLOCK, S_IRWXU);
  if (_fdIn < 0) return false;
  return true;
}
bool LyPetiRead::close() {
  if (_fdIn > 0) {
    ::close(_fdIn);
    _fdIn = -1;
  } else
    return false;
  return true;
}

/*! \fn LyPetiRead::read
* \brief Function for read raw data for daq.
* \      Format zmBuffer.
*
* \param int* pos - position indicator associated with the current stream;
* \param zdaq::buffer* buffer - raw data buffer.
*
* \return int - code of state:
*															0: Can't read or Can't read anymore;
*														 -1: Can't read number of DIF;
*														 -2: Can't read size of DIf;
*														 -3: Can't read data.	 
*/
int LyPetiRead::read(int* pos, std::vector<zdaq::buffer*>* result) {
	
  uint32_t theNumberOfDIF = 0;
  uint32_t event = 0;
  if (_fdIn > 0) {
    ::lseek(_fdIn, *pos, SEEK_SET);
    int ier = ::read(_fdIn, &event, sizeof(uint32_t));
    if (ier < 0) return 0;
    ier = ::read(_fdIn, &theNumberOfDIF, sizeof(uint32_t));
    if (ier < 0 || !(theNumberOfDIF > 0)) return -1;
    for (uint32_t idif = 0; idif < theNumberOfDIF; idif++) {
      result->push_back(new zdaq::buffer(0x100000));
      uint32_t bsize = 0;
      ier = ::read(_fdIn, &bsize, sizeof(uint32_t));
      if (ier < 0) return -2;
      ier = ::read(_fdIn, result->back()->ptr(), bsize);
      if (ier < 0)
        return -3;
      else
        *pos = lseek(_fdIn, 0, SEEK_CUR);
      result->back()->setPayloadSize(bsize - (3 * sizeof(uint32_t) + sizeof(uint64_t)));
      result->back()->uncompress();
      result->back()->setDetectorId(result->back()->detectorId() & 0xFF);

    }
  }
	else 
		return 0;
  return 1;
}
int LyPetiRead::size() {
	int size = 0;
	int pos = 0;
  if (_fdIn > 0) {
  	pos = ::lseek(_fdIn, 0, SEEK_CUR);
    size = ::lseek(_fdIn, 0, SEEK_END);
    ::lseek(_fdIn, pos, SEEK_SET);
  } else
    size = 0;
  return size;
}
//------------------------------------------------------------------------------
