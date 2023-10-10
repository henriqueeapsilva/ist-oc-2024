#include "L1Cache.h"

uint8_t DRAM[DRAM_SIZE];
uint32_t time;
Cache L1;

/**************** Time Manipulation ***************/
void resetTime() { time = 0; }

uint32_t getTime() { return time; }

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t address, uint8_t *data, uint32_t mode) {

  if (address >= DRAM_SIZE - WORD_SIZE + 1)
    exit(-1);

  if (mode == MODE_READ) {
    memcpy(data, &(DRAM[address]), BLOCK_SIZE);
    time += DRAM_READ_TIME;
  }

  if (mode == MODE_WRITE) {
    memcpy(&(DRAM[address]), data, BLOCK_SIZE);
    time += DRAM_WRITE_TIME;
  }
}

/*********************** L1 cache *************************/

void initCache() { 
  L1.init = 1;
  for(int i = 0; i < L1_SIZE / BLOCK_SIZE; i++) {
    L1.line[i].Valid = 0;
    L1.line[i].Dirty = 0;
    L1.line[i].Tag = 0;
    for (int j = 0; j < BLOCK_SIZE; j+=WORD_SIZE) {
      L1.line[i].Data[j] = 0;
    }
  }
}

void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {

  uint32_t index, Tag, MemAddress, offset;
  uint8_t TempBlock[BLOCK_SIZE];

  // address: tag (32-8-6=18), index (256 lines - 8b), offset word (16 words, 4b), offset byte (4B - 2b)
  Tag = address >> 14;

  index = address >> 6 & 0xFF; // shift 2 bits to the left and aplly mask

  offset = (address & ((BLOCK_SIZE - 1)));

  MemAddress = address >> 14;

  MemAddress = MemAddress << 14; // address of the block in memory
  
  // printf("----------------\n");
  // printf("Address: 0x%08x\t %d\n", address, address);
  // printf("Tag: 0x%08x \t %d\n", Tag, Tag);
  // printf("Index: 0x%08x\t %d\n", index, index);
  // printf("Offset: 0x%08x\t %d\n", offset, offset);
  // printf("Mem Address: 0x%08x\t %d\n", MemAddress, MemAddress);
  // printf("----------------\n"); 
  CacheLine* Line = L1.line;
  Line += index;
  /* access Cache*/

  if (!Line->Valid || Line->Tag != Tag) {         
    accessDRAM(MemAddress, TempBlock, MODE_READ);

    if ((Line->Valid) && (Line->Dirty)) {
      MemAddress = Line->Tag << 14;
      accessDRAM(MemAddress, &(Line->Data[offset]), MODE_WRITE);
    }

    memcpy(&Line->Data[offset], TempBlock, BLOCK_SIZE);
    Line->Valid = 1;
    Line->Tag = Tag;
    Line->Dirty = 0;
  }

  if (mode == MODE_READ) { // read
    memcpy(data, &(Line->Data[offset]), WORD_SIZE);
    time += L1_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write
    memcpy(&(Line->Data[offset]), data, WORD_SIZE);
    time += L1_WRITE_TIME;
    Line->Dirty = 1;
  }
}

void read(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_WRITE);
}
