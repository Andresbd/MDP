using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Cell : MonoBehaviour
{
    public float fValue;
    public int iValue, row, col;
    public bool bussy;

    enum direction {
        RR,
        UR,
        UU,
        UL,
        LL,
        DL,
        DD,
        DR,
        ST
    }

    public Cell() {
        fValue = 0.0f;
        iValue = 0;
    }

    public Cell(Cell other) {
        fValue = other.fValue;
        iValue = other.iValue;
    }
}
