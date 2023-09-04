#include "apriori.h"
#include "apriori2.h"
#include "QuadTrees.h"
#include <fstream>
#include <string>
#include <stdio.h>
#include <dirent.h>
#include "FrequentPhraseDetection.h"
#include "rstartreeold.h"
#include <malloc.h>
#include "pcta.h"

//#include "RStarTree.h"





using namespace std;

static Storage st;
static RTree rtree(&st);

int SuffixTree::maxId = 0;

string path = "/home/wcn/";

void demo();
void test_map();
void selerailize(string path);
void test3(string path,int threshold, int first, int last);


void test_construction();
void view_size(string path);
vector<double> test_single_tree(string path, int threshold);
void test_category_tree(string path, int threshold, int start_category, int end_category);
void test_date_tree(string path, int threshold, int start_date, int end_date);
void test_load();
void test_mining_tree(int threshold);
void build_rtree(string dataset, int bf, float ff);
vector<Object>  query_rtree(int is_leaf, string query);
void mining_phrases(vector<vector<Object>> objects, int threshold, int algorithm, string dataset);
void mining_phrases(vector<vector<Object>> objects, int threshold, int algorithm, string dataset, int partial_storage);

void insert_objects_inner_node();
void insert_objects_leaf_node();
/* 从文件中读出时空数据对象，根据空间数据构建R-Tree */
void build_rtree_from_file();
void test_query_inner_node();
void test_query_leaf_node();
void test_suffix_tree_query();
void test_single_suffix();
void test_multi_suffix();
void build_rtree_from_file_4sq();
void test_single_suffix_4sq(int k);
void test_multi_suffix_4sq(int k);

void test_single_sorted_suffix(int k);

void test_multi_sorted_suffix(int k);

void query();

void serialization(string path);

bool parseLine(string &line, Rectangle &mbr);

void test_suffix_storage();

//void test_full_storage();

//void test_full_storage() {
//    build_rtree("yelp_dataset", 30, 0.8);
//}

int main() {
    build_rtree("GeoText", 30, 0.8);
    int height = rtree.get_height();
    cout << height << endl;
//    int height = r

//    serialization("/home/wcn/data-process/GeoText");


//    freopen("/home/wcn/work_code/out.txt", "w", stdout);


//    query();
//    while (1) {
//
//    }

    return 0;
}


void test_suffix_storage() {
    vector<SuffixTree *> trees;
    for (int j = 0; j < 1; j++) {
        for (int i = 0; i < 1; i++) {
            SuffixTree *stree = new SuffixTree();
//            vector<ctnr> documents = readFile("../test/model/1");
            vector<ctnr> documents = readFile(path + "data-process/GeoText/basic_model/" + to_string(i));
            stree->add_string(documents);
            stree->dump_tree();
            stree->sort_tree_new();
            trees.emplace_back(stree);
        }
    }
    for (SuffixTree * tree: trees) {
        tree->partial_storage(0);
    }

}

bool list_sort(vector<int> a, vector<int> b) {
return (a[1] > b[1]);
}

void mining_phrases(vector<vector<Object>> objects, int threshold, int algorithm, string dataset) {
    vector<ctnr> result;
    clock_t start, end;
    clock_t query_time = 0, construction_time = 0;
    switch (algorithm) {
        case 1: // apriori
        {

            Apriori *apriori = new Apriori();
            for (vector<Object> &current_object_group: objects) {
                for (Object current_object: current_object_group) {
                    vector<ctnr> documents = readFile(
                            "/home/wcn/data-process/" + dataset + "/basic_model/" + to_string(current_object.id));
                    start = clock();
                    apriori->add_string(documents);
                    construction_time += clock() - start;
                }
            }
            start = clock();
            result = apriori->mining(threshold);
            end = clock();
            query_time = end - start;

            /* 部分存储 :
             * 1. 对documents进行切割，得到小块的results
             * 2. 对于小块的results进行合并 */

            break;
        }
        case 2:  // mining frequency suffix tree
        {
            vector<SuffixTree *> tree_pointers;
            vector<ctnr> res;
            int tree_id = 0;
            for (vector<Object> &current_object_group: objects) {
                SuffixTree *stree = new SuffixTree();
                stree->id = tree_id++;
                for (Object current_object: current_object_group) {
                    vector<ctnr> documents = readFile(
                            "/home/wcn/data-process/" + dataset + "/basic_model/" + to_string(current_object.id));
                    start = clock();
                    stree->add_string(documents);
                    construction_time += clock() - start;
                }
                start = clock();
                stree->dump_tree();
                construction_time += clock() - start;
                if (stree->haystack.size() != 0) {
                    tree_pointers.emplace_back(stree);
                }
            }
            // 查询所有suffix tree
            start = clock();
            result = SuffixTree::mining_tree_add_threshold(tree_pointers, threshold);
            query_time = clock() - start;
            cout << "suffix tree num: " << tree_pointers.size() << "  " << "result num: " << result.size() << " ";
            cout << res.size();
            break;
        }

        case 3: // mining sorted frequency suffix tree
        {
            vector<SuffixTree *> tree_pointers;
            vector<ctnr> res;
            int tree_id = 0;
            for (vector<Object> &current_object_group: objects) {
                SuffixTree *stree = new SuffixTree();
                stree->id = tree_id++;
                for (Object current_object: current_object_group) {
                    vector<ctnr> documents = readFile(
                            "/home/wcn/data-process/" + dataset + "/basic_model/" + to_string(current_object.id));
                    start = clock();
                    stree->add_string(documents);
                    construction_time += clock() - start;
                }

                if (stree->haystack.size() != 0) {
                    start = clock();
                    stree->dump_tree();
                    stree->sort_tree_new();
                    construction_time += clock() - start;
                    tree_pointers.emplace_back(stree);
                }
            }
            // 查询所有suffix tree
            start = clock();
            result = SuffixTree::mining_sorted_tree_add_threshold(tree_pointers, threshold);
            query_time = clock() - start;
            cout << "suffix tree num: " << tree_pointers.size() << "  " << "result num: " << result.size() << " ";
            break;
        }
        case 4:  // PCTA
        {
            vector<vector<vector<int>>> freq_lists;
            vector<vector<int>> documents;
            for (vector<Object> &current_object_group: objects) {
                map<int,int> freq_map;
                vector<vector<int>> freq_list;
                vector<vector<int>> merged_doc;
                for (Object current_object: current_object_group) {
//                    vector<vector<int>> current_freq = readFile(
//                            "/home/wcn/data-process/" + dataset + "/freq/" + to_string(current_object.id));
                    vector<vector<int>> current_doc = readFile(
                            "/home/wcn/data-process/" + dataset + "/basic_model/" + to_string(current_object.id));

//                    start = clock();
//                    construction_time += clock() - start;
                    start = clock();
//                    for (auto iter = current_freq.begin(); iter != current_freq.end(); iter++) {
//                        int word = (*iter)[0];
//                        int count = (*iter)[1];
//                        if (freq_map.count(word) != 0) {
//                            freq_map[word] = freq_map[word] + count;
//                        } else {
//                            freq_map.insert(pair<int,int>(word, count));
//                        }
//                    }
//                    construction_time += clock() - start;
//                    freq_lists.emplace_back(current_freq);
                    documents.insert(documents.end(), current_doc.begin(), current_doc.end());
                    merged_doc.insert(merged_doc.end(), current_doc.begin(), current_doc.end());
                    construction_time += clock()-start;
                }
                start = clock();
                freq_list = pcta::count_freq(&merged_doc);
                sort(freq_list.begin(), freq_list.end(), list_sort);
                freq_lists.emplace_back(freq_list);
                query_time += clock() - start;

//                for (auto iter = freq_map.begin(); iter != freq_map.end(); iter ++) {
//                    vector<int> value2freq;
//                    value2freq.emplace_back(iter->first);
//                    value2freq.emplace_back(iter->second);
//                    freq_list.emplace_back(value2freq);
//                }
//                start = clock();
//                sort(freq_list.begin(), freq_list.end(), list_sort);
//                construction_time += clock() - start;

            }
            start = clock();
            map<int, int> freq_terms = pcta::sorted_list_query(freq_lists, threshold);
            result = pcta::term2phrase(freq_terms, &documents);
            query_time = clock() - start;
            cout << "result num: " << result.size() << " ";
//            cout << "apriori" << res2.size();

        }
    }
    cout << "construction-time: " << double(construction_time) / CLOCKS_PER_SEC << "  query-time: "
         << double(query_time) / CLOCKS_PER_SEC << endl;
}

