#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stack>
#include <unordered_set>

using namespace std;

struct bnbNode{
    unordered_set<int> selectedItems;
    unordered_set<int> unselectedItems;
    int val = 0;
    int weight = 0;
    int itemIdx = -1;
};

int bnbSolver(vector<pair<int, int>>& items, int capacity, 
                                vector<bool>& selectedItems){
    
    int numItems = items.size();
    //Map with all values from linear relaxation
    vector<pair<int, int>> relaxationArr;
    for(int i = 0; i < items.size(); i++){
        float value = (float)items[i].second / items[i].first;
        relaxationArr.push_back(make_pair(value, i));
    }
    sort(relaxationArr.begin(), relaxationArr.end(), greater<>());

    int remWeight = capacity, initialExpect = 0;
    for(pair<int, int> p : relaxationArr){
        int i = p.second, itemWeight = items[i].first, itemVal = items[i].second;
        if(remWeight >= itemWeight){
            remWeight -= itemWeight;
            initialExpect += itemVal;
        }else{
            int val = p.first * remWeight;
            initialExpect += val;
            break;
        }
    }

    stack<bnbNode> bnbStack;
    bnbNode withFirst, withoutFirst;
    withoutFirst.unselectedItems.insert(0);
    withoutFirst.itemIdx = 0;
    bnbStack.push(withoutFirst);

    withFirst.selectedItems.insert(0);
    withFirst.itemIdx = 0;
    withFirst.val = items[0].second;
    withFirst.weight = items[0].first;
    bnbStack.push(withFirst);
    int best = -1;
    unordered_set<int> bestArr;
    while(!bnbStack.empty()){
        bnbNode n = bnbStack.top(); bnbStack.pop();
        if(n.itemIdx < items.size() -1){
            n.itemIdx += 1;
            int i = n.itemIdx;
            int itemVal = items[i].second;
            int itemWeight = items[i].first;
            remWeight = capacity - n.weight;
            if(itemWeight <= remWeight){
                unordered_set<int> cpySelected = n.selectedItems;
                cpySelected.insert(i);
                int b = bound(items, cpySelected, n.unselectedItems, relaxationArr, capacity);
                if(b > best){
                    bnbNode newNode = n;
                    newNode.selectedItems = cpySelected;
                    newNode.weight += itemWeight;
                    newNode.val += itemVal;
                    bnbStack.push(newNode);
                }
            }
            n.unselectedItems.insert(i);
            int b = bound(items, n.selectedItems, n.unselectedItems, relaxationArr, capacity);
            if(b > best){
                bnbStack.push(n);
            }
        }else{
            if(n.val > best){
                best = n.val; bestArr = n.selectedItems;
            }
        }
    }

    for(auto i : bestArr){
        selectedItems[i] = 1;
    }
}

int bound(vector<pair<int, int>>& items,unordered_set<int>& selected, unordered_set<int>& unselected, 
                                                    vector<pair<int, int>>& density, int& capacity){
    
    int val = 0, weight = 0;
    for(auto i : selected){
        weight += items[i].first;
        val += items[i].second;
    }
    
    for(auto item : density){
        int key = item.second;
        if(selected.find(key) != selected.end() || unselected.find(key) != unselected.end()){
            continue;
        }
        int itemWeight = items[key].first, itemVal = items[key].second, remWeight = capacity - weight;
        if(remWeight >= itemWeight){
            val += itemVal;
            weight += itemWeight;
        }else{
            val += item.first * remWeight;
            break;
        }
    }

    return val;
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
    cout << optimal << " " << "1" << endl;
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

    

    for(int i = 0; i < selectedItems.size(); i++){
        cout << selectedItems[i] << " ";
    }
    cout << endl;
}