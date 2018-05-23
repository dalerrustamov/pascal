#include <string>
#include <iostream>
#include <fstream>
#include "lexer.h"
#include <map>
#include <stack>
#include<vector>
#include<fstream>
#include<unordered_map>

using namespace std;

	struct Var_Info;
	struct Param_Info;
	struct Local_Var;

	struct Scope
	{
		//the map is what we used to store the productions, keys.. last semester//dynamic array
		unordered_map<string, Var_Info> Table;
		int offset;
		string ScopeName;
		Scope *PreviousScope;
	};

	struct Var_Info //name/type/offset ect
	{
		string name;
		string type; //datatype
		int size;//bt
		int offset;
		unordered_map<string, Param_Info> parameters;
		Scope *NextScope;
		vector<string> Param_Order;
		int Parameter_Offset;
		string Function_Type;
	};

	struct Param_Info //name/type/offset ect
	{
		string name;
		string type; //return type
		int size; 
		int offset; 
		bool PassbyReference;
	};

	struct Local_Var //name/type/offset ect
	{
		string type;
		int size; 
		int offset; 
	};


class SymbolTable
{
private:
	Scope * CurrentScope;

public:
	SymbolTable() 
	{
		CurrentScope = new Scope;
		CurrentScope->offset = 0; 
		CurrentScope->ScopeName = " ";//sp
		CurrentScope->PreviousScope = NULL;
	}

	bool AddVariable(string name, string type, int size) //to local scope
	{
		if (CurrentScope->Table.find(name) != CurrentScope->Table.end()) //making sure it hasnt been identified before
		{
			return false; //quit
		}//else{}
		CurrentScope->Table[name].type = type; //type
		CurrentScope->Table[name].size = size; //size
		CurrentScope->Table[name].offset = CurrentScope->offset;
		CurrentScope->Table[name].NextScope = NULL; //because it is a variable (NULL)
		CurrentScope->Table[name].Function_Type = ""; //because it is a variable (NULL)
		CurrentScope->offset += size;
		return true;
	}

	//making sure the vars were declared
	bool Find_Variable(string name, int &offset, string &type, string &returnType, bool &PassbyReferenceerence, bool Param, string CallingFunc)
	{
		Scope *TravScope = CurrentScope; //temp to hold the current scope
		if (!Param)
		{
			while (TravScope != NULL) //runs until NULL (last scope)
			{
				if (TravScope->Table.find(name) == TravScope->Table.end()) //not found in the local scope
				{
					TravScope = TravScope->PreviousScope;//move to the previous
				}
				else
				{
					offset = TravScope->Table[name].offset;
					type = TravScope->Table[name].type;
					returnType = TravScope->Table[name].Function_Type;
					PassbyReferenceerence = false;
					return true;
				}
			}//else{}
			return false;
		}
		else //it is a parameter 
		{
			while (TravScope != NULL) //runs until NULL (last scope)
			{
				if (TravScope->Table.find(CallingFunc) == TravScope->Table.end()) //not found in the local scope
				{
					TravScope = TravScope->PreviousScope;//move to the previous
				}
				else
				{
					if (TravScope->Table[CallingFunc].parameters.find(name) == TravScope->Table[CallingFunc].parameters.end())
					{
						return false;
					}
					else
					{
						offset = TravScope->Table[CallingFunc].parameters[name].offset; 
						type = TravScope->Table[CallingFunc].parameters[name].type;
						returnType = "";
						PassbyReferenceerence = TravScope->Table[CallingFunc].parameters[name].PassbyReference;
						return true;
					}

				}
			}//else{}
			return false;
		}

	}

	bool Find_Parameter(int count, bool &PassbyReference, string CalledFunc, string &type) //almost the same as finding variables
	{
		string tempString;

		Scope *TravScope = CurrentScope; 
		while (TravScope != NULL) 
		{
			if (TravScope->Table.find(CalledFunc) == TravScope->Table.end()) 
			{
				TravScope = TravScope->PreviousScope;
			}
			else
			{
				if (count + 1 <= TravScope->Table[CalledFunc].Param_Order.size())
				{
					tempString = TravScope->Table[CalledFunc].Param_Order[count];
					PassbyReference = TravScope->Table[CalledFunc].parameters[tempString].PassbyReference;
					type = TravScope->Table[CalledFunc].parameters[tempString].type;
					return true;
				}
				else
					return false;
			}
		}
		return false;

	}

	bool AddProcFunc(string name, string type) //Add procedures and functions
	{
		if (CurrentScope->Table.find(name) != CurrentScope->Table.end()) //making sure its not being created twice
		{
			return false; 
		}//else{}

		CurrentScope->Table[name].type = type; //return type
		CurrentScope->Table[name].size = 0; //because it is procedure or function(always 0)
		CurrentScope->Table[name].offset = 0; //because it is procedure or function(always 0)
		CurrentScope->Table[name].Function_Type = ""; //nothing in return for now

		Scope *OldCurrent = CurrentScope; //temp
		CurrentScope->Table[name].NextScope = new Scope; 
		CurrentScope = CurrentScope->Table[name].NextScope; 
		CurrentScope->offset = 0; //offset of the new scope
		CurrentScope->ScopeName = name; //names are the same
		CurrentScope->PreviousScope = OldCurrent; 
		return true;

	}

