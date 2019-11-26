using UnityEngine;

public class Board : MonoBehaviour
{
    public GameObject[] NS;
    public GameObject[] WE;
    public GameObject black, white, green,grey, MDP;

    int wCount, nCount;

    public void GenerateBoard(int HEIGHT, int WIDTH, Cell[,] rewards)
    {
        WE = new GameObject[20];
        NS = new GameObject[20];

        wCount = 0;
        nCount = 0;

        for (int r = 0; r < HEIGHT; r++)
        {
            for (int c = 0; c < WIDTH; c++)
            {

                switch (rewards[r,c].fValue) {
                    case (-0.04f ):
                    case(-2.0f):
                        white = Instantiate(white, new Vector3(r, 0, c), Quaternion.identity);
                        if (r == 39 && c > 39 && c < 61)
                        {
                            NS[nCount] = white;
                            nCount++;
                        }
                        else if (r > 39 && r < 61)
                        {
                            if (c == 39)
                            {
                                WE[wCount] = white;
                                wCount++;
                            }
                        }
                        break;
                    case (-10f):
                        black = Instantiate(black, new Vector3(r, 0, c), Quaternion.identity);
                        break;
                    case (10):
                        green = Instantiate(green, new Vector3(r, 0, c), Quaternion.identity);
                        break;

                }
            }
        }
    }
}
