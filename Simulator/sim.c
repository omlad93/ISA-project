#include "sim.h"

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
/*  ~~~~~~~~~~~~~~~     FILES  PARSING     ~~~~~~~~~~~~~~  */
/* *********************************************************/

/*
A function to open and parse files.
monitor.yuv & irq2In.txt re exulded and open seperatly
*/
void file_opening(char* argv[]) {
	int j;
	char* line = (char*)calloc(7, sizeof(char));
	//in Files
	Imem = fopen(argv[1], "r");
	Dmem = fopen(argv[2], "r");
	Diskin = fopen(argv[3], "r");
	//irq2In = fopen(argv[4], "r"); //disabled
	// out Files
	Memout = fopen(argv[5], "w");
	Regout = fopen(argv[6], "w");
	Trace = fopen(argv[7], "w");
	HwRT = fopen(argv[8], "w");
	Cycles = fopen(argv[9], "w");
	Leds = fopen(argv[10], "w");
	Monitor_txt = fopen(argv[11], "w");
	//monitor_yuv = fopen(argv[12], "wb");
	Diskout = fopen(argv[13], "w");

	if (Imem == NULL || Dmem == NULL || Diskin == NULL || irq2In != NULL) { // error
		fprintf(stderr, "Input files opening");
		exit(1);
	}
	else if (Memout == NULL || Regout == NULL || Trace == NULL || HwRT == NULL ||
		Cycles == NULL || Leds == NULL || Diskout == NULL) {
		fprintf(stderr, "Output files opening");
		exit(1);
	}
	else if (Monitor_txt == NULL || monitor_yuv != NULL) {
		fprintf(stderr, "Monitor files opening");
		exit(1);
	}


	//read Operations Memory to array
	j = 0;
	while (!feof(Imem)) {
		fgets(line, MAX_LINE, Imem);
		line[5] = '\0';
		sprintf(Op_Mem[j], "%s", line);
		j++;
	}
	op_mem_max = j;
	while (j < 1024) {
		sprintf(Op_Mem[j], "00000");
		j++;
	}
	fclose(Imem);
	printf(" [Parsed Imem] ");

	//read Data Memory to array
	j = 0;
	while (!feof(Dmem)) {
		fgets(line, MAX_LINE, Dmem);
		line[8] = '\0';
		Data_Mem[j] = strtol(line, NULL, 16);
		j++;
	}
	while (j < 4096) {
		Data_Mem[j] = 0;
		j++;
	}
	fclose(Dmem);
	printf(" [Parsed Dmem] ");

	parse_disk();
	parse_irq2(argv[4]);
}

/*
A funcrion to open and parse irq2_In which will be saved as an array irq_2
*/
void parse_irq2(char* path) {
	char line[MAX_LINE];
	int n, j = 0;
	printf(" [iq2 start]");
	irq2In = fopen(path, "r");
	//rewind(irq2In);
	//stoper(__FUNCTION__, __LINE__);

	while (!feof(irq2In)) { //check how many irq2 needed
		//stoper(__FUNCTION__, __LINE__);
		fgets(line, MAX_LINE, irq2In);
		j++;

	}
	printf(" [iq2 read]");
	//stoper(__FUNCTION__, __LINE__);
	n = j-1; // there are n cycles with irq2
	irq_2 = (int*)calloc(n, sizeof(int));
	j = 0;
	rewind(irq2In);

	while (!feof(irq2In)) {
		fgets(line, MAX_LINE, irq2In);
		irq_2[j] = atoi(line);
		j++;
	}




}

/*
A funcrion to parse diskinn and save informatin in Disk[][]
*/
void parse_disk() {
	char line[10];
	int line_count = 0, sector, block;
	memset(Disk, 0, BLK_CNT*SEC_CNT * sizeof(int)); //set all zeros


	while (!feof(Diskin)) {			  // read disk_in
		fgets(line, MAX_LINE, Diskin);
		sector = (int)(line_count / BLK_CNT);
		block = line_count % BLK_CNT;
		Disk[sector][block] = parse_immediate(line);
		line_count++;
	}
	fclose(Diskin);
	printf(" [Parsed Disk] ");

}

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~     FILES WRITTING     ~~~~~~~~~~~~~~  */
/* *********************************************************/

/* all functions here are pretty straight-forword and self explained */


void write_cycles() {
	fprintf(Cycles, "%u\n%u"
		, IOs[8], op_count);
	fclose(Cycles);
}

