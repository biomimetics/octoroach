#include "utils.h"
#include "dfmem.h"
#include "dfmem_extra.h"
#include "payload.h"
#include "radio.h"
#include "at86rf231.h"
#include "ipspi1.h"

unsigned char currentBuffer = 0;
unsigned int currentBufferLength = 0;
unsigned int nextPage = 0;

extern int samplesToSave;

extern volatile char g_last_ackd;

//int flags[45];

//////////////// These shouldn't be here
#define TRX_CMD_RW          (0xC0) // Register Write
#define TRX_CMD_RR          (0x80) // Register Read 
#define TRX_CMD_FW          (0x60) // Frame Transmit Mode
#define TRX_CMD_FR          (0x20) // Frame Receive Mode
#define TRX_CMD_SW          (0x40) // SRAM Write.
#define TRX_CMD_SR          (0x00) // SRAM Read.
extern byte phyReadReg(byte addr);
extern byte phyReadBit(byte addr, byte mask, byte pos);
///////////////////////////

#if defined(__RADIO_HIGH_DATA_RATE)
	#define READBACK_DELAY_TIME_MS 2
#else
	#define READBACK_DELAY_TIME_MS 10
#endif


// Erase entire DataFlash memory sector by sector (LED to signal erasing)
void eraseDFMemSectors0a0b(void)
{	
	//_LATB14 = 1;
	LED_2 = 1;
	//erase first three sectors
	//This erases enough for 3584 samples
	dfmemEraseSector(0);
	dfmemEraseSector(8);
	dfmemEraseSector(256);
	while(!dfmemIsReady()){
		LED_2 = ~LED_2;
		delay_ms(100);
	}
	LED_2 = 0;
	currentBuffer = 0;
	currentBufferLength = 0;
	nextPage = 0;
	//for (page = 128; page < MAXPAGES; page += 128)
	//{
	//	dfmemEraseSector(page);
	//}
	//_LATB14 = 0;
}

//Erases enough sectors to fit a specified number of samples into the flash
void eraseDFMemSectorsForSamples(unsigned int numSamples)
{	
	//Todo: Add a check to see if the number of saved samples will fit into memory!
	LED_2 = 1;
	int i = PACKETSIZE;
	unsigned int firstPageOfSector;
	
	//avoid trivial case
	if(numSamples == 0){ return;}	

	//If you don't understand the math here, too bad. Go ask a grad student about it.
	//Saves to dfMem will NOT overlap page boundaries, so we need to do this level by level:
	unsigned int samplesPerPage = BYTES_PER_PAGE / PACKETSIZE; //round DOWN int division
	unsigned int numPages = (numSamples + samplesPerPage - 1) / samplesPerPage; //round UP int division
	unsigned int numBlocks = (numPages + PAGES_PER_BLOCK - 1 ) / PAGES_PER_BLOCK; //round UP int division
	unsigned int numSectors = (numBlocks + BLOCKS_PER_SECTOR - 1) / BLOCKS_PER_SECTOR; //round UP int division
	
	//At this point, it is impossible for numSectors == 0
	//Sector 0a and 0b will be erased together always, for simplicity
	//Note that numSectors will be the actual number of sectors to erase,
	//   even though the sectors themselves are numbered starting at '0'
	dfmemEraseSector(0); //Erase Sector 0a
	dfmemEraseSector(8); //Erase Sector 0b
	//Start erasing the rest from Sector 1:
	for(i=1; i < numSectors; i++){
		firstPageOfSector = PAGES_PER_BLOCK * BLOCKS_PER_SECTOR * i;
		dfmemEraseSector(firstPageOfSector);
	}
	
	//Flash LED while memory is busy
	while(!dfmemIsReady()){
		LED_2 = ~LED_2;
		delay_ms(100);
	}
	LED_2 = 0;
	currentBuffer = 0;
	currentBufferLength = 0;
	nextPage = 0;
}



void readDFMemBySample(unsigned int num)
{
	unsigned int page, bufferByte;// maxpage;
	unsigned char dataPacket[PACKETSIZE + PKT_INDEX_SIZE];
	//unsigned long bytesleft = PACKETSIZE * num;
	unsigned long telem_index = 0;
	int delaytime_ms = READBACK_DELAY_TIME_MS;

	unsigned int i;

	//maxpage = (num * PACKETSIZE) / BUFFERSIZE + 1;
	bufferByte = 0;
	page = 0;


	_LATB13 = 1;
	_T1IE = 0; _T5IE=0;
	while(!dfmemIsReady());
	
	for(i = 0; i < num; i++){
		//advance page number, if necceasry
		if((bufferByte + PACKETSIZE) >= BUFFERSIZE){
			bufferByte = 0;
			page++;
		}
		//flags[i] = 1;
		//Actually pull data from flash chip
		//start writingn to packet after 4 bytes, allowing space for index number
		dfmemRead(page, bufferByte, PACKETSIZE, dataPacket + PKT_INDEX_SIZE);
		//Set index number
		*(unsigned long*)(dataPacket) = telem_index;
		//Reliable send, with linear backoff
		g_last_ackd = 0;
		do{
			sendDataDelay(PACKETSIZE + PKT_INDEX_SIZE, dataPacket, delaytime_ms);
			//trx_status = phyReadBit(SR_TRAC_STATUS);
			delaytime_ms += 2;
		} while(g_last_ackd == 0);
		delaytime_ms = READBACK_DELAY_TIME_MS;
		telem_index++;

		//advance buffer location
		bufferByte += PACKETSIZE;
	}
	_T1IE = 1; _T5IE=1;
	_LATB13 = 0;
}


void sendDataDelay(unsigned char data_length, unsigned char* data, int delaytime_ms)
{
	// Create Payload, set status and type (don't cares)
	Payload pld = payCreateEmpty(data_length);
	//////    FIX THIS //////////
    paySetType(pld, 0x89);			// Don't Care
    paySetStatus(pld, 0);		// Don't Care
    
	// Set Payload data
	paySetData(pld, data_length, data);
    
	// Send Payload WITH 15ms DELAY
	// Handles pld delete: Assigns pointer to payload in packet
	//    and radio command deletes payload, then packet.
	radioSendPayload(macGetDestAddr(), pld);
	delay_ms(delaytime_ms); 	// allow radio transmission time
}

void saveTelemData(telemU *data){
	if (nextPage < MAXPAGES)
	{
		// If buffer full, write to next page
		if ((currentBufferLength + PACKETSIZE) >= BUFFERSIZE)
		{
			// Transfer current buffer to next page (if available)
			dfmemWriteBuffer2MemoryNoErase(nextPage, currentBuffer);
			++nextPage;
	
			// Toggle and reset buffer
			currentBuffer = (currentBuffer) ? 0 : 1;
			currentBufferLength = 0;
		}

		dfmemWriteBuffer(data->dataArray, PACKETSIZE, currentBufferLength, currentBuffer);
		currentBufferLength += PACKETSIZE;
		
		if(samplesToSave == 0){
			//Done sampling, commit last buffer
			dfmemWriteBuffer2MemoryNoErase(nextPage, currentBuffer);
		}
	}
}

////////////////////////// fix this
/*byte phyReadReg(byte addr) {
    byte c;
    ipspi1ChipSelect(FALSE);
    ipspi1PutByte(TRX_CMD_RR | addr);
    c = ipspi1GetByte();
    ipspi1ChipSelect(TRUE);
    return c;
}
byte phyReadBit(byte addr, byte mask, byte pos) {

    byte data;
    data = phyReadReg(addr);
    data &= mask;
    data >>= pos;
    return data;
}
*/
