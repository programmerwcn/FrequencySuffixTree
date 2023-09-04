//
// Created by 王璐 on 2020/12/19.
//

#ifndef SUFFIX_TREE_MASTER_FREQUENCY_H
#define SUFFIX_TREE_MASTER_FREQUENCY_H


class frequency {
public:
    int term;
    int freq;
    frequency(int term,int freq):freq(freq),term(term){}
};

class phrase_frequency {
public:
    std::vector<int> phrase;
    int freq;
    phrase_frequency(std::vector<int> phrase, int freq) {
        for (int term: phrase) {
            this->phrase.emplace_back(term);
        }
        this->freq = freq;
    }

};


#endif //SUFFIX_TREE_MASTER_FREQUENCY_H