void mining_phrases(vector<vector<Object>> objects, int threshold, int algorithm, string dataset, int partial_storage) {
    vector<ctnr> result;
    clock_t start, end;
    clock_t query_time = 0, construction_time = 0;

    vector<SuffixTree *> tree_pointers;
    vector<ctnr> res;
    int tree_id = 0;
    int max = 10;
    int count = 0;
    switch (algorithm) {
        case 1: // apriori
        {
            Apriori *apriori = new Apriori();
            for (vector<Object> &current_object_group: objects) {
                for (Object current_object: current_object_group) {
                    vector<ctnr> documents = readFile(
                            "/home/wcn/data-process/" + dataset + "/basic_model/" + to_string(current_object.id));
                    start = clock();
                    apriori->add_string(documents);
                    construction_time += clock() - start;
                }
            }
            start = clock();
            result = apriori->mining(threshold);
            end = clock();
            query_time = end - start;

            /* 部分存储 :
             * 1. 对documents进行切割，得到小块的results
             * 2. 对于小块的results进行合并 */

            break;
        }
            for (vector<Object> &current_object_group: objects) {
                SuffixTree *stree = new SuffixTree();
                stree->id = tree_id++;
                for (Object current_object: current_object_group) {
                    vector<ctnr> documents = readFile(
                            "/home/wcn/data-process/" + dataset + "/basic_model/" + to_string(current_object.id));
                    stree->add_string(documents);
                }

                if (stree->haystack.size() != 0) {
                    stree->dump_tree();
                    stree->sort_tree_new();
                    if (partial_storage) {
                        vector<pair<vector<int>, int>> vec = stree->partial_storage(10);
                    }
                    tree_pointers.emplace_back(stree);
                }
                count++;
            }
            // 查询所有suffix tree
            start = clock();
            result = SuffixTree::mining_sorted_tree_add_threshold(tree_pointers, threshold);
            query_time = clock() - start;
            cout << "suffix tree num: " << tree_pointers.size() << "  " << "result num: " << result.size() << " ";
    }
}


vector<Object> query_rtree(int is_leaf, string query) {
    vector<Object> objects;
    Rectangle mbr;
    parseLine(query, mbr);
    if (is_leaf) {
        rtree.rangeQuery(objects, mbr);
    } else {
        rtree.rangeQueryInnerNodes(objects, mbr);
    }
    return objects;
}

vector<vector<Object>> query_rtree_inner(int is_leaf, string query) {
    vector<vector<Object>> objects;
    Rectangle mbr;
    parseLine(query, mbr);
    if (is_leaf) {
        rtree.rangeQuery(objects, mbr);
    } else {
        rtree.rangeQueryInnerNodes(objects, mbr);
    }

    return objects;
}

void query() {

    ifstream query_file;
    query_file.open( "/home/wcn/work_code/query", ios::in);
//
    string query_command;
    string dataset;
    int line = 0;
    clock_t r_tree_query_time = 0, start = 0;

    while (getline(query_file, query_command)) {
        char* s = new char[query_command.size() + 1];
        strcpy(s, query_command.c_str());
        char* p = strtok(s, " ");   // algorithm
        vector<string> commands;
        while(p) {
            commands.push_back(p);
            p = strtok(NULL, " ");
        }
        if (commands.size() == 3) { //build r-tree
            dataset = commands[0];
            int bf = atoi(commands[1].c_str());
            float ff = atof(commands[2].c_str());
            start = clock();
            build_rtree(dataset, bf, ff);
            r_tree_query_time += clock() - start;
        } else if (commands.size() == 6) {  // r-tree query already done
            ifstream result_file;
            int threshold = atoi(commands[4].c_str());
            int algorithm = atoi(commands[0].c_str());
            int is_leaf = atoi(commands[5].c_str());
            result_file.open("/home/wcn/work_code/rtree_result"+commands[3]);
            vector<vector<Object>> objects;
            string nodes_line;
            while (getline(result_file, nodes_line)) {
                vector<Object> current_objects;
                char* s1 = new char[nodes_line.size() + 1];
                strcpy(s1, nodes_line.c_str());
                char* p1 = strtok(s1, " ");   // algorithm
                while(p1) {
                    Object object;
                    object.id = atoi(p1);
                    current_objects.emplace_back(object);
                    p1 = strtok(NULL, " ");
                }
                objects.emplace_back(current_objects);
            }
            cout << "nodes num: " << objects.size() << "  ";
            mining_phrases(objects, threshold, algorithm, dataset);

        } else {
            string range = commands[2] + " " + commands[3] + " " + commands[4] + " " + commands[5];
            int threshold = atoi(commands[6].c_str());
            int algorithm = atoi(commands[0].c_str());
            int is_leaf = atoi(commands[7].c_str());
            start = clock();
            vector<vector<Object>> objects = query_rtree_inner(is_leaf, range);
            r_tree_query_time += clock() - start;
            ofstream result_file;
            result_file.open("/home/wcn/work_code/rtree_result" + to_string(line));
            for (vector<Object> nodes: objects) {
                for (Object node: nodes) {
                    result_file << node.id << " ";
                }
                result_file << endl;
            }
            result_file.close();
            cout << "nodes num: " << objects.size() << "  " << "rtree query time: " << double(r_tree_query_time) / CLOCKS_PER_SEC << " ";
            mining_phrases(objects, threshold, algorithm, dataset);
        }
        line ++;


    }

}

