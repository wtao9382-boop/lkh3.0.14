#include "LKH.h"
#include "Segment.h"

static GainType GetRoutes();

int **R;
int *RSize;
int Clients;
GainType *RScore;
GainType *RCost;

void TOP_WriteSolution(char *FileName, GainType Cost)
{
    FILE *SolutionFile;
    char *FullFileName;
    time_t Now;
    int i, j;

    if (FileName == 0)
        return;
    FullFileName = FullName(FileName, Cost);
    Now = time(&Now);
    if (TraceLevel >= 1)
        printff("Writing TOP_SOLUTION_FILE: \"%s\" ... ",
                FullFileName);
    SolutionFile = fopen(FullFileName, "w");
    fprintf(SolutionFile, "Found by LKH-3 [Keld Helsgaun] %s", ctime(&Now));
    R = (int **) malloc(Salesmen * sizeof(int *));
    for (i = 0; i < Salesmen; i++)
        R[i] = calloc(DimensionSaved - Salesmen + 1, sizeof(int));
    RSize = (int *) malloc(Salesmen * sizeof(int));
    RScore = (GainType *) malloc(Salesmen * sizeof(GainType));
    RCost = (GainType *) malloc(Salesmen * sizeof(GainType));
    fprintf(SolutionFile, "Profit = "GainFormat"\n", -GetRoutes());
    fprintf(SolutionFile, "Clients = %d\n", Clients);
    fprintf(SolutionFile, "Cost limit = %0.2f\n", 
            CostLimit / Scale);
    for (i = 0; i < Salesmen; i++) {
        fprintf(SolutionFile,
                "Route %d, Profit = "GainFormat", Cost = %0.2f\n", 
                i + 1, RScore[i], 1.0 * RCost[i] / Scale);
        fprintf(SolutionFile, "%d ", 1);
        for (j = 0; j < RSize[i]; j++)
            fprintf(SolutionFile, "%d ", R[i][j]);
        fprintf(SolutionFile, "%d\n", DimensionSaved - Salesmen + 1);
    }
    fclose(SolutionFile);
    if (TraceLevel >= 1)
        printff("done\n");
}

static GainType GetRoutes()
{
    Node *StartNode = &NodeSet[1];
    Node *EndNode = &NodeSet[DimensionSaved - Salesmen + 1];
    Node *N, *PredN, *NextN;
    GainType Score = 0, TotalScore = 0, PathCost, Cost;
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;
    int Size, Salesman = 0;

    Clients = 0;
    N = StartNode;
    do {
        Score = 0;
        PathCost = 0;
        Size = 0;
        do {
            Score += N->Score;
            PredN = Forward ? PREDD(N) : SUCC(N);
            Cost = PathCost +
                    (C(PredN, EndNode) - PredN->Pi - EndNode->Pi) / Precision;
            if (!N->DepotId) {
                R[Salesman][Size] = N->Id;
                Size++;
            }
            if (Cost <= CostLimit) {
                RSize[Salesman] = Size;
                RScore[Salesman] = Score;
                RCost[Salesman] = Cost;
            }
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN - DimensionSaved == EndNode)
                NextN = Forward ? SUCC(EndNode) : PREDD(EndNode);
            PathCost += (C(N, NextN) - N->Pi - NextN->Pi) / Precision;
            N = Forward ? SUCC(NextN) : PREDD(NextN);
        } while (N->DepotId == 0 && N != EndNode && PathCost <= CostLimit);
        while (N->DepotId == 0) {
            NextN = Forward ? SUCC(N) : PREDD(N);
            N = Forward ? SUCC(NextN) : PREDD(NextN);
        }
        Clients += RSize[Salesman];
        TotalScore += RScore[Salesman];
        Salesman++;
    } while (N != StartNode);
    return -TotalScore;
}
