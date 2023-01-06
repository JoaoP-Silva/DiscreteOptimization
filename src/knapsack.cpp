#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <map>

using namespace std;

struct bnbNode{
    bnbNode* parent;
    int expectedValue;
    int chosenValuesSum;
    bool selectedItem;

    bnbNode(bnbNode* p, int expected, bool selected) : parent(p), expectedValue(expected),
                                                        selectedItem(selected) {}
};

int bnbSolver(vector<pair<int, int>>& items, int capacity, 
                            vector<bool>& selecteditems){
    
    int numItems = items.size();
    //Map with all values from linear relaxation
    map<int, vector<int>> relaxationMap;
    for(int i = 0; i < items.size(); i++){
        float value = (float)items[i].second / items[i].first;
        auto it = relaxationMap.find(value);
        if(it != relaxationMap.end()){
            it->second.push_back(i); 
        }
        else{
            vector<int> vec; vec.push_back(i);
            relaxationMap.insert(make_pair(value, vec));
        }
    }
    int initialExpect = 0;
    int remWeight = capacity;
    for(auto it = relaxationMap.begin(); it != relaxationMap.end(); it++){
        for(int i : it->second){
            int itemWeight = items[i].first;
            if(itemWeight > remWeight){
                
            }
        }
    }
}

int dynamicProgrammingSolver(vector<pair<int, int>>& items, int capacity, 
                            vector<bool>& selecteditems){

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
            selecteditems[j - 1] = 1;
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
    vector<bool> selecteditems(numItems, 0);

    vector<pair<int, int>> items(numItems, make_pair(0, 0));
    for(int i = 0; i < numItems; i++){
            f >> items[i].second;
            f >> items[i].first;
    }

    

    for(int i = 0; i < selecteditems.size(); i++){
        cout << selecteditems[i] << " ";
    }
    cout << endl;
}