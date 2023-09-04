#include <queue>
#include "suffixtree.h"
#include "File.h"

using namespace std;

/* x1,y1     x2,y1
 *
 *
 * x1,y2     x2,y2*/
struct Quad{
    int x1;
    int x2;
    int y1;
    int y2;
    Quad(int x1,int x2,int y1,int y2):x1(x1),x2(x2),y1(y1),y2(y2){}
    Quad() {}
};
class QuadNode{
public:
    int index;
    Quad q;

    void setQ(const Quad &q) {
        QuadNode::q = q;
    }

    QuadNode* child[4]={NULL};
    QuadNode(int index):index(index){}
    QuadNode(int index,Quad q):index(index),q(q){}

};

class QuadTrees {
    QuadNode *root;
    int last_index;
    bool inRegion(Quad q){
        return false;
    }

public:
    QuadTrees():root(NULL),last_index(0){}
    QuadTrees(int x1,int x2,int y1,int y2){
        last_index = 0;
        vector<vector<QuadNode*>> v;
        int count = 0;
        for(int i=x1;i<x2;i++){
            vector<QuadNode*> vv;
            for (int j = y1; j < y2; ++j) {
                Quad quad_tmp(i-1,i,j-1,j);
                QuadNode *q = new QuadNode(++last_index,quad_tmp);
                vv.push_back(q);
                count++;
            }
            v.push_back(vv);
        }
        while (count>1){
            count=0;
            vector<vector<QuadNode*>> v_new;
            for(int i=0;i<v.size();i+=2){
                vector<QuadNode*> vv;
                for(int j=0;j<v[i].size();j+=2){
                    QuadNode *q = new QuadNode(++last_index);
                    int xx1 = v[i][j]->q.x1,yy1 = v[i][j]->q.y1,xx2 = v[i][j]->q.x2,yy2 = v[i][j]->q.y2;
                    q->child[0] = v[i][j];
                    if(i+1<v.size()){
                        xx2 = v[i+1][j]->q.x2;
                        q->child[1] = v[i+1][j];
                    }
                    if(j+1<v[i].size()){
                        yy2 = v[i][j+1]->q.y2;
                        q->child[2] = v[i][j+1];
                    }
                    if(i+1<v.size()&&j+1<v[i].size()){
                        q->child[3] = v[i+1][j+1];
                    }
                    Quad quad_tmp(xx1,xx2,yy1,yy2);
                    q->setQ(quad_tmp);
                    vv.push_back(q);
                    count++;
                }
                v_new.push_back(vv);
            }
            v = v_new;
        }
        root = v[0][0];
    }
    void print(){
        queue<QuadNode*> que;
        if(root==NULL){return;}
        que.push(root);
        while (!que.empty()){
            QuadNode* cur = que.front();
            que.pop();
            cout<<cur->q.x1<<","<<cur->q.y1<<","<<cur->q.x2<<","<<cur->q.y2<<endl;
            for (auto node:cur->child){
                if(node!=NULL)
                    que.push(node);
            }
        }
        return;
    }
    void selerailize(string path){
        string basic_path = path+"/basic_model";
        string quad_path = path + "/quad_model";
        queue<QuadNode*> que;
        if(root==NULL){return;}
        que.push(root);
        while (!que.empty()){
            QuadNode* cur = que.front();
            que.pop();
            SuffixTree tree;
            for(int p=cur->q.x1;p<cur->q.x2;p++) {
                string file_path = basic_path+"/"+to_string(p);
                for(int i=cur->q.y1;i<cur->q.y2;i++){
                    vector<ctnr> documents = readFile(file_path+"/"+to_string(i));
                    if(!documents.empty())
                        tree.add_string(documents);
                }
            }
            tree.dump_tree();
            tree.save(quad_path+"/"+to_string(cur->index));
            for (auto node:cur->child){
                if(node!=NULL)
                    que.push(node);
            }
        }
        return;
    }

    vector<int> getRegion(int x1,int x2,int y1,int y2){
        vector<int> ret;
        queue<QuadNode*> que;
        if(root==NULL){return ret;}
        que.push(root);
        while (!que.empty()){
            QuadNode* cur = que.front();
            que.pop();
            Quad q = cur->q;
            if(q.x2<=x1||q.x1>=x2||q.y2<=y1||q.y1>=y2){//完全不在划定范围内的节点
                continue;
            }else if(q.x1>=x1&&q.x2<=x2&&q.y1>=y1&&q.y1<=y2){//被划定范围完全包含的节点
                ret.push_back(cur->index);
                continue;
            }
            //剩余节点继续向下层搜索
            for (auto node:cur->child){
                if(node!=NULL)
                    que.push(node);
            }
        }
        return ret;
    }
    vector<int> getIncludeRegion(int x1,int x2,int y1,int y2){
        vector<int> ret;
        QuadNode *cur = root;
        bool flag = true;
        while (flag){
            flag = false;
            for(auto child:cur->child){
                Quad q = cur->q;
                if(q.x1>=x1&&q.y1>=y1&&q.x2<=x2&&q.y2<=y2){
                    flag=true;
                    cur = child;
                    break;
                }
            }
        }
        ret.push_back(cur->index);
        queue<QuadNode*> que;
        que.push(cur);
        while (!que.empty()){
            QuadNode* cur = que.front();
            que.pop();
            for(auto child:cur->child) {
                Quad q = child->q;
                if(q.x2<=x1||q.x1>=x2||q.y2<=y1||q.y1>=y2){//完全不在划定范围内的节点
                    ret.push_back(child->index);
                }else if(q.x1>=x1&&q.x2<=x2&&q.y1>=y1&&q.y1<=y2){//被划定范围完全包含的节点
                    continue;
                }else{
                    que.push(child);
                }
            }
        }
        return ret;
    }
};