void serialization(string path) {
    string model_path = path + "/basic_model";
    string save_path = path + "/serialization";
    DIR *dir = opendir(model_path.c_str());
    struct dirent *dirp;
    while ((dirp = readdir(dir)) != nullptr) {
        string file_name = dirp->d_name;
        if (file_name[0] == '.') {
            continue;
        }
        SuffixTree stree;
        vector<ctnr> documents = readFile(model_path+"/"+file_name);
        if(documents.empty()){continue;}
//        documents.erase(documents.begin());  // 第一行是位置信息，删掉
        stree.add_string(documents);
        stree.dump_tree();
        stree.save(save_path + "/" + file_name);
    }

}

bool parseLine(string &line, Rectangle &mbr)
{
    istringstream coordinatesStream(line);
    float coord;
    vector<float> coords;
    while(coordinatesStream >> coord)
    {
        coords.push_back(coord);
    }
    switch (coords.size()) {
        case 5:
            mbr.id = (int)coords[0];
            mbr.min.x = coords[1];
            mbr.min.y = coords[2];
            mbr.max.x = coords[3];
            mbr.max.y = coords[4];
            break;
        case 4:
            mbr.min.x = coords[0];
            mbr.min.y = coords[1];
            mbr.max.x = coords[2];
            mbr.max.y = coords[3];
            break;
        case 3:
            mbr.id = (int)coords[0];
            mbr.min.x = coords[1];
            mbr.min.y = coords[2];
            mbr.max.x = coords[1];
            mbr.max.y = coords[2];
    }
    return true;
}



void printResults(vector<Object> &objects, unsigned queryNumber)
{
    cout << "printing the answer for query #" << queryNumber << endl;
    for(unsigned i = 0; i < objects.size(); ++i)
    {
        cout << objects[i].id << endl;
    }
}

void build_rtree(string dataset, int bf, float ff) {
    string data_path = "/home/wcn/data-process/" + dataset + "/loc";
    ifstream loc_file;
    loc_file.open(data_path, ios::in);
    rtree.create(bf,ff);
    string loc;
    int count = 0;
    Rectangle range;
    clock_t start = 0, construction_time = 0;
    while (getline(loc_file, loc)) {

        Rectangle mbr;
        parseLine(loc, mbr);
        if (count == 0) {
            parseLine(loc, range);
        } else {
            range.enlarge(mbr);
        }
        Object obj;
        obj.mbr = mbr;
        obj.id = mbr.id;
//        obj.suffixId = mbr.id;
        start = clock();
        rtree.insertData(obj);
        construction_time += clock() - start;
        count ++;
    }
    cout << "size of r-tree: " << count << " range of r-tree:" << range.min.x << "  " << range.min.y << "  " << range.max.x << "  " << range.max.y << "  ";
    cout << "r-tree construction time" << double(construction_time)/CLOCKS_PER_SEC << endl;
}

void test_single_suffix() {
    /* build suffix tree */
    string path = "/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled";
    int time_begin = 0,time_end = 0,category_begin = 1,category_end = 100;
    for(int i=time_begin;i<=time_end;i++){
        string path1 = path+"/basic_model/"+to_string(i);
        string prefix_file_path = path+"/serialization/"+to_string(i);
        if(access(prefix_file_path.data(),2)!=0){
            mkdir(prefix_file_path.data(),0777);
        }
        for(int j = category_begin;j<=category_end;j++){
            SuffixTree stree;
            vector<ctnr> documents = readFile(path1+"/"+to_string(j));
            if(documents.empty()){continue;}
            cout<<to_string(i)+"/"+to_string(j)<<endl;
            stree.add_string(documents);
            string path2 = path+"/serialization/"+to_string(i)+"/"+to_string(j);
            stree.dump_tree();
            stree.save(path2);
        }
}

    /* load suffix tree */
    clock_t startTime, endTime, loadTime = 0, miningTime = 0;
    vector<SuffixTree *> tree_pointers;
    for (int category = 1; category <= 100; category++) {
        string path = "/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled/serialization/0/" + to_string(category);
        if (!opendir(path.data())) {
            continue;
        }
        startTime = clock();
        SuffixTree * stree = new SuffixTree();
        stree->load(path);
        endTime = clock();
        loadTime += endTime - startTime;
        tree_pointers.emplace_back(stree);
    }
    cout << "single suffix tree load time" <<  (double )(loadTime) / CLOCKS_PER_SEC  << endl;

    /* mining suffix tree */
    startTime = clock();
    vector<ctnr> res2 = SuffixTree::mining_tree_add_threshold(tree_pointers, 2);
    endTime = clock();
    miningTime += endTime-startTime;
    cout << "single suffix tree mining time" << (double )(miningTime) / CLOCKS_PER_SEC << endl;


}








void test_multi_suffix() {
/* build suffix tree */
    string path = "/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled";
    int time_begin = 0,time_end = 0,category_begin = 1,category_end = 100;
    for(int i=time_begin;i<=time_end;i++){
        string path1 = path+"/basic_model/"+to_string(i);
        string prefix_file_path = path+"/serialization/"+to_string(i);
        if(access(prefix_file_path.data(),2)!=0){
            mkdir(prefix_file_path.data(),0777);
        }
        vector<ctnr> totalDocuments;
        SuffixTree stree;
        for(int j = category_begin;j<=category_end;j++){
            vector<ctnr> documents = readFile(path1+"/"+to_string(j));
            totalDocuments.insert(totalDocuments.end(), documents.begin(), documents.end());
        }
        stree.add_string(totalDocuments);
        string path2 = path+"/serialization/"+to_string(i)+"/total";
        stree.dump_tree();
        stree.save(path2);
    }

    /* load suffix tree */
    clock_t startTime, endTime, loadTime = 0, miningTime = 0;
    vector<SuffixTree *> tree_pointers;
        string loadPath = "/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled/serialization/0/total/";
        startTime = clock();
        SuffixTree * stree = new SuffixTree();
        stree->load(loadPath);
        endTime = clock();
        loadTime += endTime - startTime;
        tree_pointers.emplace_back(stree);
    cout << "multi suffix tree load time" <<  (double )(loadTime) / CLOCKS_PER_SEC  << endl;

    /* mining suffix tree */
    startTime = clock();
    vector<ctnr> res2 = SuffixTree::mining_tree_add_threshold(tree_pointers, 2);
    endTime = clock();
    miningTime += endTime-startTime;
    cout << "multi suffix tree mining time" << (double )(miningTime) / CLOCKS_PER_SEC << endl;


}

void test_suffix_tree_query() {
    
}

