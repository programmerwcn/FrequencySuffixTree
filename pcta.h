//
// Created by 吴小宁 on 2/10/23.
//

#ifndef UNTITLED_PCTA_H
#define UNTITLED_PCTA_H

#include <vector>
#include <map>

using namespace std;
class pcta {
   vector<vector<int>> pc2list ;
public:
    static map<int,int>  sorted_list_query(vector<vector<vector<int>>> freq_lists, int threshold);

    static vector<vector<int>> term2phrase(map<int,int>term2freq, vector<vector<int>> *documents);

    static vector<vector<int>> count_freq(vector<vector<int>> *documents);

    static bool is_freq(vector<vector<int>> *freq_list, int term);

};


#endif //UNTITLED_PCTA_H
