#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_BANKS 100

struct Bank {
    char name[50];
    int netAmount;
    char types[10][50];
    int numTypes;
};

int getMinIndex(struct Bank listOfNetAmounts[], int numBanks) {
    int min = INT_MAX, minIndex = -1;
    for (int i = 0; i < numBanks; i++) {
        if (listOfNetAmounts[i].netAmount == 0) continue;

        if (listOfNetAmounts[i].netAmount < min) {
            minIndex = i;
            min = listOfNetAmounts[i].netAmount;
        }
    }
    return minIndex;
}

int getSimpleMaxIndex(struct Bank listOfNetAmounts[], int numBanks) {
    int max = INT_MIN, maxIndex = -1;
    for (int i = 0; i < numBanks; i++) {
        if (listOfNetAmounts[i].netAmount == 0) continue;

        if (listOfNetAmounts[i].netAmount > max) {
            maxIndex = i;
            max = listOfNetAmounts[i].netAmount;
        }
    }
    return maxIndex;
}

int getMaxIndex(struct Bank listOfNetAmounts[], int numBanks, 
int minIndex, struct Bank input[], int maxNumTypes, char matchingType[50]) {
    int max = INT_MIN;
    int maxIndex = -1;

    for (int i = 0; i < numBanks; i++) {
        if (listOfNetAmounts[i].netAmount == 0 || i == minIndex) continue;

        int numCommonTypes = 0;
        for (int j = 0; j < listOfNetAmounts[minIndex].numTypes; j++) {
            for (int k = 0; k < listOfNetAmounts[i].numTypes; k++) {
                if (strcmp(listOfNetAmounts[minIndex].types[j], listOfNetAmounts[i].types[k]) == 0) {
                    numCommonTypes++;
                    strcpy(matchingType, listOfNetAmounts[minIndex].types[j]);
                    break;
                }
            }
        }

        if (numCommonTypes > 0 && max < listOfNetAmounts[i].netAmount) {
            max = listOfNetAmounts[i].netAmount;
            maxIndex = i;
        }
    }

    return maxIndex;
}

void printAns(int numBanks, struct Bank input[], int ansGraph[MAX_BANKS][MAX_BANKS][2], char ansTypes[MAX_BANKS][MAX_BANKS][50]) {
    printf("\nThe transactions for minimum cash flow are as follows:\n\n");
    for (int i = 0; i < numBanks; i++) {
        for (int j = 0; j < numBanks; j++) {
            if (i == j) continue;
            if (ansGraph[i][j][0] != 0 && ansGraph[j][i][0] != 0) {
                if (ansGraph[i][j][0] == ansGraph[j][i][0]) {
                    ansGraph[i][j][0] = 0;
                    ansGraph[j][i][0] = 0;
                } else if (ansGraph[i][j][0] > ansGraph[j][i][0]) {
                    ansGraph[i][j][0] -= ansGraph[j][i][0];
                    ansGraph[j][i][0] = 0;
                    printf("%s pays Rs %d to %s via %s\n", input[i].name, ansGraph[i][j][0], input[j].name, ansTypes[i][j]);
                } else {
                    ansGraph[j][i][0] -= ansGraph[i][j][0];
                    ansGraph[i][j][0] = 0;
                    printf("%s pays Rs %d to %s via %s\n", input[j].name, ansGraph[j][i][0], input[i].name, ansTypes[j][i]);
                }
            } else if (ansGraph[i][j][0] != 0) {
                printf("%s pays Rs %d to %s via %s\n", input[i].name, ansGraph[i][j][0], input[j].name, ansTypes[i][j]);
            } else if (ansGraph[j][i][0] != 0) {
                printf("%s pays Rs %d to %s via %s\n", input[j].name, ansGraph[j][i][0], input[i].name, ansTypes[j][i]);
            }

            ansGraph[i][j][0] = 0;
            ansGraph[j][i][0] = 0;
        }
    }
    printf("\n");
}