/**
 * 根据objects的location，建立R树
 * 每个location与叶子节点还有内部节点关联
 */
void insert_objects_inner_node() {
    struct dirent *dirp;
    string path = "/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled/basic_model";
    DIR* dir = opendir(path.c_str());
    clock_t startTime,endTime;
    clock_t insertTime = 0;
    // 根据objects的文本数据构建suffix tree并与R Tree关联
    while ((dirp = readdir(dir)) != nullptr) {
        if (dirp->d_type == DT_DIR) {
            // 文件夹
            string file_name = dirp->d_name;
            if (file_name[0] == '.') {
                continue;
            }
            string path2 = path + "/" + file_name;
            DIR *dir2 = opendir(path2.c_str());
            struct dirent *dirp2;
            while ((dirp2 = readdir(dir2)) != nullptr) {
                // 自上而下遍历R Tree，发现包含object.location的节点，即将object.text加入该节点对应的suffix tree.
                string file_name2 = dirp2->d_name;
                if (file_name2[0] == '.') {
                    continue;
                }
                SpatialObject spatialObject;
                string data = file_name + " " + file_name2;
                Rectangle mbr;
                parseLine(data, mbr);
                spatialObject.mbr = mbr;
                spatialObject.documents = readFile(path2 + "/" + file_name2);
                startTime = clock();
                rtree.insertSpatialObjectInnerNode(spatialObject);
                endTime = clock();
                insertTime += endTime - startTime;

            }
            closedir(dir2);
        }
    }
    closedir(dir);
    cout<<"insert spatial-textual object into r-tree, inner node"<<(double)(insertTime) / CLOCKS_PER_SEC<<endl;

}


void insert_objects_leaf_node(){
    struct dirent *dirp;
    string path = "/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled/basic_model";
    DIR* dir = opendir(path.c_str());
    clock_t startTime,endTime;
    clock_t insertTime = 0;
    // 根据objects的文本数据构建suffix tree并与R Tree关联
    while ((dirp = readdir(dir)) != nullptr) {
        if (dirp->d_type == DT_DIR) {
            // 文件夹
            string file_name = dirp->d_name;
            if (file_name[0] == '.') {
                continue;
            }
            string path2 = path + "/" + file_name;
            DIR *dir2 = opendir(path2.c_str());
            struct dirent *dirp2;
            while ((dirp2 = readdir(dir2)) != nullptr) {
                // 自上而下遍历R Tree，发现包含object.location的节点，即将object.text加入该节点对应的suffix tree.
                string file_name2 = dirp2->d_name;
                if (file_name2[0] == '.') {
                    continue;
                }
                SpatialObject spatialObject;
                string data = file_name + " " + file_name2;
                Rectangle mbr;
                parseLine(data, mbr);
                spatialObject.mbr = mbr;
                spatialObject.documents = readFile(path2 + "/" + file_name2);
                startTime = clock();
                rtree.insertSpatialObjectLeafNode(spatialObject);
                endTime = clock();
                insertTime += endTime - startTime;

            }
            closedir(dir2);
        }
    }
    closedir(dir);
    cout<<"insert spatial-textual object into r-tree, leaf node"<<(double)(insertTime) / CLOCKS_PER_SEC<<endl;


}


void build_rtree_from_file() {
    struct dirent *dirp;
    string path = "/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled/basic_model";
    DIR* dir = opendir(path.c_str());
    rtree.create(4, 0.7);
    int count = 0;
// 根据objects的location构建R Tree
    while ((dirp = readdir(dir)) != nullptr) {
        if (dirp->d_type == DT_DIR) {
            // 文件夹
            string file_name = dirp->d_name;
            if (file_name[0] == '.') {
                continue;
            }
            string path2 = path + "/" + file_name;
            DIR* dir2 = opendir(path2.c_str());
            struct dirent *dirp2;
            while (dir2 && (dirp2 = readdir(dir2)) != nullptr) {
                string file_name2 = dirp2->d_name;
                string data = file_name + " " + file_name2;
                if (file_name2[0] == '.') {
                    continue;
                }
                Rectangle mbr;
                parseLine(data, mbr);
                Object obj;
                obj.mbr = mbr;
                obj.id = count;
                rtree.insertData(obj);
                count++;
            }
            closedir(dir2);
        }
    }

    closedir(dir);
}

void test_query_leaf_node() {
    // 一些统计
    clock_t startTime,endTime,miningTime=0,loadTime=0;
    vector<SuffixTree *>tree_pointers;
    vector<ctnr> res;

    vector<Object> objects;
    Rectangle range;
    string query_line = "0 0 100 100";
    parseLine(query_line, range);

    // 从R-Tree里查询得到节点们
    rtree.rangeQuery(objects, range);

    // 节点 --> suffix tree
    for (Object current_object:objects) {
        SuffixTree * stree = new SuffixTree();
        stree->load("/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled/suffix-tree-leaf/" + to_string(current_object.suffixId));
        tree_pointers.emplace_back(stree);
    }

    cout << "leaf node level query suffix tree num: " << tree_pointers.size() << endl;
    // 查询所有suffix tree
    startTime = clock();
    vector<ctnr> res2 = SuffixTree::mining_tree_add_threshold(tree_pointers, 4);
    endTime = clock();
    miningTime += endTime-startTime;
    res.insert(res.end(), res2.begin(), res2.end());
    set<ctnr > new_res (res.begin(), res.end());
    res.assign(new_res.begin(), new_res.end());
//    printResult("/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled",res);
    cout<<"mining suffix tree, leaf node "<<(double)(miningTime) / CLOCKS_PER_SEC<<endl;
}

void test_query_inner_node() {
    // 一些统计
    clock_t startTime,endTime,miningTime=0,loadTime=0;
    vector<SuffixTree *>tree_pointers;
    vector<ctnr> res;

    vector<Object> objects;
    Rectangle range;
    string query_line = "0 0 100 100";
    parseLine(query_line, range);

    // 从R-Tree里查询得到节点们
    rtree.rangeQueryInnerNodes(objects, range);

    // 节点 --> suffix tree
    for (Object current_object:objects) {
        SuffixTree * stree = new SuffixTree();
        stree->load("/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled/suffix-tree/" + to_string(current_object.suffixId));
        tree_pointers.emplace_back(stree);
    }

    cout << "inner node level query suffix tree num: " << to_string(tree_pointers.size());
    // 查询所有suffix tree
    startTime = clock();
    vector<ctnr> res2 = SuffixTree::mining_tree_add_threshold(tree_pointers, 4);
    endTime = clock();
    miningTime += endTime-startTime;
    res.insert(res.end(), res2.begin(), res2.end());
    set<ctnr > new_res (res.begin(), res.end());
    res.assign(new_res.begin(), new_res.end());
    printResult("/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled",res);
    cout<<"mining suffix tree, inner node "<<(double)(miningTime) / CLOCKS_PER_SEC<<endl;

}
void test_load() {
    string path = "/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled/serialization";
    for(int i=0;i<=0;i++){
        string path1 = path+"/" + to_string(i);
        for(int j = 0;j<=30;j++){
            SuffixTree stree;
            string category_path = path1 + "/" + to_string(j);
            if (fopen(category_path.data(),"r")) {
                stree.load(path1 + "/" + to_string(j));
            }
        }
    }
}

