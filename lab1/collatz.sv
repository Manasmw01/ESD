module collatz( input logic     	clk,   // Clock
   	 input logic     	go,	// Load value from n; start iterating
   	 input logic  [31:0] n, 	// Start value; only read when go = 1
   	 output logic [31:0] dout,  // Iteration value: true after go = 1
   	 output logic     	done); // True when dout reaches 1

   /* Replace this comment and the code below with your solution */
   always_ff @(posedge clk)
   begin
      if(go == 1)
      begin
         dout <= n;
      end
      else
      begin
         if(dout > 1)
         begin
            if(dout%2 == 1)
            begin
               dout <= 3*dout + 1;
            end
   			else
            begin
               dout <= dout/2;
            end
         end
      end
   end

assign done = (dout==1) ? 1'b1 : 1'b0;
/* Replace this comment and the code above with your solution */
endmodule



