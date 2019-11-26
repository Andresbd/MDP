using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Movement : MonoBehaviour
{
    public MDP MDP;
    private Cell currCell, nextCell;
    public bool go = false;
    private bool dir;

    public void SpawnPlayer(bool nSpawn) {

        MDP = FindObjectOfType<MDP>();

        int randomStart;
        dir = nSpawn;

        if (nSpawn)
        {

            randomStart  = Random.Range(40, 59);

            currCell = MDP.currPolicy[0,randomStart];

            MDP.nCars++;

        }
        else
        {

            randomStart = Random.Range(40, 59);

            currCell = MDP.currPolicy[randomStart,0];

            MDP.wCars++;
        }
    }

    Cell GetNextCell(Cell currCell,int row,int col) {

        int pos = currCell.iValue;

        switch (pos) {
            case 0:
                return MDP.currPolicy[row,col+1];
            case 1:
                return MDP.currPolicy[row-1, col+1];
            case 2:
                return MDP.currPolicy[row-1, col];
            case 3:
                return MDP.currPolicy[row-1, col-1];
            case 4:
                return MDP.currPolicy[row, col-1];
            case 5:
                return MDP.currPolicy[row+1, col-1];
            case 6:
                return MDP.currPolicy[row+1, col];
            case 7:
                return MDP.currPolicy[row+1, col+1];
            case 8:
                return MDP.currPolicy[row,col];
        }

        return currCell;
    }

    private void Update()
    {
        if (go) {

            int r = currCell.row;
            int c = currCell.col;

            transform.position = new Vector3(r,1,c);

            MDP.currPolicy[r, c].bussy = true;

            nextCell = GetNextCell(currCell, r, c);

            if (!nextCell.bussy)
            {
                transform.position = new Vector3(nextCell.row, 1, nextCell.col);
                MDP.currPolicy[r, c].bussy = false;
                currCell = nextCell;

                if (MDP.rewards[currCell.row, currCell.col].fValue == 10)
                {
                    if (dir)
                    {
                        MDP.nCars--;
                        Destroy(this);
                    }
                    else
                    {
                        MDP.wCars--;
                        Destroy(this);
                    }
                }
            }
        }
    }
}