void view_size(string path){
    size_t stack_size = 0;
    int node_size = 0;
    for (int i = 0;i < 1; i++) {
        for (int j = 0; j < 148; j++) {
            string file_path = path + "/" + to_string(i) + "/" + to_string(j);
            string stack_name = file_path + "/haystack";
            FILE* fp = fopen(stack_name.c_str(), "r");
            if(!fp) {
                continue;
            }
            fseek(fp, 0, SEEK_END);
            size_t size = ftell(fp);
            fclose(fp);
            stack_size += size;
        }
    }
    cout << stack_size << endl;

}

void selerailize_to_prefix(string path){
    string basic_path = path+"/basic_model";
    string prefix_path = path + "/prefix_model";
    vector<SuffixTree> trees(148);
    for(int date = 0;date<731;date++) {
        string file_path = basic_path+"/"+to_string(date);
        string prefix_file_path = prefix_path+"/"+to_string(date);
        vector<string> files = getAllFiles(file_path);
        if(files.size()<10||file_path.size()>150){
            continue;
        }
        if(access(prefix_file_path.data(),2)!=0){
            mkdir(prefix_file_path.data(),0755);
        }
        for(int i=1;i<=147;i++){
            for(int j=1;j<=i;j++){
                vector<ctnr> documents = readFile(file_path+"/"+to_string(j));
                if(!documents.empty())
                    cout<<file_path+"/"+to_string(j)<<endl;
                trees[i].add_string(documents);
            }
            trees[i].dump_tree();
            trees[i].save(prefix_file_path+"/"+to_string(i));
        }
    }
}
//void selerailize(string path) { //序列化存储
//    int time_begin = 0,time_end = 0,category_begin = 62,category_end = 62;
//    for(int i=time_begin;i<=time_end;i++){
//        string path1 = path+"/basic_model/"+to_string(i);
//        string prefix_file_path = path+"/serialization/"+to_string(i);
//        if(access(prefix_file_path.data(),2)!=0){
//            mkdir(prefix_file_path.data(),0777);
//        }
//        for(int j = category_begin;j<=category_end;j++){
//            SuffixTree stree;
//            vector<ctnr> documents = readFile(path1+"/"+to_string(j));
//            if(documents.empty()){continue;}
//            cout<<to_string(i)+"/"+to_string(j)<<endl;
//            stree.add_string(documents);
//            string path2 = path+"/serialization/"+to_string(i)+"/"+to_string(j);
//            stree.dump_tree();
//            stree.save(path2);
//        }
//    }
//}
//void test1(){
//    QuadTrees q_tree(1,50,1,50);
////    q_tree.print();
////    q_tree.selerailize("/Users/wanglu/repos/python/dataset/Amazon-Reviews");
//    vector<int> region = q_tree.getRegion(1,10,10,20);
//    for(auto r:region){
//        cout<<r<<" ";
//    }
//    vector<int> v1={1,2,3,1,2};
//    vector<int> v2={1,2,3,1,2,4};
//    vector<int> v3={1,2,3,1,2,4};
//    vector<int> v4={1,2,3,1,2,5};
//    vector<int> v5={1,2,3,1,3,4};
//    vector<int> v6={1,2,3,1,3,5};
//    SuffixTree tree1,tree2,tree3,tree4;
//    vector<vector<int>> vv;
//    vv.push_back(v1);
//    tree1.add_string(vv);
//    vv.push_back(v2);
//    tree2.add_string(vv);
//    vv.pop_back();vv.push_back(v3);vv.push_back(v5);
//    tree3.add_string(vv);
//    vv.push_back(v2);vv.push_back(v4);vv.push_back(v6);
//    tree4.add_string(vv);
//    vector<vector<frequency>> check = tree4.mining_tree(&tree1,&tree2,&tree3,3);
//    for(auto i:check){
//        for (auto j:i) {
//            cout<<j.term;
//        }cout<<endl;
//    }
//    return;
//}

//void demo() {
//    string path = "input.txt";
//    ifstream readFile(path)
//    string temp;
//    SuffixTree *stree = new SuffixTree();
//    vector<ctnr> documents = readFile(path);
//    stree->add_string(documents);
//
//}

void test_construction() {
    int time_max = 730;
    int category_max = 148;
    clock_t startTime,endTime,miningTime=0,loadTime=0;
    int time_begin = 0,time_end = 100,category_begin = 0,category_end = category_max;
//    string server_path = "/media/luchang1/luchang/workspace/fdu/dataset/Amazon-Reviews";
// TODO: 把绝对路径作为define
    string path = "/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled";
    string save_path = path;
    for(int i=0;i<=0;i++){
        string path1 = path+"/basic_model/"+to_string(i);
        string prefix_file_path = save_path+"/serialization/"+to_string(i);
        if(access(prefix_file_path.data(),2)!=0){
            mkdir(prefix_file_path.data(),0777);
        }
        for(int j = 0;j<=30;j++){
            startTime = clock();
            SuffixTree stree;
            vector<ctnr> documents = readFile(path1+"/"+to_string(j));
            if(documents.empty()){continue;}
            cout<<to_string(i)+"/"+to_string(j)<<endl;
            stree.add_string(documents);
            string path2 = save_path+"/serialization/"+to_string(i)+"/"+to_string(j);
            stree.dump_tree();
            stree.save(path2);
            endTime = clock();
            loadTime += endTime-startTime;
//            cout << "size" << sizeof(stree) << endl;
            startTime = clock();
            endTime = clock();
        }
    }
//    printResult(path,res);
    cout<<"suffixTree from file total loading time cost:"<<(double)(loadTime) / CLOCKS_PER_SEC<<endl;
}

void test2(string path,int threshold, int first, int last){

    clock_t startTime,endTime,miningTime=0,loadTime=0;
    vector<ctnr> res;
    SuffixTree *stree = new SuffixTree();
    for(int i=first;i<=last;i++){
        startTime = clock();
        string path1 = path+"/week/"+to_string(i);
        vector<string> files = getAllFiles(path1);
        for(auto fileName:files){
            vector<ctnr> documents = readFile(path1+"/"+fileName);
            stree->add_string(documents);
        }
        string path2 = path+"/serialization/"+to_string(i);
        stree->dump_tree();
        stree->save(path2);
        endTime = clock();
        loadTime += endTime-startTime;
        startTime = clock();
        endTime = clock();
        miningTime += endTime-startTime;
    }
    printResult(path,res);
    cout<<"suffixTree from file total loading time cost:"<<(double)(loadTime) / CLOCKS_PER_SEC<<endl;
    delete stree;
//    cout<<"suffixTree serial mining time cost:"<<(double)(miningTime) / CLOCKS_PER_SEC<<endl;

}