	bool AddParam(string ProcFuncName, string name, string type, int size, bool PassbyReference)//Add a parameter to a variable in the table
	{
		if (CurrentScope->PreviousScope->Table[ProcFuncName].parameters.find(name) != CurrentScope->PreviousScope->Table[ProcFuncName].parameters.end()) //checks to see if it is a duplicate identifier
		{
			return false;
		}
		CurrentScope->PreviousScope->Table[ProcFuncName].parameters[name].type = type; //type
		CurrentScope->PreviousScope->Table[ProcFuncName].parameters[name].size = size; //size
		CurrentScope->PreviousScope->Table[ProcFuncName].parameters[name].PassbyReference = PassbyReference; //how was it passed
		CurrentScope->PreviousScope->Table[ProcFuncName].parameters[name].offset = CurrentScope->PreviousScope->Table[ProcFuncName].Parameter_Offset; 
		CurrentScope->PreviousScope->Table[ProcFuncName].Param_Order.push_back(name);
		CurrentScope->PreviousScope->Table[ProcFuncName].Parameter_Offset += size;
		return true;

	}

	bool AddReturnType(string FuncName, string ReturnType)
	{
		if (CurrentScope->PreviousScope->Table.find(FuncName) != CurrentScope->PreviousScope->Table.end()) //makig sure the function exists
		{
			CurrentScope->PreviousScope->Table[FuncName].Function_Type = ReturnType;
			return true;
		}
		else
			return false;

	}

	void BackOut() //to the previous scope
	{
		if (CurrentScope->PreviousScope) {
			CurrentScope = CurrentScope->PreviousScope; 
		}		
	}
};


class CParser{
private:
	CLex Lex;
	CToken TOKEN;
	bool GotToken = false;

	SymbolTable SymTable;
	unordered_map<string, bool> reservedWords;
	int TheOffset;
	vector<Var_Info> tempVar;
	Var_Info tempVar_Info;
	string type;
	string temp;
	string ReturnType;
	string temp_datatype;
	vector<Param_Info> tempParam;
	Param_Info tempParam_Info;
	string CurProcFunc;
	bool PassbyReference;
	int Parameter_Offset;
	int ParamCount;
	string CalledFunc;

public:
	CParser()
	{
	}
	CParser(string FileName)
	{
		ReservedWords();
	}

	void ClearTempVar(Var_Info tempVar_Info) //clear the temps
	{
		tempVar_Info.name = "";
		tempVar_Info.NextScope = NULL;
		tempVar_Info.offset = NULL;
		tempVar_Info.Function_Type = "";
		tempVar_Info.parameters.clear();
		tempVar_Info.Param_Order.clear();
		tempVar_Info.size = 0;
		tempVar_Info.type = "";
	}

	void ClearTempParam(Param_Info tempParam_Info) //clear the temps
	{
		tempParam_Info.name = "";
		tempParam_Info.offset = 0;
		tempParam_Info.PassbyReference = NULL;
		tempParam_Info.size = 0;
		tempParam_Info.type = "";
	}

	void CheckParam()
	{
		string temp_datatype;
		if (SymTable.Find_Parameter(ParamCount, PassbyReference, CalledFunc, type))//making sure the parameter exists
		{
			if (TOKEN.TokenType == "word") //parameter should be a variable (identifier)
			{
				if (SymTable.Find_Variable(TOKEN.TokenValue, TheOffset, temp_datatype, ReturnType, PassbyReference, false, CurProcFunc)) //if the variable exists
				{

					if (temp_datatype.compare(type) == 0)//making sure they have the same datatypes
					{
						GotToken = Lex.GetToken(TOKEN);
						ParamCount++;
						CheckParam();
					}
					else
					{
						cout << "The parameter '" << TOKEN.TokenValue << "' that you passed to '" << CalledFunc << "' expected to be " << type << endl;
						cout << "More info: The parameter is being passed by reference. You are welcome!" << endl;
						system("pause");
						exit(1);
					}

				}
				else
				{

					if (!PassbyReference) // if it is passed by copy
					{
						if (type.compare("boolean") == 0 && (TOKEN.TokenValue == "true" || TOKEN.TokenValue == "false"))//they can just be true or false
						{
							GotToken = Lex.GetToken(TOKEN);
							ParamCount++;
							CheckParam();
						}
						else
						{
							cout << "The parameter '"<< TOKEN.TokenValue << "' that you passed to '" << CalledFunc << "' expected to be " << type << endl;
							cout << "More info: The parameter is being passed by copy. You are welcome!"<<endl;
							system("pause");
							exit(1);
						}
					}
					else
					{
						cout << "'" << TOKEN.TokenValue << "' should be declared first in order to be used as a parameter for '" << CalledFunc<<"'" << endl;
						system("pause");
						exit(1);
					}
				}
			}
			//seperator between parameters
			else if (TOKEN.TokenValue == ",")
			{
				GotToken = Lex.GetToken(TOKEN);
				CheckParam();
			}
			else if (!PassbyReference) //passed by copy
			{
				if ((type.compare("integer") == 0 || type.compare("boolean") == 0) && TOKEN.TokenType == "integer")
				{
					GotToken = Lex.GetToken(TOKEN);
					ParamCount++;
					CheckParam();
				}
				else
				{
					if (TOKEN.TokenValue == ")")
					{
						cout << "'" << CalledFunc << "' takes less than " << ParamCount << " parameters" << endl;
						system("pause");
						exit(1);
					}
					else
					{
						cout << "Invalid datatype " << endl;
						system("pause");
						exit(1);
					}
				}
			}
			else if (PassbyReference)
			{
				cout << "Please try adding more parameters to '" << CalledFunc << "', or replace the '" << TOKEN.TokenValue << "' with the correct datatype" << endl;
				system("pause");
				exit(1);
			}
			else
			{
				cout << "'" << CalledFunc << "' takes less than " << ParamCount << " parameters" << endl;
				system("pause");
				exit(1);
			}
		}
		else if (TOKEN.TokenValue == ")")
		{
			return;
		}
		else
		{
			if (ParamCount + 1 > 1)
			{
				cout << "'" << CalledFunc << "' takes less than " << ParamCount +1 << " parameters, or expected ')' after the last parameter" << endl; //also gives this error or the parameters are not closed by )
				system("pause");
				exit(1);
			}
			else
			{
				cout << "'" << CalledFunc << "' doesnt take " << ParamCount +1 << " parameters" << endl;
				system("pause");
				exit(1);
			}

		}

	}

