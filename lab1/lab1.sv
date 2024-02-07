// CSEE 4840 Lab 1: Run and Display Collatz Conjecture Iteration Counts
//
// Spring 2023
//
// By: <your name here>
// Uni: <your uni here>

module lab1( input logic        CLOCK_50,  // 50 MHz Clock input
	     
	     input logic [3:0] 	KEY, // Pushbuttons; KEY[0] is rightmost

	     input logic [9:0] 	SW, // Switches; SW[0] is rightmost

	     // 7-segment LED displays; HEX0 is rightmost
	     output logic [6:0] HEX0, HEX1, HEX2, HEX3, HEX4, HEX5,

	     output logic [9:0] LEDR // LEDs above the switches; LED[0] on right
	     );

   logic 			clk, go, done;   
   logic [31:0] 		start;
   logic [15:0] 		count;

   logic [11:0] 		n;
   
   // New
   //logic [11:0] 		n;
   assign clk = CLOCK_50;

   hex7seg h1 (.a({2'b0,SW[9:8]}), .y(HEX5)); // h1 is leftmost
   hex7seg h2 (.a(SW[7:4]), .y(HEX4));
   hex7seg h3 (.a(SW[3:0]), .y(HEX3));
   hex7seg h4 (.a(count[11:8]), .y(HEX2));
   hex7seg h5 (.a(count[7:4]), .y(HEX1));
   hex7seg h6 (.a(count[3:0]), .y(HEX0)); //h6 is rightmost
   
   //   range #(256, 8) // RAM_WORDS = 256, RAM_ADDR_BITS = 8)
   //         r ( .* ); // Connect everything with matching names


   range #(16, 4) r(.clk(clk), .go(go), .start(start), .done(done), .count(count));

   // Replace this comment and the code below it with your own code;
   // The code below is merely to suppress Verilator lint warnings
   //assign HEX0 = {KEY[2:0], KEY[3:0]};
   //assign HEX1 = SW[6:0];
   //assign HEX2 = {(n == 12'b0), (count == 16'b0) ^ KEY[1],
	//	  go, done ^ KEY[0], SW[9:7]};
   //assign HEX3 = HEX0;
   //assign HEX4 = HEX1;
   //assign HEX5 = HEX2;
   //assign LEDR = SW;
   assign LEDR[0] = go;
   assign go = !KEY[3];
   //assign start = {SW[1:0], SW, SW, SW};
   assign start = {22'b0, SW};
   //assign n = {SW[1:0], SW};
endmodule