void minimizeCashFlow(int numBanks, struct Bank input[], int numTransactions, int graph[MAX_BANKS][MAX_BANKS], int maxNumTypes) {
    struct Bank listOfNetAmounts[MAX_BANKS];

    for (int b = 0; b < numBanks; b++) {
        strcpy(listOfNetAmounts[b].name, input[b].name);
        listOfNetAmounts[b].numTypes = input[b].numTypes;
        for (int i = 0; i < input[b].numTypes; i++) {
            strcpy(listOfNetAmounts[b].types[i], input[b].types[i]);
        }

        int amount = 0;
        for (int i = 0; i < numBanks; i++) {
            amount += graph[i][b];
        }

        for (int j = 0; j < numBanks; j++) {
            amount += (-1) * graph[b][j];
        }

        listOfNetAmounts[b].netAmount = amount;
    }

    int ansGraph[MAX_BANKS][MAX_BANKS][2] = {0};
    char ansTypes[MAX_BANKS][MAX_BANKS][50] = {0};

    int numZeroNetAmounts = 0;

    for (int i = 0; i < numBanks; i++) {
        if (listOfNetAmounts[i].netAmount == 0) numZeroNetAmounts++;
    }

    while (numZeroNetAmounts != numBanks) {
        int minIndex = getMinIndex(listOfNetAmounts, numBanks);
        char matchingType[50];
        int maxIndex = getMaxIndex(listOfNetAmounts, numBanks, minIndex, input, maxNumTypes, matchingType);

        if (maxIndex == -1) {
            ansGraph[minIndex][0][0] += abs(listOfNetAmounts[minIndex].netAmount);
            strcpy(ansTypes[minIndex][0], input[minIndex].types[0]);

            int simpleMaxIndex = getSimpleMaxIndex(listOfNetAmounts, numBanks);
            ansGraph[0][simpleMaxIndex][0] += abs(listOfNetAmounts[minIndex].netAmount);
            strcpy(ansTypes[0][simpleMaxIndex], input[simpleMaxIndex].types[0]);

            listOfNetAmounts[simpleMaxIndex].netAmount += listOfNetAmounts[minIndex].netAmount;
            listOfNetAmounts[minIndex].netAmount = 0;

            if (listOfNetAmounts[minIndex].netAmount == 0) numZeroNetAmounts++;

            if (listOfNetAmounts[simpleMaxIndex].netAmount == 0) numZeroNetAmounts++;

        } else {
            int transactionAmount = (abs(listOfNetAmounts[minIndex].netAmount) < listOfNetAmounts[maxIndex].netAmount)
                ? abs(listOfNetAmounts[minIndex].netAmount)
                : listOfNetAmounts[maxIndex].netAmount;

            ansGraph[minIndex][maxIndex][0] += transactionAmount;
            strcpy(ansTypes[minIndex][maxIndex], matchingType);

            listOfNetAmounts[minIndex].netAmount += transactionAmount;
            listOfNetAmounts[maxIndex].netAmount -= transactionAmount;

            if (listOfNetAmounts[minIndex].netAmount == 0) numZeroNetAmounts++;

            if (listOfNetAmounts[maxIndex].netAmount == 0) numZeroNetAmounts++;
        }
    }

    printAns(numBanks, input, ansGraph, ansTypes);
}

int main() {
    printf("\n\t\t\t\t********************* Welcome to CASH FLOW MINIMIZER SYSTEM ***********************\n\n\n");
    printf("This system minimizes the number of transactions among multiple banks in the different corners of the world that use different modes of payment. There is one world bank (with all payment modes) to act as an intermediary between banks that have no common mode of payment.\n\n");
    printf("Enter the number of banks participating in the transactions.\n");
    int numBanks;
    scanf("%d", &numBanks);

    struct Bank input[MAX_BANKS];
    int numTransactions = 0;
    printf("Enter the details of the banks and transactions as stated:\n");
    printf("Bank name, number of payment modes it has, and the payment modes.\n");
    printf("Bank name and payment modes should not contain spaces\n");

    int maxNumTypes = 0;
    for (int i = 0; i < numBanks; i++) {
        printf("Bank %d : ", i+1); 
        scanf("%s", input[i].name);
        input[i].numTypes = 0;
        int numTypes;
        scanf("%d", &numTypes);

        if (i == 0) maxNumTypes = numTypes;

        for (int j = 0; j < numTypes; j++) {
            scanf("%s", input[i].types[input[i].numTypes]);
            input[i].numTypes++;
        }
    }

    printf("Enter the number of transactions.\n");
    scanf("%d", &numTransactions);

    int graph[MAX_BANKS][MAX_BANKS] = {0};

    printf("Enter the details of each transaction as stated:\n");
    printf("Debtor Bank, creditor Bank, and amount\n");
    printf("The transactions can be in any order\n");
    for (int i = 0; i < numTransactions; i++) {
        printf("transaction %d : ", i+1);
        char s1[50], s2[50];
        int amount;
        scanf("%s %s %d", s1, s2, &amount);

        graph[i][0] = amount;
        graph[0][i] = -amount;
    }

    minimizeCashFlow(numBanks, input, numTransactions, graph, maxNumTypes);
    return 0;
}
