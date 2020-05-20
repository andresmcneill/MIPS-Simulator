//Andres McNeill & Edric Griffin
//CPSC 3300 - Project #1 (MIPS Simulator)
//Dr. Mark Smotherman
//Fall 2018

#include<vector>
#include<string>
#include<iostream>
#include<iomanip>
#include<stdbool.h>
#include<bitset>
#include<stdio.h>
#include<stdlib.h>
#define LINES_PER_BANK 32
using namespace std;

//Register, memory, and statistics abstractions
vector<int> registers(32);
static short pc = 0;
static vector<int> memory;
static int instrCount = 0;
static int aluCount = 0;
static int lCount = 0;
static int stCount = 0;
static int jCount = 0;
static int jlCount = 0;
static int tbCount = 0;
static int ubCount = 0;
static int hits = 0;
static int misses = 0;

//Cache abstractions
unsigned int
  plru_state[LINES_PER_BANK],
  valid[4][LINES_PER_BANK],
  tag[4][LINES_PER_BANK],
  plru_bank[8]  = { 0, 0, 1, 1, 2, 3, 2, 3 },
  next_state[32]  = {  6, 4, 1, 0, 7, 5, 1, 0, 6, 4, 3, 2, 7, 5, 3, 2,
                       6, 4, 1, 0, 7, 5, 1, 0, 6, 4, 3, 2, 7, 5, 3, 2  };

void cache_init(void){
  int i;
  for(i=0;i<LINES_PER_BANK;i++){
    plru_state[i] = 0;
    valid[0][i] = tag[0][i] = 0;
    valid[1][i] = tag[1][i] = 0;
    valid[2][i] = tag[2][i] = 0;
    valid[3][i] = tag[3][i] = 0;
  }
}

//Cache loop
void cache_access(int address, int accessType) {
  unsigned int
    addr_tag,    /* tag bits of address     */
    addr_index,  /* index bits of address   */
    bank,        /* bank that hit, or bank chosen for replacement */

  if(address!=EOF){

  addr_index = (address >> 5) & 0x1f;
  addr_tag = address >> 10;

  /* check bank 0 hit */

  if(valid[0][addr_index] && (addr_tag==tag[0][addr_index])){
    hits++;
    bank = 0;

  /* check bank 1 hit */

  }else if(valid[1][addr_index] && (addr_tag==tag[1][addr_index])){
    hits++;
    bank = 1;

  /* check bank 2 hit */

  }else if(valid[2][addr_index] && (addr_tag==tag[2][addr_index])){
    hits++;
    bank = 2;

  /* check bank 3 hit */

  }else if(valid[3][addr_index] && (addr_tag==tag[3][addr_index])){
    hits++;
    bank = 3;

  /* miss - choose replacement bank */

  }else{
    misses++;

         if(!valid[0][addr_index]) bank = 0;
    else if(!valid[1][addr_index]) bank = 1;
    else if(!valid[2][addr_index]) bank = 2;
    else if(!valid[3][addr_index]) bank = 3;
    else bank = plru_bank[ plru_state[addr_index] ];

    valid[bank][addr_index] = 1;
    tag[bank][addr_index] = addr_tag;
  }

  /* update replacement state for this set (i.e., index value) */

  plru_state[addr_index] = next_state[ (plru_state[addr_index]<<2) | bank ];
  }//end if
}

//List of basic functions available to simulator
void addu(int rtarget, int r1, int r2) {
  registers.at(rtarget) = registers.at(r1) + registers.at(r2);
  aluCount+=1;
  pc+=1;
}


void addiu(int rtarget, int r1, int immediate) {
  registers.at(rtarget) = registers.at(r1) + immediate;
  aluCount+=1;
  pc+=1;
}


void _and(int rtarget, int r1, int r2) {
  registers.at(rtarget) = registers.at(r1) & registers.at(r2);
  aluCount+=1;
  pc+=1;
}


void beq(int r1, int r2, short offset) {
  if(registers.at(r1) == registers.at(r2)) {
    tbCount+=1;
    pc = pc + offset + 1;
  }
  else {
    ubCount+=1;
    pc+=1;
  }
}

void bgtz(int r, short offset) {
  if(registers.at(r) > 0) {
    tbCount+=1;
    pc = pc + offset + 1;
  }
  else {
    ubCount+=1;
    pc+=1;
  }
}


void blez(int r, short offset) {
  if(registers.at(r) <= 0) {
    tbCount+=1;
    pc = pc + offset + 1;
  }
  else {
    ubCount+=1;
    pc+=1;
  }
}


