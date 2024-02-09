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
   logic [31:0] 		start_flag;
   logic [15:0] 		count;

   logic [11:0] 		n;
   
   // New
   assign clk = CLOCK_50;




   hex7seg h1 (.a({counter[11:8]}), .y(HEX5)); // h1 is leftmost
   hex7seg h2 (.a(counter[7:4]), .y(HEX4));
   hex7seg h3 (.a(counter[3:0]), .y(HEX3));
   hex7seg h4 (.a(count[11:8]), .y(HEX2));
   hex7seg h5 (.a(count[7:4]), .y(HEX1));
   hex7seg h6 (.a(count[3:0]), .y(HEX0)); //h6 is rightmost
   
   range #(256, 8) // RAM_WORcounterDS = 256, RAM_ADDR_BITS = 8)
         r ( .* ); // Connect everything with matching names


//   range #(16, 4) r(.clk(clk), .go(go), .start(start), .done(done), .count(count));

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
   logic [11:0] 		counter;
   logic [21:0] 		millis;
   logic [7:0] 		offset;
   //assign go = !KEY[3];

   logic pressed;
   logic doneflag;
   //assign start = {SW[1:0], SW, SW, SW};
   //assign start = {22'b0, SW};
   //assign start = {20'b0, counter};
   //assign n = {SW[1:0], SW};

   always_ff @(posedge clk) 
   begin

	if( KEY [3] == 1'b0)
	begin
	n <= {2'b00, SW};
	start <= {22'd0, SW};
	go <= 1'b1;
	counter <= 12'd0;
doneflag <= 0;
	end

	else if (go == 1'b1) begin
		go <= 1'b0;
	end

	else  if (done)
	begin 
	doneflag<= 1;
	start <= 0;
	counter <= n;
	end


      else if(!KEY[0] && doneflag == 1'b1)
      begin 
           pressed <= 1;
           millis <= millis + 1;
           if(millis == 22'h2FFFFF)
           begin
               millis <= 0;
               if(counter < SW + 255)
               begin
                   counter <= n + {20'd0, start} + 12'd1;
		   start <= start + 1;
	       end
           end
      end

      else if(!KEY[1] && doneflag == 1'b1)
      begin 
           pressed <= 1;
           millis <= millis + 1;
           if(millis == 22'h2FFFFF)
           begin
               millis <= 0;
               if(counter > SW)
               begin
                   counter <= n + {20'd0, start} - 12'd1;
		   start <= start - 1;
	       end
           end
      end

      else if(!KEY[2] && doneflag == 1'b1)
      begin 
          pressed <= 1;
           millis <= millis + 1;
           if(millis == 22'h2FFFFF)
           begin
               millis <= 0;
	       start <=0 ;
               counter <= n + {20'd0, start};
	   end
           //doneflag <= 0;
//           millis <= 0;
//           offset <= 0;
           pressed <= 0;
      end
//      else 
//	begin 
//           counter <= {20'b0, SW};
//	end

    if(pressed == 0)   
        begin
           millis <= 0;
           offset <= 0;
           counter <= {20'b0, SW};

     end 


   end

endmodule


/*
make hex7seg

make obj_dir/Vcollatz

make collatz.vcd

make obj_dir/Vrange

make range.vcd

make lab1.qpf

make output_files/lab1.sof

*/
