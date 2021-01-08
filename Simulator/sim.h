
#include "opp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


/*
this module includes the main flow of the SIMP simulator
and divided into 6 sections:

1) macros:				constans to be used by implenting .c file
2) shared variables:	to be used in opp.c and sim.c
3) file parsing:		for parsing input files and store info in shared variables
4) files writting:		for writting output files
5) operation parsing:	for handling the operation needed to be simulated
6) clocks and irqs:		for handling clock cycles and irqs handeling

it was decided to store all hardware information in script and not in files.
the decision ment to make it more accesable to and quick to operate
'long' loading time vs 'quick' simulation time

*/


/* *********************************************************/
/*  ~~~~~~~~~~~~~~~         MACROS         ~~~~~~~~~~~~~~  */
/* *********************************************************/

#define GOOD 0
#define HD_CYCLE 1024
#define IMM_MASK 0xFFFFF //lower 20 Bits according to immediate length

#define SEC_CNT 128	// 128 sectors in Hard-Drive
#define BLK_CNT 128 // 128 = 512/4 - blocks of 4 Bytes according to Dmem

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~    SHARED  VARIABLES   ~~~~~~~~~~~~~~  */
/* *********************************************************/


FILE *Imem, *Dmem, *Diskin, *irq2In;				 //inputs
FILE *Memout,*Regout, *Trace, *HwRT, *Cycles, *Leds; //outputs
FILE *Monitor_txt, *monitor_yuv, *Diskout;			 //outputs

int  Data_Mem[4096];		//Data Memory of SIMP aka DMEM
int  monitor[288][352];		//Monitor pixles to be written in the end
char Op_Mem[1024][6];		//Operation Memory of SIMP aka IMEM
int* irq_2;					//A dynamic array holds all irq2 clock cycles 
int Disk[SEC_CNT][BLK_CNT]; //A two-level array to implemnt hard-disk

int disk_timer = 0, already_irq = 0; //trivial
int pc = 0, clk_resets = -1, op_mem_max; //trivial
unsigned int op_count = 0; //trivial

/*   SIMP REGISTERS    */
int REG[16] = {0,0,0,0,0,0,0,0,0,0 ,0,0,0,0,0,0};
unsigned int IOs[22] = { 0,0,0,0,0,0,0,0,0,0 ,0,0,0,0,0,0,0,0,0,0 ,0,0 };


 
/* *********************************************************/
/*  ~~~~~~~~~~~~~~~     FILES  PARSING     ~~~~~~~~~~~~~~  */
/* *********************************************************/

/*
A function to open and parse files.
monitor.yuv & irq2In.txt re exulded and open seperatly
*/
void file_opening(char* argv[]);

/*
A funcrion to open and parse irq2_In which will be saved as an array irq_2
*/
void parse_irq2(char* path);

/*
A funcrion to parse diskinn and save informatin in Disk[][]
*/
void parse_disk();

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~     FILES WRITTING     ~~~~~~~~~~~~~~  */
/* *********************************************************/

/* all functions here are pretty straight-forword and self explained */



void write_cycles();

void write_monitor(char* yuv_path);

void write_disk();

void write_dmem();

void write_regout();

/*
this function has a debug feature comapring trace file written to a given referance trace
if not debug: original_trace=NULL & debug=0
*/
int write_trace_file(operation* op, int pc, FILE* original_trace, int debug);

/*
use to check trace file during run compared to original trace.file
used only in fib simulating - did'nt remove to reduce compilation error
*/
int check_trace(FILE* original_trace, char* curr_line);

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~    OPERATION PARSER    ~~~~~~~~~~~~~~  */
/* *********************************************************/

/*
A function to handle the possibilaty of negative immediate
*/
int parse_immediate(char* hex);

/*
A function to parse operation from instruction memory aka IMEM
*/
void parse_opcode(char* line, operation* op, int pc);

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~    CLOCKS AND IRQS     ~~~~~~~~~~~~~~  */
/* *********************************************************/

/*
Hard disk timer update - to be called by update_clock
*/
void update_hardisk_timer();

/*
Check if irq signal is on: 
if there is an irq which is both enabled and statused
*/
int irq_on();

/*
A function to check if timer reached limit - and handle it if needed
*/
void timer_check();

/*
A function to check if an irq handling is needed
handle irq2 checks and prospone if needed (immediate cycle)
to be called from update_clock
*/
void check_iqs(int real_update);

/*
update a clock cycle:
	real   =  operation cycle
	unreal =  immediate cycle
*/
void update_clock(int real_update);

/*
 clock update for operations with immediate usage
 calling clock_update with 0 if an immediate was used
 nothing if wasn't
*/
int immediate_clk(operation* op, int iq2);


/*
Debugging function to stop run and print stop info
in order to handle C's awful sig-fault stops
stoper(__FUNC__,__LINE__);
*/
void stoper(const char* func, int line);