void bneq(int r1, int r2, short offset) {
  if(registers.at(r1) != registers.at(r2)) {
    tbCount+=1;
    pc = pc + offset + 1;
  }
  else {
    ubCount+=1;
    pc+=1;
  }
}


void hlt() {
  pc+=1;
}


void j(int immediate) {
  jCount+=1;
  pc = immediate;
}


void jal(int immediate) {
  jlCount+=1;
  registers.at(31) = pc+1;
  pc = immediate;
}


void jalr(int r1, int r2) {
  jlCount+=1;
  registers.at(r2) = pc+1;
  pc = registers.at(r1);
}


void jr(int r) {
  jCount+=1;
  pc = registers.at(r);
}


void lui(int r, int immediate) {
  registers.at(r) = immediate<<16;
  aluCount+=1;
  pc+=1;
}

void lw(int rtarget, short offset) {
  registers.at(rtarget) = memory.at(offset);
  lCount+=1;
  pc+=1;
}

void mul(int rtarget, int r1, int r2) {
  registers.at(rtarget) = registers.at(r1) * registers.at(r2);
  aluCount+=1;
  pc+=1;
}


void nor(int rtarget, int r1, int r2) {
  registers.at(rtarget) = ~(registers.at(r1) | registers.at(r2));
  aluCount+=1;
  pc+=1;
}


void _or(int rtarget, int r1, int r2) {
  registers.at(rtarget) = (registers.at(r1) | registers.at(r2));
  aluCount+=1;
  pc+=1;
}


void sll(int rtarget, int r1, int shift) {
  registers.at(rtarget) = (unsigned)registers.at(r1)<<shift;
  aluCount+=1;
  pc+=1;
}

void slti(int rtarget, int r1, int imm) {
  if(registers.at(r1) < imm) {
    registers.at(rtarget) = 1;
  }
  else {
    registers.at(rtarget) = 0;
  }
  aluCount+=1;
  pc+=1;
}


void sra(int rtarget, int r1, int shift) {
  registers.at(rtarget) = registers.at(r1)>>shift;
  aluCount+=1;
  pc+=1;
}


void srl(int rtarget, int r1, int shift) {
  registers.at(rtarget) = (unsigned)registers.at(r1)>>shift;
  aluCount+=1;
  pc+=1;
}


void subu(int rtarget, int r1, int r2) {
  registers.at(rtarget) = registers.at(r1) - registers.at(r2);
  aluCount+=1;
  pc+=1;
}


void sw(int rtarget, short offset) {
  memory.at(offset) = registers.at(rtarget);
  stCount+=1;
  pc+=1;
}


void _xor(int rtarget, int r1, int r2) {
  registers.at(rtarget) = registers.at(r1) ^ registers.at(r2);
  aluCount+=1;
  pc+=1;
}


void xori(int rtarget, int r1, int immediate) {
  registers.at(rtarget) = registers.at(r1) ^ immediate;
  aluCount+=1;
  pc+=1;
}