	void ReservedWords()
	{
		ifstream file("reservedwords.txt");
		string s;
		if (file.is_open())
		{
			while (!file.eof())
			{
				getline(file, s);
				reservedWords[s] = true;
			}

		}
		else
		{
			cout << "Cannot open the file " << "reservedwords.txt" << endl;
		}
	}


	void Parse()
	{
		Lex.LoadFile("OKcode.txt");
		GotToken = Lex.GetToken(TOKEN); //gets the first token
		if (GotToken)
		{
			Pascal();
			//cout << "Successfully parsed . . . " << endl;
			system("pause");
			exit(1);
		}
		else {
			cout << "File not found or is empty"<<endl; // (most likely)
		}
	}


	void E() { //T-SE-E
		SE();
		if (TOKEN.TokenValue == "<") {
			Lex.GetToken(TOKEN);
			SE();
		}
		if (TOKEN.TokenValue == ">") {
			Lex.GetToken(TOKEN);
			SE();
		}
		if (TOKEN.TokenValue == "=") {
			Lex.GetToken(TOKEN);
			SE();
		}
	}

	void SE() {
		T();
	}

	void T() { //TR &Tprime -T 
		TR();
		TPrime();
	}

	void TR() { // F-TR
		F();
	}

	void TPrime() {
		if (GotToken) {

			if (TOKEN.TokenValue == "+")
			{
				Lex.GetToken(TOKEN); //get token

					TR();
					TPrime();
			}

			if (TOKEN.TokenValue == "-")
			{
				Lex.GetToken(TOKEN);

				TR();
				TPrime();
			}
		
			if (TOKEN.TokenValue == "and")
			{
				Lex.GetToken(TOKEN); //consume

				TR();
				TPrime();
			}

			if (TOKEN.TokenValue == "or")
			{
				Lex.GetToken(TOKEN); //consume

				TR();
				TPrime();
			}

		}
		else {
			cout << "bad code(exit#1)" << endl;
			system("pause");
			exit(1);
		}
	}

	void F() { //FR Fprime - F
		FR();
		FPrime();
	}

	void FR() { //(E) - FR
		if (GotToken) {
			if (TOKEN.TokenValue == "(")
			{
				Lex.GetToken(TOKEN);
				if (TOKEN.TokenValue == ")")
				{
					cout << "bad code(exit#2)";
					system("pause");
					exit(1);
				}
				else {
					E();
					if (TOKEN.TokenValue == ")")
					{
						Lex.GetToken(TOKEN);
					}
					else
					{
						cout << "bad code(exit#3)";
						system("pause");
						exit(1);
					}
				}
			}
			else if (TOKEN.TokenType == "real") {
				Lex.GetToken(TOKEN);
				Real();
			}

			else if (TOKEN.TokenType == "integer") {
				Lex.GetToken(TOKEN);
				integer();
			}
			else { //if (TOKEN.TokenType == "word") {

				//Lex.GetToken(TOKEN);
				
				if (SymTable.Find_Variable(TOKEN.TokenValue, TheOffset, type, ReturnType, PassbyReference, false, "") || SymTable.Find_Variable(TOKEN.TokenValue, TheOffset, type, ReturnType, PassbyReference, true, CurProcFunc))
				{
					if (type.compare("integer") == 0) //making sure the type is integer
					{
						GotToken = Lex.GetToken(TOKEN);
					}
					else
					{
						cout << "Expected integer to do the operaton, but '"<< type << "' received" << endl;
						system("pause");
						exit(1);
					}

				}
				else if (TOKEN.TokenValue == "true" || TOKEN.TokenValue == "false") //the value can just be true or false if the type is bool
				{
					GotToken = Lex.GetToken(TOKEN);
				}
				else
				{
					cout << "'" << TOKEN.TokenValue << "' should be declared first in order to be used for operations" << endl;
					system("pause");
					exit(1);
				}

			}
			/*
			if (TOKEN.TokenType == "word") {
			Lex.GetToken(TOKEN);
			//	integer();
			}
			*/

		}
		else {
			cout << "bad code(exit#5)" << endl;
			system("pause");
			exit(1);
		}
	}

	void FPrime()
	{
		if (GotToken)
		{
			if (TOKEN.TokenValue == "*")
			{
				Lex.GetToken(TOKEN);
				if (TOKEN.TokenValue == "*" || TOKEN.TokenValue == "/")
				{
					cout << "bad code(exit#6)";
					system("pause");
					exit(1);
				}
				else if (TOKEN.TokenValue == "-" || TOKEN.TokenValue == "+") {
					Lex.GetToken(TOKEN); //I might get *- so I just need to consume the - and do the same thing it would do otherwise
					FR();
					FPrime();
				}
				else //normal things
				{
					FR();
					FPrime();
				}
			}
			if (TOKEN.TokenValue == "/")
			{
				Lex.GetToken(TOKEN);
				if (TOKEN.TokenValue == "*" || TOKEN.TokenValue == "/")
				{
					cout << "bad code(exit#7)" << endl;
					system("pause");
					exit(1);
				}
				else if (TOKEN.TokenValue == "-" || TOKEN.TokenValue == "+") {
					Lex.GetToken(TOKEN); //I might get /- so I just need to consume the - and do the same thing it would do otherwise
					FR();
					FPrime();
				}
				else {
					FR();
					FPrime();
				}
			}
		}
		else
		{
			cout << "bad code(exit#8)";
			system("pause");
			exit(1);
		}
	}

	void Real() {
		//do nothing for now
	}

