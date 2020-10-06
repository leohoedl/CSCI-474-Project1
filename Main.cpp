/****************************************
*           CSCI 474 Project 1          *
*               Leo Hoedl               *
*             October 7, 2019           *
*****************************************/

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <stdio.h>
#include <unistd.h>


using namespace std;

/*********************************************************************************************************/
/*Functions*/
/*Function to Select File from 4 Given .Dat Files*/
string FileSelection() {
	int fileNum;
	bool validFile = false;

	while (!validFile)
	{
		cout << "\n\nPlease select a file: File 1, 2, 3, or 4" << endl;
		cin >> fileNum;

		if (cin.good() && (fileNum == 1 || fileNum == 2 || fileNum == 3 || fileNum == 4))
		{
			/*Valid user input. Moves onto next part of method.*/
			validFile = true;
		}
		else
		{
			/*Invalid user input. Clears error flag and removes input contents that caused error.*/
			cin.clear();
			cin.ignore();
		}
	}
	/*Selects file based on previous input and returns it to main method.*/
	if (fileNum == 1)
	{
		return "file1.dat";
	}
	else if (fileNum == 2)
	{
		return "file2.dat";
	}
	else if (fileNum == 3)
	{
		return "file3.dat";
	}
	else if (fileNum == 4)
	{
		return "file4.dat";
	}
	/*No need for another else statment. cin.clear() clears error*/
}


/*Function to take user input to determine how many child processes are needed.*/
int ChildProcessSelection() {
	bool validNum = false;
	int childNum;

	while (!validNum)
	{
		cout << "1, 2, or 4 child processes?" << endl;
		cin >> childNum;

		if (cin.good() && (childNum == 1 || childNum == 2 || childNum == 4))
		{
			/*Sets validNum to true if user input is valid and equal to 1,2,or 4.*/
			validNum = true;
		}
		else
		{
			/*Invalid user input. Clears error flag and removes input contents that caused error.*/
			cin.clear();
			cin.ignore();
		}
	}

	return childNum;

}
/*******************************************************************************************************/



/******************************************************************************************************/
/*Main Simulation*/
int main() {

	/*User input for file selection*/
	string datFile = FileSelection();
	ifstream File(datFile);
 
	/*Find number of child processes with premade method*/
	int childProcessesNum = ChildProcessSelection();
	int fds[childProcessesNum][2];
	for (int i = 0;i < childProcessesNum;i++) 
	{
		pipe(fds[i]);
	}

	/*Timer Start*/
	auto start = chrono::system_clock::now();

	/*Needs to make sure that File is open*/
	if (File.is_open()) 
	{
		cout << "File is open" << endl;
		
		/*Create child processes*/
		File.seekg(0, ifstream::end);
		int fileSize = File.tellg();
		File.seekg(0, ifstream::beg);
		char * buffer = new char[fileSize];
		/*Code adapted from stackoverflow question: https://stackoverflow.com/questions/22984956/tellg-function-give-wrong-size-of-file */

		/*Chunk size calculation*/
		int chunkSize = fileSize / childProcessesNum;
		cout << "Size of Chunk: " << chunkSize << endl;

		/*Reading File*/
		File.read(buffer, fileSize);

		/*Creating all the forks*/
		for (int i = 0; i < childProcessesNum; i++) {
			cout << "Creating Child Process " << i+1 << endl;
			/*For creation*/
			if (fork() == 0) 
			{
				/*https://gist.github.com/zed/7437668 used as a reference for parallel summations*/
				int temp = 0;
				
				if (i == 0) 
				{
					for (int n = 0; n < chunkSize; n++) {
						temp += buffer[n];
					}
				}
				if (i == 1) 
				{
					for (int n = chunkSize; n < (chunkSize * 2); n++) {
						temp += buffer[n];
					}
				}
				if (i == 2) 
				{
					for (int n = (chunkSize * 2); n < (chunkSize * 3); n++) {
						temp += buffer[n];
					}
				}
				if (i == 3) 
				{
					for (int n = (chunkSize * 3); n < (chunkSize * 4); n++) {
						temp += buffer[n];
					}
				}

				/*Write back to pipe*/
				write(fds[i][1], &temp, sizeof(int));
				exit(0);
			}
		}

		File.close();
		cout << "File has been closed" << endl; 
		
		/*Calculating all sums*/
		int finalResult[childProcessesNum];
		for (int i = 0; i < childProcessesNum; i++) 
		{
			read(fds[i][0], &finalResult[i], sizeof(int));
		}

		int total=0;

		for (int i = 0; i < childProcessesNum; i++)
		{
			total = total + finalResult[i];
		}

		/*Display Total*/
		cout << "Total: " << total << endl;

		/*Timer ends*/
		auto end = chrono::system_clock::now();
		chrono::duration<double> seconds = end - start;
		cout << "Elapsed Time: " << seconds.count() << " seconds" << endl;
	}
	else 
	{
		cout << "File could not be found." << endl;
	}

	return 0;

}









