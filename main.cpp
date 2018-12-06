//------------------------------------------------------------------------------
// Description: main file
// Authors:  Shchablo, shchablo@gmail.com
//------------------------------------------------------------------------------

// C++ includes
#include <iostream>
#include <string>
#include <vector>

#include "LyPetiRead.hpp"

int main(int argc, char* argv[]) {
	LyPetiRead stream;
  bool isOpen = false; isOpen = stream.open(argv[1]);

	int size = stream.size();
  int pos = 0;
  bool isRead = true;
  std::vector<zdaq::buffer*> buffers; // raw data
  while(isRead) {
    isRead = stream.read(&pos, &buffers); // read file

    // Do analysis here (example of output data)
	  printf("================================================================================\n");
    for(int i = 0; i < buffers.size(); i++) {
      uint32_t detId = buffers.at(i)->detectorId()&0xFF;

      // header ---
	    uint32_t* ibuf = (uint32_t*)buffers.at(i)->payload();
	    int absbcid = ibuf[3];
	    absbcid = (absbcid << 32) | ibuf[2];
	  printf("================================================================================\n");
	    printf("number %d; GTC %d; ABSBCID %lu; mezzanine number %u;",
	           ibuf[0], ibuf[1] & 0xFFFF, absbcid, ibuf[4]);
	    printf("IP address %u.%u.%u.%u;", ibuf[5] & 0xFF, (ibuf[5] >> 8) & 0xFF,
	           (ibuf[5] >> 16) & 0xFF, (ibuf[5] >> 24) & 0xFF);
	    uint32_t nch = ibuf[6];
	    printf("\n channels -> %d \n", nch);
	    // ---

      // data FEB v.0 MAY ---
      if(detId == 120 && ibuf[6] > 0) {
	    	uint8_t* cbuf = ( uint8_t*)&ibuf[7];
	    	for (int j = 0; j < ibuf[6]; j++) {
          uint8_t ch = cbuf[8*j + 0];
          uint16_t bcid = cbuf[8*j + 2]|(((uint16_t)cbuf[8*j + 1])<<8);
          uint64_t coarse = ((uint64_t)cbuf[8*j + 6])|((uint64_t)cbuf[8*j + 5]<<8)|((uint64_t)cbuf[8*j + 4]<<16)|((uint64_t)cbuf[8*j + 3]<<24); 
          uint8_t fine = cbuf[8*j + 7];
          double time = (coarse + fine/256.0)*2.5;
	    	  printf("ch=%d; coarse=%lu, fine=%u -> time=%.3f[ns]; bcid=%d\n", ch, coarse, fine, time, bcid);
	      }
	    }
      // ---
      // data FEB v.0 August ---
      if(detId == 130 && ibuf[6] > 0) {
	    	uint8_t* cbuf = ( uint8_t*)&ibuf[7];
	    	for (int j = 0; j < ibuf[6]; j++) {
          uint8_t ch = cbuf[6*j + 0];
          uint8_t fine = cbuf[6*j + 5];
          uint64_t coarse = ((uint64_t)cbuf[6*j + 4])|((uint64_t)cbuf[6*j + 3]<<8)|((uint64_t)cbuf[6*j + 2]<<16)|((uint64_t)cbuf[6*j + 1]<<24); 
          double time = (coarse + fine/256.0)*2.5;
          uint16_t bcid = coarse*2.5/200;
	    	  printf("ch=%d; coarse=%lu, fine=%u -> time=%.3f[ns]; bcid=%d\n", ch, coarse, fine, time, bcid);
	      }
	    }
      // ---
    }
    // clear memory form old buffers
    for(int i = 0; i < buffers.size(); i++)
      delete buffers.at(i);
    buffers.clear();
  }

  bool isClose = false; if(isOpen) isClose = stream.close();
  return 1;
}
