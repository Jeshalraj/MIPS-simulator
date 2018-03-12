// I HAVE NEITHER GIVEN NOR RECEIVED ANY UNAUTHORIZED AID ON THIS ASSIGNMENT
//NAME: JESHALRAJ R. THAKARIA
//UFID: 8321-3912
//COMPILE WITH g++ -std=c++0x MIPSsim.cpp -o MIPSsim

#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<vector>
#include<stdlib.h>
#include<algorithm>

using namespace std;

// GLOBAL STRUCTURES AND VARIABLES
struct buffer
{
	string operation;
	string dest;
	string source1;
	string source2;
	string result;
	int occupied;
};
struct buffer1 {
	string operation_1;
	string operation_2;
	string dest_1;
	string dest_2;
	string source1_1;
	string source1_2;
	string source2_1;
	string source2_2;
	string result_1;
	string result_2;
	int occupied_1;
	int occupied_2;
};
buffer INB, LIB, ADB,AIB;
buffer1 REB;
int insttoken = 16;
int timestep = 0;
int noop = 0;

//------------------------------------------------------------
// MIPS TRANSITION FUNCTIONS
//------------------------------------------------------------


void init()  // INITIALIZE THE SIMULATOR
{
	INB.occupied = 0;
	LIB.occupied = 0;
	ADB.occupied = 0;
	REB.occupied_1 = 0;
	REB.occupied_2 = 0;
	AIB.occupied = 0;
	
	INB.operation = "";
	LIB.operation = "";
	ADB.operation = "";
	REB.operation_1 = "";
	REB.operation_2 = "";
	AIB.operation = "";
	
	INB.dest = "";
	LIB.dest = "";
	ADB.dest = "";
	REB.dest_1 = "";
	REB.dest_2 = "";
	AIB.dest = "";

	INB.source1 = "";
	LIB.source1 = "";
	ADB.source1 = "";
	REB.source1_1 = "";
	REB.source1_2 = "";
	AIB.source1 = "";

	INB.source2 = "";
	LIB.source2 = "";
	ADB.source2 = "";
	REB.source2_1 = "";
	REB.source2_2 = "";
	AIB.source2 = "";

	INB.result = "";
	LIB.result = "";
	ADB.result = "";
	REB.result_1 = "";
	REB.result_2 = "";
	AIB.result = "";

	return;
}

bool sortcol(const vector<string>&a, const vector<string> &b)
{
	return a[0] < b[0];
}

void Decode(const vector<vector<string> > &ins) {   // EXECUTE THE DECODE FUNCTION AND CHECK INPUT INSTRUCTIONS

	if (insttoken > 0 && INB.occupied == 0)
	{
		INB.operation = ins[16 - insttoken][0];
		INB.dest = ins[16 - insttoken][1];
		INB.source1 = ins[16 - insttoken][2];
		INB.source2 = ins[16 - insttoken][3];
		INB.occupied = 1;
		insttoken--;
	}
	return;
}

void Read(const vector<vector<string> > &reg) {   // READ THE REGISTER VALUES FROM REGISTER FILE IN THE INB BUFFER

	if (INB.occupied == 1)
	{
		for (int i = 0;i < 8; i++)
		{
			if (INB.source1 == reg[i][0])
			{
				INB.source1 = reg[i][1];
			}
			if (INB.source2 == reg[i][0])
			{
				INB.source2 = reg[i][1];
			}
		}
	}

	return;
}

void Issue1()	// CHECK FOR TOKEN AND PASS THE VALUES TO AIB BUFFER					
{
	if (AIB.occupied == 0 && INB.occupied == 1)
	{
		AIB.occupied = 1;
		AIB.operation = INB.operation;
		AIB.dest = INB.dest;
		//cout << INB.dest<<endl;
		AIB.source1 = INB.source1;
		AIB.source2 = INB.source2;
		INB.occupied = 0;
	}
	return;
}

void Issue2()   // CHECK IF INB IS OCCUPIED AND PASS THE VALUES TO IB BUFFER
{
	if (LIB.occupied == 0 && INB.occupied == 1)
	{
		LIB.occupied = 1;
		LIB.operation = INB.operation;
		LIB.dest = INB.dest;
		//cout << LIB.dest<<endl;
		LIB.source1 = INB.source1;
		LIB.source2 = INB.source2;
		INB.occupied = 0;
	}
	return;
}

void Alu()    // DEPENDING ON THE INPUT OPERATION PERFORM ALU OPERATIONS ON THE SOURCES
{
	if (AIB.occupied == 1 && AIB.operation!="")
	{
		int x = 0, y = 0, res = 0;
		if(AIB.source1!=""){
		switch (AIB.operation[1])
		{
		case 'D':  x = stoi(AIB.source1); y = stoi(AIB.source2); res = x + y; AIB.result = to_string(res); break;
		case 'U':  x = stoi(AIB.source1); y = stoi(AIB.source2); res = x - y; AIB.result = to_string(res); break;
		case 'N':  x = stoi(AIB.source1); y = stoi(AIB.source2); res = x & y; AIB.result = to_string(res); break;
		case 'R':   x = stoi(AIB.source1); y = stoi(AIB.source2); res = x | y; AIB.result = to_string(res); break;
		}
		}
		else{
		return;		
}
		if (REB.occupied_1 == 0 )
		{
			REB.dest_1 = AIB.dest;
			REB.result_1 = AIB.result;
			REB.occupied_1 =1;
			AIB.occupied = 0;
		}
		else if (REB.occupied_2 == 0)
		{
			REB.dest_2 = AIB.dest;
			REB.result_2 = AIB.result;
			REB.occupied_2 = 1;
			AIB.occupied = 0;
		}
	}
	return;
}

