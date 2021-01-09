#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 500
#define MAX_LABLE_LEN 50

typedef struct _lable {
	char lable_name[MAX_LABLE_LEN];
	int address;
	struct _lable *next_lable;
}lable;

typedef struct _instruction {
	
	char opcode[5];
	char rd[5];
	char rs[5];
	char rt[5];
	char imm[50];

}instruction;

/*====================================== Function Declarations =====================================*/
lable* add_lable(lable* head, char* lable_name, int address);
lable* create_single_lable(char *line, int pc, lable *lable_list);
int find_lable_pc(lable *head, char *lable_name);
lable* create_lable_list(FILE *program);
char* get_opcode(char *line, char *opcode);
char* get_rd(char *line, char *rd);
char* get_rs(char *line, char *rs);
char* get_rt(char *line, char *rt);
char* get_imm(char *line, char *imm);
void read_word_line(char *line, int *dmem_arr);
short int is_using_imm(instruction *inst);
void read_ragular_instruction(char *line, instruction *inst);
instruction** create_memin_arr(FILE *program, lable *lable_list, int *dmem_arr);
void update_labels(instruction **memin_arr, lable *lable_list);
void print_opcode(FILE *imem, char *opcode);
void print_register_num(FILE *imem, char *reg);
void print_to_file(instruction **memin_arr, int *dmem_arr, FILE *imem, FILE *dmem);
void free_instruction_mem(instruction **memin_arr);
void free_lable_list(lable *lable_list);
/*========================================= Lable functions ========================================*/


/*Adds a single lable to the lable list*/
lable* add_lable(lable* head, char* lable_name, int address) {
	lable *new_lable = (lable *)calloc(1, sizeof(lable));
		if (new_lable == NULL) {
			printf("Error accured during allocating memory\n");
			return NULL;
		}
		strcpy(new_lable->lable_name, lable_name);
		new_lable->address = address;
		new_lable->next_lable = head;
		head = new_lable;
		return head;
}

/*Creates a single lable*/
lable* create_single_lable(char *line, int pc, lable *lable_list) {
	int i;
	char *temp;

	i = 0;
	temp = &line[i];
	while (temp[i] == ' ' || temp[i] == '\t') {
		i++;
	}
	temp = &temp[i];
	i = 0;
	while (temp[i] != ':') {
		i++;
	}
	temp[i] = '\0';
	lable_list = add_lable(lable_list, temp, pc);
	return lable_list;
}

/*Finds the pc of the lable*/
int find_lable_pc(lable *head, char *lable_name) {

	while (head->lable_name != NULL) {
		if (strcmp(head->lable_name, lable_name) == 0) {
			return head->address;
		}
		else {
			head = head->next_lable;
		}
	}
	return -1; // if the lable wasn't found
}

/*Creates the lable list*/
lable* create_lable_list(FILE *program) {
	char line[MAX_LINE_LEN];
	char *temp;
	int pc, i;
	lable *lable_list;
	instruction *temp_instruction;
	char word[6] = ".word";

	pc = 0;
	i = 0;
	temp_instruction = (instruction *)calloc(1, sizeof(instruction));
	lable_list = NULL;
	while (!feof(program)) {
		fgets(line, MAX_LINE_LEN, program);
		if (strcmp(line, "\n") == 0) { // an empty line
			continue;}
		if (strstr(line, word) != NULL) {// ".word" instuction
			continue;}
		if (strstr(line, ":") != NULL) {//check if lable
			if (strchr(line, '#') != NULL) {//there is a remark in this line
				if (strchr(line, ':') > strchr(line, '#')) {//the ":" is a part of the remark
					if (strstr(line, ",") != NULL) { //may be an instruction line
						if (strchr(line, ',') > strchr(line, '#')) {//the "," is a part of the remark so it is not an instruction line
							continue; // not a lable and not an instruction
						}
						read_ragular_instruction(line, temp_instruction);//read instruction line
						if (strcmp(temp_instruction->rs, "imm") == 0 || strcmp(temp_instruction->rd, "imm") == 0 || strcmp(temp_instruction->rt, "imm") == 0) {//check if imm
							pc += 2;
						}
						else {
							pc += 1;
						}
						continue; //an instruction but not a lable
					}
				}
			}
			//lable not in the remark
			lable_list = create_single_lable(line, pc, lable_list);
			//skip the lable
			temp = &line[0];
			i = 0;
			while (temp[i] != ':') {
				i++;
			}
			temp = &temp[i + 1];
			if (strstr(temp, ",") != NULL) {//check if the rest is an instruction
				if (strstr(temp, " #") != NULL) {
					if (strchr(temp, ',') > strchr(temp, '#')) {continue;}
				}
				read_ragular_instruction(temp, temp_instruction);//read instruction line
				if (strcmp(temp_instruction->rs, "imm") == 0 || strcmp(temp_instruction->rd, "imm") == 0 || strcmp(temp_instruction->rt, "imm") == 0) {//check if imm
					pc += 2;
				}
				else {
					pc += 1;
				}
				continue; 
			}
		}
		//if there is not a lable, check if there is an instruction
		if (strstr(line, ",") != NULL) { 
			if (strchr(line, '#') != NULL) {//there is a remark in this line
				if (strchr(line, ',') > strchr(line, '#')) {//the "," is a part of the remark so it is not an instruction line
					continue;
				}
			}
			read_ragular_instruction(line, temp_instruction);//read instruction line
			if (strcmp(temp_instruction->rs, "imm") == 0 || strcmp(temp_instruction->rd, "imm") == 0 || strcmp(temp_instruction->rt, "imm") == 0) {//check if imm
				pc += 2;
			}
			else {
				pc += 1;
			}
			continue;
		}
	}
	free(temp_instruction);
	return lable_list;
}