int main() {
  int input = 0;

  //Read inputs into memory
  while(scanf("%x", &input) != EOF) {
    memory.push_back(input);
  }

  //Initialize cache
  cache_init();

  //Print contents of memory before simulation
  cout << "contents of memory" << endl;
  cout << "addr value" << endl;
  for(int i = 0; i < memory.size(); ++i) {
    cout << setw(3) << setfill('0') << right << i <<": ";
    cout << setbase(16) << memory.at(i) << endl;
  }

    //Simulation Cue
    cout << endl << "behavioral simulation of simple MIPS-like machine" << endl;
    cout << "  (all values are shown in hexadecimal)" << endl << endl;
    cout << "pc\tresult of instruction at that location" << endl;

    int op;
    int funct;
    bool isRunning = true;
    //Stat counters
    int issueCycles = 0;
    int doubleIssues = 0;
    int controlStops = 0;
    int structuralStops = 0;
    int dataDepStops = 0;
    int strctDepStops = 0;

    while (isRunning) {
        int prevWrite = -1;

        //  Decode first six bits of instruction
        op = (memory.at(pc) >> 26) & 0x3f;
        switch (op) {

            //  must now check funct
            case 00:
                //  Decode last six bits of instruction
                funct = memory.at(pc) & 0x0000003f;
                switch (funct) {
                    //  addu
                    case 0x21:
                        {
                        int r1 = (memory.at(pc) >> 21) & 0x1f;
                        int r2 = (memory.at(pc) >> 16) & 0x1f;
                        int rtarget = (memory.at(pc) >> 11) & 0x1f;
                        prevWrite = rtarget;
                        addu(rtarget, r1, r2);
                        }
                        break;

                    //  _and
                    case 0x24:
                        {
                        int r1 = (memory.at(pc) >> 21) & 0x1f;
                        int r2 = (memory.at(pc) >> 16) & 0x1f;
                        int rtarget = (memory.at(pc) >> 11) & 0x1f;
                        prevWrite = rtarget;
                        _and(rtarget, r1, r2);
                        }
                        break;

                    //  hlt OR sll
                    case 0x00:
                        //  hlt
                        if (memory.at(pc) == 0) {
                            hlt();
                            isRunning = false;
                        }
                        //  sll
                        else {
                            {
                            int rtarget = (memory.at(pc) >> 11) & 0x1f;
                            int r = (memory.at(pc) >> 16) & 0x1f;
                            int shift = (memory.at(pc) >> 6) & 0x1f;
                            prevWrite = rtarget;
                            sll(rtarget, r, shift);
                            }
                        }
                        break;

                    //  jalr
                    case 0x09:
                        {
                        int r1 = (memory.at(pc) >> 21) & 0x1f;
                        int r2 = (memory.at(pc) >> 11) & 0x1f;
                        jalr(r1, r2);
                        prevWrite = r2;
                        }
                        break;

                    //  jr
                    case 0x08:
                        {
                        int r = (memory.at(pc) >> 21) & 0x1f;
                        jr(r);
                        }
                        break;

                    //  nor
                    case 0x27:
                        {
                        int r1 = (memory.at(pc) >> 21) & 0x1f;
                        int r2 = (memory.at(pc) >> 16) & 0x1f;
                        int rtarget = (memory.at(pc) >> 11) & 0x1f;
                        nor(rtarget, r1, r2);
                        prevWrite = rtarget;
                        }
                        break;

                    //  or
                    case 0x25:
                        {
                        int r1 = (memory.at(pc) >> 21) & 0x1f;
                        int r2 = (memory.at(pc) >> 16) & 0x1f;
                        int rtarget = (memory.at(pc) >> 11) & 0x1f;
                        _or(rtarget, r1, r2);
                        prevWrite = rtarget;
                        }
                        break;

                    //  sra
                    case 0x03:
                        {
                        int rtarget = (memory.at(pc) >> 11) & 0x1f;
                        int r = (memory.at(pc) >> 16) & 0x1f;
                        int shift = (memory.at(pc) >> 6) & 0x1f;
                        sra(rtarget, r, shift);
                        prevWrite = rtarget;
                        }
                        break;

                    //  srl
                    case 0x02:
                        {
                        int rtarget = (memory.at(pc) >> 11) & 0x1f;
                        int r = (memory.at(pc) >> 16) & 0x1f;
                        int shift = (memory.at(pc) >> 6) & 0x1f;
                        srl(rtarget, r, shift);
                        prevWrite = rtarget;
                        }
                        break;

                    //  subu:
                    case 0x23:
                        {
                        int r1 = (memory.at(pc) >> 21) & 0x1f;
                        int r2 = (memory.at(pc) >> 16) & 0x1f;
                        int rtarget = (memory.at(pc) >> 11) & 0x1f;
                        subu(rtarget, r1, r2);
                        prevWrite = rtarget;
                        }
                        break;

                    //  _xor:
                    case 0x26:
                        {
                        int r1 = (memory.at(pc) >> 21) & 0x1f;
                        int r2 = (memory.at(pc) >> 16) & 0x1f;
                        int rtarget = (memory.at(pc) >> 11) & 0x1f;
                        _xor(rtarget, r1, r2);
                        prevWrite = rtarget;
                        }
                        break;

                }
                break;

            //  addiu
            case 0x09:
                {
                int r1 = (memory.at(pc) >> 21) & 0x1f;
                int rtarget = (memory.at(pc) >> 16) & 0x1f;
                int immediate = memory.at(pc) & 0xffff;
                addiu(rtarget, r1, immediate);
                prevWrite = rtarget;
                }
                break;

            //  beq
            case 0x04:
                {
                int r1 = (memory.at(pc) >> 21) & 0x1f;
                int r2 = (memory.at(pc) >> 16) & 0x1f;
                short offset = (short)(memory.at(pc) & 0xffff);
                beq(r1, r2, offset);
                }
                break;

            //  bgtz
            case 0x07:
                {
                int r = (memory.at(pc) >> 21) & 0x1f;
                short offset = (short)(memory.at(pc) & 0xffff);
                bgtz(r, offset);
                }
                break;

            //  blez
            case 0x06:
                {
                int r = (memory.at(pc) >> 21) & 0x1f;
                short offset = (short)(memory.at(pc) & 0xffff);
                blez(r, offset);
                }
                break;

            //  bne
            case 0x05:
                {
                int r1 = (memory.at(pc) >> 21) & 0x1f;
                int r2 = (memory.at(pc) >> 16) & 0x1f;
                short offset = (short)(memory.at(pc) & 0xffff);
                bneq(r1, r2, offset);
                }
                break;

            //  j
            case 0x02:
                {
                short immediate = (short)(memory.at(pc) & 0xffff);
                j(immediate);
                }
                break;

            //  jal
            case 0x03:
                {
                short immediate = (short)(memory.at(pc) & 0xffff);
                jal(immediate);
                }
                break;

            //  lui
            case 0x0f:
                {
                int r = memory.at(pc) & 0x1f;
                int immediate = memory.at(pc) & 0xffff;
                lui(r, immediate);
                prevWrite = r;
                }
                break;

            //  lw
            case 0x23:
                {
                int rtarget = (memory.at(pc) >> 16) & 0x1f;
                short offset = (short)(memory.at(pc) & 0xffff);
                lw(rtarget, offset);
                prevWrite = rtarget;
                }
                break;

            //  mul
            case 0x1c:
                {
                int r1 = (memory.at(pc) >> 21) & 0x1f;
                int r2 = (memory.at(pc) >> 16) & 0x1f;
                int rtarget = (memory.at(pc) >> 11) & 0x1f;
                mul(rtarget, r1, r2);
                prevWrite = rtarget;
                }
                break;

            //  slti
            case 0x0a:
                {
                int rtarget = (memory.at(pc) >> 16) & 0x1f;
                int r = (memory.at(pc) >> 21) & 0x1f;
                int immediate = memory.at(pc) & 0xffff;
                slti(rtarget, r, immediate);
                prevWrite = rtarget;
                }
                break;

            //  sw
            case 0x2b:
                {
                int rtarget = (memory.at(pc) >> 16) & 0x1f;
                short offset = (short)(memory.at(pc) & 0xffff);
                sw(rtarget, offset);
                }
                break;

            //  xori
            case 0x0e:
                {
                int r1 = (memory.at(pc) >> 21) & 0x1f;
                int rtarget = (memory.at(pc) >> 16) & 0x1f;
                int immediate = memory.at(pc) & 0xffff;
                xori(rtarget, r1, immediate);
                prevWrite = rtarget;
                }
                break;
        }
        if (isRunning)  instrCount+=1;
    }

  //Print contents of memory after simulation
  cout << endl << "contents of memory" << endl;
  cout << "addr value" << endl;
  for(int i = 0; i < memory.size(); ++i) {
    cout << setw(3) << setfill('0') << right << i <<": ";
    cout << setbase(16) << memory.at(i) << endl;
  }


  //Print statistics regarding simulation
  //Instruction class counts
  cout << endl << "instruction class counts (omits hlt instruction)" << endl;
  cout << setbase(10);
  cout << "  alu ops" << setw(15) << setfill(' ') << right << aluCount << endl;
  cout << "  loads/stores" << setw(10) << setfill(' ') << right << (lCount+stCount) << endl;
  cout << "  jumps/branches" << setw(8) << setfill(' ') << right << (jCount+jlCount+tbCount+ubCount) << endl;
  cout << "total" << setw(19) << setfill(' ') << right << (aluCount+lCount+stCount+jCount+jlCount+tbCount+ubCount) << endl << endl;

  //Memory access
  cout << "memory access counts (omits hlt instruction)" << endl;
  cout << "  inst. fetches" << setw(9) << setfill(' ') << right << instrCount << endl;
  cout << "  loads" << setw(17) << setfill(' ') << right << lCount << endl;
  cout << "  stores" << setw(16) << setfill(' ') << right << stCount << endl;
  cout << "total" << setw(19) << setfill(' ') << right << (instrCount+lCount+stCount) << endl << endl;

  //Branches and control transfers
  cout << "transfer of control counts" << endl;
  cout << "  jumps" << setw(17) << setfill(' ') << right << jCount << endl;
  cout << "  jump-and-links" << setw(8) << setfill(' ') << right << jlCount << endl;
  cout << "  taken branches" << setw(8) << setfill(' ') << right << tbCount << endl;
  cout << "  untaken branches" << setw(6) << setfill(' ') << right << ubCount << endl;
  cout << "total" << setw(19) << setfill(' ') << right << (jCount+jlCount+tbCount+ubCount) << endl << endl;

}