void test3(string path,int threshold, int first, int last){ //序列化存储
    clock_t startTime,endTime,miningTime=0,loadTime=0;
    vector<ctnr> res;
    SuffixTree *stree = new SuffixTree();
    for(int i=first;i<=last;i++){
        startTime = clock();
        string path1 = path+"/week/"+to_string(i);
        vector<string> files = getAllFiles(path1);
        for(auto fileName:files){
            vector<ctnr> documents = readFile(path1+"/"+fileName);
            stree->add_string(documents);
        }
        string path2 = path+"/serialization/"+to_string(i);
        stree->dump_tree();
        stree->save(path2);
        endTime = clock();
        loadTime += endTime-startTime;
        startTime = clock();
//        if(i==first){
//            res = stree->mining_tree(threshold);
//        }else{
//            res = stree->mining_tree(res,threshold);
//        }
        endTime = clock();
        miningTime += endTime-startTime;
        delete stree;
        stree = new SuffixTree();
    }
    printResult(path,res);
    cout<<"suffixTree from file total loading time cost:"<<(double)(loadTime) / CLOCKS_PER_SEC<<endl;
    delete stree;
//    cout<<"suffixTree serial mining time cost:"<<(double)(miningTime) / CLOCKS_PER_SEC<<endl;
}

void test4(string path,int threshold, int first, int last){// Apriori + intersection
    clock_t startTime,endTime,miningTime=0,loadTime=0;
    int weekday = 1,week = 1;
    vector<ctnr> res;
    Apriori *apriori = new Apriori();
    for(int i=first;i<=last;i++){
        startTime = clock();
        string path1 = path+"/week/"+to_string(i);
        vector<string> files = getAllFiles(path1);
        for(auto fileName:files){
            vector<ctnr> documents = readFile(path1+"/"+fileName);
            apriori->add_string(documents);
            if(weekday==7){
                endTime = clock();   
                loadTime += endTime-startTime;
                startTime = clock();
                vector<ctnr> res2 = apriori->mining2(threshold);
                if(week!=first){
                    res = intersection(res,res2);
                }else{
                    res = res2;
                }
                endTime = clock();
                miningTime += endTime-startTime;
                apriori = new Apriori();
                weekday=0;
            }
            weekday++;
        }
    }
//    printResult(path,res);
    cout<<(double)(loadTime) / CLOCKS_PER_SEC<<" "<<(double)(miningTime) / CLOCKS_PER_SEC<<" "<<(double )(loadTime+miningTime)/CLOCKS_PER_SEC<<endl;
    return;
}

vector<double> test_single_tree(string path, int threshold) {
    clock_t startTime,endTime,miningTime=0,loadTime=0;
    vector<ctnr> res;
    vector<double> result_time;
    int count = 0;
    for (int date = 0; date < 1; date ++) {
        string date_path = path + "/" + to_string(date);
        for (int category = 0; category < 148; category ++) {
            string category_path = date_path + "/" + to_string(category);
            if (!opendir(category_path.data())) {
                continue;
            }
            startTime = clock();
            SuffixTree stree;
            stree.load(category_path);
            endTime = clock();
            loadTime += endTime-startTime;
            startTime = clock();
            vector<ctnr> res2 = stree.mining_tree_threshold(threshold);
            endTime = clock();
            miningTime += endTime-startTime;
            res.insert(res.end(), res2.begin(), res2.end());
            count ++;
        }
    }
    set<ctnr > new_res (res.begin(), res.end());
    res.assign(new_res.begin(), new_res.end());
    printResult("/media/luchang1/luchang/workspace/fdu/dataset/Amazon-Reviews",res);
    cout << "result size: " << res.size() << endl;
    cout << (double )(loadTime) / CLOCKS_PER_SEC / count  << "  " << (double )(miningTime) / CLOCKS_PER_SEC / count << endl;

    return result_time;
}

void test_category_tree(string path, int threshold, int start_category, int end_category) {
    clock_t startTime,endTime,miningTime=0,loadTime=0;
    vector<ctnr> res;
    vector<double> result_time;
    int count = 0;
    string date_path = path + "/" + "0";
    vector<SuffixTree> trees;
    vector<SuffixTree *>tree_pointers;
        for (int category = start_category; category <= end_category; category ++) {
            string category_path = date_path + "/" + to_string(category);
            if (!opendir(category_path.data())) {
                continue;
            }
            startTime = clock();
            SuffixTree * stree = new SuffixTree();
            stree->load(category_path);
            count ++;
            endTime = clock();
            loadTime += endTime - startTime;
            tree_pointers.emplace_back(stree);
        }
//        vector<SuffixTree *>tree_pointers;
//        for (SuffixTree tree:trees) {
//            tree_pointers.emplace_back(&tree);
//        }
            startTime = clock();
            vector<ctnr> res2 = trees[0].mining_tree_add_threshold(tree_pointers, threshold);
            endTime = clock();
            miningTime += endTime-startTime;
            res.insert(res.end(), res2.begin(), res2.end());
    set<ctnr > new_res (res.begin(), res.end());
    res.assign(new_res.begin(), new_res.end());
    printResult("/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled",res);
//    printResult("/media/luchang1/luchang/workspace/fdu/dataset/Amazon-Reviews",res);
    cout << "result size: " << res.size() << endl;
    cout << (double )(loadTime) / CLOCKS_PER_SEC   << "  " << (double )(miningTime) / CLOCKS_PER_SEC  << endl;

}


