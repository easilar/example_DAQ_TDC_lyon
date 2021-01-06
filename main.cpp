//------------------------------------------------------------------------------
// Description: main file
// Authors:  Shchablo, shchablo@gmail.com ; Asilar, ece.asilar@cern.ch
//------------------------------------------------------------------------------

// C++ includes
#include <iostream>
#include <string>
#include <vector>

#include "LyPetiRead.hpp"
#include "SdhcalPmrAccess.hpp"
#include "TFile.h"
#include "TTree.h"

int main(int argc, char* argv[]) {
	 
  LyPetiRead stream;
  bool isOpen = false; isOpen = stream.open(argv[1]);

  int size = stream.size();
  int pos = 0;
  bool isRead = true;
  std::vector<zdaq::buffer*> buffers; // raw data

  TFile f("Detector_Output_Trees.root","RECREATE"); // Final file for output
  TTree tPet("tPet","Tree with pseudo Events"); // Tree for petiroc
  TTree tTele("tTele","Tree with pseudo Events"); // Tree for Telescope 
  
  Int_t nBuf;
  Int_t detID;
  uint32_t number; 
  uint32_t numOfCh;
  uint32_t gtc;
  uint64_t absbcID;
  uint32_t boardID;
  double payload;
  const Int_t maxnCh = 1000;
  uint8_t ch[maxnCh];
  uint16_t bcid[maxnCh];
  uint64_t coarse[maxnCh];
  uint8_t fine[maxnCh];
  double time[maxnCh];

  //Branches for petiroc
  tPet.Branch("nBuf",&nBuf,"nBuf/I");
  tPet.Branch("detID",&detID,"detID/I");
  tPet.Branch("number",&number,"number/I");
  tPet.Branch("gtc",&gtc,"gtc/I");
  tPet.Branch("absbcID",&absbcID,"absbcID/I");
  tPet.Branch("boardID",&boardID,"boardID/I");
  tPet.Branch("numOfCh",&numOfCh,"numOfCh/I");
  tPet.Branch("ch",ch,"ch[numOfCh]/I");
  tPet.Branch("time",time,"time[numOfCh]/F");
  tPet.Branch("coarse",coarse,"coarse[numOfCh]/F");
  tPet.Branch("fine",fine,"fine[numOfCh]/F");
  tPet.Branch("bcid",bcid,"bcid[numOfCh]/F");
  tPet.Branch("payload",&payload,"payload/F");
  //Branches for telescop
  tTele.Branch("nBuf",&nBuf,"nBuf/I");
  tTele.Branch("detID",&detID,"detID/I");
  tTele.Branch("number",&number,"number/I");
  tTele.Branch("gtc",&gtc,"gtc/I");
  tTele.Branch("absbcID",&absbcID,"absbcID/I");
  tTele.Branch("boardID",&boardID,"boardID/I");
  //tTele.Branch("numOfCh",&numOfCh,"numOfCh/I");

  f.cd();
  
  int mycounter = 0; 
  while(isRead) {
    isRead = stream.read(&pos, &buffers); // read file
    printf("mycounter");
    printf("%d" , mycounter);
    mycounter++;
    printf("\n");
    nBuf = buffers.size();
    // Do analysis here (example of output data)
    printf("================================================================================\n");
    for(int i = 0; i < nBuf; i++) {
	    
			uint32_t* ibuf = (uint32_t*)buffers.at(i)->payload();
			uint32_t detId = buffers.at(i)->detectorId()&0xFF;
			detID = detId;	
			payload = buffers.at(i)->payloadSize();
      if((detId == 120 || detId == 130  || detId == 150) && payload) {
        // header ---
        number = ibuf[0];
        gtc = ibuf[1] & 0xFFFF;
        absbcID = ((uint64_t)ibuf[3] << 32) | ibuf[2];
        boardID = ibuf[4];
        uint8_t boardIP[4];
        boardIP[0] = ibuf[5] & 0xFF;
        boardIP[1] = (ibuf[5] >> 8) & 0xFF;
        boardIP[2] = (ibuf[5] >> 16) & 0xFF;
        boardIP[3] = (ibuf[5] >> 24) & 0xFF;
        numOfCh = ibuf[6];
        
        // --- 
        printf("================================================================================\n");
        printf("number %d; GTC %d; ABSBCID %d; mezzanine number %d; detID %d",
               number, gtc, absbcID, boardID, detId);
        printf(" IP address %d.%d.%d.%d;", boardIP[0], boardIP[1], boardIP[2], boardIP[3]);
        printf("\n payload: %d bytes;  channels or length -> %d \n", payload, numOfCh);
        // ---
      }

      // data FEB v.0 and v.1.(1) MAY 2019 ---
      if(detId == 120 && ibuf[6] > 0) {
	    	uint8_t* cbuf = (uint8_t*)&ibuf[7];
	    	for(int j = 0; j < ibuf[6]; j++) {
          ch[j] = cbuf[8*j + 0];
          bcid[j] = cbuf[8*j + 2]|(((uint16_t)cbuf[8*j + 1])<<8);
          coarse[j] = ((uint64_t)cbuf[8*j + 6])|((uint64_t)cbuf[8*j + 5]<<8)|((uint64_t)cbuf[8*j + 4]<<16)|((uint64_t)cbuf[8*j + 3]<<24); 
          fine[j] = cbuf[8*j + 7];
          time[j] = (coarse[j] + fine[j]/256.0)*2.5;
	    	  
          printf("ch=%d; coarse=%lu, fine=%u -> time=%.3f[ns]; bcid=%d\n", ch[j], coarse[j], fine[j], time[j], bcid[j]);
	  tPet.Fill();
        }
	    }
      // ---
      
      // data FEB v.0 August 2018 ---
      if(detId == 130 && ibuf[6] > 0) {
	    	uint8_t* cbuf = (uint8_t*)&ibuf[7];
	    	for(int j = 0; j < ibuf[6]; j++) {
          ch[j] = cbuf[6*j + 0];
          fine[j] = cbuf[6*j + 5];
          coarse[j] = ((uint64_t)cbuf[6*j + 4])|((uint64_t)cbuf[6*j + 3]<<8)|((uint64_t)cbuf[6*j + 2]<<16)|((uint64_t)cbuf[6*j + 1]<<24); 
          time[j] = (coarse[j] + fine[j]/256.0)*2.5;
          bcid[j] = coarse[j]*2.5/200;
	    	  
          printf("ch=%d; coarse=%lu, fine=%u -> time=%.3f[ns]; bcid=%d\n", ch[j], coarse[j], fine[j], time[j], bcid[j]);
	  tPet.Fill();
	      }
	    }
      // ---

      // data telescope (HARDROC) ---
      if(detId == 150 && buffers.at(i)->payloadSize()) {
	    	uint8_t* cbuf = (uint8_t*)&ibuf[0];
        sdhcal::PMRPtr* d = new sdhcal::PMRPtr(&cbuf[0],  buffers.at(i)->payloadSize());
        
        // DIF ---
        uint32_t id = d->getID();
        gtc = d->getGTC();
        unsigned long long absoluteBCID = d->getAbsoluteBCID();
        uint32_t BCID = d->getBCID();
        uint32_t nFrames = d->getNumberOfFrames();
	    	
        printf("id=%d; gtc=%d, absoluteBCID=%llu, BCID=%d, nFrames=%d", id, gtc, absoluteBCID, BCID, nFrames);
        number = id; 
	absbcID = absoluteBCID;
	boardID = BCID;
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
    		tTele.Fill();
	    }
    }
    // ---
    
    // clear memory form old buffers
    for(int i = 0; i < buffers.size(); i++)
      delete buffers.at(i);
    buffers.clear();
  }

  tPet.Write();
  tTele.Write();
  f.Write();
  f.Close();

  bool isClose = false; if(isOpen) isClose = stream.close();
  return 1;
}