/*========================================= Memory functions ========================================*/


/*Findes the opcode and returns a pointer to the next index in the line after the opcode*/
char* get_opcode(char *line, char *opcode) {
	char *start;
	int i = 0; 
	while (line[i] == ' ' || line[i] == '\t') {
		i++;
	}
	start = &line[i];
	i = 0;
	while (start[i] != ' ' && start[i] != '\t') {
		i++;
	}
	start[i] = '\0';
	strcpy(opcode, start);
	return &start[i + 1];
	
}

/*Findes the name of the $rd register and returns a pointer to the next index in the line after the the $rd register*/
char* get_rd(char *line, char *rd) {
	int i = 0;
	while (line[i] != ' ' && line[i] != '\t' && line[i] != ',') {
		i++;
	}
	line[i] = '\0';
	strcpy(rd, line);
	return &line[i + 1];
}

/*Findes the name of the $rs register and returns a pointer to the next index in the line after the the $rs register*/
char* get_rs(char *line, char *rs) {
	int i = 0;
	while (line[i] != ' ' && line[i] != '\t' && line[i] != ',') {
		i++;
	}
	line[i] = '\0';
	strcpy(rs, line);
	return &line[i + 1];
}

/*Findes the name of the $rt register and returns a pointer to the next index in the line after the the $rt register*/
char* get_rt(char *line, char *rt) {
	int i = 0;
	while (line[i] != ' ' && line[i] != '\t' && line[i] != ',') {
		i++;
	}
	line[i] = '\0';
	strcpy(rt, line);
	return &line[i + 1];
}

/*Findes the value of $imm*/
char* get_imm(char *line, char *imm) {
	int i = 0;
	while (line[i] != ' ' && line[i] != '\t' && line[i] != ',' && line[i] != '#' && line[i] != '\n') {
		i++;
	}
	line[i] = '\0';
	strcpy(imm, line);
	return &line[i + 1];
}

/*Reads '.word' command line*/
void read_word_line(char *line, int *dmem_arr) {
	char *first, address[15], data[15];
	int i, address_int, data_int;
	char word[6] = ".word";

	//skip ".word"
	first = strstr(line, word);
	first = &first[5];
	i = 0;
	while (first[i] == ' ' || first[i] == '\t') {
		i++;
	}
	first = &first[i];
	i = 0;
	while (first[i] != ' ' && first[i] != '\t') {
		address[i] = first[i];
		i++;
	}
	address[i] = '\0';
	first = &first[i];
	i = 0;
	while (first[i] == ' ' || first[i] == '\t') {
		i++;
	}
	first = &first[i];
	i = 0;
	while (first[i] != ' ' && first[i] != '\t' && first[i] != '\n') {
		data[i] = first[i];
		i++;
	}
	data[i] = '\0';
	if (address[0] == '0' && (address[1] == 'x' || address[1] == 'X')) {//address in hexadecimal representation
		address_int = (int)strtol(address, NULL, 16);
	}
	else {
		address_int = (int)atoi(address);
	}
	if (data[0] == '0' && (address[1] == 'x' || address[1] == 'X')) {//data in hexadecimal representation
		data_int = (int)strtol(data, NULL, 16);
	}
	else {
		data_int = (int)atoi(data);
	}

	dmem_arr[address_int] = data_int;
}

/*Check if the instruction is using immidiate*/
short int is_using_imm(instruction *inst) {
	if (strcmp(inst->rt, "imm") == 0 || strcmp(inst->rs, "imm") == 0 || strcmp(inst->rd, "imm") == 0) {
		return 1;
	}
	return 0;
}

