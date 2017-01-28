/*
Parser used to grab csv files which contain multiple days of cgm data for one patient and seperates them based on times ranging from 12am to 12pm each day
*/

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <direct.h>
#include <windows.h>


using namespace std;

void getNextDay(int day, int month); //calculates the next day
void getPrevDay(int day, int month); //calculates the previous day

Date nextDay; //holds current day
Date prevDay; //holds previous day

struct Date {
	int day;
	int month;
	int year;
	int hour;
	int minute;
};


struct SimpleData { //data for indicual cgm mesaurement
	Date Time;
	int CGM;
};

struct AllData { //used to hold raw data entry fro csv file
	int PtID;
	int RecID;
	Date Time;
	int CGM;
};

vector <AllData> FullData; //vector used to hold all rows of cgm data from patient csv file




int main() {

	for (int currentID = 1; currentID < 504; currentID++) { //loop through all csv file
		FullData.clear();
		string dataLocation= to_string(currentID) + ".csv";
		std::ifstream  data(dataLocation);
		if (data) { //if file with name exists then begin parsing
			string directory = "C:\\Users\\nolan\\Documents\\Visual Studio 2015\\Projects\\CGM Parse\\CGM Parse\\" + to_string(currentID);

			CreateDirectory(directory.c_str(), NULL); //ceates directory for all the patients cgm days

			int count = 0;
			std::string line;
			while (std::getline(data, line))
			{

				vector <string> lineInput;
				std::stringstream  lineStream(line);
				std::string        cell;
				while (std::getline(lineStream, cell, ','))
				{
					lineInput.push_back(cell);
				}

				if (count > 0) {
					AllData inputData;
					inputData.RecID = stoi(lineInput.at(0));
					inputData.PtID = stoi(lineInput.at(1));

					//Tokenizing time data
					string s = lineInput.at(2);
					
					//"2000/12/30  12:56:18 AM"
					string delimiter[5] = { "/", "/", " ", ":", "\n" };

					size_t pos = 0;
					string token;
					int j = 0;
					Date currentDate;
					while ((pos = s.find(delimiter[j])) != std::string::npos) {
						token = s.substr(0, pos);
						if (j == 0) {
							currentDate.month = stoi(token);
						}
						else if (j == 1) {
							currentDate.day = stoi(token);
						}
						else if (j == 2) {
							currentDate.year = stoi(token);
						}
						else if (j == 3) {
							currentDate.hour = stoi(token);
						}
						s.erase(0, pos + delimiter[j].length());
						j++;
					}
					currentDate.minute = stoi(s);

					inputData.Time = currentDate;
					inputData.CGM = stoi(lineInput.at(3));
					FullData.push_back(inputData); //push back row information
				}
				count++;
			}
			data.close();

			Date currentDay;
			int currentPosition;
			int currentdayID = 1;
			int size = FullData.size() - 1;
			int i = 0;

			std::reverse(FullData.begin(), FullData.end()); // makes sure date information in fulldata is increasing
			

			if (FullData.at(size).Time.hour >= 12) { //determine the day of frist datapoint. 
				currentDay.day = FullData.at(size).Time.day;
				currentDay.month = FullData.at(size).Time.month;
				currentDay.hour = FullData.at(size).Time.hour;
				currentDay.minute = FullData.at(size).Time.minute;
				currentDay.year = FullData.at(size).Time.year;
				getNextDay(currentDay.day, currentDay.month);
			}
			else {
				getPrevDay(FullData.at(size).Time.day, FullData.at(size).Time.month);
				currentDay.day = prevDay.day;
				currentDay.month = prevDay.month;
				nextDay.day = FullData.at(size).Time.day;
				nextDay.month = FullData.at(size).Time.month;
			}

			vector <SimpleData> singleNight; //holds cgm data for one night

			SimpleData datapoint;

			datapoint.CGM = FullData.at(size).CGM;
			datapoint.Time.day = FullData.at(size).Time.day;
			datapoint.Time.month = FullData.at(size).Time.month;
			datapoint.Time.hour = FullData.at(size).Time.hour;
			datapoint.Time.minute = FullData.at(size).Time.minute;
			datapoint.Time.year = FullData.at(size).Time.year;

			singleNight.push_back(datapoint);

			for (i = 1; i < FullData.size(); i++) {
				SimpleData datapoint;

				datapoint.CGM = FullData.at(size - i).CGM;
				datapoint.Time.day = FullData.at(size - i).Time.day;
				datapoint.Time.month = FullData.at(size - i).Time.month;
				datapoint.Time.hour = FullData.at(size - i).Time.hour;
				datapoint.Time.minute = FullData.at(size - i).Time.minute;
				datapoint.Time.year = FullData.at(size - i).Time.year;

				//if next datpoint is still in current day then push it back into the singelNigthVector
				if ((datapoint.Time.day == currentDay.day) && (datapoint.Time.month == currentDay.month) && (datapoint.Time.hour >= 12))
					singleNight.push_back(datapoint);
				else if ((datapoint.Time.day == nextDay.day) && (datapoint.Time.month == nextDay.month) && (datapoint.Time.hour < 12))
					singleNight.push_back(datapoint);
				else {
					//otherwise save data in current singleNigth vector and recalculate current day base don latest cgm data point
					if (singleNight.size() > 100) { //only saves n ights with over 100 datapoint to filter out days with too little information
						string filename = "pt" + to_string(currentID) + "_" + to_string(currentdayID) + ".csv";
						ofstream myfile;
						string fullFile = to_string(currentID) + "/" + filename;
						myfile.open(fullFile);

						for (int j = 0; j < singleNight.size(); j++) {
							string output = to_string(singleNight.at(j).Time.month) + "\\" + to_string(singleNight.at(j).Time.day) + "\\" + to_string(singleNight.at(j).Time.year) + " " + to_string(singleNight.at(j).Time.hour) + ":" + to_string(singleNight.at(j).Time.minute) + "," + to_string(singleNight.at(j).CGM) + "\n";
							myfile << output;
						}

						myfile.close();
						currentdayID++;

						

					}

					//recalculate current day
					if (datapoint.Time.hour >= 12) {
						currentDay.day = datapoint.Time.day;
						currentDay.month = datapoint.Time.month;
						currentDay.hour = datapoint.Time.hour;
						currentDay.minute = datapoint.Time.minute;
						currentDay.year = datapoint.Time.year;
						getNextDay(currentDay.day, currentDay.month);
					}
					else {
						getPrevDay(datapoint.Time.day, datapoint.Time.month);
						currentDay.day = prevDay.day;
						currentDay.month = prevDay.month;
						nextDay.day = datapoint.Time.day;
						nextDay.month = datapoint.Time.month;
					}
					singleNight.clear();
					singleNight.push_back(datapoint);
				}

			}
			int ct = 0;
		}	
	}
}

