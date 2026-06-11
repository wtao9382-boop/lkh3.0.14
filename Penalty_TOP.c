#include "LKH.h"
#include "Segment.h"

GainType Penalty_TOP(void)
{
    Node *StartNode = &NodeSet[1];
    Node *EndNode = &NodeSet[Dim];
    Node *N, *PredN, *NextN;
    GainType Score = 0, TotalScore = 0, PathCost, Cost;
    int Forward = SUCC(Depot)->Id != Depot->Id + DimensionSaved;

    N = StartNode;
    do {
        Score = RouteScore = 0;
        PathCost = 0;
        do {
            Score += N->Score;
            PredN = Forward ? PREDD(N) : SUCC(N);
            Cost = PathCost +
                    (C(PredN, EndNode) - PredN->Pi - EndNode->Pi) / Precision;
            if (Cost <= CostLimit)
                RouteScore = Score;
            NextN = Forward ? SUCC(N) : PREDD(N);
            if (NextN - DimensionSaved == EndNode)
                NextN = Forward ? SUCC(EndNode) : PREDD(EndNode);
            PathCost += (C(N, NextN) - N->Pi - NextN->Pi) / Precision;
            N = Forward ? SUCC(NextN) : PREDD(NextN);
        } while (N->DepotId == 0 && N != EndNode && PathCost <= CostLimit);
        while (N->DepotId == 0)
            N = Forward ? SUCC(SUCC(N)) : PREDD(PREDD(N));
        TotalScore += RouteScore;
    } while (N != StartNode);
    return -TotalScore;
}

