//
// Created by 吴小宁 on 2022/8/29.
//

#include "FrequentPhraseDetection.h"

#include <map>
#include <vector>


using namespace std;

map<vector<int>, int> FrequentPhraseDetection::frequent_phrase_detection(vector<vector<int>> documents, int k) {

    vector<int> sentence = documents[0];
    int length = sentence.size();

    map<vector<int>, int> f;  // <phrase, frequency>
    map<vector<int>,vector<int>> index;   // <word, {word出现的地方}>

    // 统计每个词汇出现的地点
    for(int i = 0; i < length; i++) {
        int word = sentence[i];
        vector<int> phrase;
        phrase.emplace_back(word);
        if (index.find(phrase) != index.end()) {
            index[phrase].emplace_back(i);
        } else {
            vector<int> pos;
            pos.emplace_back(i);
            index.insert(pair<vector<int>,vector<int>>(phrase, pos));
        }
    }

    while (!index.empty()) {
        map<vector<int>,vector<int>> new_index;
        // for u 属于 index.keys
        for (map<vector<int>, vector<int>>::iterator iter = index.begin(); iter != index.end(); iter ++) {
            if (iter->second.size() >= k) {
                f.insert(pair<vector<int>, int>(iter->first, index[iter->first].size()));
                for (auto j = iter->second.begin(); j != iter->second.end(); j++) {
                    if ((*j) < sentence.size()) {
                        vector<int> new_phrase;
                        new_phrase.assign(iter->first.begin(), iter->first.end());
                        new_phrase.emplace_back(sentence[(*j) + 1]);
                        // index'[u'] <-- index'[u'] U {j+1}
                        if (new_index.find(new_phrase) != index.end()) {
                            new_index[new_phrase].emplace_back((*j) + 1);
                        } else {
                            vector<int> pos;
                            pos.emplace_back((*j) + 1);
                            new_index.insert(pair<vector<int>, vector<int>>(new_phrase, pos));
                        }
                    }
                    }


            }
        }
        index.swap(new_index);
    }
    return f;

}