void getNextDay(int day, int month) {
	//months with month with 31 days before it
	if (day == 31 && (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)) {
		nextDay.day = 1;

		if (month == 12)
			nextDay.month = 1;
		else
			nextDay.month = month + 1;
	}
	//months with month with 30 days before it
	else if (day == 30 && (month == 4 || month == 6 || month == 9 || month == 11)) {
		nextDay.day = 1;
		nextDay.month = month + 1;
	}
	//month with month with february (29) days before it
	else if (day == 28 && month == 2) {
		nextDay.month = month + 1;
		nextDay.day = 1;
	}
	else
	{
		nextDay.day = day + 1;
		nextDay.month = month;
	}

}

void getPrevDay(int day, int month) {
	//months with month with 31 days before it
	if (day == 1) {
		if (month == 2 || month == 4 || month == 6 || month == 8 || month == 9 || month == 11 || month == 1) {
			prevDay.day = 31;

			if (month == 1)
				prevDay.month = 12;
			else
				prevDay.month = month - 1;
		}
		//months with month with 30 days before it
		else if (month == 5 || month == 7 || month == 10 || month == 12) {
			prevDay.day = 30;
			prevDay.month = month - 1;
		}
		//month with month with february (29) days before it
		else if (month == 3) {
			prevDay.month = month - 1;
			prevDay.day = 28;
		}
	}
	else
	{
		prevDay.day = day - 1;
		prevDay.month = month;
	}
}