//
//  main.cpp
//  MDP
//
//  Created by Andres Bustamante on 11/19/19.
//  Copyright © 2019 Andres Bustamante. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "cCell.hpp"
#define MDP_WIDTH 15
#define MDP_HEIGHT 10
#define PROB 0.9
#define GAMMA 0.8

using namespace std;

Cell*** currValues = NULL;	// 2D array of *Cell
Cell*** prevValues = NULL;	// 2D array of *Cell
Cell*** currPolicy = NULL;	// 2D array of *Cell
Cell*** prevPolicy = NULL;	// 2D array of *Cell
// rewards is the R in MDP=<S,A,T,R>
Cell*** rewards = NULL;		// 2D array of *Cell

ostringstream iterStream(ostringstream::ate);
unsigned int iteration;
float difference = 1 - PROB;

// Copy table src to dst
void copyTable(Cell*** src, Cell*** dst)
{
	for (int r = 0; r < MDP_HEIGHT; r++)
	{
		for (int c = 0; c < MDP_WIDTH; c++)
		{
			dst[r][c] = new Cell(src[r][c]);
		}
	}
}

// Determine if [row, col] is inside the state set:
bool inBounds(int row, int col)
{
	return row >= 0 && row < MDP_HEIGHT && col >= 0 && col < MDP_WIDTH;
}

int findNeighbours(int r, int c, int positions[], float nFvalues[]) {
	int cont = 0;

	if (inBounds(r, c + 1) != false) { cont++; positions[0] = 1; nFvalues[0] = prevValues[r][c+1]->fValue; } 
	  //RR
	if (inBounds(r - 1, c + 1) != false) { cont++; positions[1] = 1; nFvalues[1] = prevValues[r-1][c + 1]->fValue;
	} //UR
	if (inBounds(r - 1, c) != false) { cont++; positions[2] = 1; nFvalues[2] = prevValues[r-1][c]->fValue;
	} //UU
	if (inBounds(r - 1, c - 1) != false) { cont++; positions[3] = 1; nFvalues[3] = prevValues[r-1][c - 1]->fValue;
	} //UL
	if (inBounds(r, c - 1) != false) { cont++; positions[4] = 1; nFvalues[4] = prevValues[r][c - 1]->fValue;
	} //LL
	if (inBounds(r + 1, c - 1) != false) { cont++; positions[5] = 1; nFvalues[5] = prevValues[r+1][c - 1]->fValue;
	} //DL
	if (inBounds(r + 1, c) != false) { cont++; positions[6] = 1; nFvalues[6] = prevValues[r+1][c]->fValue;
	} //DD
	if (inBounds(r + 1, c + 1) != false) { cont++; positions[7] = 1; nFvalues[7] = prevValues[r+1][c + 1]->fValue;
	} //DR
	nFvalues[8] = prevPolicy[r][c]->fValue;

	return cont;
}

int findPosition(float values[], float* max) {
	for (int i = 0; i < 9; i++) {
		if (values[i] == *max) {
			return i;
		}
	}
	return 0;
}

void calculateValues(int positions[], float nFvalues[], float results[], int* index, float* max, float* div) {

	int cont = 0;

	for (int i = 0; i < 9; i++) {

		bool stay = false;
		float sum = 0;

		for (int j = 0; j < 9; j++) {
			if (i == j) {
				if (positions[j] == 0) {
					stay = true;
					continue;
				}
				else {
					sum += PROB * nFvalues[j];
				}
			}
			else {
				if (j == 8 && stay == true)
				{
					stay = false;
					sum += PROB * nFvalues[j];
				}
				else {
					if (positions[j] == 0) {
						continue;
					}
					else {
						sum += *div * nFvalues[j];
					}
				}
			}
		}
		results[cont] = sum;
		cont++;
	}

	*max = *max_element(results, results+9);
	*index = findPosition(results, max);
}


// Perform the Value Iteration using the Bellman equation:
bool valueIteration()
{
	int total = 0;
	for (int r = 0; r < MDP_HEIGHT; r++)
	{
		for (int c = 0; c < MDP_WIDTH; c++)
		{
			int positions[9] = {0,0,0,0,0,0,0,0,1};
			float nFvalues[9] = { 0,0,0,0,0,0,0,0,0};
			float results[9] = { -20,-20,-20,-20,-20,-20,-20,-20,-20 };

			int neighbours = findNeighbours(r,c,positions,nFvalues);

			int index = 0;
			float max = 0;
			float div = difference / (neighbours + 1);

			calculateValues(positions, nFvalues, results, &index, &max, &div);

			currValues[r][c]->fValue = prevValues[r][c]->fValue + GAMMA * max;
			currPolicy[r][c]->iValue = index;

			total += prevPolicy[r][c]->iValue - currPolicy[r][c]->iValue;
		}
	}

	if (total == 0) {
		return true;
	}
	else {
		return false;
	}
	
}