/*Creates regular instruction*/
void read_ragular_instruction(char *line, instruction *inst) {
	char *start;
	int i;

	start = &line[0];
	start = get_opcode(start, inst->opcode);
	i = 0;
	while (start[i] != '$') {
		i++;
	}
	start = &start[i + 1];
	start = get_rd(start, inst->rd);
	i = 0;
	while (start[i] != '$') {
		i++;
	}
	start = &start[i + 1];
	start = get_rs(start, inst->rs);
	i = 0;
	while (start[i] != '$') {
		i++;
	}
	start = &start[i + 1];
	start = get_rt(start, inst->rt);
	i = 0;
	while (start[i] == ',' || start[i] == ' ' || start[i] == '\t') {
		i++;
	}
	start = &start[i];
	start = get_imm(start, inst->imm);
}

/*Creates the memory instructions array*/
instruction** create_memin_arr(FILE *program, lable *lable_list, int *dmem_arr) {
	instruction **memin_arr;
	char line[MAX_LINE_LEN];
	int index, i;
	//char opcode[8], rd[4], rs[4], rt[4], imm[20];
	char *start;
	char word[6] = ".word";
	index = 0;

	memin_arr = (instruction **)calloc(1024, sizeof(instruction *));
	while (!feof(program)) {
		fgets(line, MAX_LINE_LEN, program);
		if (strcmp(line, "\n") == 0) {// an empty line 
			continue;
		}
		if (strstr(line, word) != NULL) {
			read_word_line(line, dmem_arr);
			continue;
		}
		if (strstr(line, ",") != NULL) { //may be a regular instruction line
			if (strchr(line, '#') != NULL) {//there is a remark in this line
				if (strchr(line, ',') > strchr(line, '#')) {//the "," is a part of the remark so it is not an instruction line
					continue;
				}
			}
			start = &line[0];
			if (strstr(line, ":") != NULL) {//may be a lable
				if (strchr(line, '#') != NULL) {//there is a remark in this line
					if (strchr(line, ':') < strchr(line, '#')) {//the ":" is a part of a lable
						//read to skip the lable
						i = 0;
						start = &line[i];
						while (start[i] == ' ' || start[i] == '\t') {
							i++;
						}
						start = &line[i];
						i = 0;
						while (start[i] != ':') {
							i++;
						}
						start = &start[i + 1];
					}
				}
			}
			memin_arr[index] = (instruction *)calloc(1, sizeof(instruction));
			read_ragular_instruction(start, memin_arr[index]);
			index++;
		}
	}
	return memin_arr;
}

/*Updates the memory array with the labels pc*/
void update_labels(instruction **memin_arr, lable *lable_list) {
	int i, pc;
	for (i = 0; i < 1024; i++) {
		if (memin_arr[i] != NULL) {
			pc = find_lable_pc(lable_list, memin_arr[i]->imm);
			if (pc != -1) {
				_itoa(pc, memin_arr[i]->imm, 10);
				continue;
			}
			if (strcmp(memin_arr[i]->imm, "zero") == 0) {
				strcpy(memin_arr[i]->imm, "0");
			}
		}
	}
}


/*========================================= Print functions ========================================*/

void print_opcode(FILE *imem, char *opcode) {
	if (strcmp(opcode, "add") == 0) {
		fprintf(imem, "00");
	}
	if (strcmp(opcode, "sub") == 0) {
		fprintf(imem, "01");
	}
	if (strcmp(opcode, "and") == 0) {
		fprintf(imem, "02");
	}
	if (strcmp(opcode, "or") == 0) {
		fprintf(imem, "03");
	}
	if (strcmp(opcode, "xor") == 0) {
		fprintf(imem, "04");
	}
	if (strcmp(opcode, "mul") == 0) {
		fprintf(imem, "05");
	}
	if (strcmp(opcode, "sll") == 0) {
		fprintf(imem, "06");
	}
	if (strcmp(opcode, "sra") == 0) {
		fprintf(imem, "07");
	}
	if (strcmp(opcode, "srl") == 0) {
		fprintf(imem, "08");
	}
	if (strcmp(opcode, "beq") == 0) {
		fprintf(imem, "09");
	}
	if (strcmp(opcode, "bne") == 0) {
		fprintf(imem, "0A");
	}
	if (strcmp(opcode, "blt") == 0) {
		fprintf(imem, "0B");
	}
	if (strcmp(opcode, "bgt") == 0) {
		fprintf(imem, "0C");
	}
	if (strcmp(opcode, "ble") == 0) {
		fprintf(imem, "0D");
	}
	if (strcmp(opcode, "bge") == 0) {
		fprintf(imem, "0E");
	}
	if (strcmp(opcode, "jal") == 0) {
		fprintf(imem, "0F");
	}
	if (strcmp(opcode, "lw") == 0) {
		fprintf(imem, "10");
	}
	if (strcmp(opcode, "sw") == 0) {
		fprintf(imem, "11");
	}
	if (strcmp(opcode, "reti") == 0) {
		fprintf(imem, "12");
	}
	if (strcmp(opcode, "in") == 0) {
		fprintf(imem, "13");
	}
	if (strcmp(opcode, "out") == 0) {
		fprintf(imem, "14");
	}
	if (strcmp(opcode, "halt") == 0) {
		fprintf(imem, "15");
	}
}

