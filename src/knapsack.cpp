#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

bool sortDescend(const pair<double, int>& a, const pair<double, int>& b){
    return(a.first > b.first);
}


double bound(vector<pair<int, int>>& items, vector<pair<double, int>>& relaxationArr,
            int currVal, int remCapacity, int level){
    
    double expect = currVal;
    while(remCapacity > 0 && level < items.size()){
        int itemIdx = relaxationArr[level].second;
        pair<int, int> item = items[itemIdx];
        if(item.first > remCapacity){
            expect += relaxationArr[level].first * remCapacity;
            remCapacity = 0;
        }else{
            expect += item.second;
            remCapacity -= item.first;
        }
        level++;
    }

    return expect;
}

void bnbRecursive(vector<bool>& thisSolution, vector<bool>& bestSolution, int& optimal,
        int level, int currVal, int remCapacity, vector<pair<int, int>>& items,
        vector<pair<double, int>>& relaxationArr){
    level++;
    int numItems = items.size();
    if(level < numItems){
        int itemIdx = relaxationArr[level].second;
        int itemWeight = items[itemIdx].first;
        int itemVal = items[itemIdx].second;
        if(optimal == -1){
            if(remCapacity >= itemWeight){
                thisSolution[level] = 1;
                int newCurrVal = currVal + itemVal;
                int newRemCapacity = remCapacity - itemWeight;
                bnbRecursive(thisSolution, bestSolution, optimal,
                        level, newCurrVal, newRemCapacity, items, relaxationArr);
            }

            thisSolution[level] = 0;
            bnbRecursive(thisSolution, bestSolution, optimal,
                        level, currVal, remCapacity, items, relaxationArr);
        }
        else{
            double expectWith = bound(items, relaxationArr, currVal, remCapacity, level);
                if(remCapacity >= itemWeight && expectWith > optimal){
                    thisSolution[level] = 1;
                    int newCurrVal = currVal + itemVal;
                    int newRemCapacity = remCapacity - itemWeight;
                    bnbRecursive(thisSolution, bestSolution, optimal,
                                level, newCurrVal, newRemCapacity, items, relaxationArr);
                }
                
            double expectWout = bound(items, relaxationArr, currVal, remCapacity, level + 1);
            if(expectWout > optimal){
                thisSolution[level] = 0;
                bnbRecursive(thisSolution, bestSolution, optimal,
                            level, currVal, remCapacity, items, relaxationArr);
            } 
        }
    }
    else{
        if(currVal > optimal){
            optimal = currVal;
            for(int i = 0; i < numItems; i++){
                bool s = thisSolution[i];
                int idx = relaxationArr[i].second;
                bestSolution[idx] = s;
            }
        }
    }
}

int bnbSolver(vector<pair<int, int>>& items, int capacity, 
                                vector<bool>& selectedItems){
    
    int numItems = items.size();
    //Vector with all values from linear relaxation
    vector<pair<double, int>> relaxationArr;
    for(int i = 0; i < items.size(); i++){
        if(items[i].first > capacity){ continue; }

        double value = (double)items[i].second / items[i].first;
        relaxationArr.push_back(make_pair(value, i));
    }
    sort(relaxationArr.begin(), relaxationArr.end(), sortDescend);
    int optimal = -1;
    vector<bool> thisSolution(numItems, 0);
    bnbRecursive(thisSolution, selectedItems, optimal, -1, 0, capacity, items, relaxationArr);
    return optimal;
}


int dynamicProgrammingSolver(vector<pair<int, int>>& items, int capacity, 
                            vector<bool>& selectedItems){

    int numItems = items.size();
    vector<vector<int>> mtx(capacity + 1, vector<int>(numItems + 1, 0));
    for(int j = 1; j <= numItems; j++){
        int itemWeight = items[j - 1].first;
        int i = itemWeight;
        int itemVal = items[j - 1].second;

        //Completing the collum
        for(int k = 0; k < i; k ++){
            mtx[k][j] = mtx[k][j - 1];
        }

        //Test whether to add the item or not
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
    while(j != 0){
        if(mtx[i][j] == mtx[i][j - 1]){
            j --;
        }
        else{
            selectedItems[j - 1] = 1;
            int itemWeight = items[j - 1].first;
            i -= itemWeight; j--;
        }
    }
    return optimal;
}

int main(int argc, char* argv[]){
    string fName = argv[1];
    fstream f(fName, fstream::in | fstream::out);
    int numItems, capacity;
    f >> numItems; f >> capacity;
    vector<bool> selectedItems(numItems, 0);

    vector<pair<int, int>> items(numItems, make_pair(0, 0));
    for(int i = 0; i < numItems; i++){
            f >> items[i].second;
            f >> items[i].first;
    }

    //int sol = dynamicProgrammingSolver(items, capacity, selectedItems);
    int sol = bnbSolver(items, capacity, selectedItems);
    cout << sol << " " << 1 << endl;

    for(int i = 0; i < selectedItems.size(); i++){
        cout << selectedItems[i] << " ";
    }
    cout << endl;
}

