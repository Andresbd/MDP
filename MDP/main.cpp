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

#define MDP_WIDTH 15
#define MDP_HEIGHT 10
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
//   Copy Src to Destiny
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
    
    if(inBounds(r,c+1) != false) {cont++; directions[0] = 1; nei[0] = rewards[r][c+1]->fValue;} //RR
    if(inBounds(r-1,c+1) != false) {cont++; directions[1] = 1; nei[1] = rewards[r-1][c+1]->fValue;} //UR
    if(inBounds(r-1,c) != false) {cont++; directions[2] = 1; nei[2] = rewards[r-1][c]->fValue;} //UU
    if(inBounds(r-1,c-1) != false) {cont++; directions[3] = 1; nei[3] = rewards[r-1][c-1]->fValue;} //UL
    if(inBounds(r,c-1) != false) {cont++; directions[4] = 1; nei[4] = rewards[r][c-1]->fValue;} //LL
    if(inBounds(r+1,c-1) != false) {cont++; directions[5] = 1; nei[5] = rewards[r+1][c-1]->fValue;} //DL
    if(inBounds(r+1,c) != false) {cont++; directions[6] = 1; nei[6] = rewards[r+1][c]->fValue;} //DD
    if(inBounds(r+1,c+1) != false) {cont++; directions[7] = 1; nei[7] = rewards[r+1][c+1]->fValue;} //DR
    nei[8] = rewards[r][c]->fValue;
    
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


void calculateValues(vector<float> values, int directions[], float nei[], float* max, int* index, float* div) {
    
    int cont = 0;
    
    for (int i = 0; i < 9; i++) {
        
        bool stay = false;
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
                        sum += *div * nei[j];
                    }
                }
            }
        }
        values[cont] = sum;
        cont++;
    }
    
    *max = *max_element(values.begin(), values.end());
    *index = findPosition(values, *max);
}

// Perform the Value Iteration using the Bellman equation:
bool valueIteration()
{
    float total = 0;
    
    for (int i=0; i < MDP_HEIGHT; i++) {
        for (int j=0; j < MDP_WIDTH; j++) {
            
            int directions[9] = {0,0,0,0,0,0,0,0,1};
            float neighbour[9] ={0,0,0,0,0,0,0,0,0};
            vector<float> values{-20.0f,-20.0f,-20.0f,-20.0f,-20.0f,-20.0f,-20.0f,-20.0f,-20.0f};
            
            int found = findNeighbours(i,j, directions, neighbour);
            float div = residium / (found+1);
            
            int  index;
            float max;
            
//            calculateValues(values, directions, neighbour, &max, &index, &div);
            
            
            currValues[i][j]->fValue = prevValues[i][j]->fValue + 0.8 * max;
            currPolicy[i][j]->iValue = index;
            
            total += prevPolicy[i][j]->iValue - currPolicy[i][j]->iValue;
        }
    }
    
    if(total == 0) {
        return true;
    }else {
        return false;
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

    copyTable(currValues, prevValues);                        // Update the previous values
    copyTable(currPolicy, prevPolicy);                        // Update the previous policy

    showIteration();
    bool convergence = false;
    
    while(!convergence)
    {
        copyTable(currValues, prevValues);
        copyTable(currPolicy, prevPolicy);
        
        cout << prevValues[0][0]->fValue;
        
        convergence = valueIteration();
        iteration++;
        showIteration();
    }
    clean();
    return 0;
}
