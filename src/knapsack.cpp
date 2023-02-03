#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stack>
#include <unordered_set>

using namespace std;

bool sortDescend(const pair<double, int>& a, const pair<double, int>& b){
    return(a.first > b.first);
}

struct bnbNode{
    bnbNode* parent;
    int currVal = 0;
    int remCapacity = 0;
    int level = 0;
    bool chosen = 0;
};

void traceChosenItems(bnbNode* lastNode, vector<bool>& selected){
    bnbNode* t = lastNode;
    int i = selected.size() -1;
    while(t != nullptr){
        if(t->chosen == 0){
            selected[i] = 0;
        }else{
            selected[i] = 1;
        }
        t = t->parent;
        i--;
    }
}


float bound(vector<pair<int, int>>& items, vector<pair<double, int>>& relaxationArr,
            int currVal, int remCapacity, int level){
    
    float expect = currVal;
    while(remCapacity > 0 && level < items.size()){
        int itemIdx = relaxationArr[level].second;
        pair<int, int> item = items[itemIdx];
        if(item.first > remCapacity){
            expect += relaxationArr[level].first * remCapacity;
        }else{
            expect += item.second;
            remCapacity -= item.first;
        }
        level++;
    }

    return expect;
}


int bnbSolver(vector<pair<int, int>>& items, int capacity, 
                                vector<bool>& selectedItems){
    
    int numItems = items.size();
    //Vector with all values from linear relaxation
    vector<pair<double, int>> relaxationArr;
    for(int i = 0; i < items.size(); i++){
        if(items[i].first > capacity){ continue; }

        float value = (float)items[i].second / items[i].first;
        relaxationArr.push_back(make_pair(value, i));
    }
    sort(relaxationArr.begin(), relaxationArr.end(), sortDescend);

    bnbNode* wFirst = new bnbNode;
    wFirst->parent = nullptr;
    wFirst->chosen = 1;
    int itemIdx = relaxationArr[0].second;
    wFirst->currVal = items[itemIdx].second;
    wFirst->remCapacity = capacity - items[itemIdx].first;
    
    bnbNode* woutFirst = new bnbNode;
    woutFirst->parent = nullptr;
    woutFirst->chosen = 0;
    woutFirst->currVal= 0;
    woutFirst->remCapacity = capacity;

    stack<bnbNode*> s;
    s.push(woutFirst); s.push(wFirst);
    int best = -1;
    bnbNode* selectedNode = nullptr;
    while(!s.empty()){
        bnbNode* n = s.top();
        s.pop();
        int level = n->level + 1;
        int currVal = n->currVal;
        int remCapacity = n->remCapacity;

        if(level < numItems){
            itemIdx = relaxationArr[level].second;
            int itemWeight = items[itemIdx].first;
            int itemVal = items[itemIdx].second;
            if(best == -1){
                bnbNode* woutItem = new bnbNode;
                woutItem->chosen = 0;
                woutItem->currVal = currVal;
                woutItem->level = level;
                woutItem->parent = n;
                woutItem->remCapacity = remCapacity;
                s.push(woutItem);

                if(remCapacity >= itemWeight){
                    bnbNode* wItem = new bnbNode;
                    wItem->currVal = currVal + itemVal;
                    wItem->chosen = 1;
                    wItem->level = level;
                    wItem->parent = n;
                    wItem->remCapacity = remCapacity - itemWeight;
                    s.push(wItem);
                }
            }
            else{
                if(level < numItems - 1){
                    float expectWout = bound(items, relaxationArr, currVal, remCapacity, level + 1);
                    if(expectWout > best){
                        bnbNode* woutItem = new bnbNode;
                        woutItem->chosen = 0;
                        woutItem->currVal = currVal;
                        woutItem->level = level;
                        woutItem->parent = n;
                        woutItem->remCapacity = remCapacity;
                        s.push(woutItem);
                    }
                }

                float expectWith = bound(items, relaxationArr, currVal, remCapacity, level);
                if(remCapacity >= itemWeight && expectWith > best){
                    bnbNode* wItem = new bnbNode;
                    wItem->currVal = currVal + itemVal;
                    wItem->chosen = 1;
                    wItem->level = level;
                    wItem->parent = n;
                    wItem->remCapacity = remCapacity - itemWeight;
                    s.push(wItem);
                }                     
            }
        }
        else{
            if(currVal > best){
                best = currVal;
                selectedNode = n;
            }
        }
    }

    traceChosenItems(selectedNode, selectedItems);

    return best;
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