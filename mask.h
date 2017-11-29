#define MAX 512
#define CACHE_SET 8
#define NUMREG 32

/*
bit positions in ALU control signals

char C_ULA
0- ALU control 0
1- ALU control 1
2- ALU control 2
3- ALU control 3
g4- not used
5- not used
6- not used
7- not used


bit positions in the 16 control signals specified by the control unit
int S_C
0- (RegDst)
1- (RegWrite)
2- (ALUSrcA)
3- (ALUSrcB0)
4- (ALUSrcB1)
5- (ALUOp0)
6- (ALUOp1)
7- (PCSource0)
8- (PCSource1)
9- (PCWriteCond)
10-(PCWrite)
11-(IorD)
12-(MemRead)
13-(MemWrite)
14-(MemtoReg)
15-(IRWrite)
*/


/* Bit masks. */

/* Disable zero bit (to use with global var zero, 8-bit char) */
#define enable_bit_zero        0x01	// 00000001		(OR)
#define disable_bit_zero     0xfe  // 1111 1110		(AND)

/*  Get bit zero from global var zero.*/
#define split_bit_zero			0x01  // 0000 0001		(AND)

/* Get ALU peration (SC_ALU 3-0) */
#define split_controle_ula 	0x0f  // 0000 1111		(AND)

/* Enable/disable ALU operation (SC_ALU 3-0) */
#define enable_sum				0x02	// 00000010		(OR)
#define enable_sub			0x06	// 00000110		(OR)
#define enable_and				0x00	// 00000000		(OR)
#define enable_or				0x01	// 00000001		(OR)
#define enable_slt				0x07	// 00000111		(OR)
#define enable_nor				0x0c	// 00001100		(OR)
#define zera_ALU_control		0xf0  	// 11110000		(AND)

/* Split instruction fields.
   31  26 25 21 20 16 15 11 10  6 5    0 */
#define split_cop			0xfc000000	    // 111111 00000 00000 00000 00000 000000  (AND)
#define split_rs			0x03e00000	    // 000000 11111 00000 00000 00000 000000  (AND)
#define split_rt			0x001f0000	    // 000000 00000 11111 00000 00000 000000  (AND)
#define split_rd           0x0000f800	// 000000 00000 00000 11111 00000 000000  (AND)
#define split_cfunction		0x0000003f	// 000000 00000 00000 00000 00000 111111  (AND)
#define split_immediate     0x0000ffff	// 000000 00000 00000   1111111111111111  (AND)
#define split_jump_address 0x03ffffff // 000000     11111111111111111111111111  (AND)

/* Get rid of 2 less significant bits from field function (CU_ALU) */
#define zera_2bits_cfunction		0x0f		// 00 00 1111   (AND)

/* Get the 4 MSB from PC. */
#define split_4bits_PC			0xf0000000	// 11110000000000000000000000000000  (AND)

/* Enable control singals (SC) */
#define enable_RegDst			0x0001	   // 0000 0000 0000 0001  (OR)
#define enable_RegWrite			0x0002      // 0000 0000 0000 0010  (OR)
#define enable_ALUSrcA         	0x0004      // 0000 0000 0000 0100  (OR)
#define enable_ALUSrcB0        	0x0008      // 0000 0000 0000 1000  (OR)
#define enable_ALUSrcB1        	0x0010      // 0000 0000 0001 0000  (OR)
#define enable_ALUOp0          	0x0020      // 0000 0000 0010 0000  (OR)
#define enable_ALUOp1          	0x0040      // 0000 0000 0100 0000  (OR)
#define enable_PCSource0   		0x0080      // 0000 0000 1000 0000  (OR)
#define enable_PCSource1	 		0x0100      // 0000 0001 0000 0000  (OR)
#define enable_PCWriteCond		0x0200      // 0000 0010 0000 0000  (OR)
#define enable_PCWrite			0x0400      // 0000 0100 0000 0000  (OR)
#define enable_IorD				0x0800      // 0000 1000 0000 0000  (OR)
#define enable_MemRead			0x1000      // 0001 0000 0000 0000  (OR)
#define enable_MemWrite			0x2000      // 0010 0000 0000 0000  (OR)
#define enable_MemtoReg			0x4000      // 0100 0000 0000 0000  (OR)
#define enable_IRWrite			0x8000      // 1000 0000 0000 0000  (OR)

