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
   logic [11:0] left_display, right_display;
   logic [21:0] counter;
   
   logic flag_done;
   
   logic logic_flag;      //Edited

   assign clk = CLOCK_50;
assign right_display = count [11:0];
//assign left_display = start [11:0];
 
   range #(256, 8) // RAM_WORDS = 256, RAM_ADDR_BITS = 8)
         r ( .* ); // Connect everything with matching names

      hex7seg h1 ( .a({left_display[11:8]}), .y(HEX5) );   // leftmost
      hex7seg h2 ( .a(left_display[7:4]), .y(HEX4) );
      hex7seg h3 ( .a(left_display[3:0]), .y(HEX3) );
      hex7seg h4 ( .a(right_display[11:8]), .y(HEX2) );
      hex7seg h5 ( .a(right_display[7:4]), .y(HEX1) );
      hex7seg h6 ( .a(right_display[3:0]), .y(HEX0) );
   // Replace this comment and the code below it with your own code;
   // The code below is merely to suppress Verilator lint warnings
   always_ff@(posedge clk) begin
	if ( KEY [3] == 1'b0 ) begin
	n <= {2'b00, SW};
	start <= {22'd0, SW};
	go <= 1'b1;
	counter <= {20'hFFFFF,2'b11};
	logic_flag <= 1'b0;
        left_display <= 12'd0;
end
	else if (go == 1'b1) begin
	go <= 1'b0;
end
	else if (done == 1'b1)begin
	logic_flag <= 1'b1;
	start <= 12'd0;
	left_display <= n;
	
	
end
	else if (KEY [0] == 1'b0 && logic_flag == 1'b1) begin
	counter <= counter - 1'd1;
	if (counter == 22'd0) begin
	start <= start + 1;
	left_display <= n + {20'd0, start} + 12'd1;
	counter <= {20'hFFFFF,2'b11};
end
end
	else if (KEY [1] == 1'b0 && logic_flag == 1'b1) begin
	counter <= counter - 1'd1;
	if (counter == 22'd0) begin
	start <= start - 1;
	left_display <= n + {20'd0, start} + 12'd1;
	counter <= {20'hFFFFF,2'b11};
end
end
	else if (KEY [2] == 1'b0 && logic_flag == 1'b1) begin
	counter <= counter - 1'd1;
	if (counter == 22'd0) begin
	start <= 32'd0;
	left_display <= n + {20'd0, start};
	counter <= {20'hFFFFF,2'b11};
end
end
	
	

end
   
   
  
endmodule


/*module lab1( input logic        CLOCK_50,
	     input logic [3:0] 	SW,
	     output logic [6:0] HEX0
	     );

logic 			clk;
logic [6:0] buffer_a;

hex7seg HE1 ( .a(SW), .y(buffer_a));
assign clk = CLOCK_50;
always_ff@(posedge clk) begin
	HEX0 <= buffer_a;
end
endmodule*/
