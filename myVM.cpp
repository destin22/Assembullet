/**
 * vm for my language Assembullet
 * @author Destin Krepps
 */
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
using namespace std;

void reparse(string instruction);// this will come up later as a way to use functions and jump control flow when called


vector<string> variables; //this is a easy dynamic data type to hold variables
vector<string> methods; //dynamic way of holding method headers
vector<string> instructionTracker;// this is my RIP all instrucitons are indexed 

string stack[5]; // simple stack memory to hold parameters and instruction commands its methods are below
int top=-1;

void push(string val) {
   top++;
   stack[top]=val;
}

void pop() {
   top--;
}

void peek() {
   if(top>=0) {
      cout<<stack[top]<<" ";
      cout<<endl;
   } 
   else
      cout<<"Stack is empty";
}
bool endRead = false; //syscall for ending program
bool jumped = false; // if we jumped previously were gonna dismiss that jump so we dont loop infinitely
string textFile; // the files name that we want to run

//instruction tracker
int instructionNumber = 0;// i dont think this is used anymore but im too scared to remove

	//call register to execute syscall whatever number is in the call register will execute specific code when invoked
string call = "";
	
	//printer register to know what to print
string printer = "";
	
	//input register to grab inputs ive replaced system call input for inp
string input="";
	
	//r1-r10 gen purpose registers theyre strings and well do math with ascii
string r1 = "";
string r2 = "";
string r3 = "";
string r4 = "";
string r5 = "";
string r6 = "";
string r7 = "";
string r8 = "";
string r9 = "";
string r10 = "";
		


