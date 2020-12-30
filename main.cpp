//------------------------------------------------------------------------------
// Description: main file
// Authors:  Shchablo, shchablo@gmail.com
//------------------------------------------------------------------------------

// C++ includes
#include <iostream>
#include <string>
#include <vector>

#include "LyPetiRead.hpp"
#include "SdhcalPmrAccess.hpp"
#include "TFile.h"

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
	    
			uint32_t* ibuf = (uint32_t*)buffers.at(i)->payload();
			uint32_t detId = buffers.at(i)->detectorId()&0xFF;
			
      if((detId == 120 || detId == 130  || detId == 150) && buffers.at(i)->payloadSize()) {
        // header ---
        uint32_t number = ibuf[0];
        uint32_t gtc = ibuf[1] & 0xFFFF;
        uint64_t absbcID = ((uint64_t)ibuf[3] << 32) | ibuf[2];
        uint32_t boardID = ibuf[4];
        uint8_t boardIP[4];
        boardIP[0] = ibuf[5] & 0xFF;
        boardIP[1] = (ibuf[5] >> 8) & 0xFF;
        boardIP[2] = (ibuf[5] >> 16) & 0xFF;
        boardIP[3] = (ibuf[5] >> 24) & 0xFF;
        uint32_t numOfCh = ibuf[6];
        
        // --- 
        printf("================================================================================\n");
        printf("number %d; GTC %d; ABSBCID %d; mezzanine number %d;",
               number, gtc, absbcID, boardID);
        printf(" IP address %d.%d.%d.%d;", boardIP[0], boardIP[1], boardIP[2], boardIP[3]);
        printf("\n payload: %d bytes;  channels or length -> %d \n", buffers.at(i)->payloadSize(), numOfCh);
        // ---
      }

      // data FEB v.0 and v.1.(1) MAY 2019 ---
      if(detId == 120 && ibuf[6] > 0) {
	    	uint8_t* cbuf = (uint8_t*)&ibuf[7];
	    	for(int j = 0; j < ibuf[6]; j++) {
          uint8_t ch = cbuf[8*j + 0];
          uint16_t bcid = cbuf[8*j + 2]|(((uint16_t)cbuf[8*j + 1])<<8);
          uint64_t coarse = ((uint64_t)cbuf[8*j + 6])|((uint64_t)cbuf[8*j + 5]<<8)|((uint64_t)cbuf[8*j + 4]<<16)|((uint64_t)cbuf[8*j + 3]<<24); 
          uint8_t fine = cbuf[8*j + 7];
          double time = (coarse + fine/256.0)*2.5;
	    	  
          printf("ch=%d; coarse=%lu, fine=%u -> time=%.3f[ns]; bcid=%d\n", ch, coarse, fine, time, bcid);
        }
	    }
      // ---
      
      // data FEB v.0 August 2018 ---
      if(detId == 130 && ibuf[6] > 0) {
	    	uint8_t* cbuf = (uint8_t*)&ibuf[7];
	    	for(int j = 0; j < ibuf[6]; j++) {
          uint8_t ch = cbuf[6*j + 0];
          uint8_t fine = cbuf[6*j + 5];
          uint64_t coarse = ((uint64_t)cbuf[6*j + 4])|((uint64_t)cbuf[6*j + 3]<<8)|((uint64_t)cbuf[6*j + 2]<<16)|((uint64_t)cbuf[6*j + 1]<<24); 
          double time = (coarse + fine/256.0)*2.5;
          uint16_t bcid = coarse*2.5/200;
	    	  
          printf("ch=%d; coarse=%lu, fine=%u -> time=%.3f[ns]; bcid=%d\n", ch, coarse, fine, time, bcid);
	      }
	    }
      // ---
      
      // data telescope (HARDROC) ---
      if(detId == 150 && buffers.at(i)->payloadSize()) {
	    	uint8_t* cbuf = (uint8_t*)&ibuf[0];
        sdhcal::PMRPtr* d = new sdhcal::PMRPtr(&cbuf[0],  buffers.at(i)->payloadSize());
        
        // DIF ---
        uint32_t id = d->getID();
        uint32_t gtc = d->getGTC();
        unsigned long long absoluteBCID = d->getAbsoluteBCID();
        uint32_t BCID = d->getBCID();
        uint32_t nFrames = d->getNumberOfFrames();
	    	
        printf("id=%d; gtc=%d, absoluteBCID=%llu, BCID=%d, nFrames=%d", id, gtc, absoluteBCID, BCID, nFrames);
        
        // FRAMEs ---
	    	for(uint32_t j = 0; j < d->getNumberOfFrames(); j++) {
          uint32_t fASIC = d->getFrameAsicHeader(j);
          uint32_t fBCID = d->getFrameBCID(j);
          unsigned fBCIDW = d->getFrameBCIDW(j);
          uint32_t fTimeToTrigger = d->getFrameTimeToTrigger(j);
          printf("\nasic=%d; bcid=%d \n", fASIC, fBCID);
          // PADs --- (Need to correct with level info)
          std::cout << " pads[0->63]: [";
          for(int ipad = 0; ipad < 64; ipad++) {
            if(d->getFrameLevel(j, ipad, 0) || d->getFrameLevel(j, ipad, 1)) // ? (A,B,C)
              std::cout << 1;
            else
              std::cout << 0;
          }
          std::cout << "]" << std::endl;
        }
	    }
    }
    // ---
    
    // clear memory form old buffers
    for(int i = 0; i < buffers.size(); i++)
      delete buffers.at(i);
    buffers.clear();
  }

  bool isClose = false; if(isOpen) isClose = stream.close();
  return 1;
}
