#include "L2_1WCache.h"

uint8_t DRAM[DRAM_SIZE];
uint32_t time;
Cache cache;

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
  for(int i = 0; i < L1_SIZE / BLOCK_SIZE; i++) {
    cache.L1.line[i].Valid = 0;
    cache.L1.line[i].Dirty = 0;
    cache.L1.line[i].Tag = 0;
    for (int j = 0; j < BLOCK_SIZE; j+=WORD_SIZE) {
      cache.L1.line[i].Data[j] = 0;
    }
  
  }
  for(int i = 0; i < L2_SIZE / BLOCK_SIZE; i++) {
    cache.L2.line[i].Valid = 0;
    cache.L2.line[i].Dirty = 0;
    cache.L2.line[i].Tag = 0;
    for (int j = 0; j < BLOCK_SIZE; j+=WORD_SIZE) {
      cache.L2.line[i].Data[j] = 0;
    }
  }

  for (int i = 0; i < DRAM_SIZE; i+=WORD_SIZE) {
    DRAM[i] = 0;
  }

  cache.init = 1;
}

void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {

  uint32_t index, Tag, MemAddress, offset;
  uint8_t TempBlock[BLOCK_SIZE];

  // address: tag (32-8-6=18), index (256 lines - 8b), offset word (16 words, 4b), offset byte (4B - 2b)
  Tag = address >> 14;

  index = (address >> 6) & 0xFF; // shift 6 bits to the right and aplly mask

  offset = address & (BLOCK_SIZE - 1);

  MemAddress = (address >> 14) << 14;
  
  CacheLine* Line = cache.L1.line;
  Line += index;

  if (Line->Valid && Line->Tag == Tag){ // hit
    
    if (mode == MODE_READ) { // read
      memcpy(data, &Line->Data[offset], WORD_SIZE);
      time += L1_READ_TIME;
    }

    if (mode == MODE_WRITE) { // write
      memcpy(&Line->Data[offset], data, WORD_SIZE);
      time += L1_WRITE_TIME;
      Line->Dirty = 1;
    }
  }
  
  else {
    if(Line->Dirty){
      accessL2(address,Line->Data,MODE_WRITE);
      Line->Data[0] = 0;
      Line->Data[WORD_SIZE] = 0;
    }
    



  }

  if (!Line->Valid || Line->Tag != Tag) {         
    accessL2(MemAddress, TempBlock, MODE_READ);

    if ((Line->Valid) && (Line->Dirty)) {
      MemAddress = Line->Tag << 14;
      accessDRAM(MemAddress, Line->Data, MODE_WRITE);
    }

    memcpy(Line->Data, TempBlock, BLOCK_SIZE);
    Line->Valid = 1;
    Line->Tag = Tag;
    Line->Dirty = 0;
  }

  if (mode == MODE_READ) { // read
    memcpy(data, &Line->Data[offset], WORD_SIZE);
    time += L1_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write
    memcpy(&Line->Data[offset], data, WORD_SIZE);
    time += L1_WRITE_TIME;
    Line->Dirty = 1;
  }
}

void accessL2(uint32_t address, uint8_t *data, uint32_t mode) {
  uint32_t index, Tag, MemAddress, offset;
  uint8_t TempBlock[BLOCK_SIZE];

  // address: tag (32-9-6=17)
  Tag = address >> 15;

  index = (address >> 6) & 0x1FF; // shift 6 bits to the right and aplly mask

  offset = address & (BLOCK_SIZE - 1);

  MemAddress = (address >> 15) << 15;
  
  CacheLine* Line = cache.L2.line;
  Line += index;

  if (!Line->Valid || Line->Tag != Tag) {         
  accessDRAM(MemAddress, TempBlock, MODE_READ);

  if ((Line->Valid) && (Line->Dirty)) {
    MemAddress = Line->Tag << 14;
    accessDRAM(MemAddress, Line->Data, MODE_WRITE);
  }

    memcpy(Line->Data, TempBlock, BLOCK_SIZE);
    Line->Valid = 1;
    Line->Tag = Tag;
    Line->Dirty = 0;
  }

  if (mode == MODE_READ) { // read
    memcpy(data, &Line->Data[offset], WORD_SIZE);
    time += L1_READ_TIME;
  }

  if (mode == MODE_WRITE) { // write
    memcpy(&Line->Data[offset], data, WORD_SIZE);
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