	void integer() {
		//do nothing for now
	}

	void stat()
	{
		//if
		if (TOKEN.TokenValue == "if")
		{
			GotToken = Lex.GetToken(TOKEN); //consumed "if"
			//call expression
			//E();

			if (SymTable.Find_Variable(TOKEN.TokenValue, TheOffset, type, ReturnType, PassbyReference, false, "") || (SymTable.Find_Variable(TOKEN.TokenValue, TheOffset, type, ReturnType, PassbyReference, true, CurProcFunc))) //making sure the variable is declared
			{
				if (type.compare("integer") == 0)
				{
					E();
				}
				else if (type.compare("boolean") == 0)
				{
					GotToken = Lex.GetToken(TOKEN);
				}
				else
				{
					cout << "In one if your statements, you are not using a valid datatype" << endl;
					system("pause");
					exit(1);
				}

			}
			else {
				E();
			}

			

			if (TOKEN.TokenValue == "then")
			{
				GotToken = Lex.GetToken(TOKEN); //consumed "then"
				stat();
				stat1();
			}
			else
			{
				cout << "bad code(exit#9)" << endl;
				system("pause");
				exit(1);
			}
		}

		//while

		if (TOKEN.TokenValue == "while")
		{
			GotToken = Lex.GetToken(TOKEN);
			//call expression
			E();

			if (TOKEN.TokenValue == "do")
			{
				GotToken = Lex.GetToken(TOKEN);
				stat();
			}
			else
			{
				cout << "bad code(exit#10)" << endl;
				system("pause");
				exit(1);
			}
		}

		//begin

		if (TOKEN.TokenValue == "begin")
		{
			GotToken = Lex.GetToken(TOKEN);
			stat();
			mstat();

			if (TOKEN.TokenValue == "end")
			{
				GotToken = Lex.GetToken(TOKEN);
			}
			else {
				cout << "bad code(exit#11)" << endl;
				system("pause");
				exit(1);
			}
		}

		//word type, just initializing a variable in the scope
		
		if (TOKEN.TokenType == "word" && TOKEN.TokenValue != "end" && TOKEN.TokenValue != "else")
		{
			
			/*	
			GotToken = Lex.GetToken(TOKEN);
			*/

			if (reservedWords.find(TOKEN.TokenValue) != reservedWords.end()) //if it is a reserved word 
			{
				cout << "You cannot use '" << TOKEN.TokenValue << "' as a variable name since it is a reserved word in the language" << endl;
				system("pause");
				exit(1);
			}
			if (SymTable.Find_Variable(TOKEN.TokenValue, TheOffset, type, ReturnType, PassbyReference, false, "") || (SymTable.Find_Variable(TOKEN.TokenValue, TheOffset, type, ReturnType, PassbyReference, true, CurProcFunc))) //If the identifier exists
			{
				temp = TOKEN.TokenValue; //temp
				temp_datatype = type;
				GotToken = Lex.GetToken(TOKEN);
				if (type.compare("procedure") == 0)
				{
					CalledFunc = temp;
					if (TOKEN.TokenValue == "(")
					{
						GotToken = Lex.GetToken(TOKEN);
						ParamCount = 0;
						CheckParam();
						ParamCount = 0;
						if (TOKEN.TokenValue == ")")
						{
							GotToken = Lex.GetToken(TOKEN);
						}
						else
						{
							cout << "The parameter should be closed, replace the token '" << TOKEN.TokenValue << "' by ')'" << endl;
							system("pause");
							exit(1);
						}
					}
					else
					{
						cout << "The parameter should be opened, replace the token '" << TOKEN.TokenValue << "' by '('" << endl;
						system("pause");
						exit(1);
					}
				}
				else if (type.compare("function") == 0) 
				{
					if (temp.compare(CurProcFunc) == 0) //making sure it is the current function
					{
						temp_datatype = ReturnType;
						if (TOKEN.TokenType == ":=")
						{
							GotToken = Lex.GetToken(TOKEN);
							temp = TOKEN.TokenValue;
							if (SymTable.Find_Variable(TOKEN.TokenValue, TheOffset, type, ReturnType, PassbyReference, false, ""))
							{
								if (type.compare("function") == 0)
								{
									CalledFunc = temp;
									if (ReturnType.compare(temp_datatype) == 0) //datatype check
									{
										GotToken = Lex.GetToken(TOKEN);
										if (TOKEN.TokenValue == "(")
										{
											GotToken = Lex.GetToken(TOKEN);
											ParamCount = 0;
											CheckParam();
											ParamCount = 0;
											if (TOKEN.TokenValue == ")")
											{
												GotToken = Lex.GetToken(TOKEN);
											}
											else
											{
												cout << "The parameter should be closed, replace the token '" << TOKEN.TokenValue << "' by ')'" << endl;
												system("pause");
												exit(1);
											}
										}
										else
										{
											cout << "The parameter should be opened, replace the token '" << TOKEN.TokenValue << "' by '('" << endl;
											system("pause");
											exit(1);
										}
									}
									else
									{
										cout << "'" << temp << " returns '" << temp_datatype << "' but you compared it with a different datatype" << endl;
										system("pause");
										exit(1);
									}
								}
								else if (type.compare("integer") == 0 || type.compare("boolean") == 0)
								{
									if (type.compare(temp_datatype) == 0)
									{
										E();
									}
									else
									{
										cout << "'" << temp << " returns '" << temp_datatype << "' but you compared it with a different datatype" << endl;
										system("pause");
										exit(1);
									}
								}

							}
							else if (TOKEN.TokenType == "word" && (TOKEN.TokenValue == "true" || TOKEN.TokenValue == "false"))
							{
								if (temp_datatype.compare("boolean") == 0)
								{
									GotToken = Lex.GetToken(TOKEN);
								}
								else
								{
									cout << "'" << temp << " returns '" << temp_datatype << "' but you compared it with a different datatype" << endl;
									system("pause");
									exit(1);
								}

							}
							else if (TOKEN.TokenType == "integer")
							{
								if (temp_datatype.compare("integer") == 0 || temp_datatype.compare("boolean") == 0)
								{
									E();
								}
								else
								{
									cout << "'" << temp << " returns '" << temp_datatype << "' but you compared it with a different datatype" << endl;
									system("pause");
									exit(1);
								}
							}
							else if (TOKEN.TokenType == "word")
							{
								cout << "'" << TOKEN.TokenValue << "' should be declared first in order to be used in the program" << endl;
								system("pause");
								exit(1);
							}
							else
							{
								cout << "Unexpected error, please double check your datatypes" << endl; //not sure what is causing it to fall here
								system("pause");
								exit(1);
							}
						}
						else
						{
							cout << "Expected ':=' but received '" << TOKEN.TokenValue << "'" << endl;
							system("pause");
							exit(1);
						}
					}
					else
					{
						cout << "You can only set return value in local functions" << endl;
						system("pause");
						exit(1);
					}
				}

				else if (TOKEN.TokenValue == ":=")
				{
					GotToken = Lex.GetToken(TOKEN);
					if (SymTable.Find_Variable(TOKEN.TokenValue, TheOffset, type, ReturnType, PassbyReference, false, "") || (SymTable.Find_Variable(TOKEN.TokenValue, TheOffset, type, ReturnType, PassbyReference, true, CurProcFunc))) 
					{
						if (type.compare("function") == 0) //if it is a function
						{
							CalledFunc = TOKEN.TokenValue;
							if (ReturnType.compare(temp_datatype) == 0) //makin sure the datatypes match
							{
								GotToken = Lex.GetToken(TOKEN);
								if (TOKEN.TokenValue == "(")
								{
									GotToken = Lex.GetToken(TOKEN); //consume
									ParamCount = 0;
									CheckParam();
									ParamCount = 0;
									if (TOKEN.TokenValue == ")")
									{
										GotToken = Lex.GetToken(TOKEN); //consume
									}
									else
									{
										cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of )" << endl;
										system("pause");
										exit(1);
									}
								}
								else
								{
									cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of )" << endl;
									system("pause");
									exit(1);
								}
							}
							else
							{
								cout << "'" << temp << " returns '" << temp_datatype << "' but you compared it with a different datatype" << endl;
								system("pause");
								exit(1);
							}
						}
						else if (type.compare("integer") == 0 || type.compare("boolean") == 0)
						{
							if (type.compare(temp_datatype) == 0)
							{
								E();
							}
							else
							{
								cout << "'" << temp << " returns '" << temp_datatype << "' but you compared it with a different datatype" << endl;
								system("pause");
								exit(1);
							}
						}

					}
					else if (TOKEN.TokenType == "word" && (TOKEN.TokenValue == "true" || TOKEN.TokenValue == "false"))
					{
						if (temp_datatype.compare("boolean") == 0)
						{
							GotToken = Lex.GetToken(TOKEN); //consume
						}
						else
						{
							cout << "'" << temp << " returns '" << temp_datatype << "' but you compared it with a different datatype" << endl;
							system("pause");
							exit(1);
						}

					}
					else if (TOKEN.TokenType == "word")
					{
						cout << "'" << TOKEN.TokenValue << "' should be declared first in order to be used for operations" << endl;
						system("pause");
						exit(1);
					}
					else if (TOKEN.TokenType == "integer")
					{
						if (temp_datatype.compare("integer") == 0 || temp_datatype.compare("boolean") == 0)
						{
							E();
						}
						else
						{
							cout << "'" << temp << " returns '" << temp_datatype << "' but you compared it with a different datatype" << endl;
							system("pause");
							exit(1);
						}
					}
					else
					{
						cout << "Unexpected error, please make sure you are using right datatypes" << endl;
						system("pause");
						exit(1);
					}
				}
			}
			else
			{
				cout << "'" << TOKEN.TokenValue << "' should be declared first in order to be used for operations" << endl;
				system("pause");
				exit(1);
			}

		}

		//stat also can be empty
	}

	void stat1() //stat prime
	{
		if (TOKEN.TokenValue == "else")
		{
			GotToken = Lex.GetToken(TOKEN);
			stat();
			
		}
		//else do nothing
	}

	void mstat() //mstat
	{
		if (TOKEN.TokenValue == ";")
		{
			GotToken = Lex.GetToken(TOKEN); //consumed the ";"
			stat();
			mstat();
		}
		//else do nothing
	}

	void dtt()
	{
		//the 3 datatypes
		if (TOKEN.TokenValue == "integer" || TOKEN.TokenValue == "boolean" || TOKEN.TokenValue == "char") 
		{
			GotToken = Lex.GetToken(TOKEN); //next
		}
		else
		{
			//no any other datatype can be accepted for now
			cout << "Bad code" << endl;

			if (!tempVar.empty()) 
			{
				tempVar.clear();
			}

			system("pause");
			exit(1);
		}
	}

	//added parameter
	void variable(bool Var)//variables on single line
	{
		//, means I have multiple variables, if : then it is over
		if (TOKEN.TokenValue == ",")
		{
			GotToken = Lex.GetToken(TOKEN);

			//there is some more variables which is always word
			if (TOKEN.TokenType == "word")
			{
				
				//GotToken = Lex.GetToken(TOKEN);
				//variable();
				//if (TOKEN.TokenType == "word")
				//{
					if (Var) //if its a variable
					{
						tempVar_Info.name = TOKEN.TokenValue;
						tempVar.push_back(tempVar_Info); 
						GotToken = Lex.GetToken(TOKEN); //consume
						variable(1);
					}
					else
					{
						tempParam_Info.name = TOKEN.TokenValue;
						tempParam.push_back(tempParam_Info); 
						GotToken = Lex.GetToken(TOKEN); //consume
						variable(0);
					}

				//}
			}
			else
			{
				cout << "Bad code" << endl;
				system("pause");
				exit(1);
			}
		}
		else
		{
			return;
		}
	}

	void mv()//1< variables
	{
		//it must be word, but cannot be a keyword
		if (TOKEN.TokenValue == "var")
		{
			GotToken = Lex.GetToken(TOKEN); //consume
			if (TOKEN.TokenType == "word" && TOKEN.TokenValue != "procedure" && TOKEN.TokenValue != "function" && TOKEN.TokenValue != "begin")
			{
				tempVar_Info.name = TOKEN.TokenValue; 
				tempVar.push_back(tempVar_Info); 
				GotToken = Lex.GetToken(TOKEN); //consume
				variable(1);
				if (TOKEN.TokenValue == ":")//no more variables
				{
					GotToken = Lex.GetToken(TOKEN); //consume
					for (int i = 0; i < tempVar.size(); i++) //temp
					{
						tempVar[i].type = TOKEN.TokenValue; //sets the type of each variable
						if (tempVar[i].type == "integer")
						{
							tempVar[i].size = 4; //if it is an integer, the size of the variable is 4 Bytes
						}
						else
						{
							tempVar[i].size = 1; //if it is a character or a boolean, the size of the variable is 1 Byte
						}
					}

					dtt();

					for (int i = 0; i < tempVar.size(); i++) //for all variables in the temporary storage vector
					{
						SymTable.AddVariable(tempVar[i].name, tempVar[i].type, tempVar[i].size); //add the variables to the symbol table
					}

					if (!tempVar.empty()) //if the temporary storage vector is not empty
					{
						tempVar.clear();//clear the temporary storage vector
					}

					if (TOKEN.TokenValue == ";")
					{
						GotToken = Lex.GetToken(TOKEN); //consume
						mv();
					}
					else
					{
						cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of ;" << endl;
						if (!tempVar.empty()) //if the temporary storage vector is not empty
						{
							tempVar.clear();//clear the temporary storage vector
						}
						system("pause");
						exit(1);
					}
				}
				else
				{
					cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of :" << endl;
					system("pause");
					exit(1);
				}
			}
			else
			{
				cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of an identifier" << endl;
				system("pause");
				exit(1);
			}
		}
		else if (TOKEN.TokenType == "word" && TOKEN.TokenValue != "procedure" && TOKEN.TokenValue != "function" && TOKEN.TokenValue != "begin")
		{
			tempVar_Info.name = TOKEN.TokenValue; 
			tempVar.push_back(tempVar_Info); 
			GotToken = Lex.GetToken(TOKEN); //consume
			variable(1);
			if (TOKEN.TokenValue == ":")
			{
				GotToken = Lex.GetToken(TOKEN); //consume
				for (int i = 0; i < tempVar.size(); i++) //temp
				{
					tempVar[i].type = TOKEN.TokenValue; //type
					if (tempVar[i].type == "integer")
					{
						tempVar[i].size = 4; //int is 4 bytes
					}
					else
					{
						tempVar[i].size = 1; //bool is one byte
					}
				}

				dtt();

				for (int i = 0; i < tempVar.size(); i++) //temp
				{
					SymTable.AddVariable(tempVar[i].name, tempVar[i].type, tempVar[i].size); 
				}

				if (!tempVar.empty())
				{
					tempVar.clear();
				}

				if (TOKEN.TokenValue == ";")
				{
					GotToken = Lex.GetToken(TOKEN); //consume
					mv();
				}
				else
				{
					cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of ;" << endl;
					system("pause");
					exit(1);
				}
			}
			else
			{
				cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of :" << endl;
				system("pause");
				exit(1);
			}
		}
		else //otherwise, it must be procedure or function
		{
			pfv();
		}
	}


