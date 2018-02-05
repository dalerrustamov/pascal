# Pascal Compiler

Pascal Compiler uses top-down parsing to compile the Pascal code, and works with the lexer that breaks down the code into tokens. There are 6 token types that lexer recognizes: word, integer, real, real with an e., special characters and column-equal.

Below, I gave an example of a pascal code that my compiler compiles in less than half a second:
begin
    while z=6 do
       if x > 5 then
          x:=5
       else
           begin
          	y:=6
            end   
end.
