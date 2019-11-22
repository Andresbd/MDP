//
//  main.cpp
//  MDP
//
//  Created by Andres Bustamante on 11/19/19.
//  Copyright © 2019 Andres Bustamante. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include "cCell.hpp"

#define MDP_WIDTH 3
#define MDP_HEIGHT 2
#define GAMMA 0.9

using namespace std;

Cell*** currValues = NULL;    // 2D array of *Cell
Cell*** prevValues = NULL;    // 2D array of *Cell
Cell*** currPolicy = NULL;    // 2D array of *Cell
Cell*** prevPolicy = NULL;    // 2D array of *Cell
// rewards is the R in MDP=<S,A,T,R>
Cell*** rewards = NULL;       // 2D array of *Cell

float residium = 1-GAMMA;

ostringstream iterStream(ostringstream::ate);
unsigned int iteration;

// Copy table src to dst
void copyTable(Cell*** src, Cell*** dst)
{
    if(dst != NULL) delete dst;
    dst = new Cell**[MDP_HEIGHT];
    for(int r = 0; r < MDP_HEIGHT; r++)
    {
        dst[r] = new Cell*[MDP_WIDTH];
        for(int c = 0; c < MDP_WIDTH; c++)
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

int findNeighbours(int r, int c, int directions[], float nei[]) {
    int cont = 0;
    
    if(inBounds(c-1, r+0) != false) {cont++; directions[0] = 1; nei[0] = rewards[c-1][r]->fValue;} //UP
    if(inBounds(c-1, r+1) != false) {cont++; directions[1] = 1; nei[1] = rewards[c-1][r+1]->fValue;} //RU
    if(inBounds(c+0, r+1) != false) {cont++; directions[2] = 1; nei[2] = rewards[c][r+1]->fValue;} //R
    if(inBounds(c+1, r+1) != false) {cont++; directions[3] = 1; nei[3] = rewards[c+1][r+1]->fValue;} //RD
    if(inBounds(c+1, r+0) != false) {cont++; directions[4] = 1; nei[4] = rewards[c+1][r]->fValue;} //D
    if(inBounds(c+1, r-1) != false) {cont++; directions[5] = 1; nei[5] = rewards[c+1][r-1]->fValue;} //LD
    if(inBounds(c+0, r-1) != false) {cont++; directions[6] = 1; nei[6] = rewards[c][r-1]->fValue;} //L
    if(inBounds(c-1, r-1) != false) {cont++; directions[7] = 1; nei[7] = rewards[c-1][r-1]->fValue;} //LU
    nei[8] = rewards[c][r]->fValue;
    
    return cont;
}

int findPosition(vector<float> values, float max){
    for (int i = 0; i< values.size(); i++) {
        if(values[i] == max){
            return i;
        }
    }
    return 0;
}


void calculateValues(int directions[], float nei[], float div, int index, float max) {
    
    std::vector<float> values{-20.0f,-20.0f,-20.0f,-20.0f,-20.0f,-20.0f,-20.0f,-20.0f,-20.0f};
    
    for (int i = 0; i < 9; i++) {
        
        bool stay = false;
        int cont = 0;
        float sum = 0;
        
        for (int j = 0; j < 9; j++) {
            if(i == j) {
                if(directions[j] == 0) {
                    stay = true;
                    continue;
                }else {
                    sum += GAMMA * nei[j];
                }
            }else {
                if (j == 8 && stay == true)
                {
                    stay = false;
                    sum += GAMMA * nei[j];
                }else {
                    if(directions[j] == 0)  {
                        continue;
                    }else {
                        sum += div * nei[j];
                    }
                }
            }
        }
        values[cont] = sum;
        cont++;
    }
    max = *max_element(values.begin(), values.end());
    index = findPosition(values, max);
    
    cout << index;
}

// Perform the Value Iteration using the Bellman equation:
void valueIteration()
{
    for (int i=0; i < MDP_WIDTH; i++) {
        for (int j=0; j < MDP_HEIGHT; j++) {
            
            int directions[9] = {0,0,0,0,0,0,0,0,1};
            float neighbour[9] ={0,0,0,0,0,0,0,0,0};
            
            int found = findNeighbours(i,j, directions, neighbour);
            float div = residium / (found+1);
            
            int index;
            float max;
            
            calculateValues(directions, neighbour, div, index, max);
        }
    }
}


// Print a message, followed by a table:
void showTable(string msg, Cell*** table)
{
    cout << endl << msg << endl;
    for(int r = 0; r < MDP_HEIGHT; r++)
    {
        for(int c = 0; c < MDP_WIDTH; c++)
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
    cout << endl << "All clean." << endl;
}

// Compute the MDP solution (don't forget to write a file with the optimal policy):
int main()
{
    iteration = 0;
    currValues = new Cell**[MDP_HEIGHT];    // Reserve memory for the rows
    prevValues = new Cell**[MDP_HEIGHT];    // Reserve memory for the rows
    currPolicy = new Cell**[MDP_HEIGHT];    // Reserve memory for the rows
    prevPolicy = new Cell**[MDP_HEIGHT];    // Reserve memory for the rows
    rewards = new Cell**[MDP_HEIGHT];        // Reserve memory for the rows
    for(int r = 0; r < MDP_HEIGHT; r++)
    {
        currValues[r] = new Cell*[MDP_WIDTH];    // Reserve memory for the columns
        prevValues[r] = new Cell*[MDP_WIDTH];    // Reserve memory for the columns
        currPolicy[r] = new Cell*[MDP_WIDTH];    // Reserve memory for the columns
        prevPolicy[r] = new Cell*[MDP_WIDTH];    // Reserve memory for the columns
        rewards[r] = new Cell*[MDP_WIDTH];        // Reserve memory for the columns
        for(int c = 0; c < MDP_WIDTH; c++)
        {
            currValues[r][c] = new Cell(Cell::CT_VALUE);    // Instance of the value cell
            currValues[r][c]->row = r;
            currValues[r][c]->column = c;
            currPolicy[r][c] = new Cell(Cell::CT_POLICY);    // Instance of the policy cell
            currPolicy[r][c]->row = r;
            currPolicy[r][c]->column = c;
            rewards[r][c] = new Cell(Cell::CT_REWARD);        // Instance of the rewards cell
            rewards[r][c]->row = r;
            rewards[r][c]->column = c;
            rewards[r][c]->fValue = -0.04;                    // Default cost for just moving
        }
    }

    rewards[0][1]->fValue = -10.0f;                            // Penalty for an obstacle
    rewards[0][2]->fValue = +10.0f;                            // Reward for the goal

    copyTable(currValues, prevValues);                        // Update the previous values
    copyTable(currPolicy, prevPolicy);                        // Update the previous policy

    showIteration();
    valueIteration();
    /*
        ToDo:
        while(!convergence)
        {
            valueIteration();
            iteration++;
            showIteration();
        }
    */
    clean();
    return 0;
}
