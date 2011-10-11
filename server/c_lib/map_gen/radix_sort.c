
unsigned char InputValues[] = { 54, 18, 2, 128, 3 };
int SortedBuffer[256];
memset(SortedBuffer, -1, 256*sizeof(int));   // Fill with –1


int Counters[256];
memset(Counters, 0, 256*sizeof(int));  // Set all counters to 0
for( i =0 ; i < NbItems ; i++){        // Loop over the input array
    unsigned char c = InputValues[i]; // Get current byte…
    Counters[c]++;                    // …and update counter
}


int OffsetTable[256];
OffsetTable[0] = 0;
for(i=1;i<256;i++){
    OffsetTable[i] = OffsetTable[i-1] + Counters[i-1];
}