void print_register_num(FILE *imem, char *reg) {
	if (strcmp(reg, "zero") == 0) {
		fprintf(imem, "0");
	}
	if (strcmp(reg, "imm") == 0) {
		fprintf(imem, "1");
	}
	if (strcmp(reg, "v0") == 0) {
		fprintf(imem, "2");
	}
	if (strcmp(reg, "a0") == 0) {
		fprintf(imem, "3");
	}
	if (strcmp(reg, "a1") == 0) {
		fprintf(imem, "4");
	}
	if (strcmp(reg, "t0") == 0) {
		fprintf(imem, "5");
	}
	if (strcmp(reg, "t1") == 0) {
		fprintf(imem, "6");
	}
	if (strcmp(reg, "t2") == 0) {
		fprintf(imem, "7");
	}
	if (strcmp(reg, "t3") == 0) {
		fprintf(imem, "8");
	}
	if (strcmp(reg, "s0") == 0) {
		fprintf(imem, "9");
	}
	if (strcmp(reg, "s1") == 0) {
		fprintf(imem, "A");
	}
	if (strcmp(reg, "s2") == 0) {
		fprintf(imem, "B");
	}
	if (strcmp(reg, "gp") == 0) {
		fprintf(imem, "C");
	}
	if (strcmp(reg, "sp") == 0) {
		fprintf(imem, "D");
	}
	if (strcmp(reg, "fp") == 0) {
		fprintf(imem, "E");
	}
	if (strcmp(reg, "ra") == 0) {
		fprintf(imem, "F");
	}
}

/*Creates the imemin.txt and dmemin.txt files*/
void print_to_file(instruction **memin_arr, int *dmem_arr, FILE *imem, FILE *dmem) {
	int i, imm_int;

	//create imemin
	for (i = 0; i < 1024; i++) {
		if (memin_arr[i] == NULL) {
			fprintf(imem, "00000\n");
		}
		else {//print an instruction
			print_opcode(imem, memin_arr[i]->opcode);
			print_register_num(imem, memin_arr[i]->rd);
			print_register_num(imem, memin_arr[i]->rs);
			print_register_num(imem, memin_arr[i]->rt);
			// if imm print imm and newline
			if (is_using_imm(memin_arr[i])) {
				fprintf(imem, "\n");
				if (memin_arr[i]->imm[0] == '0' && (memin_arr[i]->imm[1] == 'x' || memin_arr[i]->imm[1] == 'X')) {//the immidiate is a hex number
					imm_int = strtol(memin_arr[i]->imm, NULL, 16);
				}
				else {//the immidiate is a decimal number
					imm_int = atoi(memin_arr[i]->imm);
				}
				if (imm_int >= 0) {
					fprintf(imem, "%05X\n", imm_int);
				}
				else {
					fprintf(imem, "%05X\n", imm_int & 0xfffff);
				}
				
			}
			else {
				fprintf(imem, "\n");
			}
		}
	}

	//create dmemin
	for (i = 0; i < 4096; i++) {
		fprintf(dmem, "%08X\n", dmem_arr[i]);
	}
}

/*========================================= Clean memory functions ========================================*/

void free_instruction_mem(instruction **memin_arr) {
	int i;

	for (i = 0; i < 1024; i++) {
		if (memin_arr[i] != NULL) {
			free(memin_arr[i]);
		}
	}
	free(memin_arr);
}

void free_lable_list(lable *lable_list) {
	lable *temp;

	while (lable_list->next_lable != NULL) {
		temp = lable_list;
		lable_list = lable_list->next_lable;
		free(temp);
	}
	free(lable_list);
}

int main(int argc, char* argv[]) {
	FILE *program, *imem, *dmem;
	lable *lable_list;
	instruction **memin_arr;
	int *dmem_arr;

	program = fopen(argv[1], "r");
	lable_list = create_lable_list(program); //the first run - creates the lable list

	rewind(program);
	dmem_arr = (int *)calloc(4096, sizeof(int));
	memin_arr = create_memin_arr(program, lable_list, dmem_arr);// the second run - creates the imem and dmem datat structures
	update_labels(memin_arr, lable_list); // update the lables to their pc

	imem = fopen(argv[2], "w");
	dmem = fopen(argv[3], "w");
	print_to_file(memin_arr, dmem_arr, imem, dmem); // creates the output files

	fclose(program);
	fclose(imem);
	fclose(dmem);

	//claen the memory
	free(dmem_arr);
	free_instruction_mem(memin_arr);
	free_lable_list(lable_list);

	return 0;
}

