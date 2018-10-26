//
//  main.cpp
//  RedesDeSensores
//
//  Created by Raul C. Almeida Jr. on 14/09/2018.
//  Copyright © 2018 UFPE - DES. All rights reserved.
//

#include <iostream>
#include <assert.h>
#include <string.h>

#include <fstream>      // std::ifstream
#include <iomanip>
using std::cout;
using std::endl;



const int Nnodes = 25;
const int maxB = 7*(Nnodes - 1);
float minP;

int C = 1;
int B[Nnodes];
float Q[Nnodes][Nnodes][Nnodes];
bool A[Nnodes][Nnodes][Nnodes];
float epr[Nnodes];
float et[Nnodes][Nnodes];
float ept[Nnodes];
float P[Nnodes];

std::ifstream ifet ("et.txt");
std::ifstream ifB ("baterias.txt");

float calculateMaxP();
int Battery();


/////////////////////////////////////////////////////////////
int main(int argc, const char * argv[]) {
    std::cout << "inicio do Programa"<<std::endl;
    minP = 1000000000000.0;
    //Inputs:
    for(int i = 0; i < Nnodes; i++){
        epr[i] = ept[i] = 50E-5;
        for(int j = 0; j < Nnodes; j++)
            ifet >> et[i][j];
    }
    
    //Baterias:
    for(int i = 0; i < Nnodes; i++)
        ifB >> B[i];
    
    //
    memset(A, 0, sizeof(A));

    //Roteamento:
/*
    A[1][1][0] = 1;
    A[2][2][0] = 1;
    A[3][3][1] = A[3][1][0] = 1;
    A[4][4][2] = A[4][2][0] = 1;
    A[5][5][2] = A[5][2][0] = 1;
    A[6][6][3] = A[6][3][1] = A[6][1][0] = 1;
    A[7][7][4] = A[7][4][2] = A[7][2][0] = 1;
    A[8][8][7] = A[8][7][5] = A[8][5][2] = A[8][2][0] = 1;
*/
    A[1][1][0] = 1;
    A[2][2][0] = 1;
    A[3][3][1] = A[3][1][0] = 1;
    A[4][4][1] = A[4][1][0] = 1;
    A[5][5][2] = A[5][2][0] = 1;
    A[6][6][3] = A[6][3][1] = A[6][1][0] = 1;
    A[7][7][4] = A[7][4][1] = A[7][1][0] = 1;
    A[8][8][5] = A[8][5][2] = A[8][2][0] = 1;
    A[9][9][5] = A[9][5][2] = A[9][2][0] = 1;
    A[10][10][6] = A[10][6][3] = A[10][3][1] = A[10][1][0] = 1;
    A[11][11][6] = A[11][6][3] = A[11][3][1] = A[11][1][0] = 1;
    A[12][12][8] = A[12][8][5] = A[12][5][2] = A[12][2][0] = 1;
    A[13][13][9] = A[13][9][5] = A[13][5][2] = A[13][2][0] = 1;
    A[14][14][9] = A[14][9][5] = A[14][5][2] = A[14][2][0] = 1;
    A[15][15][10] = A[15][10][6] = A[15][6][3] = A[15][3][1] = A[15][1][0] = 1;
    A[16][16][11] = A[16][11][6] = A[16][6][3] = A[16][3][1] = A[16][1][0] = 1;
    A[17][17][13] = A[17][13][8] = A[17][8][5] = A[17][5][2] = A[17][2][0] = 1;
    A[18][18][13] = A[18][13][8] = A[18][8][5] = A[18][5][2] = A[18][2][0] = 1;
    A[19][19][16] = A[19][16][11] = A[19][11][6] = A[19][6][3] = A[19][3][1] = A[19][1][0] = 1;
    A[20][20][17] = A[20][17][13] = A[20][13][8] = A[20][8][4] = A[20][4][1] = A[20][1][0] = 1;
    A[21][21][17] = A[21][17][13] = A[21][13][8] = A[21][8][5] = A[21][5][2] = A[21][2][0] = 1;
    A[22][22][20] = A[22][20][16] = A[22][16][11] = A[22][11][6] = A[22][6][3] = A[22][3][1] = A[22][1][0] = 1;
    A[23][23][21] = A[23][21][17] = A[23][17][13] = A[23][13][8] = A[23][8][5] = A[23][5][2] = A[23][2][0] = 1;
    A[24][24][22] = A[24][22][19] = A[24][19][15] = A[24][15][11] = A[24][11][6] = A[24][6][3] = A[24][3][1] = A[24][1][0] = 1;

    //Trafego Escoado:
    for(int s = 0; s < Nnodes; s++)
        for(int i = 0; i < Nnodes; i++)
            for(int j = 0; j < Nnodes; j++)
                Q[s][i][j] = 100*A[s][i][j];
   
    assert(Battery() == maxB);
    //
    calculateMaxP();
    
    return 0;
}

// ------------------------------------------------------------------------------------- //
int Battery(){
    int sum = 0;
    for(int i = 0; i < Nnodes; i++)
        sum += B[i];
    // cout << "Total de Baterias = " << sum << endl;
    // cout << "Numero total de bancos = " << maxB << endl;
    return sum;
}

// ------------------------------------------------------------------------------------- //
float calculateMaxP(){
    float maxP = 0.0; minP = 1000000000.0; int critico = -1; int folgado = -1;
    for(int i = 1; i < Nnodes; i++){
        P[i]=0;
        for(int s = 0; s < Nnodes; s++)
            for(int j = 0; j < Nnodes; j++)
                if(j != i)
                    P[i] += Q[s][j][i]*epr[i] + Q[s][i][j]*et[i][j] + Q[s][i][j]*ept[i];
        P[i] = (float)P[i]/(B[i]*C);
        std::cout<<"PP["<<i<<"]="<<P[i]<<std::endl;
        std::cout<<"TT["<<i<<"]="<<1.0/P[i]<<std::endl;
        if(P[i]>maxP){
            maxP = P[i];
            critico = i;
        }
        if(P[i] < minP){
            minP = P[i];
            folgado = i;
        }
    }
    std::cout<<"Maior valor de P="<<maxP<<std::endl;
    std::cout<<"Tempo de vida da rede="<<1/maxP<<std::endl;
    std::cout<<"No +critico="<<critico<<std::endl;
    std::cout<<"No +folgado="<<folgado<<std::endl;
    return maxP;
}

