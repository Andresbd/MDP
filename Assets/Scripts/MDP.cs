using System;
using System.IO;
using UnityEngine;
using System.Linq;
using UnityEngine.UI;

public class MDP : MonoBehaviour
{
    public GameObject Car;
    private GameObject[] NS_Cars;
    private GameObject[] WE_Cars;

    public InputField NS_Ammount;
    public InputField WE_Ammount;
    private int contNS, contWE, setN, setW;
    public int nCars, wCars;

    public GameObject tl;
    public GameObject b;

    public Cell[,] rewards;
    public Cell[,] currPolicy;

    private int WIDTH =  100;
    private int HEIGHT = 100;

    public void GenerateCars()
    {

        setN = int.Parse(NS_Ammount.text);
        setW = int.Parse(WE_Ammount.text);

        contNS = setN;
        contWE = setW;

        for (int i = 0; i <= setN; i++)
        {
            NS_Cars[i] = InvokeCarNS();
            NS_Cars[i].GetComponent<Movement>().SpawnPlayer(true);

        }

        for (int i = 0; i <= setW; i++)
        {
            WE_Cars[i] = InvokeCarWE();
            WE_Cars[i].GetComponent<Movement>().SpawnPlayer(false);
        }

        InvokeRepeating("InstantiateCars",0.0f,1f);
    }

    private void InstantiateCars() {

        if (contNS >= 0) {
            NS_Cars[contNS].GetComponent<Movement>().go = true;
            contNS--;
        }

        if(contWE >= 0)
        {
            WE_Cars[contWE].GetComponent<Movement>().go = true;
            contWE--;
        }
    }

    GameObject InvokeCarNS()
    {
        return Instantiate(Car, new Vector3(0, -1, 0), Quaternion.identity);
    }

    GameObject InvokeCarWE()
    {
        return Instantiate(Car, new Vector3(0, -1, 0), Quaternion.identity);
    }

    private void Update()
    {
        if (contWE < 0 && contNS < 0) {
            CancelInvoke("InstantiateCars");

            contWE = 0;
            contNS = 0;
        }
    }

    void ReadMap(Cell[,] rewards)
    {
        try
        {   // Open the text file using a stream reader.
            using (StreamReader sr = new StreamReader("FinalBoard.csv"))
            {
                // Read the stream to a string
                String line;
                int row = 0;

                while ((line = sr.ReadLine()) != null)
                {
                    String[] value = line.Split(',');

                    for (int col = 0; col < WIDTH; col++)
                    {
                        rewards[row, col].fValue = float.Parse(value[col]);
                    }
                    row++;
                }
            }
        }
        catch (IOException e)
        {
            Debug.Log("The file could not be read:");
            Debug.Log(e.Message);
        }
    }

    void ReadPolicy(Cell[,] currPolicy) {
        try
        {   // Open the text file using a stream reader.
            using (StreamReader sr = new StreamReader("Policy.txt"))
            {
                // Read the stream to a string
                String line;
                int row = 0;

                while ((line = sr.ReadLine()) != null)
                {
                    String[] value = line.Split(',');

                    for (int col = 0; col < WIDTH; col++)
                    {
                        currPolicy[row, col].iValue = Int32.Parse(value[col]);
                    }
                    row++;
                }
            }
        }
        catch (IOException e)
        {
            Debug.Log("The file could not be read:");
            Debug.Log(e.Message);
        }
    }

    void MDPSolution() {

        rewards = new Cell[HEIGHT, WIDTH];
        currPolicy = new Cell[HEIGHT, WIDTH];

        for (int r = 0; r < HEIGHT; r++)
        {
            for (int c = 0; c < WIDTH; c++)
            {
                rewards[r, c] = new Cell();
                rewards[r, c].row = r;
                rewards[r, c].col = c;
                currPolicy[r, c] = new Cell();
                currPolicy[r, c].row = r;
                currPolicy[r, c].col = c;
            }
        }

        ReadMap(rewards);
        ReadPolicy(currPolicy);
    }

    private void Start()
    {
        NS_Cars = new GameObject[2000];
        WE_Cars = new GameObject[2000];
        MDPSolution();

        b.GetComponent<Board>().GenerateBoard(HEIGHT, WIDTH, rewards);
        tl.GetComponent<TrafficLight>().SetTraficLight();
    }
}
