//
//  main.cpp
//  RedesDeSensores
//
//  Created by Raul C. Almeida Jr. on 14/09/2018.
//  Copyright © 2018 UFPE - DES. All rights reserved.
//

#include <iostream>
#include <stdio.h>              /* printf */
#include <math.h>               /* pow */
#include <assert.h>

#include <fstream>              // std::ifstream
#include <iomanip>

using std::cout;
using std::endl;

/////////////////////////////////////////////////////////////

int main(int argc, const char *argv[])
{
    std::ifstream ifet("et.txt");

    // insert code here...

    std::cout << "inicio do Programa" << std::endl;

    const int Nnodes = 9;
    const int NB = 5;
    int C = 1;
    float Q[Nnodes][Nnodes][Nnodes];
    bool A[Nnodes][Nnodes][Nnodes];
    float epr[Nnodes];
    float et[Nnodes][Nnodes];
    float ept[Nnodes];
    float P[Nnodes];
    bool B[Nnodes][NB];

    //Inputs:

    float value;
    for (int i = 0; i < Nnodes; i++) {
        epr[i] = 50E-9;
        ept[i] = 50E-9;
        for (int j = 0; j < Nnodes; j++) {
            ifet >> value;
            et[i][j] = value;
        }
    }
    //
    for (int s = 0; s < Nnodes; s++)
        for (int i = 0; i < Nnodes; i++)
            for (int j = 0; j < Nnodes; j++)
                A[s][i][j] = 0;

    //Roteamento:

    A[1][1][0] = 1;
    A[2][2][0] = 1;
    A[3][3][0] = 1;
    A[4][4][0] = 1;
    A[5][5][2] = 1;
    A[5][2][0] = 1;
    A[6][6][1] = 1;
    A[6][1][0] = 1;
    A[7][7][5] = 1;
    A[7][5][2] = 1;
    A[7][2][0] = 1;
    A[8][8][7] = 1;
    A[8][7][4] = 1;
    A[8][4][2] = 1;
    A[8][2][0] = 1;

    //Trafego Escoado:

    for (int s = 0; s < Nnodes; s++)
        for (int i = 0; i < Nnodes; i++)
            for (int j = 0; j < Nnodes; j++)
                Q[s][i][j] = 100 * A[s][i][j];

    //Baterias:

    for (int i = 0; i < Nnodes; i++)
        for (int b = 0; b < NB; b++)
            B[i][b] = 0;

    B[1][0] = 1;
    B[1][3] = 1;
    B[2][3] = 1;
    B[3][1] = 1;
    B[4][4] = 1;
    B[5][1] = 1;
    B[6][2] = 1;
    B[7][3] = 1;
    B[8][0] = 1;

    float maxP = 0.0;

    for (int i = 0; i < Nnodes; i++)
        for (int b = 0; b < NB; b++)
            maxP += B[i][b] * pow(2, b);

    std::cout << "Numero total de bancos=" << maxP << std::endl;

    //

    maxP = 0.0;
    for (int i = 1; i < Nnodes; i++) {
        P[i] = 0;
        for (int s = 0; s < Nnodes; s++)
            for (int j = 0; j < Nnodes; j++)
                if (j != i)
                    P[i] +=
                        Q[s][j][i] * epr[i] + Q[s][i][j] * et[i][j] +
                        Q[s][i][j] * ept[i];

        int sum = 0;
        for (int b = 0; b < NB; b++)
            sum += B[i][b] * pow(2, b) * C;

        P[i] = (float)P[i] / sum;
        if (P[i] > maxP)
            maxP = P[i];
    }

    std::cout << "Menor valor de P=" << maxP << std::endl;
    std::cout << "Tempo de vida da rede=" << 1 / maxP << std::endl;
    for (int i = 1; i < Nnodes; i++)
        std::cout << "P[" << i << "] = " << P[i] << std::endl;

    return 0;
}