void test_mining_tree(int threshold) {
    // 一些统计
    clock_t startTime,endTime,miningTime=0,loadTime=0;
    vector<SuffixTree *>tree_pointers;
    vector<ctnr> res;

    // 读文件得到查出来的suffix tree
    char * result_file = "/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled/result.txt";
    ifstream file;
    file.open(result_file, ios::in);
    string suffix_index_buff;
    vector<vector<string>> suffix_trees;
    while (getline(file, suffix_index_buff)) {
        char * temp = new char[suffix_index_buff.length() + 1];
        const char * delim = " ";
        strcpy(temp, suffix_index_buff.c_str());
        char * suffix_index = strtok(temp, delim);
        vector<string> suffix_tree;
        string date = suffix_index;
        suffix_tree.push_back(date);
        suffix_index = strtok(NULL, delim);
        string category = suffix_index;
        suffix_tree.push_back(category);
        suffix_trees.push_back(suffix_tree);
    }

    // 把suffix tree读入内存
    for (vector<string> suffix_index: suffix_trees) {
        string path = "/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled/serialization/" + suffix_index[0] + "/" + suffix_index[1];
        if (!opendir(path.data())) {
            continue;
        }
        startTime = clock();
        SuffixTree * stree = new SuffixTree();
        stree->load(path);
        endTime = clock();
        loadTime += endTime - startTime;
        tree_pointers.emplace_back(stree);
    }
    // 利用广搜遍历
    startTime = clock();
    vector<ctnr> res2 = SuffixTree::mining_tree_add_threshold(tree_pointers, threshold);
    endTime = clock();
    miningTime += endTime-startTime;
    res.insert(res.end(), res2.begin(), res2.end());
    set<ctnr > new_res (res.begin(), res.end());
    res.assign(new_res.begin(), new_res.end());
    printResult("/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled",res);
//    printResult("/media/luchang1/luchang/workspace/fdu/dataset/Amazon-Reviews",res);
    cout << "result size: " << res.size() << endl;
    cout << (double )(loadTime) / CLOCKS_PER_SEC   << "  " << (double )(miningTime) / CLOCKS_PER_SEC  << endl;

}
void test_date_tree(string path, int threshold, int start_date, int end_date) {
    clock_t startTime,endTime,miningTime=0,loadTime=0;
    vector<ctnr> res;
    vector<double> result_time;
    int count = 0;
    vector<SuffixTree> trees;
    vector<SuffixTree *>tree_pointers;
    for (int date = start_date; date <= end_date; date++) {
        string category_path = path + "/" + to_string(date) + "/27";
        if (!opendir(category_path.data())) {
            continue;
        }
        startTime = clock();
        SuffixTree * stree = new SuffixTree();
        stree->load(category_path);
        count ++;
        endTime = clock();
        loadTime += endTime - startTime;
        tree_pointers.emplace_back(stree);
    }
//        vector<SuffixTree *>tree_pointers;
//        for (SuffixTree tree:trees) {
//            tree_pointers.emplace_back(&tree);
//        }
    startTime = clock();
    vector<ctnr> res2 = trees[0].mining_tree_add_threshold(tree_pointers, threshold);
    endTime = clock();
    miningTime += endTime-startTime;
    res.insert(res.end(), res2.begin(), res2.end());
    set<ctnr > new_res (res.begin(), res.end());
    res.assign(new_res.begin(), new_res.end());
    printResult("/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled",res);
//    printResult("/media/luchang1/luchang/workspace/fdu/dataset/Amazon-Reviews",res);
cout << "tree num:" << count << endl;
    cout << "result size: " << res.size() << endl;
    cout << (double )(loadTime) / CLOCKS_PER_SEC   << "  " << (double )(miningTime) / CLOCKS_PER_SEC  << endl;

}

void test5(string path,int threshold, int first, int last){
    clock_t startTime,endTime,miningTime=0,loadTime=0;
    vector<ctnr> res;
    for(int i=first;i<=last;i++){
        startTime = clock();
        SuffixTree stree;
        stree.load(path+"/serialization/"+to_string(i));
        endTime = clock();
        loadTime += endTime-startTime;
        startTime = clock();
        vector<ctnr> res2 = stree.mining_tree_threshold(threshold);
        if(i!=first){
            res = intersection(res,res2);
        }else{
            res = res2;
        }
        endTime = clock();
        miningTime += endTime-startTime;
    }
////    printResult(path,res);
    cout<<(double)(loadTime) / CLOCKS_PER_SEC<<" "<<(double)(miningTime) / CLOCKS_PER_SEC<<" "<<(double )(loadTime+miningTime)/CLOCKS_PER_SEC<<endl;
}

void test6(string path,int threshold, int first, int last){ //suffixTree + serial
    clock_t startTime,endTime,miningTime=0,loadTime=0;
    vector<ctnr> res;
    for(int i=first;i<=last;i++){
        startTime = clock();
        SuffixTree stree;
        stree.load(path+"/serialization/"+to_string(i));
        endTime = clock();
        loadTime += endTime-startTime;
        startTime = clock();
        if(i==first){
            res = stree.mining_tree_threshold(threshold);
        }else{
            res = stree.mining_tree(res,threshold);
        }
        endTime = clock();
        miningTime += endTime-startTime;
    }
//    printResult(path,res);
    cout<<(double)(loadTime) / CLOCKS_PER_SEC<<" "<<(double)(miningTime) / CLOCKS_PER_SEC<<" "<<(double )(loadTime+miningTime)/CLOCKS_PER_SEC<<endl;

}
vector<Apriori*> _apriori;



void experiment1(){
    string path = "/Users/wanglu/repos/python/dataset/blogs";
    string output_path = path+"/experiement/output1.out";
    freopen("/Users/wanglu/repos/python/dataset/blogs/output1.out","w",stdout);
    vector<int> end_week = {5,10,20,50};
    for(auto end_time:end_week){
        test4(path,100,1,end_time);
        test5(path,100,1,end_time);
        test6(path,100,1,end_time);
    }
}
void experiment2(){
    string path = "/Users/wanglu/repos/python/dataset/blogs";
    freopen("/Users/wanglu/repos/python/dataset/blogs/output2.out","w",stdout);
    vector<int> thres = {10,20,50,100,200};
    for(auto t:thres){
        test4(path,t,1,50);
        test5(path,t,1,50);
        test6(path,t,1,50);
    }
}

void experiment3(){
    string path = "/Users/wanglu/repos/python/dataset/twitter";
    string output_path = path+"/experiement/output1.out";
    freopen("/Users/wanglu/repos/python/dataset/twitter/output1.out","w",stdout);
    vector<int> end_week = {1,3,6,9,12};
    for(auto end_time:end_week){
        test4(path,100,1,end_time);
        test5(path,100,1,end_time);
        test6(path,100,1,end_time);
    }
}

void experiment4(){
    string path = "/Users/wanglu/repos/python/dataset/twitter";
    freopen("/Users/wanglu/repos/python/dataset/twitter/output2.out","w",stdout);
    vector<int> thres = {10,20,50,100,200};
    for(auto t:thres){
        test4(path,t,1,12);
        test5(path,t,1,12);
        test6(path,t,1,12);
    }
}


