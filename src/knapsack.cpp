#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

using namespace std;

int main(int argc, char* argv[]){
    string fName = argv[1];
    fstream f(fName, fstream::in | fstream::out);
    int numItems, capacity;
    f >> numItems; f >> capacity;
    vector<bool> selectedItens(numItems, 0);

    vector<pair<int, int>> itens(numItems, make_pair(0, 0));
    for(int i = 0; i < numItems; i++){
            f >> itens[i].second;
            f >> itens[i].first;
    }

    vector<vector<int>> mtx(capacity + 1, vector<int>(numItems + 1, 0));
    for(int j = 1; j <= numItems; j++){
        int itemWeight = itens[j - 1].first;
        int i = itemWeight;
        int itemVal = itens[j - 1].second;

        //Completing the collum
        for(int k = 0; k < i; k ++){
            mtx[k][j] = mtx[k][j - 1];
        }

        //Test if adds the item or not
        while(i <= capacity){
            int valWithoutItem = mtx[i][j - 1];
            int valWithItem = itemVal + mtx[i - itemWeight][j - 1];
            if(valWithItem > valWithoutItem){
                mtx[i][j] = valWithItem;
            }
            else{
                mtx[i][j] = valWithoutItem;
                if(j < numItems){
                    mtx[i][j + 1] = valWithItem;
                }
            }
            i++;
        }
    }

    int i = capacity, j = numItems;
    int optimal = mtx[i][j];
    cout << optimal << " " << "1" << endl;
    while(j != 0){
        if(mtx[i][j] == mtx[i][j - 1]){
            j --;
        }
        else{
            selectedItens[j - 1] = 1;
            int itemWeight = itens[j - 1].first;
            i -= itemWeight; j--;
        }
    }

    for(int i = 0; i < selectedItens.size(); i++){
        cout << selectedItens[i] << " ";
    }
    cout << endl;
}