//
//  main.cpp
//  RedesDeSensores
//
//  Created by Raul C. Almeida Jr. on 14/09/2018.
//  Copyright © 2018 UFPE - DES. All rights reserved.
//

#include <iostream>
#include <stdio.h>      /* printf */
#include <math.h>       /* pow */
#include <assert.h>

#include <fstream>      // std::ifstream
#include <iomanip>
using std::cout;
using std::endl;



const int Nnodes = 16;
const int NB=7, maxB=105;
float minP;

int C = 1;
bool B[Nnodes][NB];
float Q[Nnodes][Nnodes][Nnodes];
bool A[Nnodes][Nnodes][Nnodes];
float epr[Nnodes];
float et[Nnodes][Nnodes];
float ept[Nnodes];
float P[Nnodes];

std::ifstream ifet ("et.txt");



void Recursiva(int, int);
//int value();
float calculateMaxP();
int Battery(int);
int Battery();


/////////////////////////////////////////////////////////////
int main(int argc, const char * argv[]) {
    std::cout << "inicio do Programa"<<std::endl;
    minP = 1000000000000.0;
    //Inputs:
    float value;
    for(int i = 0; i < Nnodes; i++){
        epr[i] = 50E-9;
        ept[i] = 50E-9;
        for(int j = 0; j < Nnodes; j++){
            ifet>>value;
            et[i][j] = value;
            //cout<<value<<endl;
        }
    }
    
    //
    for(int s = 0; s < Nnodes; s++)
        for(int i = 0; i < Nnodes; i++)
            for(int j = 0; j < Nnodes; j++)
                A[s][i][j] = 0;
    
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
    A[4][4][0] = 1;
    A[5][5][2] = A[5][2][0] = 1;
    A[6][6][3] = A[6][3][1] = A[6][1][0] = 1;
    A[7][7][0] = 1;
    A[8][8][2] = A[8][2][0] = 1;
    A[9][9][5] = A[9][5][2] = A[9][2][0] = 1;
    A[10][10][3] = A[10][3][1] = A[10][1][0] = 1;
    A[11][11][4] = A[11][4][0] = 1;
    A[12][12][2] = A[12][2][0] = 1;
    A[13][13][4] = A[10][4][0] = 1;
    A[14][14][8] = A[14][8][0] = 1;
    A[15][15][11] = A[15][11][4] = A[15][4][0] = 1;

    //Trafego Escoado:
    for(int s = 0; s < Nnodes; s++)
        for(int i = 0; i < Nnodes; i++)
            for(int j = 0; j < Nnodes; j++)
                Q[s][i][j] = 100*A[s][i][j];
   
    //Baterias:
    for(int i = 0; i < Nnodes; i++)
         for(int b = 0; b < NB; b++)
             B[i][b]=0;
    
    B[1][0] = B[1][2] = B[1][3] = 1;
    B[2][0] = B[2][1] = B[2][2] = B[2][3] = 1;
    B[3][0] = B[3][3] = 1;
    B[4][2] = B[4][3] = 1;
    B[5][0] = B[5][1] = B[5][2] = 1;
    B[6][0] = B[6][1] = 1;
    B[7][1] = B[7][2] = 1;
    B[8][0] = B[8][3] = 1;
    B[9][0] = B[9][1] = 1;
    B[10][0] = B[10][1] = 1;
    B[11][0] = B[11][1] = B[11][2] = 1;
    B[12][1] = B[12][2] = 1;
    B[13][1] = B[13][2] = 1;
    B[14][0] = B[14][1] = 1;
    B[15][0] = B[15][1] = 1;

    //Recursiva(1, 0);
    assert(Battery() == maxB);
    //
    calculateMaxP();
    
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////
void Recursiva(int n, int b){
    float aux;
    if(Battery() == 50){
        aux = calculateMaxP();
        if(aux < minP)
            minP = aux;
        B[n][b] = 0;
        B[n][b+1]+=1;
        Recursiva(n,b);
    }
    else
    if(n == 0){
        n++;
    }
    else
        Recursiva(n, b);
    
}

// ------------------------------------------------------------------------------------- //
int Battery(int n){
    int sum = 0; int pot = 1;
    for(int b = 0; b < NB; b++){
        sum += B[n][b]*pot;
        pot *= 2;
    }
    //cout<<"B["<<n<<"]="<<sum<<endl;
    return sum;
}

// ------------------------------------------------------------------------------------- //
int Battery(){
    int sum = 0;
    for(int i = 0; i < Nnodes; i++){
        sum += Battery(i);
    }
    //cout<<"Total de Baterias="<<sum<<endl;
    return sum;
    //std::cout<<"Numero total de bancos="<<maxP<<std::endl;
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
        P[i] = (float)P[i]/(Battery(i)*C);
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