void write_monitor(char* yuv_path) {
	int x, y, pix_val;
	char *pixel = (char*)calloc(3, sizeof(char));
	//stoper(__FUNCTION__, __LINE__);
	monitor_yuv = fopen(yuv_path, "wb");

	for (y = 0; y < 288; y++) {
		for (x = 0; x < 352; x++) {
			pix_val = monitor[y][x];
			sprintf(pixel, "%02X\n", pix_val);
			fprintf(Monitor_txt, "%s", pixel);
			fwrite((char*)&pix_val, sizeof(char), 1, monitor_yuv);
			//stoper(__FUNCTION__, __LINE__);

		}
		//stoper(__FUNCTION__, __LINE__);
	}
	free(pixel);

}

void write_disk() {
	int s, b;
	for (s = 0; s < SEC_CNT; s++) { //for each sector
		for (b = 0; b < BLK_CNT; b++) { //for each block
			fprintf(Diskout, "%08X\n",
				Disk[s][b]);
		}

	}
}

void write_dmem() {
	int j;
	for (j = 0; j < 4096; j++) {
		fprintf(Memout, "%08X\n", Data_Mem[j]);
	}
	fclose(Memout);
}

void write_regout() {
	fprintf(Regout, "%08X\n%08X\n%08X\n%08X\n%08X\n%08X\n%08X\n%08X\n%08X\n%08X\n%08X\n%08X\n%08X\n%08X",
		REG[2], REG[3], REG[4], REG[5], REG[6], REG[7], REG[8], REG[9], REG[10],
		REG[11], REG[12], REG[13], REG[14], REG[15]);
	fclose(Regout);
}

/*
this function has a debug feature comapring trace file written to a given referance trace
if not debug: original_trace=NULL & debug=0
*/
int write_trace_file(operation* op, int pc, FILE* original_trace, int debug) {
	char* new_line = (char*)calloc(170, sizeof(char));
	//sprintf(new_line, "CYCLE %d %03X %s %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X\n",
	//	IOs[8], pc, Op_Mem[pc], REG[0], REG[1], REG[2], REG[3], REG[4], REG[5], REG[6], REG[7], REG[8],
	//	REG[9], REG[10], REG[11], REG[12], REG[13], REG[14], REG[15]);

	sprintf(new_line, "%03X %s %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X %08X\n",
		pc, Op_Mem[pc], REG[0], REG[1], REG[2], REG[3], REG[4], REG[5], REG[6], REG[7], REG[8],
		REG[9], REG[10], REG[11], REG[12], REG[13], REG[14], REG[15]);

	fprintf(Trace, "%s", new_line);
	if (debug) {
		return check_trace(original_trace, new_line);
	}
	return GOOD;
}

/*
use to check trace file during run compared to original trace.file
used only in fib simulating - did'nt remove to reduce compilation error
*/
int check_trace(FILE* original_trace, char* curr_line) {
	int  status = 0;
	if (!feof(original_trace)) {
		char* ref_line = calloc(200, sizeof(char));
		fgets(ref_line, MAX_LINE, original_trace);
		if (strcmp(ref_line, curr_line)) {
			//fprintf(Trace,"\n ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
			status = 1;
		}
		else {
			status = 0;
		}
	}
	return status;


}

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~    OPERATION PARSER    ~~~~~~~~~~~~~~  */
/* *********************************************************/

/*
A function to handle the possibilaty of negative immediate
*/
int parse_immediate(char* hex) {
	int imm = (int)strtol(hex, NULL, 16);
	imm = ((imm & IMM_MASK) << 12) >> 12; //fix for negative , nop for positive
	
	return imm;


}

/*
A function to parse operation from instruction memory aka IMEM
*/
void parse_opcode(char* line, operation* op, int pc) {
	int rd, rs, rt, code;
	char* temp = (char*)calloc(7,sizeof(char));

	//parse op type
	sprintf(temp, "%s", line);
	temp[2] = '\0';
	code = strtol(&temp[0], NULL, 16);

	//parse rd
	sprintf(temp, "%s", line);
	temp[3] = '\0';
	rd = strtol(&temp[2], NULL, 16);

	//parse rs
	sprintf(temp, "%s", line);
	temp[4] = '\0';
	rs = strtol(&temp[3], NULL, 16);

	//parse rt
	sprintf(temp, "%s", line);
	rt = strtol(&temp[4], NULL, 16);

	//parse immediate if needed -- unsigned
	if (rd == 1 || rs == 1 || rt == 1) { //immediate is used	
		REG[1] = parse_immediate(Op_Mem[pc+1]);	
	}
	set_operation(op, rd, rt, rs, code, line);
}

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~    CLOCKS AND IRQS     ~~~~~~~~~~~~~~  */
/* *********************************************************/

