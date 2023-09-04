//
// Created by 吴小宁 on 2/10/23.
//

#include "pcta.h"
#include <vector>
#include <map>
#include <climits>
#include <set>


/**
     * @param freq_lists {[term,freq],[term,freq]...},{[term,freq],[term,freq]...}
     * @param threshold
     * @return
     */
map<int,int> pcta::sorted_list_query(vector<vector<vector<int>>> freq_lists, int threshold) {
    map<int,int> result;
    int current_pos = 0;
    int size = freq_lists.size();
    int total_upper_bound = INT_MAX;

    // 横向：R1, R2, ... Rn, lower bound, upper bound   纵向：x1, x2, ..., xn
    map<int, vector<int>> term2frequency;
    int current_level = 0;
    int max_size = 0;
    for (int j = 0; j < size; j++) {
        int current_list_size = freq_lists[j].size();
        if (max_size < current_list_size) {
            max_size = current_list_size;
        }
    }
    for (int i = 0; i < max_size; i++) { // 循环每一层
        /* 遍历每个list, 记录当前层 term的出现情况 */
        for (int j = 0; j < size; j++) {
            int current_list_size = freq_lists[j].size();
            if (i >= current_list_size) {  // 当前list已经结束了
                continue;
            }
            int term = freq_lists[j][i][0];
            int freq = freq_lists[j][i][1];
            /* 统计词频 */
            if (term2frequency.empty() || term2frequency.count(term) == 0) { // current phrase没有被统计过
                // 纵向：x1, x2, ..., xn
                vector<int> count;
                for (int k = 0; k < size; k++) {
                    if (k == j) {
                        count.emplace_back(freq);
                    } else {
                        count.emplace_back(0);
                    }
                }
                // lower bound & upper bound
                count.emplace_back(0);
                count.emplace_back(0);
                term2frequency.insert(
                        pair<int, vector<int>>(term, count));
            } else {
                term2frequency[term][j] = term2frequency[term][j] + freq;
            }
        }
        /* 更新LB & UB */
        for (auto iter = term2frequency.begin(); iter != term2frequency.end(); ++iter) {
            int lower_bound = 0, upper_bound = 0;
            for (int j = 0; j < size; j++) {
                int current_list_size = freq_lists[j].size();
                lower_bound += iter->second[j];
                if (iter->second[j] == 0) {
                    if (i < current_list_size) {
                        upper_bound += freq_lists[j][i][1];
                    } else {
                        upper_bound += freq_lists[j][current_list_size-1][1];
                    }
                } else {
                    upper_bound += iter->second[j];
                }
            }
            iter->second[size] = lower_bound;
            iter->second[size+1] = upper_bound;
            if (total_upper_bound > upper_bound) {
                total_upper_bound = upper_bound;
            }

        }
        /* 中止判断 */
        if (total_upper_bound < threshold) {
            current_level = i;
            break;
        }
    }
    /* 回溯检查 */
    for (auto iter = term2frequency.begin(); iter != term2frequency.end(); ++iter) {
        int term = iter->first;
        if (iter->second[size] >= threshold) {  // 如果LB>K,直接加入result
            result.insert(pair<int,int>(term,iter->second[size]));
        } else if (iter->second[size + 1] >= threshold) {  // 如果 LB < K & UB >K, 再去确认一番
            int frequency = 0;
            for (int j = 0; j < size; j++) {
                if (iter->second[j] != 0) {
                    frequency += iter->second[j];
                } else {
                    // 检查一下会不会在之后的level出现
                    for (int i = current_level+1; i < freq_lists[j].size(); i++) {
                        if (freq_lists[j][i][0] == term) {
                            frequency += freq_lists[j][i][1];
                            break;
                        }
                    }
                }
            }
            if (frequency >= threshold) {
                result.insert(pair<int,int>(term,frequency));
            }
        }
    }

    return result;
}

vector<vector<int>> pcta::term2phrase(map<int, int> term2freq, vector<vector<int>> *documents) {
    vector<vector<int>> phrases;
    vector<vector<int>> term2freq_list;
    for (auto iter = term2freq.begin(); iter != term2freq.end(); iter++) {
        vector<int> pair;
        pair.emplace_back(iter->first);
        pair.emplace_back(iter->second);
        term2freq_list.emplace_back(pair);
    }
    vector<int> phrase;
    for (vector<int> &sentence: *documents) {
        int left_p = 0, right_p = 0;
        int size = sentence.size();
        while (right_p < size) {
            if (pcta::is_freq(&term2freq_list, sentence[right_p])) {  // 满足条件 继续往后
                phrase.emplace_back(sentence[right_p]);
            } else {  // 不满足条件，从头来过
                if (phrase.size() != 0) {  // 把之前的存起来
                    vector<int> store_phrase;
                    store_phrase.swap(phrase);
                    phrases.emplace_back(store_phrase);
                }
            }
            right_p ++;
        }
        vector<int> store_phrase;
        store_phrase.swap(phrase);
        phrases.emplace_back(store_phrase);
    }
    vector<vector<int>> res;
    res.insert(res.end(), phrases.begin(), phrases.end());
    set<vector<int>> new_res(res.begin(), res.end());
    phrases.assign(new_res.begin(), new_res.end());
    return phrases;
}


vector<vector<int>> pcta::count_freq(vector<vector<int>> *documents) {
    map<int,int> term2freq;
    for (int i = 0; i < documents->size(); i++) {
        for (int j = 0; j < (*documents)[i].size(); j++) {
            int term = (*documents)[i][j];
            if (term2freq.count(term) == 0) {
                term2freq.insert(pair<int,int>(term,1));
            } else {
                term2freq[term] = term2freq[term] + 1;
            }
        }
    }
    vector<vector<int>> freq_list;
    for (auto iter = term2freq.begin(); iter != term2freq.end(); iter++) {
        vector<int> pair;
        pair.emplace_back(iter->first);
        pair.emplace_back(iter->second);
        freq_list.emplace_back(pair);
    }
    return freq_list;
}

bool pcta::is_freq(vector<vector<int>> *freq_list, int term) {
    int size = freq_list->size();
    for (int i = 0; i < size; i++) {
        if (term == (*freq_list)[i][0]) {
            return true;
        }
    }
    return false;
}