/* Disable control signals (SC) */
#define disable_RegDst				0xfffe		// 1111 1111 1111 1110  (AND)
#define disable_RegWrite			0xfffd      // 1111 1111 1111 1101  (AND)
#define disable_ALUSrcA			0xfffb      // 1111 1111 1111 1011  (AND)
#define disable_ALUSrcB0			0xfff7      // 1111 1111 1111 0111  (AND)
#define disable_ALUSrcB1			0xffef      // 1111 1111 1110 1111  (AND)
#define disable_ALUOp0				0xffdf      // 1111 1111 1101 1111  (AND)
#define disable_ALUOp1				0xffbf      // 1111 1111 1011 1111  (AND)
#define disable_PCSource0			0xff7f      // 1111 1111 0111 1111  (AND)
#define disable_PCSource1			0xfeff      // 1111 1110 1111 1111  (AND)
#define disable_PCWriteCond		0xfdff      // 1111 1101 1111 1111  (AND)
#define disable_PCWrite			0xfbff      // 1111 1011 1111 1111  (AND)
#define disable_IorD				0xf7ff      // 1111 0111 1111 1111  (AND)
#define disable_MemRead			0xefff      // 1110 1111 1111 1111  (AND)
#define disable_MemWrite			0xdfff      // 1101 1111 1111 1111  (AND)
#define disable_MemtoReg			0xbfff      // 1011 1111 1111 1111  (AND)
#define disable_IRWrite			0x7fff      // 0111 1111 1111 1111  (AND)

/* Split control signals. */
#define split_RegDst		   0x0001	   // 0000 0000 0000 0001  (OR)
#define split_RegWrite		   0x0002      // 0000 0000 0000 0010  (OR)
#define split_ALUSrcA    	   0x0004      // 0000 0000 0000 0100  (OR)
#define split_ALUSrcB0        0x0008      // 0000 0000 0000 1000  (OR)
#define split_ALUSrcB1        0x0010      // 0000 0000 0001 0000  (OR)
#define split_ALUOp0          0x0020      // 0000 0000 0010 0000  (OR)
#define split_ALUOp1          0x0040      // 0000 0000 0100 0000  (OR)
#define split_PCSource0       0x0080      // 0000 0000 1000 0000  (OR)
#define split_PCSource1       0x0100      // 0000 0001 0000 0000  (OR)
#define split_PCWriteCond     0x0200      // 0000 0010 0000 0000  (OR)
#define split_PCWrite         0x0400      // 0000 0100 0000 0000  (OR)
#define split_IorD            0x0800      // 0000 1000 0000 0000  (OR)
#define split_MemRead         0x1000      // 0001 0000 0000 0000  (OR)
#define split_MemWrite        0x2000      // 0010 0000 0000 0000  (OR)
#define split_MemtoReg        0x4000      // 0100 0000 0000 0000  (OR)
#define split_IRWrite         0x8000      // 1000 0000 0000 0000  (OR)

/* Control of explicit sequencing for microprogrammed control. */
#define sequence   0x00030000
#define tab_desp1   0x00010000
#define tab_desp2   0x00020000
#define return_search 0x00000000

/* Split sequencing control. */
#define split_AddrCtrl 0x00030000


/* Split status bits. */
#define split_S0 0x01
#define split_S1 0x02
#define split_S2 0x04
#define split_S3 0x08

#define split_COp0 0x01
#define split_COp1 0x02
#define split_COp2 0x04
#define split_COp3 0x08
#define split_COp4 0x10
#define split_COp5 0x20

#define split_bitmenos_sig 0x01
