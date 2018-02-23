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
				Lex.GetToken(TOKEN); //get token

				TR();
				TPrime();
			}

			if (TOKEN.TokenValue == "or")
			{
				Lex.GetToken(TOKEN); //get token

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
				else
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
		//do nothing
	}

	void Integer() {
		//do nothing
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

	void Block()
	{
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