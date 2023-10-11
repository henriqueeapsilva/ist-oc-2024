#include "L2_2WCache.h"

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
  for(int i = 0; i < L2_SIZE / BLOCK_SIZE / L2_ASSOCIATIVITY; i++) {
    for(int k = 0; k < L2_ASSOCIATIVITY; k++) {
      cache.L2.line[i][k].Valid = 0;
      cache.L2.line[i][k].Dirty = 0;
      cache.L2.line[i][k].Tag = 0;
      cache.L2.line[i][k].Time = 0;
      for (int j = 0; j < BLOCK_SIZE; j+=WORD_SIZE) {
        cache.L2.line[i][k].Data[j] = 0;
      }
    }
  }

  for (int i = 0; i < DRAM_SIZE; i+=WORD_SIZE) {
    DRAM[i] = 0;
  }

  cache.init = 1;
}

void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {

  uint32_t index, Tag, offset;

  // address: tag (32-8-6=18), index (256 lines - 8b), offset word (16 words, 4b), offset byte (4B - 2b)
  Tag = address >> 14;

  index = (address >> 6) & 0xFF; // shift 6 bits to the right and aplly mask

  offset = address & (BLOCK_SIZE - 1);
  
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
      uint32_t addressL2 = ((Tag << 8) + index) << 6;
      accessL2(addressL2, Line->Data, MODE_WRITE);
      Line->Data[0] = 0;
      Line->Data[WORD_SIZE] = 0;
    }
    accessL2(address - offset, Line->Data , MODE_READ);
    
    if (mode == MODE_READ) {
      memcpy(data, &(Line->Data[offset]), WORD_SIZE);
      time += L1_READ_TIME;
      Line->Dirty = 0; 
      Line->Valid = 1;
      Line->Tag = Tag;
    }
    if (mode == MODE_WRITE) {
      memcpy(&(Line->Data[offset]), data, WORD_SIZE);
      time += L1_WRITE_TIME;
      Line->Dirty = 1;
      Line->Valid = 1;
      Line->Tag = Tag;
    }
  }
}

void accessL2(uint32_t address, uint8_t *data, uint32_t mode) {
  uint32_t index, Tag, MemAddress, offset;
  int i = 0;
  int found = 0;
  // address: tag (32-8-6=17)
  Tag = address >> 14;

  index = (address >> 6) & 0x1FF; // shift 6 bits to the right and aplly mask

  offset = address & (BLOCK_SIZE - 1);

  MemAddress = (address >> 14) << 14;
  
  CacheLine *Line = *cache.L2.line;
  Line += index;

  for(;i < L2_ASSOCIATIVITY; i++) {
    if (Line[i].Valid && Line[i].Tag == Tag) {
      found = 1;
      break;
    }
  }

  if (found){ // hit
    if (mode == MODE_READ) { // read
      memcpy(data, &Line[i].Data[offset], WORD_SIZE);
      time += L2_READ_TIME;
      Line[i].Time += time;
    }

    if (mode == MODE_WRITE) { // write
      memcpy(&Line[i].Data[offset], data, WORD_SIZE);
      time += L2_WRITE_TIME;
      Line[i].Dirty = 1;
      Line[i].Time += time;
    }
  }

  else {
    int idx = 0;
    int min = Line[0].Time;

    for(int i = 0; i < L2_ASSOCIATIVITY; i++){
      if(!Line[i].Valid){
        idx = i;
        break;
      }
      if(Line[i].Time < min){
        min = Line[i].Time;
        idx = i;
      }
    }

    if(Line->Dirty){
      uint32_t MemAddressOld = ((Line[i].Tag << 8) + index) << 6;
      accessDRAM(MemAddressOld, Line[idx].Data, MODE_WRITE);
      Line->Data[0] = 0;
      Line->Data[WORD_SIZE] = 0;
    }

    accessDRAM(MemAddress, Line[idx].Data , MODE_READ);
    
    if (mode == MODE_READ) {
      memcpy(data, &(Line[idx].Data[offset]), WORD_SIZE);
      time += L2_READ_TIME;
      Line[idx].Dirty = 0; 
      Line[idx].Valid = 1;
      Line[idx].Tag = Tag;
      Line[idx].Time = time;
    }
    if (mode == MODE_WRITE) {
      memcpy(&(Line[idx].Data[offset]), data, WORD_SIZE);
      time += L2_WRITE_TIME;
      Line[idx].Dirty = 1;
      Line[idx].Valid = 1;
      Line[idx].Tag = Tag;
      Line[idx].Time = time;
    }
  }
}

void read(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) {
  accessL1(address, data, MODE_WRITE);
}