void Param()
{
	if (TOKEN.TokenValue == "var")//Pass by reference
	{
		GotToken = Lex.GetToken(TOKEN); //consume
		if (TOKEN.TokenType == "word")
		{
			tempParam_Info.name = TOKEN.TokenValue;
			tempParam.push_back(tempParam_Info);
			GotToken = Lex.GetToken(TOKEN); //consume
			variable(0);
			if (TOKEN.TokenValue == ":")
			{
				GotToken = Lex.GetToken(TOKEN); //consume

				for (int i = 0; i < tempParam.size(); i++)
				{
					if (tempParam[i].type == "")
					{
						if (TOKEN.TokenValue == "integer")
						{
							tempParam[i].size = 4;
							tempParam[i].type = "integer";
						}
						else if (TOKEN.TokenValue == "boolean")
						{
							tempParam[i].size = 1;
							tempParam[i].type = "boolean";
						}
						else
						{
							tempParam[i].size = 1;
							tempParam[i].type = "char";
						}
						tempParam[i].PassbyReference = true;
					}
				}
				//after : always comes datatype
				dtt();
				//there might be more parameters
				mParam();
			}
			else
			{
				cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of :" << endl;
				system("pause");
				exit(1);
			}
		}
		else
		{
			cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of an identifier :" << endl;
			system("pause");
			exit(1);
		}
	}
	else if (TOKEN.TokenType == "word")//Pass by copy
	{
		tempParam_Info.name = TOKEN.TokenValue;
		tempParam.push_back(tempParam_Info);
		GotToken = Lex.GetToken(TOKEN); //consume
		variable(0);
		if (TOKEN.TokenValue == ":")
		{
			GotToken = Lex.GetToken(TOKEN); //consume

			for (int i = 0; i < tempParam.size(); i++)
			{
				if (tempParam[i].type == "")
				{
					if (TOKEN.TokenValue == "integer")
					{
						tempParam[i].size = 4;
						tempParam[i].type = "integer";
					}
					else if (TOKEN.TokenValue == "boolean")
					{
						tempParam[i].size = 1;
						tempParam[i].type = "boolean";
					}
					else
					{
						tempParam[i].size = 1;
						tempParam[i].type = "char";
					}
					tempParam[i].PassbyReference = false;
				}
			}
			dtt();
			mParam();
		}
		else
		{
			cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of :" << endl;
			system("pause");
			exit(1);
		}
	}
	else
	{
		return;
	}
}