void Addr()  // GET THE LOAD ADDRESS FOR DATA MEMORY FROM INPUT SOURCES
{
	if (LIB.occupied == 1)
	{
		LIB.result = to_string(stoi(LIB.source1) + stoi(LIB.source2));
	}
	if (ADB.occupied == 0)
	{
		ADB.dest = LIB.dest;
		ADB.result = LIB.result;
		ADB.occupied = 1;
		LIB.occupied = 0;
	}
	return;
}

void Load(const vector<vector<string> > &datamemory)  // LOAD THE VALUE FROM DATAMEMORY FROM ADDRESS CALCULATED IN ADDR STAGE
{
	if (ADB.occupied == 1)
	{
		for (int i = 0; i < 8;i++)
		{
			if (ADB.result == datamemory[i][0])
			{
				cout << "-------0--------::" << datamemory[i][0]<<endl;
				cout << "-------1--------::" << datamemory[i][1]<<endl;
				ADB.result = datamemory[i][1];
				cout << "-----adb result----::" << ADB.result << endl;
				break;
			}
		}
		if (REB.occupied_1 == 0)
		{
			cout << "-----REB1-----result::" << ADB.result << endl;
			REB.result_1 = ADB.result;
			REB.dest_1 = ADB.dest;
			REB.occupied_1= 1;
			ADB.occupied = 0;
		}
		else if (REB.occupied_2 == 0)
		{
			cout << "-----REB1-----result::" << ADB.result << endl;
			REB.result_2 = ADB.result;
			REB.dest_2 = ADB.dest;
			REB.occupied_2 = 1;
			ADB.occupied = 0;
		}
	}
	return;
}

void Write(vector<vector<string> > &reg)  // WRITE BACK THE RESULTS IN REGISTER FILE
{
	if (REB.occupied_1 == 1)
	{
		for (int i = 0; i < 8; i++)
		{
			if (REB.dest_1 == reg[i][0])
			{
				reg[i][1] = REB.result_1;
			}
		}
		REB.occupied_1 = 0;
		noop--;
	}
	else if (REB.occupied_2 == 1)
	{
		for (int i = 0; i < 8; i++)
		{
			if (REB.dest_2 == reg[i][0])
			{
				reg[i][1] = REB.result_2;
			}
		}
		REB.occupied_2 = 0;
		noop--;
	}
	return;
}

void parseIns(ifstream &file, int rows, int column, vector<vector<string>> &t1)  // PARSER FOR TAKING OUT INFORMATION FROM INPUT FILE
{
	vector<string> t(column, "");
	t1.resize(rows, t);
	stringstream ss;
	string s, temp;
	char c;
	int i = 0, j = 0, len = 0;
	while(!file.is_open());
	
	while (!file.eof())
	{
		getline(file, s);
		ss.str(s);
		ss >> c;
		j = 0;
		while (!ss.eof()) {
			getline(ss, temp, ',');
			if (temp.find('>') != string::npos)
			{
				temp = temp.substr(0, temp.find('>'));
					
			}
			t1[i][j] = temp;
			j++;
		}
		ss.str("");
		ss.clear();
		i++;
	}
	return;
}

void Petrinet(vector<vector <string>> &ins, buffer &INB, buffer &LIB, buffer &ADB, buffer1 &REB, buffer &AIB, vector<vector<string>> &datamemory, vector<vector<string>> &reg) // PETRINET IMPLEMENTAION, MAIN SIMULATOR FUNCTIONALITY IS IMPLEMENTED IN THIS FUNCTION
{
	int dec_t = 0, inb_t = 0, lib_t = 0, adb_t = 0, reb_t = 0, aib_t = 0;
	if (insttoken>0)
	{
		dec_t = 1;
	}
	if (INB.occupied == 1)
	{
		inb_t = 1;
	}
	if (LIB.occupied == 1)
	{
		lib_t = 1;
	}
	if (ADB.occupied == 1)
	{
		adb_t = 1;
	}
	if (REB.occupied_1 == 1 || REB.occupied_2==1)
	{
		reb_t = 1;
	}
	if (AIB.occupied == 1)
	{
		aib_t = 1;
	}
	if (reb_t == 1)
	{
		Write(reg);
	}
	if (adb_t == 1)
	{
		Load(datamemory);
	}
	if (aib_t == 1)
	{
		Alu();
	}
	if (lib_t == 1)
	{
		Addr();
	}
	if (inb_t == 1)
	{
		if (INB.operation == "LD")
		{
			Issue2();
		}
		else
		{
			Issue1();
		}
	}
	if (dec_t == 1)
	{
		Decode(ins);
		Read(reg);
	}
	return ;
}

