#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <vector>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;
typedef vector<int> ctnr;

vector<string> getAllFiles(string path)
{
    vector<string> files;
    struct dirent *dirp;

    DIR* dir = opendir(path.data());

    while ((dirp = readdir(dir)) != nullptr) {
        if (dirp->d_type == DT_REG) {
            files.push_back(dirp->d_name);
        }
    }
    closedir(dir);
    return files;
}

vector<ctnr> readFile(string path){
    vector<ctnr> ret;
    string filePath = path;
    ifstream in(filePath);
    if(!in.is_open()){
        return ret;
    }
    string s;
    while (getline(in,s)){
        ctnr tmp;
        string s_split;
        istringstream iss(s);
        while (getline(iss,s_split,' ')){
            tmp.push_back(stoi(s_split));
        }
        ret.push_back(tmp);
    }
    return ret;
}

vector<ctnr> readFile_4sq(string path) {
    vector<ctnr> ret;
    string filePath = path;
    ifstream in(filePath);
    if(!in.is_open()){
        return ret;
    }
    string s;
    // 第一行是location，不要读进去
    getline(in,s);
    while (getline(in,s)){
        ctnr tmp;
        string s_split;
        istringstream iss(s);
        while (getline(iss,s_split,' ')){
            tmp.push_back(stoi(s_split));
        }
        ret.push_back(tmp);
    }
    return ret;
}

vector<string> printResult(string path,vector<ctnr> res){
    vector<string> dict;
    dict.push_back("");
    ifstream in(path+"/dictionary");
    if(!in.is_open()){
        cout<<"cannot open file"<<path<<endl;
        return dict;
    }
    string s;
    while (getline(in,s)){
        dict.push_back(s);
    }
    for(auto r:res){
        for(auto word:r){
            cout<<dict[word]<<" ";
        }cout<<endl;
    }
    return dict;
}

void move(string root){
    string path = root+"/number";
    vector<string> files;
    struct dirent *dirp;
    int weekday = 0,week = 1;
    string week_path;
    DIR* dir = opendir(path.data());
    while ((dirp = readdir(dir)) != nullptr) {
        if (dirp->d_type == DT_DIR) {
            string file_path = path+"/"+dirp->d_name;
            vector<string> files = getAllFiles(file_path);
            if(files.size()<5){continue;}
            for(auto file:files){
                ifstream in(file_path+"/"+file);
                if(!in.is_open()){
                    cout<<"error"<<endl;
                    continue;
                }
                if(weekday==0){
                    week_path = root+"/week/"+to_string(week);
                    if(access(week_path.data(),2)!=0){
                        mkdir(week_path.data(),0755);
                    }
                    week++;
                    weekday++;
                }
                std::ofstream ofs(week_path+"/"+to_string(weekday));

                string s;
                while (getline(in,s)){
                    ofs<<s<<endl;
                }
                weekday = (weekday+1)%8;
            }
        }
    }
    closedir(dir);
}

vector<ctnr> intersection(vector<ctnr> &v1, vector<ctnr> &v2){
    vector<ctnr> ret;
    set<ctnr> retSet;
    sort(v1.begin(),v1.end());
    sort(v2.begin(),v2.end());
    vector<ctnr>::iterator iter = v2.begin();
    for(auto vv1:v1){
        int l = -1;
        while (iter!=v2.end()){
            vector<int>::iterator iter1 = vv1.begin(),iter2 = iter->begin();
            while(iter1!=vv1.end()&&iter2!=iter->end()){
                if(*iter1==*iter2){
                    iter1++;
                    iter2++;
                } else{
                    break;
                }
            }
            if(iter1==vv1.end()){
                retSet.insert(vv1);
                break;
            }else if(iter2==iter->end()||*iter1>*iter2){
                iter++;
                continue;
            } else{
                break;
            }
        }

    }
    ret.assign(retSet.begin(),retSet.end());
    return ret;
}