void mParam()
{
	if (TOKEN.TokenValue == ";")
	{
		GotToken = Lex.GetToken(TOKEN); //consume
		if (TOKEN.TokenValue == "var")//pass by reference
		{
			GotToken = Lex.GetToken(TOKEN); //consume
			if (TOKEN.TokenType == "word")
			{
				tempParam_Info.name = TOKEN.TokenValue; //temp
				tempParam.push_back(tempParam_Info); //temp
				GotToken = Lex.GetToken(TOKEN); //consume
				variable(0);
				if (TOKEN.TokenValue == ":")
				{
					GotToken = Lex.GetToken(TOKEN); //consume
					for (int i = 0; i < tempParam.size(); i++)
					{
						if (tempParam[i].type == "")
						{
							if (TOKEN.TokenValue == "integer")
							{
								tempParam[i].size = 4;
								tempParam[i].type = "integer";
							}
							else if (TOKEN.TokenValue == "boolean")
							{
								tempParam[i].size = 1;
								tempParam[i].type = "boolean";
							}
							else
							{
								tempParam[i].size = 1;
								tempParam[i].type = "char";
							}
							tempParam[i].PassbyReference = true;
						}

					}
					//after : always comes datatype
					dtt();
					//more parameters?
					mParam();
				}
				else
				{
					cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of :" << endl;
					system("pause");
					exit(1);
				}
			}
			else
			{
				cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of an identifier" << endl;
				system("pause");
				exit(1);
			}
		}

		else if (TOKEN.TokenType == "word")//pass by copy
		{
			tempParam_Info.name = TOKEN.TokenValue; //temp
			tempParam.push_back(tempParam_Info); 
			GotToken = Lex.GetToken(TOKEN); //consume
			variable(0);
			if (TOKEN.TokenValue == ":")
			{
				GotToken = Lex.GetToken(TOKEN); //consume
				for (int i = 0; i < tempParam.size(); i++)
				{
					if (tempParam[i].type == "")
					{
						if (TOKEN.TokenValue == "integer")
						{
							tempParam[i].size = 4;
							tempParam[i].type = "integer";
						}
						else if (TOKEN.TokenValue == "boolean")
						{
							tempParam[i].size = 1;
							tempParam[i].type = "boolean";
						}
						else
						{
							tempParam[i].size = 1;
							tempParam[i].type = "char";
						}
						tempParam[i].PassbyReference = false;
					}

				}
				//after: always comes datatype
				dtt();
				//more variables?
				mParam();
			}
			else
			{
				cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of :" << endl;
				system("pause");
				exit(1);
			}
		}
		else
		{
			cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of an identifier" << endl;
			system("pause");
			exit(1);
		}
	}

	else
	{
		return;
	}//nothing

}


