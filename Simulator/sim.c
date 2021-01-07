#include "sim.h"
/*
 code is explaind in header file to keep source file clean
*/

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~     FILES  PARSING     ~~~~~~~~~~~~~~  */
/* *********************************************************/

void parse_disk() {
	int j;
	char line[10];
	int line_count = 0, sector,block;
	for (j = 0; j < SEC_CNT;j++) {
		memset(Disk[j], 0, BLK_CNT); //set all zeros
	}
	
	while (!feof(Diskin)) {			  // read disk_in
		fgets(line, 10, Diskin);
		sector = (int)(line_count / BLK_CNT);
		block = line_count % BLK_CNT;
		Disk[sector][block] = parse_immediate(line);
		line_count++;
	}

	//free (line);

}

void file_opening(char* argv[]) {
	int j;
	char* line = (char*)calloc(7, sizeof(char));
	//in Files
	Imem = fopen(argv[1], "r");
	Dmem = fopen(argv[2], "r");
	Diskin = fopen(argv[3], "r");
	irq2In = fopen(argv[4], "r");
	// out Files
	Memout = fopen(argv[5], "w");
	Regout = fopen(argv[6], "w");
	Trace = fopen(argv[7], "w");
	HwRT = fopen(argv[8], "w");
	Cycles = fopen(argv[9], "w");
	Leds = fopen(argv[10], "w");
	Monitor_txt = fopen(argv[11], "w");
	monitor_yuv = fopen(argv[12], "wb");
	Diskout = fopen(argv[13], "w");

	if (Imem == NULL || Dmem == NULL || Diskin == NULL || irq2In == NULL) {
		fprintf(stderr, "Input files opening");
		exit(1);
	}
	else if (Memout == NULL || Regout == NULL || Trace == NULL || HwRT == NULL ||
		Cycles == NULL || Leds == NULL || Diskout == NULL) {
		fprintf(stderr, "Output files opening");
		exit(1);
	}
	else if (Monitor_txt == NULL || monitor_yuv == NULL) {
		fprintf(stderr, "Monitor files opening");
		exit(1);
	}

	//read Operations Memory to array
	j = 0;
	while (!feof(Imem)) {
		fgets(line, 7, Imem);
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

	//read Data Memory to array
	j = 0;
	while (!feof(Dmem)) {
		fgets(line, 10, Dmem);
		line[8] = '\0';
		Data_Mem[j] = strtol(line, NULL, 16);
		j++;
	}
	while (j < 4096) {
		Data_Mem[j] = 0;
		j++;
	}
	fclose(Dmem);

	parse_disk();
}

int parse_irq2() {
	char* line = (char*) calloc(MAX_LINE, sizeof(char));
	int n, j = 0;
	while (!feof(irq2In)) { //check how many irq2 needed
		fgets(line, MAX_LINE, irq2In);
		if (strcmp(line, "\n")) {
			j++;
		}
	}
	n = j-1; // there are n cycles with irq2
	irq_2 = (int*)calloc(n, sizeof(int));
	j = 0;
	rewind(irq2In);

	while (!feof(irq2In)) {
		fgets(line, MAX_LINE, irq2In);
		irq_2[j] = atoi(line);
		j++;
	}
	return n;


}

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~     FILES WRITTING     ~~~~~~~~~~~~~~  */
/* *********************************************************/

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
	return 0;
}

void write_cycles() {
	fprintf(Cycles, "%u\n%u"
		, IOs[8], op_count);
	fclose(Cycles);
}

void write_monitor() {
	int x, y, pix_val;
	char *pixel = (char*)calloc(3, sizeof(char));
	for (y = 0; y < 288; y++) {
		for (x = 0; x < 352; x++) {
			pix_val = monitor[x][y];
			sprintf(pixel, "%02X\n", pix_val);
			fprintf(Monitor_txt, "%s", pixel);
			//fprintf(Monitor_txt, "%s", pixel);
			fwrite((char*)&pix_val, sizeof(char), 1, monitor_yuv);
		}
	}
	//free(pixel);

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

//debug - fib only
int check_trace(FILE* original_trace, char* curr_line) {
	int  status = 0;
	if (!feof(original_trace)) {
		char* ref_line = calloc(200, sizeof(char));
		fgets(ref_line, 155, original_trace);
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

int parse_immediate(char* hex) {
	int imm = (int)strtol(hex, NULL, 16);

	if (!is_positive_imm(hex)) {
		imm = ((imm & IMM_MASK) << 12) >> 12;
	}

	return imm;


}

void parse_opcode(char* line, operation* op, int pc) {
	int rd, rs, rt, code;
	char* temp=(char*) calloc(6, sizeof(char));

	//parse op type
	sprintf(temp, "%s", line);
	temp[2] = '\0';
	code = strtol(temp, NULL, 16);

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
	free(temp);
	set_operation(op, rd, rt, rs, code, line);
}

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~    CLOCKS AND IRQS     ~~~~~~~~~~~~~~  */
/* *********************************************************/

int init_clock_cycle() {

	return GOOD;
}

int irq_on() {
	int zero, one, two;
	zero = IOs[0] & IOs[3];
	one = IOs[1] & IOs[4];
	two = IOs[2] & IOs[5];
	return (zero | one | two);
}

void timer_check() {
	if ((IOs[12] == IOs[13]) && IOs[11]) {
		IOs[3] = 1; //set irq0 on
		IOs[12] = 0;
	}
	else if (IOs[11]) {
		IOs[12] ++;
	}
}

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

void update_clock(int real_update) {
	if (IOs[8] == 0) { //reset
		clk_resets++;
	}
	IOs[8] ++; //update clock cycle
	update_hardisk_timer(); //update hardisk timer
	timer_check();			//check timer for irqs
	check_iqs(real_update); // check irqs
}

int immediate_clk(operation* op, int iq2) {
	if (op->rs == 1 || op->rd == 1 || op->rt == 1) {
		update_clock(0);
	}
	return iq2;
}

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

/* *********************************************************/
/*  ~~~~~~~~~~~~~~~          MAIN          ~~~~~~~~~~~~~~  */
/* *********************************************************/

int main(int argc, char* argv[]) {
	
	// FILE* original_trace = fopen("tracewithcycles.txt", "r"); //debugging
	FILE* original_trace = fopen("fibex/trace.txt", "r"); //debugging

	int st = 0;
	//int old_reg[16], old_pc;
	int read = -1, n, next_iq2=0;
	char* line = NULL;
	if (argc != 14) { exit(1); }
	file_opening(argv);
	n = parse_irq2();


	operation* op = (operation*)calloc(1, sizeof(op));
	if (!op) {
		printf("error allocating operation struct");
		exit(1);
	}
	memset(op, 0, sizeof(operation));


		while ((-1 < pc) && (pc <= op_mem_max)) {//  start clock cycle

			
			line = Op_Mem[pc];					 //  get line to parse & operate
			parse_opcode(line, op, pc); 
			st = write_trace_file(op,pc, original_trace,1);
			pc = (op->op_code)(op, pc);
			update_clock( 1);	 // new clock cycle - check irq
			immediate_clk(op,next_iq2);

			op_count++;	
		}

	write_regout();
	write_cycles();
	write_dmem();
	fclose(Leds);
	fclose(HwRT);
	write_monitor();
	write_disk();
	fclose(Trace);
	fclose(original_trace); //
	

	return GOOD;
	

}

