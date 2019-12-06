# Task 1

* Implemented createNumberNode, createFunctionNode, eval, evalFuncNode, and printRetVal

* Created findFunctionType to find the NUM_TYPE for basic math operations like add sub mult div and neg

* Added "exp2"|"cbrt"|"hypot" but REMEMBER to add the rest of the functions in the grammar

<img width="346" alt="Screen Shot 2019-12-05 at 4 35 16 PM" src="https://user-images.githubusercontent.com/46359299/70286545-7d14f680-1780-11ea-8ec0-3aab8c7c9003.png">

# Task 2

* Added grammar in the lex file for to allow for symbol ast nodes
* Added symbol\_table\_node
* Added symNodes: let\_section let\_list let\_elem

* Implemented functions for createSymbolNode
* attachSTtoAST
* addToSymbolTable
* createSTNode
* evalSymbol

<img width="629" alt="Screen Shot 2019-12-05 at 10 46 58 PM" src="https://user-images.githubusercontent.com/46359299/70302180-38558380-17b1-11ea-91a2-f0abfb0e01d0.png">


### Remember to put function signatures in your header file, you will get compilation errors from not doing 

# Task 3

* Added type as a sval token
* Added a case for let\_elem that allows a symbol table node's val\_type to be set to INT or DOUBLE type
* Modified createSTNode because it needed another parameter for NUM\_TYPE
* Implemented another helper function for number precision –– compareValTypeWithRetVal()

<img width="629" alt="Screen Shot 2019-12-05 at 11 35 57 PM" src="https://user-images.githubusercontent.com/46359299/70305342-fb8d8a80-17b8-11ea-81bb-423fb1207046.png">