void print_step(ofstream &sim,vector<vector<string>> &ins, vector<vector<string>> &datamemory, vector<vector<string>> &reg) // PRINT THE STEPS
{
	sim << "STEP " << timestep << ":" << endl;
	sim << "INM:";
	for (int i = timestep; i < 16;i++)
	{
		int brk = 0;
		for (int j = 0; j < 4;j++)
		{
			if (ins[i][j] == "")
			{
				sim <<" ";
				brk = 1;
				break;
			}
			if (j == 0)
			{
				if(i==timestep){ sim << "<" << ins[i][j] << ","; }
				else { sim << ",<" << ins[i][j] << ","; }
			}
			else if (j == 3)
			{
				sim << ins[i][j] << ">";
			}
			else
			{
				sim << ins[i][j] << ",";
			}

		}
		if(brk==1)
		{
			break;
		}
	}
	if(INB.occupied==1 && INB.operation!="")
	{
		sim << endl <<"INB:" << "<" << INB.operation << "," << INB.dest << "," << INB.source1 << "," << INB.source2 << ">" << endl;
	}
	else
	{
		sim << endl <<"INB:" << endl;
	}
	if(AIB.occupied==1 && AIB.operation!="")
	{
		sim << "AIB:" << "<" << AIB.operation << "," << AIB.dest << "," << AIB.source1 << "," << AIB.source2 << ">" << endl;
	}
	else
	{
		sim << "AIB:" << endl;
	}
	if(LIB.occupied==1 && LIB.dest!="")
	{
		sim << "LIB:" << "<" << LIB.operation << "," << LIB.dest << "," << LIB.source1 << "," << LIB.source2 << ">" << endl;
	}
	else
	{
		sim << "LIB:" << endl;
	}
	if(ADB.occupied==1 && ADB.dest!="")
	{
		sim << "ADB:" << "<" << ADB.dest << "," << ADB.result << ">" << endl;
	}
	else
	{
		sim << "ADB:" << endl;
	}
	if (REB.occupied_1 == 1)
	{
		sim << "REB:<" << REB.dest_1 << "," << REB.result_1 << ">";
		if (REB.occupied_2 == 1)
		{
			sim << ",<" << REB.dest_2 << "," << REB.result_2 << ">" << endl;
		}
		else
		{
			sim << endl;
		}
	}
	else if (REB.occupied_2 == 1)
	{
		sim << "REB:<" << REB.dest_2 << "," << REB.result_2 << ">" << endl;
	}
	else
	{
		sim << "REB:" << endl;
	}

	sort(reg.begin(), reg.end(), sortcol);
	sim << "RGF:";
	for (int i = 0; i < 8;i++)
	{
		for (int j = 0; j < 2;j++)
		{
			if (j == 0)
			{
				if (i == 0) { sim << "<" << reg[i][j] << ","; }
				else { sim << ",<" << reg[i][j] << ","; }
			}
			else
			{
				sim << reg[i][j] << ">";
			}
		}
		if(i==7)
		{
			sim << endl;
		}
	}
	sort(datamemory.begin(), datamemory.end(), sortcol);
	sim << "DAM:";
	for (int i = 0; i < 8;i++)
	{
		for (int j = 0; j < 2;j++)
		{
			if (j == 0)
			{
				if (i == 0) { sim << "<" << datamemory[i][j] << ","; }
				else { sim << ",<" << datamemory[i][j] << ","; }
			}
			else
			{
				sim << datamemory[i][j] << ">";
			}
		}
		if(i==7)
		{
			sim << endl;
		}
	}
	sim << endl;

}
//-------------------------------------------------------
//MAIN FUNCTION IMPLEMENTATION
//-------------------------------------------------------

int main()
{
	// FILE OBJECTS CREATION
	ifstream inst,regis,dtmem;
	ofstream sim;
	sim.open("simulation.txt");
	inst.open("instructions.txt");
	regis.open("registers.txt");
	dtmem.open("datamemory.txt");
	vector<vector<string> > ins,reg,datamemory;

	//PARSE THE FILES
	parseIns(inst, 16, 4, ins);
	parseIns(regis, 8, 2, reg);
	parseIns(dtmem, 8, 2, datamemory);
	init();

	// CHECK THE NUMBER OF INPUT INSTRCTIONS
	int  j = 0;
    for(int i=0;i<16;i++)
	{
		if(ins[i][j]!="")
		{
			noop++;
		}
	}
	print_step(sim,ins, datamemory, reg);
	timestep++;

	// WHILE THE NUMBER OF INSTRUCTIONS ARE NOT PROCESSED KEEP RUNNING SIMULATOR
	while (noop>0)
	{
		Petrinet(ins, INB, LIB, ADB, REB, AIB, datamemory, reg);
		print_step(sim,ins, datamemory, reg);
		timestep++;
	}
	return 0;
}