// Print a message, followed by a table:
void showTable(string msg, Cell*** table)
{
	cout << endl << msg << endl;
	for (int r = 0; r < MDP_HEIGHT; r++)
	{
		for (int c = 0; c < MDP_WIDTH; c++)
		{
			cout << table[r][c]->toString() << "\t";
		}
		cout << endl;
	}
}

// Print the current iteration to console:
void showIteration()
{
	iterStream.str("\nITERATION ");
	iterStream << iteration;
	cout << iterStream.str() << endl;
	showTable("---REWARDS--------", rewards);
	showTable("---VALUES---------", currValues);
	showTable("---PREV. VALUES---", prevValues);
	showTable("---POLICY---------", currPolicy);
	showTable("---PREV. POLICY---", prevPolicy);
	cout << endl;
}

// Free all the memory used by this program:
void clean()
{
	delete currValues;
	delete prevValues;
	delete currPolicy;
	delete prevPolicy;
	delete rewards;
	cout << endl << "All clean. Bye." << endl;
}

// Compute the MDP solution (don't forget to write a file with the optimal policy):
int main()
{
	iteration = 0;
	currValues = new Cell**[MDP_HEIGHT];	// Reserve memory for the rows
	prevValues = new Cell**[MDP_HEIGHT];	// Reserve memory for the rows
	currPolicy = new Cell**[MDP_HEIGHT];	// Reserve memory for the rows
	prevPolicy = new Cell**[MDP_HEIGHT];	// Reserve memory for the rows
	rewards = new Cell**[MDP_HEIGHT];		// Reserve memory for the rows
	for (int r = 0; r < MDP_HEIGHT; r++)
	{
		currValues[r] = new Cell*[MDP_WIDTH];	// Reserve memory for the columns
		prevValues[r] = new Cell*[MDP_WIDTH];	// Reserve memory for the columns
		currPolicy[r] = new Cell*[MDP_WIDTH];	// Reserve memory for the columns
		prevPolicy[r] = new Cell*[MDP_WIDTH];	// Reserve memory for the columns
		rewards[r] = new Cell*[MDP_WIDTH];		// Reserve memory for the columns
		for (int c = 0; c < MDP_WIDTH; c++)
		{
			currValues[r][c] = new Cell(Cell::CT_VALUE);	// Instance of the value cell
			currValues[r][c]->row = r;
			currValues[r][c]->column = c;
			prevValues[r][c] = new Cell(Cell::CT_VALUE);	// Instance of the value cell
			prevValues[r][c]->row = r;
			prevValues[r][c]->column = c;
			prevValues[r][c]->fValue = -0.03f;
			currPolicy[r][c] = new Cell(Cell::CT_POLICY);	// Instance of the policy cell
			currPolicy[r][c]->row = r;
			currPolicy[r][c]->column = c;
			prevPolicy[r][c] = new Cell(Cell::CT_POLICY);	// Instance of the policy cell
			prevPolicy[r][c]->row = r;
			prevPolicy[r][c]->column = c;
			rewards[r][c] = new Cell(Cell::CT_REWARD);		// Instance of the rewards cell
			rewards[r][c]->row = r;
			rewards[r][c]->column = c;
			rewards[r][c]->fValue = -0.04f;					// Default cost for just moving
		}
	}

	rewards[2][1]->fValue = -10.0f;                            // Penalty for an obstacle
	rewards[2][3]->fValue = -10.0f;                            // Penalty for an obstacle
	rewards[2][5]->fValue = -10.0f;                            // Penalty for an obstacle
	rewards[2][7]->fValue = -10.0f;                            // Penalty for an obstacle
	rewards[2][9]->fValue = -10.0f;                            // Penalty for an obstacle
	rewards[5][0]->fValue = -10.0f;
	rewards[5][2]->fValue = -10.0f;
	rewards[5][4]->fValue = -10.0f;
	rewards[5][6]->fValue = -10.0f;
	rewards[5][8]->fValue = -10.0f;
	rewards[9][1]->fValue = -10.0f;
	rewards[9][3]->fValue = -10.0f;
	rewards[9][5]->fValue = -10.0f;

	rewards[0][14]->fValue = +10.0f;                            // Reward for the goal
	rewards[9][9]->fValue = +10.0f;                            // Reward for the goal

	copyTable(rewards, prevValues);

	//showIteration();
	
	bool convergence = false;
	
		ToDo:
		while(!convergence)
		{
			convergence = valueIteration();
			iteration++;
			showIteration();

			copyTable(currValues, prevValues);
			copyTable(currPolicy, prevPolicy);
		}
	
	clean();
	return 0;
}
