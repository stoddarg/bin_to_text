// bin_to_txt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

#define INPUT_SIZE	110160

using namespace std;

int main()
{
	int ii = 0;
	//int inputSize = 0;
	//int arraySize = 0;
	string input = "";
	string binfile = "";
	string txtfile = "";

	struct event_processed {
		double time;
		double total_events;
		double event_num;
		double bl;
		double si;
		double li;
		double fi;
		double psd;
		double energy;
	} trial_array[INPUT_SIZE/9] = {};
	
	//event_processed * trial_array;
	//trial_array = (calloc(INPUT_SIZE / 9, sizeof(event_processed));

	/* I want to read in the filename that the user would like to convert from .bin -> .txt 
	* Use getline to get user input, check the input for no bad characters, append .bin to the end of it
	* then place that string into the fstream function below.
	*/
	cout << "Before entering files to convert, place them in: \n";
	cout << "K:\\users\\GStoddard\\Miscellany\\bin_to_txt\\bin_to_txt\n";
	while (true)
	{
		cout << "Enter a binary file (without .bin extension) to convert: \n";		// Get the input file that should be converted
		getline(cin, input);
		binfile = input + ".bin";
		//cout << "What is the size of this file in bytes?\n";
		//cin >> inputSize;
		//arraySize = inputSize / (8 * 9);					// Divide the size by 9 doubles, each at 8 bytes

		//event_processed *trial_array;
		//trial_array = new event_processed[arraySize];

		fstream mxbinfile(binfile, ios::in | ios::binary);	// Open stream for reading
		if (mxbinfile.is_open()) {							// Check to make sure the file is open
			mxbinfile.read((char *)&trial_array, INPUT_SIZE);	// Read in all of the information from the binary file	// Change the number to read in a different size file	//587520
			mxbinfile.close();
		}

		ofstream ascii_file;								// Declare a file to write to

		txtfile = input + ".txt";
		ascii_file.open(txtfile, ios::app);					// Open with append flag, in case it is already there, so we don't lose info
		if (ascii_file.is_open()) {
			ascii_file << setw(6) << left << "Time (us)\t"	// Write the header here
				<< setw(6) << left << "Tot Evts\t"
				<< setw(16) << left << "Evt Num\t"
				<< setw(6) << left << "Base Line\t"
				<< setw(6) << left << "Short Int\t"
				<< setw(6) << left << "Long Int\t"
				<< setw(6) << left << "Full Int\t"
				<< setw(6) << left << "PSD     \t"
				<< setw(6) << left << "Energy  \t"
				<< "\n";

			for (ii = 0; ii < INPUT_SIZE/9; ++ii) {					// Loop over the trial array to put the data into the .txt file in a formatted way //8159
				ascii_file << setw(10) << left << trial_array[ii].time << "\t"
					<< setw(10) << left << trial_array[ii].total_events << "\t"
					<< setw(10) << left << trial_array[ii].event_num << "\t" << "\t"
					<< setw(10) << left << trial_array[ii].bl << "\t"
					<< setw(10) << left << trial_array[ii].si << "\t"
					<< setw(10) << left << trial_array[ii].li << "\t"
					<< setw(10) << left << trial_array[ii].fi << "\t"
					<< setw(10) << left << trial_array[ii].psd << "\t"
					<< setw(10) << left << trial_array[ii].energy << "\t"
					<< "\n";
			}
			ascii_file.close();								// Make sure to close the file
		}
		else
		{
			cout << "Unable to open file: '" << input << "'\n";
			return 0;
		}
		while (true)
		{
			cout << "Are they any more files to convert? (y/n) \n";
			getline(cin, input);
			if (input == "y")
			{
				cout << "Got it. ";
				break;
			}
			if (input == "n")
			{
				cout << "File conversion complete.\n";
				getline(cin, input);
				return 0;
			}
			if (input != "n" && input != "y")
			{
				cout << "Please enter y/n. \n";
			}
		}
	}
	return 0;
}

