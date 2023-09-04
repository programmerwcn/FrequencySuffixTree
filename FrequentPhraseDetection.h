//
// Created by 吴小宁 on 2022/8/29.
//

#ifndef UNTITLED_FREQUENTPHRASEDETECTION_H
#define UNTITLED_FREQUENTPHRASEDETECTION_H


#include <map>
#include <vector>

using namespace std;

class FrequentPhraseDetection {
public:
    static map<vector<int>, int> frequent_phrase_detection(vector<vector<int>> documents, int k);


};


#endif //UNTITLED_FREQUENTPHRASEDETECTION_H
