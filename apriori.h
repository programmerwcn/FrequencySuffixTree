#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>

using namespace std;
class Apriori{
public:
    int minSupport;
    unordered_map<int, vector<int>> haystack;
    map<vector<int>, int> supportLabel;
    vector<vector<int>> freqSet;
    int last_index;
    Apriori():last_index(0){}
    int add_string(std::vector<std::vector<int>> inputs){
        for(auto input:inputs){
            last_index++;
            haystack[last_index] = input;
        }
        return last_index;
    }
    vector<vector<int>> mining2(int threshold){
        minSupport = threshold;
        vector<vector<int>> freq = AprioriGenC1();
        int length = 2;
        while (!freq.empty()){
            freqSet.insert(freqSet.end(),freq.begin(),freq.end());
            AprioriGenCN(length++,freq);
        }
        return freqSet;
    }
    vector<vector<int>> mining(int threshold){
        minSupport = threshold;
        vector<vector<int>> freq = AprioriGenC1();
        while (!freq.empty()){
            freqSet.insert(freqSet.end(),freq.begin(),freq.end());
            AprioriGenCN(freq);
        }
//        cout<<freqSet.size()<<endl;
        return freqSet;
    }
    vector<vector<int>> AprioriGenC1(){
        vector<int> v;
        vector<vector<int>> freq;
        for(auto words:haystack){
            for(auto word:words.second){
                v.push_back(word);
                if (supportLabel.count(v)>0){
                    supportLabel[v]++;
                }else{
                    supportLabel[v]=1;
                }
                v.pop_back();
            }
        }
        for (auto m:supportLabel){
            if(m.second>=minSupport){
                freq.push_back(m.first);
            }
        }
        return freq;
    }

    bool CanGen(const vector<int> &a, const vector<int> &b)
    {
        for(int i=1;i<a.size()-1;i++){
            if(a[i]!=b[i]){
                return false;
            }
        }
        return true;
    }

    bool isSubset(vector<int> &sentence, vector<int> &words){
        for(vector<int>::iterator iterA = sentence.begin();iterA<sentence.end();iterA++){
            vector<int>::iterator iterC = words.begin();
            for( vector<int>::iterator iterB = iterA;iterB!=sentence.end()&&iterC!=words.end();iterB++,iterC++){
                if(*iterB!=*iterC){
                    break;
                }
            }
            if(iterC==words.end()){
                return true;
            }
        }
        return false;
    }

    void AprioriGenCN(int length,vector<vector<int>> &freq){
        //generate canidate
        supportLabel.clear();
        freq.clear();
        for(auto sentence:haystack){
            if(sentence.second.size()<length){
                continue;
            }
            vector<int> v;
            int i = 0;
            for( ;i<length;i++){
                v.push_back(sentence.second[i]);
            }
            if (supportLabel.count(v)>0){
                supportLabel[v]++;
            }else{
                supportLabel[v]=1;
            }
            while (i<sentence.second.size()){
                v.erase(v.begin());
                v.push_back(sentence.second[i++]);
                if (supportLabel.count(v)>0){
                    supportLabel[v]++;
                }else{
                    supportLabel[v]=1;
                }
            }

        }
        freq.clear();
        //Delete values thathaiz are less than the threshold
        for (auto m:supportLabel){
            if(m.second>=minSupport){
                freq.push_back(m.first);
            }
        }
        return;
    }

    void AprioriGenCN(vector<vector<int>> &newFreqSet){
        //generate canidate
        supportLabel.clear();
        for (vector<vector<int>>::iterator iterA = newFreqSet.begin(); iterA != newFreqSet.end(); ++iterA){
            for(vector<vector<int>>::iterator iterB = iterA+1;iterB!=newFreqSet.end();++iterB){
                if (CanGen(*iterA,*iterB)){
                    vector<int> tmp(*iterA);
                    tmp.push_back(iterB->back());
                    supportLabel[tmp] = 0;
                }
            }
        }
        //calculate frequency
        for(map<vector<int>,int>::iterator ma = supportLabel.begin(); ma != supportLabel.end(); ++ma){
            for(auto sentence:haystack){
                if (isSubset(sentence.second, const_cast<vector<int> &>(ma->first))){
                    ma->second++;
                }
            }
        }
        newFreqSet.clear();
        //Delete values that are less than the threshold
        for (auto m:supportLabel){
            if(m.second>=minSupport){
                newFreqSet.push_back(m.first);
            }
        }
        return;
    }
};