void testQuery(string path){
    string prefix_path = path + "/prefix_model/";
    string quad_path = path+"/quad_model/";
    string basic_path = path+"/serialization/";
    QuadTrees q_tree(1,50,1,50);
    int threshold = 20;
    int time_begin = 1,time_end = 40,category_begin = 1,category_end = 10;
    clock_t  startTime,endTime;
    startTime = clock();
    SuffixTree stree1,stree2,stree3,stree4;
    stree4.load(prefix_path+to_string(time_end-1)+"/"+to_string(category_end-1));
    stree3.load(prefix_path+to_string(time_end-1)+"/"+to_string(category_begin));
    stree2.load(prefix_path+to_string(time_begin)+"/"+to_string(category_end-1));
    stree1.load(prefix_path+to_string(time_begin)+"/"+to_string(category_begin));
    vector<vector<frequency>> check = stree4.mining_tree_sub(&stree1,&stree2,&stree3,threshold);
    cout<<check.size()<<endl;
    endTime = clock();
    cout<<"prefix model time cost:"<<(double )(endTime-startTime)/CLOCKS_PER_SEC<<endl;

    startTime = clock();
    for (int i=time_begin;i<time_end;i++){
        for(int j = category_begin;j<category_end;j++){
            SuffixTree stree;
            stree.load(basic_path+to_string(i)+"/"+to_string(j));
        }
    }
    endTime = clock();
    cout<<"basic model time cost:"<<(double )(endTime-startTime)/CLOCKS_PER_SEC<<endl;startTime = clock();
    vector<int> region = q_tree.getIncludeRegion(time_begin,time_end,category_begin,category_end);
    cout<<"quad region number: "<<region.size()<<endl;
    vector<SuffixTree*> strees;
    for (auto i:region){
        SuffixTree *tree = new SuffixTree();
        tree->load(quad_path+to_string(i));
        strees.push_back(tree);
    }
    check = strees[0]->mining_tree(strees,threshold);
    cout<<check.size()<<endl;
    endTime = clock();
    cout<<"quad model time cost:"<<(double)(endTime-startTime)/CLOCKS_PER_SEC<<endl;
}

void prefix_query(string path,int threshold,int time_begin,int time_end,int category_begin,int category_end){
    string prefix_path = path + "/prefix_model/";
    SuffixTree stree1,stree2,stree3,stree4;
    long file_size=0;
    file_size += stree4.load(prefix_path+to_string(time_end-1)+"/"+to_string(category_end-1));
    file_size += stree3.load(prefix_path+to_string(time_end-1)+"/"+to_string(category_begin));
    file_size += stree2.load(prefix_path+to_string(time_begin)+"/"+to_string(category_end-1));
    file_size += stree1.load(prefix_path+to_string(time_begin)+"/"+to_string(category_begin));
    clock_t  startTime,endTime;
    startTime = clock();
    vector<vector<frequency>> check = stree4.mining_tree_sub(&stree1,&stree2,&stree3,threshold);
    endTime = clock();
    cout<<"prefix ftree: "<<file_size/1024<<"KB, "<<(double )(endTime-startTime)/CLOCKS_PER_SEC<<"ms"<<endl;
}

void basic_query(string path,int threshold,int time_begin,int time_end,int category_begin,int category_end){
    string basic_path = path+"/basic_model";
    clock_t  startTime,endTime;
    SuffixTree tree;
    long file_size=0;
    for(int date = time_begin;date<time_end;date++) {
        string file_path = basic_path+"/"+to_string(date);
        vector<string> files = getAllFiles(file_path);
        if(files.size()<10||file_path.size()>150){
            continue;
        }
        for(int i=category_begin;i<category_end;i++){
            for(int j=1;j<=i;j++){
                ifstream in(file_path+"/"+to_string(j));
                if(!in.is_open()){
                    continue;
                }
                file_size+=in.tellg();
                vector<ctnr> documents = readFile(file_path+"/"+to_string(j));
                if(!documents.empty())
                    cout<<file_path+"/"+to_string(j)<<endl;
                tree.add_string(documents);
            }
        }
    }
    startTime = clock();
    tree.dump_tree();
    tree.mining_tree_threshold(threshold);
    endTime = clock();
    cout<<"basic ftree: "<<file_size/1024<<"KB, "<<(double )(endTime-startTime)/CLOCKS_PER_SEC<<"ms"<<endl;
}



vector<int> split_to_int(const string &str)
{
    vector<string> v;
    string::size_type pos = 0;
    bool isStringHead = true;
    for (string::size_type i = 0; i != str.size(); ++i)
    {
        if (str[i] != ' '&&isStringHead==false)
        {
            isStringHead = true;
            pos = i;
        }

        if (str[i] == ' '&&isStringHead)
        {
            v.push_back(string(str,pos,i-pos));
            isStringHead = false;
        }
        if (i == str.size()-1)
        {
            v.push_back(string(str,pos,v.size()-pos));
        }
    }
    vector<int> ret;
    for(int i=1;i<v.size();i++){
        ret.push_back(atoi(v[i].c_str()));
    }
    return ret;
}
//void build(string path){
//    selerailize(path);
//    selerailize_to_prefix(path);
//}
void query(string path){
    vector<int> category_begin,category_end,time_begin,time_end,threshold;
    ifstream in(path+"/input");
    if(!in.is_open()){
        cout<<"error: cannot open input!"<<endl;
        return;
    }
    string s;
    getline(in,s);
    category_begin = split_to_int(s);
    getline(in,s);
    category_end = split_to_int(s);
    getline(in,s);
    time_begin = split_to_int(s);
    getline(in,s);
    time_end = split_to_int(s);
    getline(in,s);
    threshold = split_to_int(s);
    for(auto thres:threshold){
        for(int i=0;i<category_begin.size();i++){
            for(int j=0;j<time_begin.size();j++){
                basic_query(path,thres,time_begin[j],time_end[j],category_begin[i],category_end[i]);
                prefix_query(path,thres,time_begin[j],time_end[j],category_begin[i],category_end[i]);
            }
        }
    }
    return;
}

void test_map() {
    vector<int> a;
    vector<int> b;
    a.push_back(1);
    b.push_back(1);
    map<vector<int>, int> my_map;
    my_map.insert(pair<vector<int>, int>(a, 1));
    if (my_map.count(b) != 0) {
        cout << "success" << endl;
    }
    my_map.insert(pair<vector<int>, int>(b, 2));
    cout << my_map[b] << endl;

}

void demo() {
    string path = "/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled/input.txt";
    SuffixTree *stree = new SuffixTree();
    vector<ctnr> documents = readFile(path);
    stree->add_string(documents);
    stree->dump_tree();
    vector<vector<int>> phrases = stree->mining_tree_threshold(3);

    path = "/Users/christine/Documents/Study/Grade 4/大四上/lab/untitled/input2.txt";
    SuffixTree *stree2 = new SuffixTree();
    vector<ctnr> documents2 = readFile(path);
    stree2->add_string(documents2);
    stree2->dump_tree();

    vector<SuffixTree *> trees;
    trees.emplace_back(stree);
    trees.emplace_back(stree2);
    vector<vector<frequency>> phrases2;
    vector<vector<int>> result = stree->mining_tree_add_threshold(trees, 3);
}
//    for (const phrase_frequency& frequency: frequency_list) {
//        cout << "term: ";
//        for (int term: frequency.phrase) {
//            cout << term << " " ;
//        }
//        cout << "frequency: " << frequency.freq << endl;
//    }
////    vector<SuffixTree *> trees;
////    trees.emplace_back(stree);
////    trees.emplace_back(stree2);
////    vector<vector<frequency>> phrases = stree->mining_tree(trees, 3);
//
//}