
#include "opp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>



/* *********************************************************/
/*  ~~~~~~~~~~~~~~~         MACROS         ~~~~~~~~~~~~~~  */
/* *********************************************************/

#define GOOD 0
#define of(x,y) fopen(argv[x],y)
#define HD_CYCLE 1024
#define is_positive_imm(hex) hex[0]==0
#define IMM_MASK 0xFFFFF

#define SEC_CNT 128
#define BLK_CNT 128 // 128 = 512/4 - blocks of 4 Bytes

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~         STATICS        ~~~~~~~~~~~~~~  */
/* *********************************************************/


FILE *Imem, *Dmem, *Diskin, *irq2In;
FILE *Memout,*Regout, *Trace, *HwRT, *Cycles, *Leds;
FILE *Monitor_txt, *monitor_yuv, *Diskout;

int  Data_Mem[4096];
int  monitor[288][352];
char Op_Mem[1024][6];    //1024 lines of 5 char strings (+'/0')
int* irq_2;
int Disk[SEC_CNT][BLK_CNT];

int REG[16] = {0,0,0,0,0,0,0,0,0,0 ,0,0,0,0,0,0};
unsigned int IOs[22] = { 0,0,0,0,0,0,0,0,0,0 ,0,0,0,0,0,0,0,0,0,0 ,0,0 };

int disk_timer = 0, already_irq = 0;
int pc = 0, clk_resets = -1, op_mem_max;
unsigned int op_count = 0;
 
/* *********************************************************/
/*  ~~~~~~~~~~~~~~~     FILES  PARSING     ~~~~~~~~~~~~~~  */
/* *********************************************************/

void parse_disk();

void file_opening(char* argv[]);

int parse_irq2();

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~     FILES WRITTING     ~~~~~~~~~~~~~~  */
/* *********************************************************/

int write_trace_file(operation* op, int pc, FILE* original_trace, int debug);

void write_cycles();

void write_monitor();

void write_disk();

void write_dmem();

void write_regout();

//debug - fib only
int check_trace(FILE* original_trace, char* curr_line);

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~    OPERATION PARSER    ~~~~~~~~~~~~~~  */
/* *********************************************************/

int parse_immediate(char* hex);

void parse_opcode(char* line, operation* op, int pc);

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~    CLOCKS AND IRQS     ~~~~~~~~~~~~~~  */
/* *********************************************************/

void update_hardisk_timer();

int init_clock_cycle();

int irq_on();

void timer_check();

void check_iqs(int real_update);

void update_clock(int real_update);

int immediate_clk(operation* op, int iq2);


