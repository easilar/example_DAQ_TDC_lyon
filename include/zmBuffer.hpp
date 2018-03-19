#ifndef _zdaq_buffer_h
#define _zdaq_buffer_h

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <iostream>
namespace zdaq {
/**
  \class buffer
  \brief the buffer is the basic data object exchanged between process
  \details The buffer structure consists of
      - the detector id (int32_t)
      - the datasource id (int32_t)
      - the event id (int32_t)
      - the bunch crossing id (int64_t)
      - the payload ( char array of (buffer size - 20)
  \author    Laurent Mirabito
  \version   1.0
  \date      May 2016
  \copyright GNU Public License.
*/
/* Output buf example ----------------------------------------------------------
	  uint32_t* ibuf=(uint32_t*) b.payload();
		int absbcid = ibuf[3];
		absbcid = (absbcid << 32) | ibuf[2];
		printf("\n number %d, GTC %d, ABSBCID %lu, mezzanine number %u,",
		       ibuf[0], ibuf[1] & 0xFFFF, absbcid, ibuf[4]);
		printf("IP address %u.%u.%u.%u,", ibuf[5] & 0xFF, (ibuf[5] >> 8) & 0xFF,
		       (ibuf[5] >> 16) & 0xFF, (ibuf[5] >> 24) & 0xFF);
		uint32_t nch = ibuf[6];
		printf("\n channels -> %d \n", nch);
		if (ibuf[6] > 0) {
			uint8_t* cbuf = ( uint8_t*)&ibuf[7];
			for (int i = 0; i < ibuf[6]; i++) {
					printf("ch=%d", cbuf[8*i]);
		  	}
		}
------------------------------------------------------------------------------*/

class buffer {
 public:
  /**
    \brief  Constructor
     \param max_size , the size of the allocated buffer
     The buffer data are allocated and free when the buffer is destroyed
   */
  buffer(uint32_t max_size) : _allocate(true) {
    _ptr = new char[max_size];
    _iptr = (uint32_t*)_ptr;
    _i64ptr = (uint64_t*)&_ptr[3 * sizeof(uint32_t)];
    _psize = 0;
  }
  /**
    \brief Constructor
    \param ptr , char pointer to a memmory
    \param offset , offste in the ptr array wher the buffer is mapped
    No allocation / deallocation is made
  */
  buffer(char* ptr, uint32_t offset) : _allocate(false) {
    _ptr = &ptr[offset];
    _iptr = (uint32_t*)_ptr;
    _i64ptr = (uint64_t*)&_ptr[3 * sizeof(uint32_t)];
    _psize = 0;
  }
  /**
    \brief  Destructor
     The memmory is free if it was allocated at creation
   */
  ~buffer() {
    if (_allocate) {
      delete _ptr;
    }
  }
  /**
     \brief Set the detector id
     \param id, the detetcor id
  */
  void setDetectorId(uint32_t id) { _iptr[0] = id; }
  /**
     \brief Set the datasource id
     \param id, the source id
  */
  void setDataSourceId(uint32_t id) { _iptr[1] = id; }
  /**
     \brief Set the event id
     \param id, the event id
  */
  void setEventId(uint32_t id) { _iptr[2] = id; }
  /**
     \brief Set the bunch crossing  id
     \param int64_t id, the bx id
  */
  void setBxId(uint64_t id) { _i64ptr[0] = id; }
  /**
     \brief Set the payload and its size
     \param payload, pointer to the data to be copied in the buffer
     \param payload_size , the size of the payload
     the payload_size+20 should be smaller than the buffer size
  */
  void setPayload(void* payload, uint32_t payload_size) {
    memcpy(&_ptr[3 * sizeof(uint32_t) + sizeof(uint64_t)], payload,
           payload_size);
    _psize = payload_size;
  }
  //! set the payload size
  void setPayloadSize(uint32_t s) { _psize = s; }
  //! Detector id
  uint32_t detectorId() { return _iptr[0]; }
  //! Data source id
  uint32_t dataSourceId() { return _iptr[1]; }
  //! Event number
  uint32_t eventId() { return _iptr[2]; }
  //! Bunch crossing id
  uint64_t bxId() { return _i64ptr[0]; }
  //! Pointer to the whole buffer
  char* ptr() { return _ptr; }
  //! full buffer size
  uint32_t size() { return _psize + 3 * sizeof(uint32_t) + sizeof(uint64_t); }
  //! pointer to te payload
  char* payload() { return &_ptr[3 * sizeof(uint32_t) + sizeof(uint64_t)]; }
  //! payload size
  uint32_t payloadSize() { return _psize; }
  //! compress (gzip) the payload (size < 128k)
  int compress() {
		int rc = 0;
    unsigned char obuf[0x20000];
    unsigned long ldest = 0x20000;
    rc = ::compress(obuf, &ldest, (unsigned char*)payload(), payloadSize());
    // std::cout<<_psize<<" "<<ldest<<std::endl;
    memcpy(payload(), obuf, ldest);
    _psize = ldest;
		return rc;
  }
  //! uncompress the payload
  int uncompress() {
		int rc = 0;
    unsigned char obuf[0x20000];
    unsigned long ldest = 0x20000;
    rc = ::uncompress(obuf, &ldest, (unsigned char*)payload(), payloadSize());
    // std::cout<<_psize<<" "<<ldest<<std::endl;
    memcpy(payload(), obuf, ldest);
    _psize = ldest;
		return rc;
  }

 private:
  bool _allocate;
  char* _ptr;
  uint32_t* _iptr;
  uint64_t* _i64ptr;
  uint32_t _psize;
};
};
#endif
