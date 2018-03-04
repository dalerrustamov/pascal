#include <string>
#include <iostream>
#include <fstream>
#include "lexer.h"
#include <map>
#include <stack>

using namespace std;

class CParser{
private:
	CLex Lex;
	CToken TOKEN;
	bool GotToken = false;
public:
	CParser()
	{
	}
	CParser(string FileName)
	{
	}

	void Parse()
	{
		Lex.LoadFile("OKcode.txt");
		GotToken = Lex.GetToken(TOKEN);
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
				Integer();
			}
			else { //if (TOKEN.TokenType == "word") {
				Lex.GetToken(TOKEN);
			}
			/*
			if (TOKEN.TokenType == "word") {
			Lex.GetToken(TOKEN);
			//	Integer();
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

	void Integer() {
		//do nothing for now
	}

	void stat()
	{
		//if
		if (TOKEN.TokenValue == "if")
		{
			GotToken = Lex.GetToken(TOKEN); //consumed "if"
			//call expression
			E();

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

		//word type
		
		if (TOKEN.TokenType == "word" && TOKEN.TokenValue != "end" && TOKEN.TokenValue != "else")
		{

			GotToken = Lex.GetToken(TOKEN);

			if (TOKEN.TokenValue == ":=") {
				GotToken = Lex.GetToken(TOKEN);
				//call expression
				E();
			}
			else {
				cout << "bad code(exit#12)" << endl;
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
			system("pause");
			exit(1);
		}
	}


	void variable()//variables on single line
	{
		//, means I have multiple variables, if : then it is over
		if (TOKEN.TokenValue == ",")
		{
			GotToken = Lex.GetToken(TOKEN);
			//there is some more variables which is always word
			if (TOKEN.TokenType == "word")
			{
				GotToken = Lex.GetToken(TOKEN);
				variable();
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


	void mv()//Mult. variables
	{
		//it must be word, but cannot be a keyword
		if (TOKEN.TokenType == "word" && TOKEN.TokenValue != "procedure" && TOKEN.TokenValue != "function" && TOKEN.TokenValue != "begin")
		{
			GotToken = Lex.GetToken(TOKEN);
			variable();
			
			if (TOKEN.TokenValue == ":") //no more variables
			{
				GotToken = Lex.GetToken(TOKEN);
				dtt();
				if (TOKEN.TokenValue == ";")
				{
					GotToken = Lex.GetToken(TOKEN);
					mv();
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
				cout << "Bad code" << endl;
				system("pause");
				exit(1);
			}
		}
		else //otherwise, it must be procedure or function
		{
			pfv();
		}
	}

	void Parameters()
	{
		if (TOKEN.TokenValue == "var")//Pass by reference
		{
			GotToken = Lex.GetToken(TOKEN);
			if (TOKEN.TokenType == "word")
			{
				GotToken = Lex.GetToken(TOKEN);
				variable();
				if (TOKEN.TokenValue == ":")
				{
					GotToken = Lex.GetToken(TOKEN);
					dtt();
					mp();
				}
			}
			else
			{
				cout << "Bad code" << endl;
				system("pause");
				exit(1);
			}
		}
		else if (TOKEN.TokenType == "word")//Pass by copy p1,p2,p3
		{
			GotToken = Lex.GetToken(TOKEN);
			variable();
			//end of the varables
			if (TOKEN.TokenValue == ":")
			{
				GotToken = Lex.GetToken(TOKEN);
				//I should expect the datatype after :
				dtt();
				mp();
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

	void mp()//Mult. parameters
	{
		//when i get multiple parameters, I seperate them by ;
		if (TOKEN.TokenValue == ";")//pass by copy
		{
			GotToken = Lex.GetToken(TOKEN);
			//the next parameter is variable
			if (TOKEN.TokenValue == "var")//pass by reference
			{
				GotToken = Lex.GetToken(TOKEN);
				//the variable name is always word
				if (TOKEN.TokenType == "word")
				{
					GotToken = Lex.GetToken(TOKEN);
					variable();
					//end of the variables
					if (TOKEN.TokenValue == ":")
					{
						GotToken = Lex.GetToken(TOKEN);
						//after : always comes datatype
						dtt();
						//there might be more parameters
						mp();
					}
					else
					{
						cout << "Bad code" << endl;
						system("pause");
						exit(1);
					}
				}
				else if (TOKEN.TokenType == "word")
				{
					GotToken = Lex.GetToken(TOKEN);
					variable();
					if (TOKEN.TokenValue == ":")
					{
						GotToken = Lex.GetToken(TOKEN);
						dtt();
						mp();
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
					cout << "Bad code" << endl;
					system("pause");
					exit(1);
				}
			}
		}
		else
		{
			return;
		}

	}

	void pfv()//Procedures, Functions, and Variables
	{
		if (GotToken) //else error
		{
			if (TOKEN.TokenValue == "var")//Variables else function or procedure
			{
				GotToken = Lex.GetToken(TOKEN);
				if (TOKEN.TokenType == "word") //else error
				{
					GotToken = Lex.GetToken(TOKEN);
					variable();
					if (TOKEN.TokenValue == ":") //else error
					{
						GotToken = Lex.GetToken(TOKEN);
						dtt();
						if (TOKEN.TokenValue == ";") //else error
						{
							GotToken = Lex.GetToken(TOKEN);
							mv();
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
						cout << "Bad code" << endl;
						system("pause");
						exit(1);
					}
				}
				else
				{
					cout << "Bad code" << endl;
					system("pause");
					exit(1);
				}

			}
			else if (TOKEN.TokenValue == "function")//Functions
			{
				GotToken = Lex.GetToken(TOKEN);
				if (TOKEN.TokenType == "word") //else error
				{
					GotToken = Lex.GetToken(TOKEN);
					//I need an ( after the name
					if (TOKEN.TokenValue == "(") //else error
					{
						GotToken = Lex.GetToken(TOKEN);
						//I always get the parameter after the (
						Parameters();
						if (TOKEN.TokenValue == ")") //else error
						{
							GotToken = Lex.GetToken(TOKEN);
							if (TOKEN.TokenValue == ":") //else error
							{
								GotToken = Lex.GetToken(TOKEN);
								dtt();
								if (TOKEN.TokenValue == ";") //else error
								{
									GotToken = Lex.GetToken(TOKEN);
									Block();
									if (TOKEN.TokenValue == ";") //else . else error
									{
										GotToken = Lex.GetToken(TOKEN);
									}
									//it just can be . and the end of the program
									else if (TOKEN.TokenValue == ".") {
										cout << "Good code" << endl;
										system("pause");
										exit(1);
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
									cout << "Bad code" << endl;
									system("pause");
									exit(1);
								}
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
							cout << "Bad code" << endl;
							system("pause");
							exit(1);
						}
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
					cout << "Bad code" << endl;
					system("pause");
					exit(1);
				}
			}
			else if (TOKEN.TokenValue == "procedure")//Procedures
			{
				GotToken = Lex.GetToken(TOKEN);
				//we expect a word because the name of the variable is always word
				if (TOKEN.TokenType == "word")
				{
					GotToken = Lex.GetToken(TOKEN);
					if (TOKEN.TokenValue == "(") //beginning of the param.
					{
						GotToken = Lex.GetToken(TOKEN);
						//after ( come the parameter
						Parameters();
						if (TOKEN.TokenValue == ")") //the end of the param.
						{
							GotToken = Lex.GetToken(TOKEN); //after ) I must get ;
							if (TOKEN.TokenValue == ";")
							{
								GotToken = Lex.GetToken(TOKEN);
								Block();
								if (TOKEN.TokenValue == ";")
								{
									GotToken = Lex.GetToken(TOKEN);
								}
								//It might just be the end of the whole thing
								else if (TOKEN.TokenValue == ".") {
									cout << "Good code" << endl;
									system("pause");
									exit(1);
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
								cout << "Bad code" << endl;
								system("pause");
								exit(1);
							}
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
						cout << "Bad code" << endl;
						system("pause");
						exit(1);
					}
				}
				else
				{
					cout << "Bad code" << endl;
					system("pause");
					exit(1);
				}
			}
		}
		else
		{
			return;
		}
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
	CToken t1;
	system("pause");
	return 0;
}