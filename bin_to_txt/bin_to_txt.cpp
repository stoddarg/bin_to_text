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
	int firstAlignedEvent = 0;
	//int inputSize = 0;
	//int arraySize = 0;
	string input = "";
	string infile = "";
	string outfile = "";
	unsigned int data_array[BUFFER_SIZE] = {};
	EventData eventsSorted[512] = {};
	unsigned int FirstEvent = 0;
	/*char begintime[10] = {};
	int begintimehour = 0;
	int begintimemin = 0;
	int localtime_us = 0; */
	double aablavgArray[512] = {};	//sorting variables
	double bl1(0);	double bl2(0); double bl3(0); double bl4(0); double bl_avg(0);
	int lineWriteCounter = 0;

	fstream inputfileStream;
	ofstream outputFileStream;

	//sorting variables
	

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
		infile = input + ".bin";
		outfile = input + " v2.txt";
		cout << "New file will be named: " + outfile + "\n";

		cout << "Enter the first event registered for this run: \n";
		cout << "A zero for this value will write all events.\n";
		cin >> FirstEvent;
		//localTime_us = (beginTimeHour * 360 + beginTimeMin * 60) * 10 ^ 6;	//puts the entered time into microseconds

		inputfileStream.open(infile, ios::in | ios::binary);
		outputFileStream.open(outfile, std::ios::app);

		//write a header to the output file
		outputFileStream << "Total Events" << '\t'
			<< "Event Number" << '\t'
			<< "TTL Signal" << '\t'
			<< "Time (s)" << '\t'
			<< "AA Baseline Int" << '\t'
			<< "AA Short Int" << '\t'
			<< "AA Long Int" << '\t'
			<< "AA Full Int" << '\t'
			<< "LPF Basline Int" << '\t'
			<< "LPF Short Int" << '\t'
			<< "LPF Full Int" << '\t'
			<< "DFF Baseline Int" << '\t'
			<< "DFF Short Int" << '\t'
			<< std::endl;

		while (outputFileStream.is_open() && inputfileStream.is_open())
		{
			ii = 0;
			eventIndex = 0;
			//read in a chunk from the binary file, find the first instance of the identifier
			
			inputfileStream.read((char *)&data_array, 49152);	//12288 * 4 = 49152 //buffer size * 4 bytes/int = bytes to read in a full buffer
			if (!inputfileStream)
				break;
			while (1)
			{
				if (data_array[ii] == 111111)
				{
					//sometimes there is a second 111111, sometimes there is not
					if (data_array[ii+1] == 111111 && data_array[ii+2] > 0 && data_array[ii+7] == 8)	//if the following number is also 111111, move over it //check there is a value for total events and an 8 placeholder
					{
						ii++;	//there is a second identifier which follows the first one
						firstAlignedEvent = data_array[ii + 2];
						break;
					}
					else if (data_array[ii + 7] == 8 && data_array[ii + 1] > 0) //if it is not, see if there is a full event and read normally //check there is an 8 placeholder and a total events value above 0
					{
						firstAlignedEvent = data_array[ii + 2];
						break;
					}
					//if neither case is satisfied, then leave the if and find the next identifier
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
					eventIndex = data_array[ii + 2] - firstAlignedEvent;	//check to see if we have the correct event
					if (eventIndex < 0)		//if the eventIndex < 0 then we must skip it (otherwise errors) 
					{
						ii += 8;	//these events are not inline, skip them
						break;		//go to the next event
					}
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
					eventIndex = data_array[ii + 1] - firstAlignedEvent;	//check to see if we have the correct event
					if (eventIndex < 0)		//if the eventIndex < 0 then we must skip it (otherwise errors) 
					{
						ii += 8;	//these events are not inline, skip them
						break;		//go to the next event
					}
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
					eventIndex = data_array[ii + 3] - firstAlignedEvent;
					if (eventIndex < 0)
					{
						ii += 8;
						break;
					}
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

				if (ii > 12280)	//if we are near the top, in the last event, jump out (it's garbage)
					break;
			}

			//eventIndex = 0;	//reset this value
			//while (eventIndex < 512)	//plots the charts for each event
			//{
			//	bl4 = bl3; bl3 = bl2; bl2 = bl1;
			//	bl1 = eventsSorted[eventIndex].aaBaselineInt / (16.0 * 38.0);
			//	if (bl4 == 0.0)
			//		bl_avg = bl1;
			//	else
			//		bl_avg = (bl4 + bl3 + bl2 + bl1) / 4.0;
			//	aablavgArray[eventIndex] = bl_avg;

			//	eventIndex++;
			//}

			eventIndex = 0;	//reset again
			for (eventIndex = 0; eventIndex < 511; eventIndex++)	//Add in a check on dffTimeBig when times are small initially
			{
				if (eventsSorted[eventIndex].aaTotalEvents == 0 || eventsSorted[eventIndex].aaEventNumber == 0	|| eventsSorted[eventIndex].aaBaselineInt == 0	|| 
					eventsSorted[eventIndex].aaShortInt == 0	|| eventsSorted[eventIndex].aaLongInt == 0		|| eventsSorted[eventIndex].aaFullInt == 0		|| 
					eventsSorted[eventIndex].lpfEventNumber == 0|| eventsSorted[eventIndex].lpfBaselineInt == 0 || eventsSorted[eventIndex].lpfFullInt == 0		|| 
					eventsSorted[eventIndex].dffTimeSmall == 0	|| eventsSorted[eventIndex].dffEventNumber == 0	|| eventsSorted[eventIndex].dffShortInt == 0)
					continue;
				if (eventsSorted[eventIndex].aaEventNumber < FirstEvent)	//make sure we don't write events before the "first"
					continue;
				
				outputFileStream << std::setw(12) << eventsSorted[eventIndex].aaTotalEvents << '\t'
					<< eventsSorted[eventIndex].aaEventNumber << '\t'
					<< eventsSorted[eventIndex].lpfTTLSignal << '\t'
					<< std::setprecision(16) << (eventsSorted[eventIndex].dffTimeSmall * 128.0e-9) + (eventsSorted[eventIndex].dffTimeBig * 549.7558139) << '\t'
					<< eventsSorted[eventIndex].aaBaselineInt / 16.0 << '\t'
					<< eventsSorted[eventIndex].aaShortInt / 16.0 << '\t'
					<< eventsSorted[eventIndex].aaLongInt / 16.0 << '\t'
					<< eventsSorted[eventIndex].aaFullInt / 16.0 << '\t'
					<< eventsSorted[eventIndex].lpfBaselineInt / 16.0 << '\t'
					<< eventsSorted[eventIndex].lpfShortInt / 16.0 << '\t'
					<< eventsSorted[eventIndex].lpfFullInt / 16.0 << '\t'
					<< eventsSorted[eventIndex].dffBaselineInt / 16.0 << '\t'
					<< eventsSorted[eventIndex].dffShortInt / 16.0 << '\t'
					<< std::endl;
			}

			//if ((++lineWriteCounter) > 100)	//use this is to convert a small portion of the binary file
			//	break;
		}
		outputFileStream.close();
		inputfileStream.close();

		while (true)
		{
			getline(cin, input);
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

