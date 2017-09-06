// bin_to_txt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

#define BUFFER_SIZE	12288

struct EventData
{
	unsigned int aaTotalEvents;	// AA stands for adjacent average
	unsigned int aaEventNumber;	// should be matched across all integrators
	double aaBaselineInt;
	double aaShortInt;
	double aaLongInt;
	double aaFullInt;
	unsigned int aaEight;		// placeholder
	unsigned int lpfEventNumber;	// LPF stands for Low Pass Filter
	unsigned int lpfTTLSignal;	// PNG signal in
	double lpfBaselineInt;
	double lpfShortInt;
	double lpfFullInt;
	unsigned int lpfSeven;		// placeholder
	unsigned int lpfEight;		// placeholder
	double dffTimeSmall;	// DFF stands for Differential filter	//timesmall+timebig = time in seconds
	double dffTimeBig;		//small * 128e-9 + big * 549.7558139 = time (s)
	unsigned int dffEventNumber;
	double dffBaselineInt;
	double dffShortInt;
	unsigned int dffSeven;		// placeholder
	unsigned int dffEight;		// placeholder
};

using namespace std;

int main()
{
	int ii = 0;
	int eventIndex = 0;
	//int inputSize = 0;
	//int arraySize = 0;
	string input = "";
	string binfile = "";
	string txtfile = "";
	unsigned int data_array[BUFFER_SIZE] = {};
	EventData eventsSorted[512] = {};

	//sorting variables
	double aablavgArray[512] = {};
	double bl1(0);	double bl2(0); double bl3(0); double bl4(0); double bl_avg(0);
	double si(0); double li(0); double fi(0);
	double psd(0);
	double energy(0);

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

		fstream mxbinfile; // (binfile, ios::in | ios::binary);	// Open stream for reading

		txtfile = input + ".txt";
		mxbinfile.open(binfile, ios::in | ios::binary);
		std::ofstream outputFile_Erik;
		outputFile_Erik.open(txtfile, std::ios::app);

		while (outputFile_Erik.is_open() && mxbinfile.is_open()) 
		{
			ii = 0;
			eventIndex = 0;
			//read in a chunk from the binary file, find the first instance of the identifier
			
			mxbinfile.read((char *)&data_array, 49152);	//12288 * 4 = 49152 //buffer size * 4 bytes/int = bytes to read in a full buffer
			if (!mxbinfile)
				break;
			while (1)
			{
				if (data_array[ii] == 111111)
				{
					ii++;
					break;
				}
				else if (ii > BUFFER_SIZE)	//catch if we accidentally have no data
					break;
				ii++;
			}
			//we want ii to be maintained, it tells us where to begin sorting 
			while (ii < BUFFER_SIZE)	//for sorting data //comment while verifying that we get data
			{
				switch (data_array[ii])
				{
				case 111111:
					//process the AA integrator data
					eventsSorted[eventIndex].aaTotalEvents = data_array[ii + 1];
					eventsSorted[eventIndex].aaEventNumber = data_array[ii + 2];
					eventsSorted[eventIndex].aaBaselineInt = data_array[ii + 3];
					eventsSorted[eventIndex].aaShortInt = data_array[ii + 4];
					eventsSorted[eventIndex].aaLongInt = data_array[ii + 5];
					eventsSorted[eventIndex].aaFullInt = data_array[ii + 6];
					eventsSorted[eventIndex].aaEight = data_array[ii + 7];
					ii += 8;
					eventIndex++;	//move to the next event in the struct
					break;
				case 121212:
					//process the LPF data
					eventsSorted[eventIndex].lpfEventNumber = data_array[ii + 1];
					eventsSorted[eventIndex].lpfTTLSignal = data_array[ii + 2];
					eventsSorted[eventIndex].lpfBaselineInt = data_array[ii + 3];
					eventsSorted[eventIndex].lpfShortInt = data_array[ii + 4];
					eventsSorted[eventIndex].lpfFullInt = data_array[ii + 5];
					eventsSorted[eventIndex].lpfSeven = data_array[ii + 6];
					eventsSorted[eventIndex].lpfEight = data_array[ii + 7];
					ii += 8;
					eventIndex++;
					break;
				case 131313:
					//process the LPF data
					eventsSorted[eventIndex].dffTimeSmall = data_array[ii + 1];
					eventsSorted[eventIndex].dffTimeBig = data_array[ii + 2];
					eventsSorted[eventIndex].dffEventNumber = data_array[ii + 3];
					eventsSorted[eventIndex].dffBaselineInt = data_array[ii + 4];
					eventsSorted[eventIndex].dffShortInt = data_array[ii + 5];
					eventsSorted[eventIndex].dffSeven = data_array[ii + 6];
					eventsSorted[eventIndex].dffEight = data_array[ii + 7];
					ii += 8;
					eventIndex++;
					break;
				default:
					//if we hit this, we didn't find an identifier or the array is finished
					ii++;	//check the next entry
					break;
				}

				if (eventIndex > 511)	//if we are at the bottom, reset for the next parts of the struct
					eventIndex = 0;
				if (ii > 12280)	//if we are near the top, in the last event, jump out (it's garbage)
					break;
			}

			eventIndex = 0;	//reset this value
			while (eventIndex < 512)	//plots the charts for each event
			{
				//reset variables
				si = 0; li = 0;	fi = 0;	psd = 0; energy = 0;

				bl4 = bl3; bl3 = bl2; bl2 = bl1;
				bl1 = eventsSorted[eventIndex].aaBaselineInt / (16.0 * 38.0);
				if (bl4 == 0.0)
					bl_avg = bl1;
				else
					bl_avg = (bl4 + bl3 + bl2 + bl1) / 4.0;
				aablavgArray[eventIndex] = bl_avg;
				si = eventsSorted[eventIndex].aaShortInt / 16.0 - (bl_avg * 73.0);
				li = eventsSorted[eventIndex].aaLongInt / 16.0 - (bl_avg * 169.0);
				fi = eventsSorted[eventIndex].aaFullInt / 16.0 - (bl_avg * 1551.0);
				if (si > 0 && li > 0)
					psd = si / (li - si);
				energy = 1.0 * fi + 0.0;

				eventIndex++;
			}

			eventIndex = 0;	//reset again
			for (eventIndex = 0; eventIndex < 511; eventIndex++)
			{
				outputFile_Erik << std::setw(12) << eventsSorted[eventIndex].aaEventNumber << '\t'
					<< std::setw(12) << eventsSorted[eventIndex].aaTotalEvents << '\t'
					<< std::setw(11) << eventsSorted[eventIndex].lpfTTLSignal << '\t'
					<< std::setw(16) << ((eventsSorted[eventIndex].dffTimeSmall * 128.0e-9) + (eventsSorted[eventIndex].dffTimeBig * 549.7558139)) << '\t'
					<< std::setw(11) << (eventsSorted[eventIndex].aaBaselineInt) << '\t'
					<< std::setw(11) << (eventsSorted[eventIndex].aaShortInt / 16.0 - aablavgArray[eventIndex] * 73.0) << '\t'
					<< std::setw(11) << (eventsSorted[eventIndex].aaLongInt / 16.0 - aablavgArray[eventIndex] * 169.0) << '\t'
					<< std::setw(11) << (eventsSorted[eventIndex].aaFullInt / 16.0 - aablavgArray[eventIndex] * 1551.0) << '\t'
					<< std::setw(11) << (eventsSorted[eventIndex].lpfBaselineInt / 16.0) << '\t'
					<< std::setw(11) << (eventsSorted[eventIndex].lpfShortInt / 16.0) << '\t'
					<< std::setw(11) << (eventsSorted[eventIndex].lpfFullInt / 16.0) << '\t'
					<< std::setw(11) << (eventsSorted[eventIndex].dffBaselineInt / 16.0) << '\t'
					<< std::setw(11) << (eventsSorted[eventIndex].dffShortInt / 16.0) << '\t'
					<< std::endl;
			}
		}
		outputFile_Erik.close();
		mxbinfile.close();
		/*else
		{
			cout << "Unable to open file: '" << input << "'\n";
			return 0;
		}*/
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

