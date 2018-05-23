#include <string>
#include <iostream>
#include <fstream>
using namespace std;

class CToken{
public:
	string TokenValue;
	string TokenType;
	int size;
};

class CLex{
private:
	ifstream inCode, Lexer;
	int TwoDArray[14][128];
	int CurrentState;
	int PreviousState;
public:
	CLex(){ //constr

	}
	CToken t1;

	CLex(string FileName){

		inCode.open(FileName);
		Lexer.open("dfa2_new_tab.txt");
		if (Lexer.is_open()){

			while (!Lexer.eof()){

				for (int x = 0; x < 128; x++){

					for (int y = 0; y < 14; y++)
					{
						Lexer >> TwoDArray[y][x];
					}
				}
			}
		}
		else
		{
			cout << "file not found";
		}
		Lexer.close();
	}

	void LoadFile(string FileName){

		inCode.open(FileName);
		Lexer.open("dfa2_new_tab.txt");
		if (Lexer.is_open())
		{
			while (!Lexer.eof())
			{
				for (int x = 0; x < 128; x++)
				{
					for (int y = 0; y < 14; y++)
					{
						Lexer >> TwoDArray[y][x];
					}
				}
			}
		}
		else
		{
			cout << "file not found";
		}
		Lexer.close();
	}

	bool GetToken(CToken &t1)
	{
		CurrentState = 0;
		PreviousState = 0;
		t1.TokenValue = "";
		t1.TokenType = "";
		while ((inCode.peek() != -1) || (CurrentState != 0))
		{
			char TempChar = inCode.peek();
			if (inCode.peek() == -1)
			{
				TempChar = ' ';

			}
			PreviousState = CurrentState;
			CurrentState = TwoDArray[CurrentState][TempChar];
			if (CurrentState == 55)
			{
				if (PreviousState == 1)
				{
					t1.TokenType = "word";
				}
				if (PreviousState == 2)
				{
					t1.TokenType = "integer";
				}
				if (PreviousState == 4)
				{
					t1.TokenType = "real";
				}
				if (PreviousState == 8)
				{
					t1.TokenType = "real with an e.";
				}
				if (PreviousState == 10)
				{
					t1.TokenType = "special";
				}
				if (PreviousState == 11)
				{
					t1.TokenType = "special";
				}
				if (PreviousState == 13)
				{
					t1.TokenType = "special";
				}

				if (PreviousState == 12)
				{
					t1.TokenType = "column_equal";
				}
				return true;
			}
			if (CurrentState == 0)
			{
				inCode.get();
			}
			if (CurrentState == 99)
			{
				cout << "Bad Token" << endl;
				return false;
			}
			if ((CurrentState != 55) && (CurrentState != 99) && (CurrentState != 0))
			{
				t1.TokenValue += TempChar;
				inCode.get();
			}
		}
		return false;
	}

};