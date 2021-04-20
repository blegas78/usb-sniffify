// SPDX-License-Identifier: GPL-2.0-only

#include <cstdio>
#include <sys/time.h>

#include "raw-gadget.hpp"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

class DualsenseSniffer : public EndpointObserver {
public:
	DualsenseSniffer() {
		count = 0;
		totalSamples = 1;
		gettimeofday(&oldTime, NULL);
	}
private:
	int count;
	int totalSamples;
	bool shouldHold;
	double dtime;
	struct timeval oldTime;
	void notification(unsigned char* buffer, int length) {
		count++;
		if(count >= totalSamples) {
			struct timeval currentTime;
			gettimeofday(&currentTime, NULL);
			
			double difftime = (currentTime.tv_sec - oldTime.tv_sec) +
			(currentTime.tv_usec - oldTime.tv_usec)*0.000001;
			
			
			printf("Read: ");
			for (int i = 0; i < length; i++) {
				printf("%02X ", buffer[i]);
			}
			printf("\n");
//
//			printf("Reverse: ");
//			for (int i = length-1; i >= 0; i--) {
//				printf("%02X ", buffer[i]);
//			}
//			printf("\n");
//
//			printf("Compact: ");
//			for (int i = 0; i < length; i++) {
//				printf("%02X", buffer[i]);
//			}
//			printf("\n");
//			printf("Compact Reverse: ");
//			for (int i = length-1; i >= 0; i--) {
//				printf("%02X", buffer[i]);
//			}
//			printf("\n");
			
//			for (int i = 0; i < length-8; i++) {
//				printf("%02X", buffer[i]);
//			}
//			for (int i = length-1; i >= length-8; i--) {
//				printf("%02X", buffer[i]);
//			}
//			printf(" \\\n");
			/*
			//static int firstTime = true;
			static int savedCount = 0;
			const int totalToSave = 8192*8;
			static unsigned long long savedBuffer[totalToSave][8];
			
			unsigned long long* bufferAsInt = (unsigned long long*) buffer;
			unsigned long long bufferSuccess[8];
			if(savedCount < totalToSave) {
				for (int i = 0; i < 8; i++) {
					savedBuffer[savedCount][i] = bufferAsInt[i];
				}
				//printf("savedCount = %d\n", savedCount);
				savedCount++;
			}
			
			for (int i = 0; i < savedCount; i++) {
				if (savedBuffer[i][7] == bufferAsInt[7]) {
					if (savedCount-1 != i) {
						
					
					printf("Read: ");
					for (int j = 0; j < length; j++) {
						printf("%02X ", buffer[j]);
					}
					printf("\n");
					printf(" - Found a match!\n");
					printf("Match: ");
					for (int j = 0; j < length; j++) {
						printf("%02X ", ((unsigned char*)savedBuffer[i])[j]);
					}
					printf("\n");
					}
				}
			}
			 */
			 
			/*
			unsigned long long sum = 0;
			for (int i = 0; i < 7; i++) {
				sum += bufferAsInt[i];
			}
			printf("unsigned long long sum: ");
			for (int i = 0; i < 8; i++) {
				printf("%02X ", ((unsigned char*)&sum)[i]);
			}
			printf("\n");
			
			sum = 1;
			for (int i = 0; i < 7; i++) {
				sum *= bufferAsInt[i];
			}
			printf("unsigned long long mult: ");
			for (int i = 0; i < 8; i++) {
				printf("%02X ", ((unsigned char*)&sum)[i]);
			}
			printf("\n");
			
			sum = 0;
			for (int i = 0; i < 7; i++) {
				sum ^= bufferAsInt[i];
			}
			printf("unsigned long long xor: ");
			for (int i = 0; i < 8; i++) {
				printf("%02X ", ((unsigned char*)&sum)[i]);
			}
			printf("\n");
			*/
			//printf("Byte 7: "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(buffer[7]));
			
//			printf("Frequency: %0.2f\n", (double)totalSamples/difftime);
			
			count = 0;
			oldTime = currentTime;
			
//			shouldHold = !shouldHold;
//			printf("Should Hold: %d\n", shouldHold);
		}
		
//		if (shouldHold) {
//			buffer[8] |= 0x20;
//		}
	}
};

int main(int argc, char **argv) {
	
	
	RawGadgetPassthrough mRawGadgetPassthrough;
	
	DualsenseSniffer mDualsenseSniffer;
	mDualsenseSniffer.setEndpoint(0x84);
//	mDualsenseSniffer.setEndpoint(0x03);
	mRawGadgetPassthrough.addObserver( &mDualsenseSniffer );
	
	mRawGadgetPassthrough.initialize();
	mRawGadgetPassthrough.start();
	
	printf("Press any key to quit\n");
	getc(stdin);

	
	mRawGadgetPassthrough.stop();
	
	return 0;
}