int main()
{


	//prompt file from user
   cout << "Welcome to my Virtual environment\n\nPlease enter the name of the file you want to run followed by .txt for example\n 'HelloWorld.txt' , 'Math.txt' , 'Func.txt' , or 'jumpTest.txt'\n *!case and space sensitive!* " << endl;
   cin >> textFile;
   cout << endl <<"youve entered '" << textFile << "'" <<  endl << "Running..." <<endl <<endl;
	
	
	// first run through of the file is to search for variables
   ifstream varFile(textFile);
   string instruction = ""; //used for grabbing lines
   string varSearch = "";	//same thing but for searching for variables
	
   getline(varFile,varSearch);
	////////////////////////////////////////////////////////////////////Variable initializing first code run//////////////////////////////////////////////////////////
   if(varSearch == "Start:") {
      while(endRead == false)//search for vars to initialize before running any code
      {
         getline(varFile,varSearch);
      
         if(varSearch == "Vars:" || varSearch == "vars:")// if vars section found start setting variables
         {
            while(endRead == false)// if the end of file is reached or if you find start end setting variables because this section could be at top or bottom
            {
            
               getline(varFile,varSearch);
            	
               if(varFile.eof() || varSearch == "Code:" || varSearch == "code:" )// this stops the search if you find the end of the file or another section
               {
               	//cout << "--end of variables--\n";
                  endRead = true;
                  break;
               }
            	
            	//cout << varSearch<< endl;
               variables.push_back(varSearch);// this puts the variables into the vector 
               variables.push_back(""); // this refers to the information in that the variables stores (i+1)
            }
         
         }
         else
         {
            endRead =true;// if vars isnt found theres no variables
         }
      	
      }
   }
   else
   {
      cout << "could not find file (check if you added.txt to the end)\nor you did not lead the file with keyword 'Start:'";
      return 0;
   }
 
		
	//////////////////////////////////////////////////////Actual code run through//////////////////////////////////////////////////////	
   endRead = false;//set back to rerun code portion
		
	// now run through the file to run the code	
   ifstream infile(textFile); 
   getline(infile,instruction);
   if(instruction == "Start:")// must lead program with start to be recognized
   {
      while(endRead == false)
      {
      
         //string instruction = "";
      	
         getline(infile,instruction);
         if(instruction == "code:" || instruction == "Code:")
         {
            while(endRead==false)// read the code line by line
            {
            	
               getline(infile,instruction);//read the lines in the code section
               string command = instruction.substr(0,instruction.find(" "));
               instructionTracker.push_back(instruction);
               if(instruction == "call")
               {
               	//ready method calls
               	
                  if(find(methods.begin(), methods.end(), call)!= methods.end())
                  {
                     vector<string>::iterator it;				
                     it = find(methods.begin(), methods.end(), call); //find the location of the variable stored in vector
                  
                       
                     it = find(instructionTracker.begin(), instructionTracker.end(), "func " + call);
                     int codeLine = distance(instructionTracker.begin(), it);
                  		
                     while(instructionTracker.at(codeLine) != "endFunc")
                     {
                     		//cout << "re entered function :" << instructionTracker.at(codeLine) << endl;
                        reparse(instructionTracker.at(codeLine));
                        codeLine++;
                     }
                  }
               	
                  if(call == "0")// different sytem calls
                  {
                     endRead = true;
                  }
                  else if(call == "1" && printer !="")
                  {
                     cout << printer << endl;
                  }
               	
               }
               else if(command == "func")// if the function command is found skip the insides until called
               {
                  string cut = instruction.replace(0,5,"");//cut command out
                  stringstream check1(cut);  // create a tokenizer for the parameters to get method name 
                  methods.push_back(cut);		// add method name to data
               	
                  while(instruction != "endFunc"){ // skip the methods insides until called
                     getline(infile,instruction);
                     instructionTracker.push_back(instruction);// add the instruction to the tracker
                  }
               }
               else if(command == "mov")// if the mov command is found process the move
               {
                  string cut = instruction.replace(0,4,"");//cut command out
                  stringstream check1(cut);  // create a tokenizer for the parameters to pull command and parameters
                  string param; 
               
                  while(getline(check1, param, ',')) //push its parameters onto the stack
                  { 
                     push(param); 
                  }
               	
                  if(stack[0]== "call" ||stack[0]== "Call") // wanted to use switch for this but easier just to use ifstatement with strings
                  {	
                     call = stack[1];
                    
                  }
                  else if(stack[0]== "printer" ||stack[0]== "Printer")
                  {
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())
                     {
                        vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        printer = variables.at(location+1); // set printer to the variables content which is just i+1
                        
                     }
                     else
                     {
                        printer = stack[1];
                     }
                  		
                  }
                  else if(stack[0]== "r1" ||stack[0]== "R1") // general registers move between each other or a random string all possibilities accounted for
                  {
                     if(stack[1] == "r2"){r1 = r2;}
                     if(stack[1] == "r3"){r1 = r3;}
                     if(stack[1] == "r4"){r1 = r4;}
                     if(stack[1] == "r5"){r1 = r5;}
                     if(stack[1] == "r6"){r1 = r6;}
                     if(stack[1] == "r7"){r1 = r7;}
                     if(stack[1] == "r8"){r1 = r8;}
                     if(stack[1] == "r9"){r1 = r9;}
                     if(stack[1] == "r10"){r1 = r10;}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        r1 = variables.at(location+1); // set printer to the variables content which is just i+1}
                     }
                     else{r1 = stack[1];}
                  	
                  }	
                  else if(stack[0]== "r2" ||stack[0]== "R2") // same as above but for r2
                  {
                     if(stack[1] == "r1"){r2 = r1;}
                     if(stack[1] == "r3"){r2 = r3;}
                     if(stack[1] == "r4"){r2 = r4;}
                     if(stack[1] == "r5"){r2 = r5;}
                     if(stack[1] == "r6"){r2 = r6;}
                     if(stack[1] == "r7"){r2 = r7;}
                     if(stack[1] == "r8"){r2 = r8;}
                     if(stack[1] == "r9"){r2 = r9;}
                     if(stack[1] == "r10"){r2 = r10;}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        r2 = variables.at(location+1); // set printer to the variables content which is just i+1}
                     }
                     else{r2 = stack[1];}
                  }
                  else if(stack[0]== "r3" ||stack[0]== "R3")
                  {
                     if(stack[1] == "r1"){r3 = r1;}
                     if(stack[1] == "r2"){r3 = r2;}
                     if(stack[1] == "r4"){r3 = r4;}
                     if(stack[1] == "r5"){r3 = r5;}
                     if(stack[1] == "r6"){r3 = r6;}
                     if(stack[1] == "r7"){r3 = r7;}
                     if(stack[1] == "r8"){r3 = r8;}
                     if(stack[1] == "r9"){r3 = r9;}
                     if(stack[1] == "r10"){r3 = r10;}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        r3 = variables.at(location+1); // set printer to the variables content which is just i+1}
                     }
                     else{r3 = stack[1];}
                  }
                  else if(stack[0]== "r4" ||stack[0]== "R4")
                  {
                     if(stack[1] == "r2"){r4 = r2;}
                     if(stack[1] == "r3"){r4 = r3;}
                     if(stack[1] == "r1"){r4 = r1;}
                     if(stack[1] == "r5"){r4 = r5;}
                     if(stack[1] == "r6"){r4 = r6;}
                     if(stack[1] == "r7"){r4 = r7;}
                     if(stack[1] == "r8"){r4 = r8;}
                     if(stack[1] == "r9"){r4 = r9;}
                     if(stack[1] == "r10"){r4 = r10;}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        r4 = variables.at(location+1); // set printer to the variables content which is just i+1}
                     }
                     else{r4 = stack[1];}
                  }
                  else if(stack[0]== "r5" ||stack[0]== "R5")
                  {
                     if(stack[1] == "r2"){r5 = r2;}
                     if(stack[1] == "r3"){r5 = r3;}
                     if(stack[1] == "r4"){r5 = r4;}
                     if(stack[1] == "r1"){r5 = r1;}
                     if(stack[1] == "r6"){r5 = r6;}
                     if(stack[1] == "r7"){r5 = r7;}
                     if(stack[1] == "r8"){r5 = r8;}
                     if(stack[1] == "r9"){r5 = r9;}
                     if(stack[1] == "r10"){r5 = r10;}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        r5 = variables.at(location+1); // set printer to the variables content which is just i+1}
                     }
                     else{r5 = stack[1];}
                  }
                  else if(stack[0]== "r6" ||stack[0]== "R6")
                  {
                     if(stack[1] == "r2"){r6 = r2;}
                     if(stack[1] == "r3"){r6 = r3;}
                     if(stack[1] == "r4"){r6 = r4;}
                     if(stack[1] == "r5"){r6 = r5;}
                     if(stack[1] == "r1"){r6 = r1;}
                     if(stack[1] == "r7"){r6 = r7;}
                     if(stack[1] == "r8"){r6 = r8;}
                     if(stack[1] == "r9"){r6 = r9;}
                     if(stack[1] == "r10"){r6 = r10;}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        r6 = variables.at(location+1); // set printer to the variables content which is just i+1}
                     }
                     else{r6 = stack[1];}
                  }
                  else if(stack[0]== "r7" ||stack[0]== "R7")
                  {
                     if(stack[1] == "r2"){r7 = r2;}
                     if(stack[1] == "r3"){r7 = r3;}
                     if(stack[1] == "r4"){r7 = r4;}
                     if(stack[1] == "r5"){r7 = r5;}
                     if(stack[1] == "r6"){r7 = r6;}
                     if(stack[1] == "r1"){r7 = r1;}
                     if(stack[1] == "r8"){r7 = r8;}
                     if(stack[1] == "r9"){r7 = r9;}
                     if(stack[1] == "r10"){r7 = r10;}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        r7 = variables.at(location+1); // set printer to the variables content which is just i+1}
                     }
                     else{r7 = stack[1];}
                  }
                  else if(stack[0]== "r8" ||stack[0]== "R8")
                  {
                     if(stack[1] == "r2"){r8 = r2;}
                     if(stack[1] == "r3"){r8 = r3;}
                     if(stack[1] == "r4"){r8 = r4;}
                     if(stack[1] == "r5"){r8 = r5;}
                     if(stack[1] == "r6"){r8 = r6;}
                     if(stack[1] == "r7"){r8 = r7;}
                     if(stack[1] == "r1"){r8 = r1;}
                     if(stack[1] == "r9"){r8 = r9;}
                     if(stack[1] == "r10"){r8 = r10;}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        r8 = variables.at(location+1); // set printer to the variables content which is just i+1}
                     }
                     else{r8 = stack[1];}
                  }
                  else if(stack[0]== "r9" ||stack[0]== "R9")
                  {
                     if(stack[1] == "r2"){r9 = r2;}
                     if(stack[1] == "r3"){r9 = r3;}
                     if(stack[1] == "r4"){r9 = r4;}
                     if(stack[1] == "r5"){r9 = r5;}
                     if(stack[1] == "r6"){r9 = r6;}
                     if(stack[1] == "r7"){r9 = r7;}
                     if(stack[1] == "r8"){r9 = r8;}
                     if(stack[1] == "r1"){r9 = r1;}
                     if(stack[1] == "r10"){r9 = r10;}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        r9 = variables.at(location+1); // set printer to the variables content which is just i+1}
                     }
                     else{r9 = stack[1];}
                  }
                  else if(stack[0]== "r10" ||stack[0]== "R10")
                  {
                     if(stack[1] == "r2"){r10 = r2;}
                     if(stack[1] == "r3"){r10 = r3;}
                     if(stack[1] == "r4"){r10 = r4;}
                     if(stack[1] == "r5"){r10 = r5;}
                     if(stack[1] == "r6"){r10 = r6;}
                     if(stack[1] == "r7"){r10 = r7;}
                     if(stack[1] == "r8"){r10 = r8;}
                     if(stack[1] == "r9"){r10 = r9;}
                     if(stack[1] == "r1"){r10 = r1;}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        r10 = variables.at(location+1); // set printer to the variables content which is just i+1}
                     }
                     else{r10 = stack[1];};
                  }
               	
               
               	//cout << "this is the call: " << call << endl;
               	//cout << "^^^^^^^^^^^^^stack content: " <<stack[0] << " - " << stack[1] << " - " << stack[2]<<endl;
                  pop();pop();// pop the paremeters off the stack for next use
               				
               }
               else if(command == "set" || command == "Set")// this is for variables only
               {
               
               	
                  string cut = instruction.replace(0,4,"");//cut the command out so we can use a space
                  stringstream check1(cut);  // create a tokenizer for the string instruction to pull command and parameters
                  string param; 
                	
               	
                  while(getline(check1, param, ',')) //push initial command and its parameters onto the stack
                  { 
                     push(param);  // were only putting in the parameters to the stackto be set so we can use spaces
                  }
               	
                  string varName = stack[0];
               	
                  vector<string>::iterator it;				
                  it = find(variables.begin(), variables.end(), varName); // locate the variable name that they chose and set the variable which is just the next spot in the vector
               	      	
                  int location = distance(variables.begin(), it); //this is where the var name is stored
               	
                  variables.at(location+1) = stack[1]; // set variables information to the next spot
               		
                  pop();pop();//remove stack so new commands can be used
               }
               else if(command == "inp" || command == "Inp")// input command uses a variable initialized to be set to the users choice
               {
               	
               					
                  string cut = instruction.replace(0,4,"");//cut the command out so we can use a space
                     		  // create a tokenizer for the string instruction to pull command and parameters
               
                  push(cut);  // were only putting in the parameters to the stack
               
                  string varName = stack[0];// the variable name is what we put on the stack
               	
                  vector<string>::iterator it;				
                  it = find(variables.begin(), variables.end(), varName); // locate the variable name that they chose and set the variable which is just the next spot in the vector
               
                  int location = distance(variables.begin(), it); //this is where the var name is stored
               	
                  cin >> input; // grab input from user
                  variables.at(location+1) = input; // set users information to the next spot to represent var data
                  pop();//remove parameter from stack as were done with it
               	
               }	
               else if(command == "jmp" || command == "Jmp")
               {
                 	
                  string cut = instruction.replace(0,4,"");//cut the command out so we can use the index to find where to jump
                  int codeLine = stoi(cut);
                  string trackedInstruction = instructionTracker.at(codeLine);
               
                  while(jumped == false)// while the jump hasnt happened or youve reached the end of the file continue re running instructions
                  {
                     if(instructionTracker.at(codeLine) == "jmp " + cut)
                     {
                        jumped = true;
                     }
                     reparse(instructionTracker.at(codeLine));
                     codeLine++;
                  }
                  jumped = false;// this is to reset jumped
               
               }
               else if(command == "add"||command == "Add")
               {
                  string cut = instruction.replace(0,4,"");//cut the command out so we can use a space
                  stringstream check1(cut);  // create a tokenizer for the string instruction to pull command and parameters
                  string param; 
                	
               	
                  while(getline(check1, param, ',')) //push initial command and its parameters onto the stack
                  { 
                     push(param);  // were only putting in the parameters to the stackto be set so we can use spaces
                  }
               	
                  string varName1 = stack[0];
                  string varName2 = stack[1];
                  int total;
                  if(varName1 == "r1" || varName1 == "R1")
                  {
                     if(stack[1] == "r2"){total = stoi(r1) +stoi(r2); r1 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r1) +stoi(r3); r1 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r1) +stoi(r4); r1 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r1) +stoi(r5); r1 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r1) +stoi(r6); r1 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r1) +stoi(r7); r1 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r1) +stoi(r8); r1 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r1) +stoi(r9); r1 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r1) +stoi(r10); r1 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r1) + stoi(variables.at(location+1));
                        r1 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r1) + stoi(varName2); r1 = to_string(total);}// if its just a number on the other end add it and return it in register
                  	
                  }
                  else if(varName1 == "r2" || varName1 == "R2")
                  {
                     if(stack[1] == "r1"){total = stoi(r2) +stoi(r1); r2 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r2) +stoi(r3); r2 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r2) +stoi(r4); r2 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r2) +stoi(r5); r2 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r2) +stoi(r6); r2 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r2) +stoi(r7); r2 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r2) +stoi(r8); r2 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r2) +stoi(r9); r2 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r2) +stoi(r10); r2 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r2) + stoi(variables.at(location+1));
                        r2 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r2) + stoi(varName2); r2 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r3" || varName1 == "R3")
                  {
                     if(stack[1] == "r1"){total = stoi(r3) +stoi(r1); r3 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r2"){total = stoi(r3) +stoi(r2); r3 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r3) +stoi(r4); r3 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r3) +stoi(r5); r3 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r3) +stoi(r6); r3 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r3) +stoi(r7); r3 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r3) +stoi(r8); r3 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r3) +stoi(r9); r3 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r3) +stoi(r10); r3 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r3) + stoi(variables.at(location+1));
                        r3 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r3) + stoi(varName2); r3 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r4" || varName1 == "R4")
                  {
                     if(stack[1] == "r1"){total = stoi(r4) +stoi(r1); r4 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r4) +stoi(r3); r4 = to_string(total);}
                     if(stack[1] == "r2"){total = stoi(r4) +stoi(r2); r4 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r4) +stoi(r5); r4 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r4) +stoi(r6); r4 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r4) +stoi(r7); r4 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r4) +stoi(r8); r4 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r4) +stoi(r9); r4 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r4) +stoi(r10); r4 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r4) + stoi(variables.at(location+1));
                        r4 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r4) + stoi(varName2); r4 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r5" || varName1 == "R5")
                  {
                     if(stack[1] == "r1"){total = stoi(r5) +stoi(r1); r5 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r5) +stoi(r3); r5 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r5) +stoi(r4); r5 = to_string(total);}
                     if(stack[1] == "r2"){total = stoi(r2) +stoi(r5); r5 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r5) +stoi(r6); r5 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r5) +stoi(r7); r5 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r5) +stoi(r8); r5 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r5) +stoi(r9); r5 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r5) +stoi(r10); r5 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r5) + stoi(variables.at(location+1));
                        r5 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r5) + stoi(varName2); r5 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r6" || varName1 == "R6")
                  {
                     if(stack[1] == "r1"){total = stoi(r6) +stoi(r1); r6 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r6) +stoi(r3); r6 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r6) +stoi(r4); r6 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r6) +stoi(r5); r6 = to_string(total);}
                     if(stack[1] == "r2"){total = stoi(r6) +stoi(r2); r6 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r6) +stoi(r7); r6 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r6) +stoi(r8); r6 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r6) +stoi(r9); r6 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r6) +stoi(r10); r6 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r6) + stoi(variables.at(location+1));
                        r6 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r6) + stoi(varName2); r6 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r7" || varName1 == "R7")
                  {
                     if(stack[1] == "r1"){total = stoi(r7) +stoi(r1); r7 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r7) +stoi(r3); r7 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r7) +stoi(r4); r7 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r7) +stoi(r5); r7 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r7) +stoi(r6); r7 = to_string(total);}
                     if(stack[1] == "r2"){total = stoi(r7) +stoi(r2); r7 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r7) +stoi(r8); r7 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r7) +stoi(r9); r7 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r7) +stoi(r10); r7 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r7) + stoi(variables.at(location+1));
                        r7 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r7) + stoi(varName2); r7 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r8" || varName1 == "R8")
                  {
                     if(stack[1] == "r1"){total = stoi(r8) +stoi(r1); r8 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r8) +stoi(r3); r8 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r8) +stoi(r4); r8 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r8) +stoi(r5); r8 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r8) +stoi(r6); r8 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r8) +stoi(r7); r8 = to_string(total);}
                     if(stack[1] == "r2"){total = stoi(r8) +stoi(r2); r8 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r8) +stoi(r9); r8 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r8) +stoi(r10); r8 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r8) + stoi(variables.at(location+1));
                        r8 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r8) + stoi(varName2); r8 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r9" || varName1 == "R9")
                  {
                     if(stack[1] == "r1"){total = stoi(r9) +stoi(r1); r9 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r9) +stoi(r3); r9 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r9) +stoi(r4); r9 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r9) +stoi(r5); r9 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r9) +stoi(r6); r9 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r9) +stoi(r7); r9 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r9) +stoi(r8); r9 = to_string(total);}
                     if(stack[1] == "r2"){total = stoi(r9) +stoi(r2); r9 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r9) +stoi(r10); r9 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r9) + stoi(variables.at(location+1));
                        r9 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r9) + stoi(varName2); r9 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r2" || varName1 == "R2")
                  {
                     if(stack[1] == "r1"){total = stoi(r10) +stoi(r1); r10 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r10) +stoi(r3); r10 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r10) +stoi(r4); r10 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r10) +stoi(r5); r10 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r10) +stoi(r6); r10 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r10) +stoi(r7); r10 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r10) +stoi(r8); r10 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r10) +stoi(r9); r10 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r10) +stoi(r2); r10 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r10) + stoi(variables.at(location+1));
                        r10 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r10) + stoi(varName2); r10 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else
                  {
                     vector<string>::iterator it;				
                     it = find(variables.begin(), variables.end(), varName1); // locate the variable name that they chose and set the variable which is just the next spot in the vector
                     int location1 = distance(variables.begin(), it); //this is where the var name is stored
                  
                     it = find(variables.begin(), variables.end(), varName2); // locate the variable name that they chose and set the variable which is just the next spot in the vector
                     int location2 = distance(variables.begin(), it);
                  
                  
                     string num1 = variables.at(location1+1);
                     string num2 = variables.at(location2+1);
                  
                     int sum = stoi(num1) + stoi(num2);//do the math on ints not strings
                  	
                     variables.at(location1+1) = to_string(sum); // return the sum in the left parameters location
                  	
                  	//cout << "sum: "<< sum << endl;
                  }
               	
               
               	//cout << "stack contents " <<stack[0] << " " << stack[1] << endl;
               	//cout << "vector contents " <<variables.at(0)<< " - "<< variables.at(1) << " - "<<variables.at(2) << " - " << variables.at(3)<< endl;
               
                  pop();pop();//clear stack so new commands can be used
               
               	
               }
               else if(command == "sub"||command == "Sub")
               {
                  string cut = instruction.replace(0,4,"");//cut the command out so we can use a space
                  stringstream check1(cut);  // create a tokenizer for the string instruction to pull command and parameters
                  string param; 
                	
               	
                  while(getline(check1, param, ',')) //push initial command and its parameters onto the stack
                  { 
                     push(param);  // were only putting in the parameters to the stackto be set so we can use spaces
                  }
               	
                  string varName1 = stack[0];
                  string varName2 = stack[1];
                  int total;
                  if(varName1 == "r1" || varName1 == "R1")
                  {
                     if(stack[1] == "r2"){total = stoi(r1) -stoi(r2); r1 = to_string(total);}// this checks if r2 was called then sub the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r1) -stoi(r3); r1 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r1) -stoi(r4); r1 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r1) -stoi(r5); r1 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r1) -stoi(r6); r1 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r1) -stoi(r7); r1 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r1) -stoi(r8); r1 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r1) -stoi(r9); r1 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r1) -stoi(r10); r1 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r1) - stoi(variables.at(location+1));
                        r1 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r1) - stoi(varName2); r1 = to_string(total);}// if its just a number on the other end add it and return it in register
                  	
                  }
                  else if(varName1 == "r2" || varName1 == "R2")
                  {
                     if(stack[1] == "r1"){total = stoi(r2) -stoi(r1); r2 = to_string(total);}// this checks if r2 was called then sub the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r2) -stoi(r3); r2 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r2) -stoi(r4); r2 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r2) -stoi(r5); r2 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r2) -stoi(r6); r2 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r2) -stoi(r7); r2 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r2) -stoi(r8); r2 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r2) -stoi(r9); r2 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r2) -stoi(r10); r2 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r2) - stoi(variables.at(location+1));
                        r2 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r2) - stoi(varName2); r2 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r3" || varName1 == "R3")
                  {
                     if(stack[1] == "r1"){total = stoi(r3) -stoi(r1); r3 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r2"){total = stoi(r3) -stoi(r2); r3 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r3) -stoi(r4); r3 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r3) -stoi(r5); r3 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r3) -stoi(r6); r3 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r3) -stoi(r7); r3 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r3) -stoi(r8); r3 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r3) -stoi(r9); r3 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r3) -stoi(r10); r3 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r3) - stoi(variables.at(location+1));
                        r3 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r3) - stoi(varName2); r3 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r4" || varName1 == "R4")
                  {
                     if(stack[1] == "r1"){total = stoi(r4) -stoi(r1); r4 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r4) -stoi(r3); r4 = to_string(total);}
                     if(stack[1] == "r2"){total = stoi(r4) -stoi(r2); r4 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r4) -stoi(r5); r4 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r4) -stoi(r6); r4 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r4) -stoi(r7); r4 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r4) -stoi(r8); r4 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r4) -stoi(r9); r4 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r4) -stoi(r10); r4 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r4) - stoi(variables.at(location+1));
                        r4 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r4) - stoi(varName2); r4 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r5" || varName1 == "R5")
                  {
                     if(stack[1] == "r1"){total = stoi(r5) -stoi(r1); r5 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r5) -stoi(r3); r5 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r5) -stoi(r4); r5 = to_string(total);}
                     if(stack[1] == "r2"){total = stoi(r2) -stoi(r5); r5 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r5) -stoi(r6); r5 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r5) -stoi(r7); r5 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r5) -stoi(r8); r5 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r5) -stoi(r9); r5 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r5) -stoi(r10); r5 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r5) - stoi(variables.at(location+1));
                        r5 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r5) - stoi(varName2); r5 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r6" || varName1 == "R6")
                  {
                     if(stack[1] == "r1"){total = stoi(r6) -stoi(r1); r6 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r6) -stoi(r3); r6 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r6) -stoi(r4); r6 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r6) -stoi(r5); r6 = to_string(total);}
                     if(stack[1] == "r2"){total = stoi(r6) -stoi(r2); r6 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r6) -stoi(r7); r6 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r6) -stoi(r8); r6 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r6) -stoi(r9); r6 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r6) -stoi(r10); r6 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r6) - stoi(variables.at(location+1));
                        r6 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r6) - stoi(varName2); r6 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r7" || varName1 == "R7")
                  {
                     if(stack[1] == "r1"){total = stoi(r7) -stoi(r1); r7 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r7) -stoi(r3); r7 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r7) -stoi(r4); r7 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r7) -stoi(r5); r7 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r7) -stoi(r6); r7 = to_string(total);}
                     if(stack[1] == "r2"){total = stoi(r7) -stoi(r2); r7 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r7) -stoi(r8); r7 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r7) -stoi(r9); r7 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r7) -stoi(r10); r7 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r7) - stoi(variables.at(location+1));
                        r7 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r7) - stoi(varName2); r7 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r8" || varName1 == "R8")
                  {
                     if(stack[1] == "r1"){total = stoi(r8) -stoi(r1); r8 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r8) -stoi(r3); r8 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r8) -stoi(r4); r8 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r8) -stoi(r5); r8 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r8) -stoi(r6); r8 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r8) -stoi(r7); r8 = to_string(total);}
                     if(stack[1] == "r2"){total = stoi(r8) -stoi(r2); r8 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r8) -stoi(r9); r8 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r8) -stoi(r10); r8 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r8) - stoi(variables.at(location+1));
                        r8 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r8) - stoi(varName2); r8 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r9" || varName1 == "R9")
                  {
                     if(stack[1] == "r1"){total = stoi(r9) -stoi(r1); r9 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r9) -stoi(r3); r9 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r9) -stoi(r4); r9 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r9) -stoi(r5); r9 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r9) -stoi(r6); r9 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r9) -stoi(r7); r9 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r9) -stoi(r8); r9 = to_string(total);}
                     if(stack[1] == "r2"){total = stoi(r9) -stoi(r2); r9 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r9) -stoi(r10); r9 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r9) - stoi(variables.at(location+1));
                        r9 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r9) - stoi(varName2); r9 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else if(varName1 == "r2" || varName1 == "R2")
                  {
                     if(stack[1] == "r1"){total = stoi(r10) -stoi(r1); r10 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
                     if(stack[1] == "r3"){total = stoi(r10) -stoi(r3); r10 = to_string(total);}
                     if(stack[1] == "r4"){total = stoi(r10) -stoi(r4); r10 = to_string(total);}
                     if(stack[1] == "r5"){total = stoi(r10) -stoi(r5); r10 = to_string(total);}
                     if(stack[1] == "r6"){total = stoi(r10) -stoi(r6); r10 = to_string(total);}
                     if(stack[1] == "r7"){total = stoi(r10) -stoi(r7); r10 = to_string(total);}
                     if(stack[1] == "r8"){total = stoi(r10) -stoi(r8); r10 = to_string(total);}
                     if(stack[1] == "r9"){total = stoi(r10) -stoi(r9); r10 = to_string(total);}
                     if(stack[1] == "r10"){total = stoi(r10) -stoi(r2); r10 = to_string(total);}
                     if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
                     {  vector<string>::iterator it;				
                        it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
                        int location = distance(variables.begin(), it); //this is where the var name is stored
                        total = stoi(r10) - stoi(variables.at(location+1));
                        r10 = to_string(total); // add register to the variables content which is just i+1}
                     }
                     else{total = stoi(r10) - stoi(varName2); r10 = to_string(total);}// if its just a number on the other end add it and return it in register
                  
                  }
                  else
                  {
                     vector<string>::iterator it;				
                     it = find(variables.begin(), variables.end(), varName1); // locate the variable name that they chose and set the variable which is just the next spot in the vector
                     int location1 = distance(variables.begin(), it); //this is where the var name is stored
                  
                     it = find(variables.begin(), variables.end(), varName2); // locate the variable name that they chose and set the variable which is just the next spot in the vector
                     int location2 = distance(variables.begin(), it);
                  
                  
                     string num1 = variables.at(location1+1);
                     string num2 = variables.at(location2+1);
                  
                     int sum = stoi(num1) - stoi(num2);//do the math on ints not strings
                  	
                     variables.at(location1+1) = to_string(sum); // return the sum in the left parameters location
                  	
                  	//cout << "sum: "<< sum << endl;
                  }
               	
               
               	//cout << "stack contents " <<stack[0] << " " << stack[1] << endl;
               	//cout << "vector contents " <<variables.at(0)<< " - "<< variables.at(1) << " - "<<variables.at(2) << " - " << variables.at(3)<< endl;
               
                  pop();pop();//clear stack so new commands can be used
               
               	
               }
            }
         }
      }
   }			
   else
   {
      cout << "could not find file (check if you added.txt to the end)\nor you did not lead the file with keyword 'Start:'";
   }
   return 0;
}



