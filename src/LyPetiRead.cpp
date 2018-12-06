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
bool LyPetiRead::read(int* pos, std::vector<zdaq::buffer*>* result) {
	
  uint32_t theNumberOfDIF = 0;
  uint32_t event = 0;
  if (_fdIn > 0) {
    ::lseek(_fdIn, *pos, SEEK_SET);
    int ier = ::read(_fdIn, &event, sizeof(uint32_t));
    if (ier < 0) return false;
    ier = ::read(_fdIn, &theNumberOfDIF, sizeof(uint32_t));
    if (ier < 0 || !(theNumberOfDIF > 0)) return false;
    for (uint32_t idif = 0; idif < theNumberOfDIF; idif++) {
      result->push_back(new zdaq::buffer(0x100000));
      uint32_t bsize = 0;
      ier = ::read(_fdIn, &bsize, sizeof(uint32_t));
      if (ier < 0) return false;
      ier = ::read(_fdIn, result->back()->ptr(), bsize);
      if (ier < 0)
        return false;
      else
        *pos = lseek(_fdIn, 0, SEEK_CUR);
      result->back()->setPayloadSize(bsize - (3 * sizeof(uint32_t) + sizeof(uint64_t)));
      result->back()->uncompress();
      result->back()->setDetectorId(result->back()->detectorId() & 0xFF);

    }
  }
	else 
		return false;
  return true;
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
