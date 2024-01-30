module collatz( input logic         clk,   // Clock
		input logic 	    go,    // Load value from n; start iterating
		input logic  [31:0] n,     // Start value; only read when go = 1
		output logic [31:0] dout,  // Iteration value: true after go = 1
		output logic 	    done); // True when dout reaches 1

reg [31:0] curr;
   /* Replace this comment and the code below with your solution */
   always_ff @(posedge clk) begin

   if(go == 1)
   begin
      curr <= n;
      dout <= 0;
      done <= 1'b0;
   end
   else
   begin
      if(curr == 1)
         begin
               done <= 1;
               dout <= curr;
               curr <= dout;
         end
      else
      begin
         if(curr%2 == 1)
            begin 
               done <= 0;
               dout <= 3*curr + 1;
               curr <= dout;
            end
         else
            begin
               done <= 0;
               dout <= curr/2;
               curr <= dout;
            end
         end
      end
   end
   /* Replace this comment and the code above with your solution */

endmodule