////////////////////////////////this is to reparse chunk of codes for methods////////////////////////


void reparse(string instruction) /// all this does is runs the method chunks through above code when called
{

	
   string command = instruction.substr(0,instruction.find(" "));
   if(instruction == "call")
   {
   					//ready method calls
      if(call == "0")// different sytem calls
      {
         endRead = true;
      }
      else if(call == "1" && printer !="")
      {
         cout << printer << endl;
         call = "";
      }
   					
   }
   else if(command == "mov")// if the mov command is found process the move
   {
      string cut = instruction.replace(0,4,"");//cut command out
      stringstream check1(cut);  // create a tokenizer for the parameters to pull command and parameters
      string param; 
               
      while(getline(check1, param, ',')) //push its parameters onto the stack
      { 
         push(param); 
      }
               	
      if(stack[0]== "call" ||stack[0]== "Call") // wanted to use switch for this but easier just to use ifstatement with strings
      {	
         call = stack[1];
                    
      }
      else if(stack[0]== "printer" ||stack[0]== "Printer")
      {
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())
         {
            vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            printer = variables.at(location+1); // set printer to the variables content which is just i+1
                        
         }
         else
         {
            printer = stack[1]; // if a variable cant be found the printer will just be hard coded
         }
                  		
      }
      else if(stack[0]== "r1" ||stack[0]== "R1") // general registers move between each other or a random string all possibilities accounted for
      {
         if(stack[1] == "r2"){r1 = r2;}
         if(stack[1] == "r3"){r1 = r3;}
         if(stack[1] == "r4"){r1 = r4;}
         if(stack[1] == "r5"){r1 = r5;}
         if(stack[1] == "r6"){r1 = r6;}
         if(stack[1] == "r7"){r1 = r7;}
         if(stack[1] == "r8"){r1 = r8;}
         if(stack[1] == "r9"){r1 = r9;}
         if(stack[1] == "r10"){r1 = r10;}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            r1 = variables.at(location+1); // set printer to the variables content which is just i+1}
         }
         else{r1 = stack[1];}
      					
      }	
      else if(stack[0]== "r2" ||stack[0]== "R2") // same as above but for r2
      {
         if(stack[1] == "r1"){r2 = r1;}
         if(stack[1] == "r3"){r2 = r3;}
         if(stack[1] == "r4"){r2 = r4;}
         if(stack[1] == "r5"){r2 = r5;}
         if(stack[1] == "r6"){r2 = r6;}
         if(stack[1] == "r7"){r2 = r7;}
         if(stack[1] == "r8"){r2 = r8;}
         if(stack[1] == "r9"){r2 = r9;}
         if(stack[1] == "r10"){r2 = r10;}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            r2 = variables.at(location+1); // set printer to the variables content which is just i+1}
         }
         else{r2 = stack[1];}
      }
      else if(stack[0]== "r3" ||stack[0]== "R3")
      {
         if(stack[1] == "r1"){r3 = r1;}
         if(stack[1] == "r2"){r3 = r2;}
         if(stack[1] == "r4"){r3 = r4;}
         if(stack[1] == "r5"){r3 = r5;}
         if(stack[1] == "r6"){r3 = r6;}
         if(stack[1] == "r7"){r3 = r7;}
         if(stack[1] == "r8"){r3 = r8;}
         if(stack[1] == "r9"){r3 = r9;}
         if(stack[1] == "r10"){r3 = r10;}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            r3 = variables.at(location+1); // set printer to the variables content which is just i+1}
         }
         else{r3 = stack[1];}
      }
      else if(stack[0]== "r4" ||stack[0]== "R4")
      {
         if(stack[1] == "r2"){r4 = r2;}
         if(stack[1] == "r3"){r4 = r3;}
         if(stack[1] == "r1"){r4 = r1;}
         if(stack[1] == "r5"){r4 = r5;}
         if(stack[1] == "r6"){r4 = r6;}
         if(stack[1] == "r7"){r4 = r7;}
         if(stack[1] == "r8"){r4 = r8;}
         if(stack[1] == "r9"){r4 = r9;}
         if(stack[1] == "r10"){r4 = r10;}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            r4 = variables.at(location+1); // set printer to the variables content which is just i+1}
         }
         else{r4 = stack[1];}
      }
      else if(stack[0]== "r5" ||stack[0]== "R5")
      {
         if(stack[1] == "r2"){r5 = r2;}
         if(stack[1] == "r3"){r5 = r3;}
         if(stack[1] == "r4"){r5 = r4;}
         if(stack[1] == "r1"){r5 = r1;}
         if(stack[1] == "r6"){r5 = r6;}
         if(stack[1] == "r7"){r5 = r7;}
         if(stack[1] == "r8"){r5 = r8;}
         if(stack[1] == "r9"){r5 = r9;}
         if(stack[1] == "r10"){r5 = r10;}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            r5 = variables.at(location+1); // set printer to the variables content which is just i+1}
         }
         else{r5 = stack[1];}
      }
      else if(stack[0]== "r6" ||stack[0]== "R6")
      {
         if(stack[1] == "r2"){r6 = r2;}
         if(stack[1] == "r3"){r6 = r3;}
         if(stack[1] == "r4"){r6 = r4;}
         if(stack[1] == "r5"){r6 = r5;}
         if(stack[1] == "r1"){r6 = r1;}
         if(stack[1] == "r7"){r6 = r7;}
         if(stack[1] == "r8"){r6 = r8;}
         if(stack[1] == "r9"){r6 = r9;}
         if(stack[1] == "r10"){r6 = r10;}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            r6 = variables.at(location+1); // set printer to the variables content which is just i+1}
         }
         else{r6 = stack[1];}
      }
      else if(stack[0]== "r7" ||stack[0]== "R7")
      {
         if(stack[1] == "r2"){r7 = r2;}
         if(stack[1] == "r3"){r7 = r3;}
         if(stack[1] == "r4"){r7 = r4;}
         if(stack[1] == "r5"){r7 = r5;}
         if(stack[1] == "r6"){r7 = r6;}
         if(stack[1] == "r1"){r7 = r1;}
         if(stack[1] == "r8"){r7 = r8;}
         if(stack[1] == "r9"){r7 = r9;}
         if(stack[1] == "r10"){r7 = r10;}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            r7 = variables.at(location+1); // set printer to the variables content which is just i+1}
         }
         else{r7 = stack[1];}
      }
      else if(stack[0]== "r8" ||stack[0]== "R8")
      {
         if(stack[1] == "r2"){r8 = r2;}
         if(stack[1] == "r3"){r8 = r3;}
         if(stack[1] == "r4"){r8 = r4;}
         if(stack[1] == "r5"){r8 = r5;}
         if(stack[1] == "r6"){r8 = r6;}
         if(stack[1] == "r7"){r8 = r7;}
         if(stack[1] == "r1"){r8 = r1;}
         if(stack[1] == "r9"){r8 = r9;}
         if(stack[1] == "r10"){r8 = r10;}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            r8 = variables.at(location+1); // set printer to the variables content which is just i+1}
         }
         else{r8 = stack[1];}
      }
      else if(stack[0]== "r9" ||stack[0]== "R9")
      {
         if(stack[1] == "r2"){r9 = r2;}
         if(stack[1] == "r3"){r9 = r3;}
         if(stack[1] == "r4"){r9 = r4;}
         if(stack[1] == "r5"){r9 = r5;}
         if(stack[1] == "r6"){r9 = r6;}
         if(stack[1] == "r7"){r9 = r7;}
         if(stack[1] == "r8"){r9 = r8;}
         if(stack[1] == "r1"){r9 = r1;}
         if(stack[1] == "r10"){r9 = r10;}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            r9 = variables.at(location+1); // set printer to the variables content which is just i+1}
         }
         else{r9 = stack[1];}
      }
      else if(stack[0]== "r10" ||stack[0]== "R10")
      {
         if(stack[1] == "r2"){r10 = r2;}
         if(stack[1] == "r3"){r10 = r3;}
         if(stack[1] == "r4"){r10 = r4;}
         if(stack[1] == "r5"){r10 = r5;}
         if(stack[1] == "r6"){r10 = r6;}
         if(stack[1] == "r7"){r10 = r7;}
         if(stack[1] == "r8"){r10 = r8;}
         if(stack[1] == "r9"){r10 = r9;}
         if(stack[1] == "r1"){r10 = r1;}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this moves variables into registers
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            r10 = variables.at(location+1); // set printer to the variables content which is just i+1}
         }
         else{r10 = stack[1];};
      }
               	
               
               	//cout << "^^^^^^^^^^^^^stack content: " <<stack[0] << " - " << stack[1] << " - " << stack[2]<<endl;
      pop();pop();// pop the paremeters off the stack for next use
               				
   }
   else if(command == "set" || command == "Set")// this is for variables only
   {
               
               	
      string cut = instruction.replace(0,4,"");//cut the command out so we can use a space
      stringstream check1(cut);  // create a tokenizer for the string instruction to pull command and parameters
      string param; 
                	
               	
      while(getline(check1, param, ',')) //push initial command and its parameters onto the stack
      { 
         push(param);  // were only putting in the parameters to the stackto be set so we can use spaces
      }
               	
      string varName = stack[0];
               	
      //cout << "^^^^^^^^^^^^^stack content: " <<stack[0] << " - " << stack[1] << " - " << stack[2]<<endl;
      vector<string>::iterator it;				
      it = find(variables.begin(), variables.end(), varName); // locate the variable name that they chose and set the variable which is just the next spot in the vector
               	
               	//cout <<"look here dummy:" << distance(variables.begin(), it) << endl;
               	
      int location = distance(variables.begin(), it); //this is where the var name is stored
               	
      variables.at(location+1) = stack[1]; // set variables information to the next spot
               		
      pop();pop();//remove stack so new commands can be used
   }
   else if(command == "inp" || command == "Inp")// input command uses a variable initialized to be set to the users choice
   {
               	
               					
      string cut = instruction.replace(0,4,"");//cut the command out so we can use a space
                     		  // create a tokenizer for the string instruction to pull command and parameters
               
      push(cut);  // were only putting in the parameters to the stack
               
      string varName = stack[0];// the variable name is what we put on the stack
               	
      vector<string>::iterator it;				
      it = find(variables.begin(), variables.end(), varName); // locate the variable name that they chose and set the variable which is just the next spot in the vector
               
      int location = distance(variables.begin(), it); //this is where the var name is stored
               	
      cin >> input; // grab input from user
      variables.at(location+1) = input; // set users information to the next spot to represent var data
      pop();//remove parameter from stack as were done with it
               	
   }	
   else if(command == "jmp" || command == "Jmp")
   {
      string cut = instruction.replace(0,4,"");//cut the command out so we can use the index to find where to jump
      int codeLine = stoi(cut);
      string trackedInstruction = instructionTracker.at(codeLine);
   	
   
   	
      while(jumped == false)// while the jump hasnt happened or youve reached the end of the file continue re running instructions
      {
      
         if(instructionTracker.at(codeLine) == "jmp " + cut)
         {
            jumped = true;
         }
         reparse(instructionTracker.at(codeLine));
         codeLine++;
      }  	
   	
   }
   else if(command == "add"||command == "Add")
   {
      string cut = instruction.replace(0,4,"");//cut the command out so we can use a space
      stringstream check1(cut);  // create a tokenizer for the string instruction to pull command and parameters
      string param; 
                	
               	
      while(getline(check1, param, ',')) //push initial command and its parameters onto the stack
      { 
         push(param);  // were only putting in the parameters to the stackto be set so we can use spaces
      }
               	
      string varName1 = stack[0];
      string varName2 = stack[1];
      int total;
      if(varName1 == "r1" || varName1 == "R1")
      {
         if(stack[1] == "r2"){total = stoi(r1) +stoi(r2); r1 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r1) +stoi(r3); r1 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r1) +stoi(r4); r1 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r1) +stoi(r5); r1 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r1) +stoi(r6); r1 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r1) +stoi(r7); r1 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r1) +stoi(r8); r1 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r1) +stoi(r9); r1 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r1) +stoi(r10); r1 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r1) + stoi(variables.at(location+1));
            r1 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r1) + stoi(varName2); r1 = to_string(total);}// if its just a number on the other end add it and return it in register
                  	
      }
      else if(varName1 == "r2" || varName1 == "R2")
      {
         if(stack[1] == "r1"){total = stoi(r2) +stoi(r1); r2 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r2) +stoi(r3); r2 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r2) +stoi(r4); r2 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r2) +stoi(r5); r2 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r2) +stoi(r6); r2 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r2) +stoi(r7); r2 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r2) +stoi(r8); r2 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r2) +stoi(r9); r2 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r2) +stoi(r10); r2 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r2) + stoi(variables.at(location+1));
            r2 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r2) + stoi(varName2); r2 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r3" || varName1 == "R3")
      {
         if(stack[1] == "r1"){total = stoi(r3) +stoi(r1); r3 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r2"){total = stoi(r3) +stoi(r2); r3 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r3) +stoi(r4); r3 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r3) +stoi(r5); r3 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r3) +stoi(r6); r3 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r3) +stoi(r7); r3 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r3) +stoi(r8); r3 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r3) +stoi(r9); r3 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r3) +stoi(r10); r3 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r3) + stoi(variables.at(location+1));
            r3 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r3) + stoi(varName2); r3 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r4" || varName1 == "R4")
      {
         if(stack[1] == "r1"){total = stoi(r4) +stoi(r1); r4 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r4) +stoi(r3); r4 = to_string(total);}
         if(stack[1] == "r2"){total = stoi(r4) +stoi(r2); r4 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r4) +stoi(r5); r4 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r4) +stoi(r6); r4 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r4) +stoi(r7); r4 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r4) +stoi(r8); r4 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r4) +stoi(r9); r4 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r4) +stoi(r10); r4 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r4) + stoi(variables.at(location+1));
            r4 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r4) + stoi(varName2); r4 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r5" || varName1 == "R5")
      {
         if(stack[1] == "r1"){total = stoi(r5) +stoi(r1); r5 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r5) +stoi(r3); r5 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r5) +stoi(r4); r5 = to_string(total);}
         if(stack[1] == "r2"){total = stoi(r2) +stoi(r5); r5 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r5) +stoi(r6); r5 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r5) +stoi(r7); r5 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r5) +stoi(r8); r5 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r5) +stoi(r9); r5 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r5) +stoi(r10); r5 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r5) + stoi(variables.at(location+1));
            r5 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r5) + stoi(varName2); r5 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r6" || varName1 == "R6")
      {
         if(stack[1] == "r1"){total = stoi(r6) +stoi(r1); r6 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r6) +stoi(r3); r6 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r6) +stoi(r4); r6 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r6) +stoi(r5); r6 = to_string(total);}
         if(stack[1] == "r2"){total = stoi(r6) +stoi(r2); r6 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r6) +stoi(r7); r6 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r6) +stoi(r8); r6 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r6) +stoi(r9); r6 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r6) +stoi(r10); r6 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r6) + stoi(variables.at(location+1));
            r6 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r6) + stoi(varName2); r6 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r7" || varName1 == "R7")
      {
         if(stack[1] == "r1"){total = stoi(r7) +stoi(r1); r7 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r7) +stoi(r3); r7 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r7) +stoi(r4); r7 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r7) +stoi(r5); r7 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r7) +stoi(r6); r7 = to_string(total);}
         if(stack[1] == "r2"){total = stoi(r7) +stoi(r2); r7 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r7) +stoi(r8); r7 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r7) +stoi(r9); r7 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r7) +stoi(r10); r7 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r7) + stoi(variables.at(location+1));
            r7 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r7) + stoi(varName2); r7 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r8" || varName1 == "R8")
      {
         if(stack[1] == "r1"){total = stoi(r8) +stoi(r1); r8 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r8) +stoi(r3); r8 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r8) +stoi(r4); r8 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r8) +stoi(r5); r8 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r8) +stoi(r6); r8 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r8) +stoi(r7); r8 = to_string(total);}
         if(stack[1] == "r2"){total = stoi(r8) +stoi(r2); r8 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r8) +stoi(r9); r8 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r8) +stoi(r10); r8 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r8) + stoi(variables.at(location+1));
            r8 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r8) + stoi(varName2); r8 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r9" || varName1 == "R9")
      {
         if(stack[1] == "r1"){total = stoi(r9) +stoi(r1); r9 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r9) +stoi(r3); r9 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r9) +stoi(r4); r9 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r9) +stoi(r5); r9 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r9) +stoi(r6); r9 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r9) +stoi(r7); r9 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r9) +stoi(r8); r9 = to_string(total);}
         if(stack[1] == "r2"){total = stoi(r9) +stoi(r2); r9 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r9) +stoi(r10); r9 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r9) + stoi(variables.at(location+1));
            r9 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r9) + stoi(varName2); r9 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r2" || varName1 == "R2")
      {
         if(stack[1] == "r1"){total = stoi(r10) +stoi(r1); r10 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r10) +stoi(r3); r10 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r10) +stoi(r4); r10 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r10) +stoi(r5); r10 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r10) +stoi(r6); r10 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r10) +stoi(r7); r10 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r10) +stoi(r8); r10 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r10) +stoi(r9); r10 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r10) +stoi(r2); r10 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r10) + stoi(variables.at(location+1));
            r10 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r10) + stoi(varName2); r10 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else
      {
         vector<string>::iterator it;				
         it = find(variables.begin(), variables.end(), varName1); // locate the variable name that they chose and set the variable which is just the next spot in the vector
         int location1 = distance(variables.begin(), it); //this is where the var name is stored
                  
         it = find(variables.begin(), variables.end(), varName2); // locate the variable name that they chose and set the variable which is just the next spot in the vector
         int location2 = distance(variables.begin(), it);
                  
                  
         string num1 = variables.at(location1+1);
         string num2 = variables.at(location2+1);
                  
         int sum = stoi(num1) + stoi(num2);//do the math on ints not strings
                  	
         variables.at(location1+1) = to_string(sum); // return the sum in the left parameters location
                  	
                  	//cout << "sum: "<< sum << endl;
      }
               	
               
               	//cout << "stack contents " <<stack[0] << " " << stack[1] << endl;
               	//cout << "vector contents " <<variables.at(0)<< " - "<< variables.at(1) << " - "<<variables.at(2) << " - " << variables.at(3)<< endl;
               
      pop();pop();//clear stack so new commands can be used
               
               	
   }
   else if(command == "sub"||command == "Sub")
   {
      string cut = instruction.replace(0,4,"");//cut the command out so we can use a space
      stringstream check1(cut);  // create a tokenizer for the string instruction to pull command and parameters
      string param; 
                	
               	
      while(getline(check1, param, ',')) //push initial command and its parameters onto the stack
      { 
         push(param);  // were only putting in the parameters to the stackto be set so we can use spaces
      }
               	
      string varName1 = stack[0];
      string varName2 = stack[1];
      int total;
      if(varName1 == "r1" || varName1 == "R1")
      {
         if(stack[1] == "r2"){total = stoi(r1) -stoi(r2); r1 = to_string(total);}// this checks if r2 was called then sub the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r1) -stoi(r3); r1 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r1) -stoi(r4); r1 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r1) -stoi(r5); r1 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r1) -stoi(r6); r1 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r1) -stoi(r7); r1 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r1) -stoi(r8); r1 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r1) -stoi(r9); r1 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r1) -stoi(r10); r1 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r1) - stoi(variables.at(location+1));
            r1 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r1) - stoi(varName2); r1 = to_string(total);}// if its just a number on the other end add it and return it in register
                  	
      }
      else if(varName1 == "r2" || varName1 == "R2")
      {
         if(stack[1] == "r1"){total = stoi(r2) -stoi(r1); r2 = to_string(total);}// this checks if r2 was called then sub the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r2) -stoi(r3); r2 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r2) -stoi(r4); r2 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r2) -stoi(r5); r2 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r2) -stoi(r6); r2 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r2) -stoi(r7); r2 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r2) -stoi(r8); r2 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r2) -stoi(r9); r2 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r2) -stoi(r10); r2 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r2) - stoi(variables.at(location+1));
            r2 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r2) - stoi(varName2); r2 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r3" || varName1 == "R3")
      {
         if(stack[1] == "r1"){total = stoi(r3) -stoi(r1); r3 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r2"){total = stoi(r3) -stoi(r2); r3 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r3) -stoi(r4); r3 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r3) -stoi(r5); r3 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r3) -stoi(r6); r3 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r3) -stoi(r7); r3 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r3) -stoi(r8); r3 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r3) -stoi(r9); r3 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r3) -stoi(r10); r3 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r3) - stoi(variables.at(location+1));
            r3 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r3) - stoi(varName2); r3 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r4" || varName1 == "R4")
      {
         if(stack[1] == "r1"){total = stoi(r4) -stoi(r1); r4 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r4) -stoi(r3); r4 = to_string(total);}
         if(stack[1] == "r2"){total = stoi(r4) -stoi(r2); r4 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r4) -stoi(r5); r4 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r4) -stoi(r6); r4 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r4) -stoi(r7); r4 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r4) -stoi(r8); r4 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r4) -stoi(r9); r4 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r4) -stoi(r10); r4 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r4) - stoi(variables.at(location+1));
            r4 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r4) - stoi(varName2); r4 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r5" || varName1 == "R5")
      {
         if(stack[1] == "r1"){total = stoi(r5) -stoi(r1); r5 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r5) -stoi(r3); r5 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r5) -stoi(r4); r5 = to_string(total);}
         if(stack[1] == "r2"){total = stoi(r2) -stoi(r5); r5 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r5) -stoi(r6); r5 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r5) -stoi(r7); r5 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r5) -stoi(r8); r5 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r5) -stoi(r9); r5 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r5) -stoi(r10); r5 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r5) - stoi(variables.at(location+1));
            r5 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r5) - stoi(varName2); r5 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r6" || varName1 == "R6")
      {
         if(stack[1] == "r1"){total = stoi(r6) -stoi(r1); r6 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r6) -stoi(r3); r6 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r6) -stoi(r4); r6 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r6) -stoi(r5); r6 = to_string(total);}
         if(stack[1] == "r2"){total = stoi(r6) -stoi(r2); r6 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r6) -stoi(r7); r6 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r6) -stoi(r8); r6 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r6) -stoi(r9); r6 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r6) -stoi(r10); r6 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r6) - stoi(variables.at(location+1));
            r6 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r6) - stoi(varName2); r6 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r7" || varName1 == "R7")
      {
         if(stack[1] == "r1"){total = stoi(r7) -stoi(r1); r7 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r7) -stoi(r3); r7 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r7) -stoi(r4); r7 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r7) -stoi(r5); r7 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r7) -stoi(r6); r7 = to_string(total);}
         if(stack[1] == "r2"){total = stoi(r7) -stoi(r2); r7 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r7) -stoi(r8); r7 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r7) -stoi(r9); r7 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r7) -stoi(r10); r7 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r7) - stoi(variables.at(location+1));
            r7 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r7) - stoi(varName2); r7 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r8" || varName1 == "R8")
      {
         if(stack[1] == "r1"){total = stoi(r8) -stoi(r1); r8 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r8) -stoi(r3); r8 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r8) -stoi(r4); r8 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r8) -stoi(r5); r8 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r8) -stoi(r6); r8 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r8) -stoi(r7); r8 = to_string(total);}
         if(stack[1] == "r2"){total = stoi(r8) -stoi(r2); r8 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r8) -stoi(r9); r8 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r8) -stoi(r10); r8 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r8) - stoi(variables.at(location+1));
            r8 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r8) - stoi(varName2); r8 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r9" || varName1 == "R9")
      {
         if(stack[1] == "r1"){total = stoi(r9) -stoi(r1); r9 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r9) -stoi(r3); r9 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r9) -stoi(r4); r9 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r9) -stoi(r5); r9 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r9) -stoi(r6); r9 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r9) -stoi(r7); r9 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r9) -stoi(r8); r9 = to_string(total);}
         if(stack[1] == "r2"){total = stoi(r9) -stoi(r2); r9 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r9) -stoi(r10); r9 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r9) - stoi(variables.at(location+1));
            r9 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r9) - stoi(varName2); r9 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else if(varName1 == "r2" || varName1 == "R2")
      {
         if(stack[1] == "r1"){total = stoi(r10) -stoi(r1); r10 = to_string(total);}// this checks if r2 was called then add the two strings as if they were ints then convert and store string total back into r1
         if(stack[1] == "r3"){total = stoi(r10) -stoi(r3); r10 = to_string(total);}
         if(stack[1] == "r4"){total = stoi(r10) -stoi(r4); r10 = to_string(total);}
         if(stack[1] == "r5"){total = stoi(r10) -stoi(r5); r10 = to_string(total);}
         if(stack[1] == "r6"){total = stoi(r10) -stoi(r6); r10 = to_string(total);}
         if(stack[1] == "r7"){total = stoi(r10) -stoi(r7); r10 = to_string(total);}
         if(stack[1] == "r8"){total = stoi(r10) -stoi(r8); r10 = to_string(total);}
         if(stack[1] == "r9"){total = stoi(r10) -stoi(r9); r10 = to_string(total);}
         if(stack[1] == "r10"){total = stoi(r10) -stoi(r2); r10 = to_string(total);}
         if(find(variables.begin(), variables.end(), stack[1])!= variables.end())// this adds registers to variables
         {  vector<string>::iterator it;				
            it = find(variables.begin(), variables.end(), stack[1]); //find the location of the variable stored in vector
            int location = distance(variables.begin(), it); //this is where the var name is stored
            total = stoi(r10) - stoi(variables.at(location+1));
            r10 = to_string(total); // add register to the variables content which is just i+1}
         }
         else{total = stoi(r10) - stoi(varName2); r10 = to_string(total);}// if its just a number on the other end add it and return it in register
      				
      }
      else
      {
         vector<string>::iterator it;				
         it = find(variables.begin(), variables.end(), varName1); // locate the variable name that they chose and set the variable which is just the next spot in the vector
         int location1 = distance(variables.begin(), it); //this is where the var name is stored
                  
         it = find(variables.begin(), variables.end(), varName2); // locate the variable name that they chose and set the variable which is just the next spot in the vector
         int location2 = distance(variables.begin(), it);
                  
                  
         string num1 = variables.at(location1+1);
         string num2 = variables.at(location2+1);
                  
         int sum = stoi(num1) - stoi(num2);//do the math on ints not strings
                  	
         variables.at(location1+1) = to_string(sum); // return the sum in the left parameters location
                  	
      }
               	
               
      pop();pop();//clear stack so new commands can be used
                   	
   }

}