void pfv()//procedures functions vars
{
	if (GotToken) //making sure I got the token
	{
		if (TOKEN.TokenValue == "var")//if it is a variable, enter
		{
			GotToken = Lex.GetToken(TOKEN); //consume
			if (TOKEN.TokenType == "word")
			{
				if (SymTable.Find_Variable(TOKEN.TokenValue, TheOffset, type, ReturnType, PassbyReference, false, ""))
				{
					cout << "Identifier '" << TOKEN.TokenValue << "' has been used more than once, please choose a different name" << endl;
					system("pause");
					exit(1);
				}
				tempVar_Info.name = TOKEN.TokenValue; //temp
				tempVar.push_back(tempVar_Info); //temp vector
				GotToken = Lex.GetToken(TOKEN); //consume
				variable(1); //multiple variables?
				if (TOKEN.TokenValue == ":")
				{
					GotToken = Lex.GetToken(TOKEN); //consume
					for (int i = 0; i < tempVar.size(); i++)
					{
						tempVar[i].type = TOKEN.TokenValue; 
						if (tempVar[i].type == "integer")
						{
							tempVar[i].size = 4; //int = 4 b
						}
						else
						{
							tempVar[i].size = 1; //bool = 4 b
						}
					}

					dtt();

					for (int i = 0; i < tempVar.size(); i++) //temp
					{
						SymTable.AddVariable(tempVar[i].name, tempVar[i].type, tempVar[i].size); 
					}

					ClearTempVar(tempVar_Info); 
					if (!tempVar.empty())
					{
						tempVar.clear();
					}

					if (TOKEN.TokenValue == ";")
					{
						GotToken = Lex.GetToken(TOKEN); //consume
						mv();
					}
					else
					{
						cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of ;" << endl;
						system("pause");
						exit(1);
					}
				}
				else
				{
					cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of :" << endl;
					system("pause");
					exit(1);
				}
			}
			else
			{
				cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of an identifier" << endl;
				system("pause");
				exit(1);
			}

		}
		else if (TOKEN.TokenValue == "function")//Functions
		{
			GotToken = Lex.GetToken(TOKEN); //consume
			if (TOKEN.TokenType == "word")
			{
				CurProcFunc = TOKEN.TokenValue; 
				if (!(SymTable.AddProcFunc(CurProcFunc, "function"))) 
				{
					cout << "Identifier " << TOKEN.TokenValue << " has been used more than once" << endl; 
					system("pause");
					exit(1);
				}
				
				GotToken = Lex.GetToken(TOKEN); //consume
				if (TOKEN.TokenValue == "(")
				{
					GotToken = Lex.GetToken(TOKEN); //consume
					Param();
					for (int i = 0; i < tempParam.size(); i++)
					{
						if (!SymTable.AddParam(CurProcFunc, tempParam[i].name, tempParam[i].type, tempParam[i].size, tempParam[i].PassbyReference)) //Add the parameter to the symbol table
						{
							cout << "Identifier " << tempParam[i].name << " has been used more than once" << endl;
							system("pause");
							exit(1);
						}

					}
					ClearTempParam(tempParam_Info);
					tempParam.clear();
					if (TOKEN.TokenValue == ")")
					{
						GotToken = Lex.GetToken(TOKEN); //consume
						if (TOKEN.TokenValue == ":")
						{
							GotToken = Lex.GetToken(TOKEN); //consume
							SymTable.AddReturnType(CurProcFunc, TOKEN.TokenValue);
							dtt();
							if (TOKEN.TokenValue == ";")
							{
								GotToken = Lex.GetToken(TOKEN); //consume
								Block();
								if (TOKEN.TokenValue == ";")
								{
									GotToken = Lex.GetToken(TOKEN); //consume
									CurProcFunc = "";
									SymTable.BackOut();
								}
								else
								{
									cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of ;" << endl;
									system("pause");
									exit(1);
								}
							}
							else
							{
								cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of ;" << endl;
								system("pause");
								exit(1);
							}
						}
						else
						{
							cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of :" << endl;
							system("pause");
							exit(1);
						}
					}
					else
					{
						cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of )" << endl;
						system("pause");
						exit(1);
					}
				}
				else
				{
					cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of (" << endl;
					system("pause");
					exit(1);
				}
			}
			else
			{
				cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of an identifier" << endl;
				system("pause");
				exit(1);
			}
		}
		else if (TOKEN.TokenValue == "procedure")//Procedures
		{
			GotToken = Lex.GetToken(TOKEN); //consume

			if (TOKEN.TokenType == "word")
			{
				CurProcFunc = TOKEN.TokenValue;
				GotToken = Lex.GetToken(TOKEN); //consume
				if (!SymTable.AddProcFunc(CurProcFunc, "procedure"))
				{
					cout << "the identifier" << TOKEN.TokenValue << " has been declared more than once, please choose a different name" << endl;
					system("pause");
					exit(1);
				}
				if (TOKEN.TokenValue == "(")
				{
					GotToken = Lex.GetToken(TOKEN); //consume
					Param();
					for (int i = 0; i < tempParam.size(); i++)
					{
						if (!SymTable.AddParam(CurProcFunc, tempParam[i].name, tempParam[i].type, tempParam[i].size, tempParam[i].PassbyReference)) //Add the parameter to the symbol table
						{
							cout << "the identifier" << tempParam[i].name << " has been declared more than once, please choose a different name" << endl;
							system("pause");
							exit(1);
						}
					}
					ClearTempParam(tempParam_Info);
					tempParam.clear();
					if (TOKEN.TokenValue == ")")
					{
						GotToken = Lex.GetToken(TOKEN); //consume
						if (TOKEN.TokenValue == ";")
						{
							GotToken = Lex.GetToken(TOKEN); //consume
							Block();
							if (TOKEN.TokenValue == ";")
							{
								GotToken = Lex.GetToken(TOKEN); //consume
								CurProcFunc = "";
								SymTable.BackOut();
							}
							else
							{
								cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of ;" << endl;
								system("pause");
								exit(1);
							}
						}
						else
						{
							cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of ;" << endl;
							system("pause");
							exit(1);
						}
					}
					else
					{
						cout << "Expected end of parameter but received '" << TOKEN.TokenValue << "'" << endl;
						system("pause");
						exit(1);
					}
				}
				else
				{
					cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of (" << endl;
					system("pause");
					exit(1);
				}
			}
			else
			{
				cout << "Unexpected token '" << TOKEN.TokenValue << "' found instead of an identifier" << endl;
				system("pause");
				exit(1);
			}
		}
	}
	else
	{
		return;
	}//nothign
}

	void Block()
	{
		//just need to check any variable, function or procedure was declared before the actual block
		while (TOKEN.TokenValue != "begin")
		{
			pfv();
		}

		//GotToken = Lex.GetToken(TOKEN); //got the first token before calling pascal()

		if (TOKEN.TokenValue == "begin")
		{
			GotToken = Lex.GetToken(TOKEN); //consumed the "begin"
			stat();
			mstat();

			if (TOKEN.TokenValue == "end")
			{
				GotToken = Lex.GetToken(TOKEN); //consumed the "end" and go back to Pascal 
			}
			else {
				cout << "bad code(exit#13)"<<endl;
				system("pause");
				exit(1);
			}
		}
	}

	void Pascal() //pascal - (Block) (.)
	{
		Block();
		
		//GotToken = Lex.GetToken(TOKEN);

		string prevToken = TOKEN.TokenValue;
		GotToken = Lex.GetToken(TOKEN); //need to consume . first before seeing if any token left

		if (prevToken == "." && GotToken != true)
		{
			cout << "Good code" << endl;
			/*
			system("pause");
			exit(1);
			*/
		}
		else
		{
			cout << "bad code(exit#14)" << endl;
			system("pause");
			exit(1);
		}
		/*
		if (GotToken) { //should not have any token left after end.
		cout << "bad code" << endl;
		system("pause");
		exit(1);
		}
		*/
	}
};

int main()
{
	CParser p1("OKcode.txt");
	p1.Parse();
	system("pause");
	return 0;
}