/*
Hard disk timer update - to be called by update_clock
*/
void update_hardisk_timer() {
	//IOs[4] = 0;		 //zero irq1
	if (IOs[17]) {	// disk is busy
		disk_timer++;
		if (disk_timer >= HD_CYCLE) {
			IOs[4] = 1;  //set irq1
			IOs[14] = 0; //clear disk command
			IOs[17] = 0; //set disk not busy
			disk_timer = 0;
		}
	}
}

/*
Check if irq signal is on:
if there is an irq which is both enabled and statused
*/
int irq_on() {
	int zero, one, two;
	zero = IOs[0] & IOs[3];
	one = IOs[1] & IOs[4];
	two = IOs[2] & IOs[5];
	return (zero | one | two);
}

/*
A function to check if timer reached limit - and handle it if needed
*/
void timer_check() {
	if ((IOs[12] == IOs[13]) && IOs[11]) {
		IOs[3] = 1; //set irq0 on
		IOs[12] = 0;
	}
	else if (IOs[11]) {
		IOs[12] ++;
	}
}

/*
A function to check if an irq handling is needed
handle irq2 checks and prospone if needed (immediate cycle)
to be called from update_clock
*/
void check_iqs(int real_update) {
	if (already_irq) {
		return;
	}
	//irq2 settings
	if (*irq_2 == IOs[8]) { // if its a irq_2 clock
		if (real_update) { //set irq2_status on
			IOs[5] = 1;
		}
		else {			   //delay irq2_status on
			(*irq_2)++;
		}
	}
	else {
		IOs[5] = 0;
	}
	if (irq_on()) {
		IOs[7] = pc; //set return adress
		already_irq = 1;
		pc = IOs[6];
		irq_2++;    // move array to next index
	}



	
}

/*
update a clock cycle:
	real   =  operation cycle
	unreal =  immediate cycle
*/
void update_clock(int real_update) {
	if (IOs[8] == 0) { //reset
		clk_resets++;
	}
	IOs[8] ++; //update clock cycle
	update_hardisk_timer(); //update hardisk timer
	timer_check();			//check timer for irqs
	check_iqs(real_update); // check irqs
}

/*
 clock update for operations with immediate usage
 calling clock_update with 0 if an immediate was used
 nothing if wasn't
*/
int immediate_clk(operation* op, int iq2) {
	if (op->rs == 1 || op->rd == 1 || op->rt == 1) {
		update_clock(0);
	}
	return iq2;
}


/* *********************************************************/
/*  ~~~~~~~~~~~~~~~          MAIN          ~~~~~~~~~~~~~~  */
/* *********************************************************/

/*
Debugging function to stop run and print stop info
in order to handle C's awful sig-fault stops
stoper(__FUNC__,__LINE__);
*/
void stoper(const char* func, int line) {
	printf("\n>>forced to stop in \n\t%s -> line %d", func,line);
	exit(0);

}

int main(int argc, char* argv[]) {
	
	// FILE* original_trace = fopen("tracewithcycles.txt", "r"); //debugging
	//FILE* original_trace = fopen("fibex/trace.txt", "r"); //debugging
	//int st;

	int next_iq2=0;
	char* line = NULL;
	if (argc != 14) { exit(1); }

	printf("Welcome,\nSIMP Simulator is starting: \n\tOpening files");
	file_opening(argv);
	printf(" -> DONE;\n\t");
	

	operation* op = (operation*)calloc(1, sizeof(op));

	if (!op) {
		printf("error allocating operation struct");
		exit(1);
	}

	//memset(op, 0, sizeof(operation));

	printf("\n\tStarting Operation sequance.\n\t");

	while ((-1 < pc) && (pc <= op_mem_max)) {//  start clock cycle

			
		line = Op_Mem[pc];					 //  get line to parse & operate
		parse_opcode(line, op, pc); 
		write_trace_file(op,pc, NULL,0);
		pc = (op->op_code)(op, pc);
		update_clock( 1);	 // new clock cycle - check irq
		immediate_clk(op,next_iq2);
		op_count++;	
	}
	if (pc != -1) { printf("  > Operation sequance finished [pc=%d]\n\t", pc); }
	
	//stoper(__FUNCTION__, __LINE__);
	printf("\n\tWriting files and free memory");

	write_regout();
	write_cycles();
	write_dmem();
	write_monitor(argv[12]);
	write_disk();
	fclose(Leds);
	fclose(HwRT);
	fclose(Trace);
	// fclose(original_trace); //debug
	
	printf("\n\n\tSimulator finished running.\n ");

	return GOOD;
	

}

