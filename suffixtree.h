#ifndef _SUFFIX_TREE_HPP_INCLUDED_
#define _SUFFIX_TREE_HPP_INCLUDED_

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <list>
#include <utility>
#include <memory>
#include <iterator>
#include <limits.h>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "frequency.h"
#include <algorithm>


using namespace std;

class SuffixTree {

public:static int maxId;
    int id;
    struct Node;
    typedef typename std::vector<int> string;
    typedef typename std::iterator_traits<typename string::iterator>::difference_type index_type;
    typedef std::tuple<Node*,index_type, index_type> ReferencePoint;
    // NESTED CLASSES DEFINITIONS

    // Node class:
    // Contains the suffix link to another Node
    // The Transitions "g(s,(k,i)) = s'" to children nodes
    // 
    // Note:
    // Transitions are stored in a hashtable indexed by the first substring
    // character. A given character can only have at most one Transition in a
    // node.
    
    // A Generalized Suffix Tree can contain more than one string at a time
    // Each string is labeled with an int. Thus each substring is related to
    // an appropriate reference string:
    // (ref string id, left ptr, right ptr)
    struct MappedSubstring {
//        SuffixTree * suffixTree;
        int ref_str;
        index_type l;
        index_type r;
//        MappedSubstring(SuffixTree * suffixTree, int ref_str, index_type l, index_type r) {
//            this->suffixTree = suffixTree;
//            this->ref_str = ref_str;
//            this->l = l;
//            this->r = r;
//        }
        MappedSubstring() : ref_str(0), l(0), r(0) {}
        MappedSubstring(int ref, index_type left, index_type right) :
          ref_str(ref),
          l(left),
          r(right)
          {}
        bool empty() const {
            return (this->l > this->r);
        }

//        bool operator < (const MappedSubstring & mappedSubstring) const {
//            return (suffixTree != mappedSubstring.suffixTree) && (ref_str < mappedSubstring.ref_str ||
//            (ref_str == mappedSubstring.ref_str && l < mappedSubstring.l) ||
//                    (ref_str == mappedSubstring.ref_str && l == mappedSubstring.l && (r < mappedSubstring.r)));
//        }

        void save(std::ofstream & ofs){
            ofs <<" "<< ref_str << " " << l <<" "<< r;
        }
         vector<int> to_string(SuffixTree * suffixTree) {
            vector<int> phrase;
            vector<int> current_string = suffixTree->haystack[ref_str];
            for (int i = l; i <= r; i++) {
                phrase.emplace_back(current_string[i]);
            }
             return phrase;
        }
    };


    struct Transition {
        MappedSubstring sub;
        Node *tgt;
        Transition *next_trans;
        Transition *parent_trans;
        int ranking;
        Transition() : sub(), tgt(nullptr), next_trans(nullptr), parent_trans(nullptr) {}
        Transition(MappedSubstring s, Node *t) : sub(s), tgt(t), next_trans(nullptr), parent_trans(nullptr) {}
        bool is_valid() {
            return (tgt != nullptr);
        }

    };

    vector<vector<int>> get_phrase(Transition &transition) {
        vector<int> base;
        vector<vector<int>> phrases;
        Transition *parent = transition.parent_trans;
        while (parent) {
            for (int i = parent->sub.l; i <= parent->sub.r && i < haystack[parent->sub.ref_str].size(); i++) {
            base.push_back(haystack[parent->sub.ref_str][i]);
                }
            parent = parent->parent_trans;
        }
        for (int i = transition.sub.l; i <= transition.sub.r && i < haystack[transition.sub.ref_str].size() - 1; i++) {
            base.push_back(haystack[transition.sub.ref_str][i]);
            phrases.push_back(vector<int>(base.begin(), base.end()));
        }
        return phrases;

    }

    int match_phrase(const vector<int>& phrase, int l, int r, Node & current_node, int frequency) {
        if (l > r) {
            return frequency;
        }
        if (current_node.g.find(phrase[l]) == current_node.g.end()) {  // 匹配不到
            return 0;
        } else {
            const Transition & transition = current_node.g[phrase[l]];
            vector<int> node_phrase = haystack[transition.sub.ref_str];
            for (int i = transition.sub.l; i <= transition.sub.r && l <= r; i++,l++) {
                if (node_phrase[i] != phrase[l]) { // 如果不匹配
                    return 0;
                }
            }
            return match_phrase(phrase, l, r, * transition.tgt, transition.tgt->freq);
        }

    }

    struct Node {
        std::unordered_map<int, Transition> g;
        Node *suffix_link;
        int freq=0;
        virtual Transition find_alpha_transition(int alpha) {
            auto it = g.find(alpha);
            if (g.end() == it) {
                return Transition(MappedSubstring(0, 0, -1), nullptr);
            }
            return it->second;
        }
        Node() : suffix_link(nullptr),freq(1){}
        Node(int freq) : suffix_link(nullptr),freq(freq){}
        virtual ~Node() {
//            std::unordered_map<int, Transition>().swap(g);
//            delete suffix_link;
        }

        void dump_info() {
            for (auto t : g) {
                std::cout << "Transition for character: " << t.first << std::endl;
            }
        }
        void save(std::ofstream & ofs,std::unordered_map<Node*, int> &add_to_key){
            int suffix_num = add_to_key.find(suffix_link)==add_to_key.end()?-1:add_to_key[suffix_link];
            ofs << " "<<add_to_key[this]<< " " << freq <<" "<< g.size() << " "<< suffix_num;
        }
        bool isFrequent(int threshold){
            return freq>=threshold;
        }
    };


    // Simple workaround for the specific sink node
    // This node must have a transition for every char of the input alphabet.
    // Instead of creating such transitions, we just make them up through
    // an override of `find_alpha_transition`
    struct SinkNode : public Node {
        virtual Transition find_alpha_transition(int alpha) override {
            return Transition(MappedSubstring(0, 0, 0), this->suffix_link);
        }
    };

    // Leaf nodes:
    // Leaves must contain an explicit reference to the suffix they represent
    // Some strings might have common suffixes, hence the map.
    // The suffix link **remains** UNIQUE nonetheless.
    struct Leaf : public Node {
//        friend class boost::serialization::access;
        // TODO
    };

    // Base - A tree nested base class
    // This clase is here to hide implementation details
    // And to handle destruction properly.
    //
    // The processing (insertion, deletion of strings) is done by SuffixTree,
    // Base handles the cleanup.
    struct Base {
        SinkNode sink;
        Node root;
        Transition *first_trans;
        Base() {
            root.suffix_link = &sink;
            sink.suffix_link = &root;
        }
        ~Base() {
            clean();
        }
        void clean() {
            std::list<Node*> del_list {&root};
            while (!del_list.empty()) {
                Node *current = del_list.front();
                del_list.pop_front();
                for (auto it : current->g) {
                    del_list.push_back(it.second.tgt);
                }
                if (&root != current) {
                    delete current;
                }
            }
        }
        bool save(std::string path){
            int key = 0;
            std::unordered_map<Node*, int> add_to_key;
            add_to_key[&sink] = key++;
            Node *cur;
            std::queue<Node*> q;
            q.push(&root);
            while(!q.empty()){
                cur = q.front();
                q.pop();
                add_to_key[cur] = key++;
                for(auto it:cur->g){
                    q.push(it.second.tgt);
                }
            }
            std::ofstream ofs_node(path+"/node");
            sink.save(ofs_node,add_to_key);
            q.push(&root);
            while(!q.empty()){
                cur = q.front();
                q.pop();
                cur->save(ofs_node,add_to_key);
                for(auto it:cur->g){
                    ofs_node<<" "<<it.first;
                    it.second.sub.save(ofs_node);
                    q.push(it.second.tgt);
                }
            }
            ofs_node.close();
            return true;
        }
        long load(std::string path){
            std::ifstream ifs_node(path+"/node");
            if(!ifs_node.is_open()){
                std::cout<<"Open file failure"<<std::endl;
                return 0;
            }
            ifs_node.seekg(0, std::ios::end);    // go to the end
            int length = ifs_node.tellg();           // report location (this is the length)
            ifs_node.seekg(0, std::ios::beg);    // go back to the beginning
            char * node_buffer = new char[length];    // allocate memory for a buffer of appropriate dimension
            char * copy = node_buffer;
            ifs_node.read(node_buffer, length);       // read the whole file into the buffer
            ifs_node.close();
            node_buffer = strtok(node_buffer," ");

            std::unordered_map<int, Node*> key_to_addr;
            std::unordered_map<int, int> suffix_map;
            int key,size,suffix_link,first,ref_str;
            index_type l, r;
            key = atoi(node_buffer);
            node_buffer = strtok(NULL," ");
            sink.freq = atoi(node_buffer);
            node_buffer = strtok(NULL," ");
            size = atoi(node_buffer);
            node_buffer = strtok(NULL," ");
            suffix_link = atoi(node_buffer);
            node_buffer = strtok(NULL," ");
            key_to_addr.insert(std::make_pair(key,&sink));
            Node *cur;
            std::queue<Node*> q;
            q.push(&root);
            //添加节点
            while (!q.empty()){
                cur = q.front();
                q.pop();
                key = atoi(node_buffer);
                node_buffer = strtok(NULL," ");
                cur->freq = atoi(node_buffer);
                node_buffer = strtok(NULL," ");
                size = atoi(node_buffer);
                node_buffer = strtok(NULL," ");
                suffix_link = atoi(node_buffer);
                node_buffer = strtok(NULL," ");
//                key_to_addr.insert(std::make_pair(key,cur));
//                ifs_node>>key>>cur->freq>>size>>suffix_link;
                suffix_map[key] = suffix_link;
                key_to_addr.insert(std::make_pair(key,cur));
                if(size<0){
                    std::cout<<"error";
                }
                while (size--){
                    first = atoi(node_buffer);
                    node_buffer = strtok(NULL," ");
                    ref_str = atoi(node_buffer);
                    node_buffer = strtok(NULL," ");
                    l = atoi(node_buffer);
                    node_buffer = strtok(NULL," ");
                    r = atoi(node_buffer);
                    node_buffer = strtok(NULL," ");
                    Node *child = new Node();
                    cur->g.insert(std::pair<int, Transition>(
                            first, Transition(MappedSubstring(ref_str, l, r), child)));
                    q.push(child);
                }
            }
            //添加suffix_link
            for(auto m:suffix_map){
                if(m.second!=-1)
                    key_to_addr[m.first]->suffix_link = key_to_addr[m.second];
            }
//            root.suffix_link = &sink;
//            sink.suffix_link = &root;
            ifs_node.close();
            delete [] copy;
            return length;
        }

        void unload() {
        }
    };

    // "OUTER" CLASS MEMBERS

    Base tree;

public:std::unordered_map<int, string> haystack;
    int last_index;
    
    std::string to_string(string const & s, index_type b, index_type e) {
        std::string result;
        if (0 <= b && e < s.size()) {
            for (auto i = b; i <= e; ++i) {
                result.push_back(s[i]);
            }
        }
        return result;
    }
    
    std::string to_string(string const & s) {
        return to_string(s, 0, s.size()-1);
    }

    bool test_and_split(Node *n, MappedSubstring kp, int t, const string& w, Node **r) {
        int tk = w[kp.l];
        index_type delta = kp.r - kp.l;
        if (0 <= delta) {
            Transition tk_trans = n->find_alpha_transition(tk);
            MappedSubstring kp_prime = tk_trans.sub;
            const auto& str_prime = haystack.find(kp_prime.ref_str);
            if (str_prime->second[kp_prime.l + delta + 1] == t) { //不需要插入新的节点
                *r = n;
                return true;
            } 
            *r = new Node(tk_trans.tgt->freq); //这里开始插入新的中间节点
            Transition new_t = tk_trans;
            new_t.sub.l += delta+1;
            (*r)->g.insert(std::pair<int, Transition>(
                str_prime->second[new_t.sub.l], new_t));
            tk_trans.sub.r = tk_trans.sub.l + delta;
            tk_trans.tgt = *r;
            n->g[tk] = tk_trans;
            return false;
                
        } else {
            Transition t_Transition = Transition(MappedSubstring(0, 0, -1), nullptr);
            t_Transition = n->find_alpha_transition(t);
            *r = n;
            return (t_Transition.is_valid());
        }
    }

    // update performs the heart of an iteration:
    // It walks the border path from the active point to the end point
    // and adds the required Transitions brought by the insertion of
    // the string's i-th character.
    //
    // It returns the end point.
    ReferencePoint update(Node *n, MappedSubstring ki) {
        Node *oldr = &tree.root;
        Node *r = nullptr;
        bool is_endpoint = false;
        MappedSubstring ki1 = ki;
        const auto& ref_str_it = haystack.find(ki.ref_str);
        const string& w = ref_str_it->second;
        ReferencePoint sk(n, ki.ref_str, ki.l);
        ki1.r = ki.r-1;
        is_endpoint = test_and_split(n, ki1, w[ki.r], w, &r);
        while (!is_endpoint) { //需要插入新的节点的时候，不需要插入新的节点的时候会返回is_endpoint = false
            Node *r_prime = new Node(); //插入新的叶子结点
            r->g.insert(std::make_pair( //新的叶子节点的插入
              w[ki.r], Transition(MappedSubstring(
              ki.ref_str, ki.r, INT_MAX), r_prime)));
            if (&tree.root != oldr) {
                oldr->suffix_link = r;
            }
            oldr = r;
            sk = canonize(std::get<0>(sk)->suffix_link, ki1);
            ki1.l = ki.l = std::get<2>(sk);
            is_endpoint = test_and_split(std::get<0>(sk), ki1, w[ki.r], w, &r);
        }
        if (&tree.root != oldr) {
            oldr->suffix_link = std::get<0>(sk);
        }
        return sk;
    }

    // canonize - Get canonical pair
    // Given a Node and a substring,
    // This returns the canonical pair for this particular combination
    ReferencePoint canonize(Node *n, MappedSubstring kp) {
        if (kp.r < kp.l)
            return ReferencePoint(n, kp.ref_str, kp.l);
        const auto& kp_ref_str = haystack.find(kp.ref_str);
        index_type delta;
        Transition tk_trans = n->find_alpha_transition(kp_ref_str->second[kp.l]);
        while ((delta = tk_trans.sub.r - tk_trans.sub.l) <= kp.r - kp.l) {
            kp.l += 1 + delta;
            n = tk_trans.tgt;
            if (kp.l <= kp.r)
                tk_trans = n->find_alpha_transition(kp_ref_str->second[kp.l]);
        }
        return ReferencePoint(n, kp.ref_str, kp.l);
    }

    // get_starting_node - Find the starting node
    // @s[in]: The string to insert
    // @r[in/out]: The walk starting/ending point
    //
    // get_starting_node walks down the tree until s does not match anymore
    // character.
    // @r is updated through this process and contains the reference pair of the
    // diverging point between @s and the tree.
    // The result '(s,k)' of this function may then be used to resume the Ukkonen's
    // algorithm.
    index_type get_starting_node(const string& s, ReferencePoint *r) {
        auto k = std::get<2>(*r);
        auto s_len = s.size();
        bool s_runout = false;
        while (!s_runout) {
            Node *r_node = std::get<0>(*r);
            if (k >= s_len) {
                s_runout = true;
                break;
            }
            auto t = r_node->find_alpha_transition(s[k]);
            if (nullptr != t.tgt) {
                index_type i;
                const auto& ref_str = haystack.find(t.sub.ref_str);
                for (i=1; (i <= t.sub.r - t.sub.l); ++i) {
                    if (k+i >= s_len) {
                        s_runout = true;
                        break;
                    }
                    if (s[k+i] != ref_str->second[t.sub.l+i]) {
                        std::get<2>(*r) = k;
                        return k+i;
                    }
                }
                if (!s_runout) {
                   std::get<0>(*r) = t.tgt;
                   k += i;
                   std::get<2>(*r) = k;
                }
            } else {
                return k;
            }
        }
        std::get<2>(*r) = INT_MAX;
        return INT_MAX;
    }

    // deploy_suffixes - Deploy suffixes
    // @s[in]: The string to insert in the tree
    // @sindex[in]: The index id of @s
    //
    // deploy_suffixes performs the Ukkonen's algorithm to inser @s into the
    // tree.
public: int deploy_suffixes(const string& s, int sindex) {
        ReferencePoint active_point(&tree.root, sindex, 0);
        auto i = 0;
        for (; i < s.size(); ++i) {
            MappedSubstring ki(sindex,std::get<2>(active_point), i);
            active_point = update(std::get<0>(active_point), ki);
            ki.l = std::get<2>(active_point);
            active_point = canonize(std::get<0>(active_point), ki);
        }
        return sindex;
    }

    int dump_node(Node *n, bool same_line, index_type padding, MappedSubstring orig) {
        index_type delta = 0;
//        if (!same_line) {
//            for (index_type i = 0; i < padding; ++i) {
//                std::cout << "  ";
//            }
//        }
        if (!orig.empty()) {
            const auto& s = haystack.find(orig.ref_str);
//            for (index_type i = orig.l; i <= orig.r && i < s->second.size(); ++i) {
//                std::cout << s->second[i] << " ";
//            }
//            std::cout << "- ";
            delta = orig.r - orig.l + 2;
            if (orig.r == INT_MAX) {
                delta = s->second.size() - orig.l;
            }
        }
        same_line = true;
        if(n->g.size()!=0){
            n->freq = 0;
        }
        for (auto t_it : n->g) {
            n->freq += dump_node(t_it.second.tgt, same_line, padding + delta, t_it.second.sub);
            same_line = false;
        }
//        if (same_line) {
//            std::cout << "##" << std::endl;
//        }
        return n->freq;
    }

    /**
     * 找出在threshold以上频率的phrases
     * @param n
     * @param threshold
     * @param path
     * @param orig
     * @param res
     * @return
     */
    int mining_node_threshold(Node *n, int threshold, std::vector<int> &path,MappedSubstring orig,std::vector<std::vector<int>>& res) {
        int ret = 0;
        if(n->freq<threshold){
            return ret;
        }
        index_type delta = 0;
        if (!orig.empty()) {
            const auto& s = haystack.find(orig.ref_str);
            for (index_type i = orig.l; i <= orig.r && i < s->second.size(); ++i) {
                path.push_back(s->second[i]);
            }
            delta = orig.r - orig.l + 2;
            if (orig.r == INT_MAX) {
                delta = s->second.size() - orig.l;
            }
        }

        for (auto t_it : n->g) {
            ret += mining_node_threshold(t_it.second.tgt, threshold,path, t_it.second.sub,res);
        }
        if(ret == 0&&path.size()>1){
            res.push_back(path);
            ret++;
        }
        while(--delta>0){
            path.pop_back();
        }
        return ret;
    }

    //根据已有短语判断是否频繁
    int mining_node(Node *n, int threshold, MappedSubstring orig, std::vector<int> &path,int index) {
        if(n->freq<threshold){
            return index;
        }
        if (!orig.empty()) {
            const auto& s = haystack.find(orig.ref_str);
            for (index_type i = orig.l; i <= orig.r && i < s->second.size(); ++i) {
                if(index < path.size() && s->second[i]==path[index]){
                    index++;
                }else{return index;}
            }
        }
        if(index==path.size()){
            return index;
        }

        auto it = n->g.find(path[index]);
        if (n->g.end() == it) {
            return index;
        }
        return mining_node(it->second.tgt, threshold,it->second.sub,path,index);
    }

    //做加法
public: void mining_node_add(Node *n, MappedSubstring orig, std::vector<frequency> &path,int index) {
        if (!orig.empty()) {
            const auto& s = haystack.find(orig.ref_str);
            for (index_type i = orig.l; i <= orig.r && i < s->second.size(); ++i) {
                if(index < path.size() && s->second[i]==path[index].term){
                    path[index].freq += n->freq;
                    index++;
                }else{return;}
            }
        }
        if(index==path.size()){
            return;
        }

        auto it = n->g.find(path[index].term);
        if (n->g.end() == it) {
            return;
        }
        return mining_node_add(it->second.tgt,it->second.sub,path,index);
    }
    //做减法
    int mining_node_sub(Node *n, int threshold, MappedSubstring orig, std::vector<frequency> &path,int index) {
        if (!orig.empty()) {
            const auto& s = haystack.find(orig.ref_str);
            for (index_type i = orig.l; i <= orig.r && i < s->second.size(); ++i) {
                if(index < path.size() && s->second[i]==path[index].term){
                    path[index].freq = path[index].freq - n->freq;
                    index++;
                }else{return index;}
            }
        }
        if(index==path.size()){
            return index;
        }

        auto it = n->g.find(path[index].term);
        if (n->g.end() == it||path[index].freq - it->second.tgt->freq < threshold) {
            return index;
        }
        return mining_node_sub(it->second.tgt, threshold,it->second.sub,path,index);
    }
    //记录路径及词频的挖掘
    int mining_node(Node *n, int threshold, std::vector<frequency> &path,MappedSubstring orig,std::vector<std::vector<frequency>>& res) {
        int ret = 0;
        if(n->freq<threshold){
            return ret;
        }
        index_type delta = 0;
        if (!orig.empty()) {
            const auto& s = haystack.find(orig.ref_str);
            for (index_type i = orig.l; i <= orig.r && i < s->second.size(); ++i) {
                path.push_back(frequency(s->second[i],n->freq));
            }
            delta = orig.r - orig.l + 2;
            if (orig.r == INT_MAX) {
                delta = s->second.size() - orig.l;
            }
        }

        for (auto t_it : n->g) {
            ret += mining_node(t_it.second.tgt, threshold,path, t_it.second.sub,res);
        }
        if(ret == 0&&path.size()>1){
            res.push_back(path);
            ret++;
        }
        while(--delta>0){
            path.pop_back();
        }
        return ret;
    }

    template <typename InputIterator>
    bool contain_end_token(InputIterator const & str_begin, InputIterator const & str_end) {
        return (std::find(str_begin, str_end, -1*last_index) != str_end);
    }
    
    template <typename InputIterator, bool append_end_token = true>
    string make_string(InputIterator const & str_begin, InputIterator const & str_end) {
        if (contain_end_token(str_begin, str_end)) {
            throw std::invalid_argument("Input range contains the end token");
        }
        return make_string(str_begin, str_end, std::integral_constant<bool, append_end_token>());
    }
    
    template <typename InputIterator>
    string make_string(InputIterator const & str_begin, InputIterator const & str_end, std::true_type) {
        string s(str_begin, str_end);
        s.push_back(-1*last_index);
        return s;
    }
    
    template <typename InputIterator>
    string make_string(InputIterator const & str_begin, InputIterator const & str_end, std::false_type) {
        index_type str_len = std::distance(str_begin, str_end);
        string s(str_begin, str_end);
        return s;
    }
public:
    SuffixTree() : last_index(0){
    }

    int add_string(std::vector<std::vector<int>> strings){
        for(auto doc: strings){
            if(doc.size()<2)
                continue;
            add_string(doc.begin(),doc.end());
        }
        return last_index;
    }
    template <typename InputIterator>
    int add_string(InputIterator const & str_begin, InputIterator const & str_end) {
        ++last_index;
        auto s = make_string(str_begin, str_end);
        haystack.emplace(last_index, std::move(s));
        const auto& s_from_map = haystack.find(last_index);
        if (0 > deploy_suffixes(s_from_map->second, last_index)) {
            haystack.erase(last_index--);
            return -1;
        }
        // tests
//        dump_node(&this->tree.root, true, 0, MappedSubstring(1,0,-1));
////        vector<frequency> path;
////        path.emplace_back(1,0);
////        path.emplace_back(2, 0);
////        mining_node(&this->tree.root, MappedSubstring(1,1,0), path,  0);
//
//vector<vector<int>> result = mining_tree(3);
return last_index;
    }

    template <typename InputIterator>
    bool is_suffix(InputIterator const & str_begin, InputIterator const & str_end) {
        auto s = make_string(str_begin, str_end);
        ReferencePoint root_point(&tree.root, -1, 0);
        return (get_starting_node(s, &root_point) == std::numeric_limits<index_type>::max());
    }
    
    template <typename InputIterator>
    bool is_substring(InputIterator const & str_begin, InputIterator const & str_end) {
        string s(str_begin, str_end);
        ReferencePoint root_point(&tree.root, -1, 0);
        return (get_starting_node(s, &root_point) == std::numeric_limits<index_type>::max());
    }

    ~SuffixTree() {
    }

public:void dump_tree() {
        dump_node(&tree.root, true, 0, MappedSubstring(0,0,-1));
    }

    static bool frequency_cmp(const Transition *n1, const Transition *n2) {
        if ((*n1).tgt->freq > (*n2).tgt->freq) {
            return true;
        }
        return false;
    }


    void sort_tree_new() {
        std::vector<Transition *> sort_trans;
        for (auto iter = tree.root.g.begin(); iter != tree.root.g.end(); ++iter) {
            if (iter->first != -1) {
                sort_trans.push_back(&(iter->second));
            }
        }
        int finish = 0;
        int start = 0;
        int end = sort_trans.size();
        while (!finish) {
            finish = 1;
            for (int i = start; i < end; i++) {
                for (auto iter = sort_trans[i]->tgt->g.begin(); iter != sort_trans[i]->tgt->g.end(); ++iter) {
                    if (iter->first != -1) {
                        sort_trans.push_back(&(iter->second));
                        iter->second.parent_trans = sort_trans[i];
                        finish = 0;
                    }
                }
            }
            start = end;
            end = sort_trans.size();
        }
        sort(sort_trans.begin(), sort_trans.end(), frequency_cmp);
        // 用链接的方式存储顺序
        for (int i = 0; i < sort_trans.size() -1; i++) {
            (*sort_trans[i]).next_trans = sort_trans[i + 1];
            sort_trans[i]->ranking = i;
//            if (id == 0 && sort_trans[i]->tgt->freq == 18 ) {
//                vector<vector<int>> phrases = get_phrase(*(sort_trans[i]));
//                for (vector<int> phrase: phrases) {
//                    for (int term: phrase) {
//                        cout << term << " ";
//                    }
//                    cout << endl;
//                }
//            }
        }
        tree.first_trans = sort_trans[0];
    }

    static bool sortByValue(const pair<vector<int>, int>& a, const pair<vector<int>, int>& b) {
        return a.second > b.second;
    }

    /* 把sfst转化成phrase2frequency的list，为了方便部分存储 */
    vector<pair<vector<int>, int>> tree_2_list() {
        map<vector<int>,int> phrase2freq;
        Transition *trans = tree.first_trans;
        while (trans) {
            vector<vector<int>> phrases = get_phrase(*trans);
            int freq = trans->tgt->freq;
            for (auto & phrase : phrases) {
                phrase2freq.insert(pair<vector<int>, int>(phrase, freq));
            }
            trans = trans->next_trans;

        }
        vector<pair<vector<int>, int>> vec(phrase2freq.begin(), phrase2freq.end());
        sort(vec.begin(), vec.end(), sortByValue);
        return vec;

    }

    struct VectorKey {
        std::vector<int> vec;

        bool operator<(const VectorKey& other) const {
            return vec < other.vec;
        }
    };

    /* 把两颗suffix tree合并成一颗 */
    vector<pair<vector<int>, int>> tree_merge(vector<SuffixTree*> trees) {
        map<VectorKey, int> phrase2freq;
        for (const auto& current_tree :trees) {
            vector<pair<vector<int>, int>> current_list = current_tree->tree_2_list();
            for (const auto& edge :current_list) {
                VectorKey my_edge{edge.first};
                if (phrase2freq.find(my_edge) == phrase2freq.end())  {
                    phrase2freq.insert(pair<VectorKey, int>(my_edge, edge.second));
                } else {
                    int freq = phrase2freq[my_edge] + edge.second;
                    phrase2freq[my_edge] = freq;
                }
            }
        }
        vector<pair<vector<int>, int>> vec;
        for (const auto& p : phrase2freq) {
           vec.emplace_back(p.first.vec, p.second);
        }
        sort(vec.begin(), vec.end(), sortByValue);
        return vec;
    }

    // suffix tree只保存前l个transition
    vector<pair<vector<int>, int>> partial_storage(int l) {
        vector<pair<vector<int>, int>> vec = tree_2_list();
        vec.erase(vec.begin() + l, vec.end());
        return vec;

    }

    map<vector<int>,int> partial_query(vector<vector<pair<vector<int>, int>>> *lists, int threshold) {
        map<vector<int>,int> result;
        int current_pos = 0;
        int size = lists->size();
        int total_upper_bound = INT_MAX;

        // 横向：R1, R2, ... Rn, lower bound, upper bound   纵向：x1, x2, ..., xn
        map<vector<int>, vector<int>> phrase2frequency;
        int current_level = 0;
        int max_size = 0;
        for (int j = 0; j < size; j++) {
            int current_list_size = lists[j].size();
            if (max_size < current_list_size) {
                max_size = current_list_size;
            }
        }
        for (int i = 0; i < max_size; i++) { // 循环每一层
            /* 遍历每个list, 记录当前层 term的出现情况 */
            for (int j = 0; j < size; j++) {
                int current_list_size = lists[j].size();
                if (i >= current_list_size) {  // 当前list已经结束了
                    continue;
                }
                vector<int> phrase = (*lists)[j][i].first;
                int freq = (*lists)[j][i].second;
                /* 统计词频 */
                if (phrase2frequency.empty() || phrase2frequency.count(phrase) == 0) { // current phrase没有被统计过
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
                    phrase2frequency.insert(
                            pair<vector<int>, vector<int>>(phrase, count));
                } else {
                    phrase2frequency[phrase][j] = phrase2frequency[phrase][j] + freq;
                }
            }
            /* 更新LB & UB */
            for (auto iter = phrase2frequency.begin(); iter != phrase2frequency.end(); ++iter) {
                int lower_bound = 0, upper_bound = 0;
                for (int j = 0; j < size; j++) {
                    int current_list_size = lists[j].size();
                    lower_bound += iter->second[j];
                    if (iter->second[j] == 0) {
                        if (i < current_list_size) {
                            upper_bound += (*lists)[j][i].second;
                        } else {
                            upper_bound += (*lists)[j][current_list_size-1].second;
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
        for (auto iter = phrase2frequency.begin(); iter != phrase2frequency.end(); ++iter) {
            vector<int> phrase = iter->first;
            if (iter->second[size] >= threshold) {  // 如果LB>K,直接加入result
                result.insert(pair<vector<int>,int>(phrase,iter->second[size]));
            } else if (iter->second[size + 1] >= threshold) {  // 如果 LB < K & UB >K, 再去确认一番
                int frequency = 0;
                for (int j = 0; j < size; j++) {
                    if (iter->second[j] != 0) {
                        frequency += iter->second[j];
                    } else {
                        // 检查一下会不会在之后的level出现
                        for (int i = current_level+1; i < lists[j].size(); i++) {
                            if (equal((*lists)[j][i].first.begin(), (*lists)[j][i].first.end(), phrase.begin(), phrase.end())) {
                                frequency += (*lists)[j][i].second;
                                break;
                            }
                        }
                    }
                }
                if (frequency >= threshold) {
                    result.insert(pair<vector<int>,int>(phrase,frequency));
                }
            }
        }

        return result;
    }


    std::vector<std::vector<int>> mining_tree(std::vector<std::vector<int>> &inputs,int threshold){
        std::vector<std::vector<int>> ret;
        std::vector<int> tmp;
        for(auto input:inputs){
            int length = mining_node(&tree.root, threshold,MappedSubstring(0,0,-1), input,0);
            if(length>1){
                std::vector<int> tmp(input.begin(),input.begin()+length);
                ret.push_back(tmp);
            }
        }
        return ret;
    }

    /**
     * 在后缀树中找到 frequency > hreshold 的所有phrase
     * @param threshold
     * @return
     */
    std::vector<std::vector<int>> mining_tree_threshold(int threshold) {
        std::vector<int> v;
        std::vector<std::vector<int>> res;
        mining_node_threshold(&tree.root, threshold, v, MappedSubstring(0,0,-1),res);
        return res;
    }

    std::vector<std::vector<frequency>> mining_tree_sub(SuffixTree *t1,SuffixTree *t2,SuffixTree *t3,int threshold){
        std::vector<std::vector<frequency>> inputs = mining_tree1(threshold);
        t1->mining_tree_add(inputs);
        inputs = t2->mining_tree_sub(inputs,threshold);
        inputs = t3->mining_tree_sub(inputs,threshold);
        return inputs;
    }

public:std::vector<std::vector<frequency>> mining_tree(std::vector<SuffixTree*> &trees, int threshold){
        std::vector<std::vector<frequency>> inputs = mining_tree1(threshold);
        for(int i=1;i<trees.size();i++){
//            cout<<inputs.size()<<endl;
            inputs = trees[i]->mining_tree_sub(inputs,threshold);
        }

        return inputs;
    }

    struct Sequence{
        vector<int> seq;
        vector<Node*> nodes;
        int freq;
        Sequence(){}
    };
//    std::vector<std::vector<frequency>> mining_tree(std::vector<SuffixTree*> trees, int threshold){
//        //这个方法暂时没实现出来，先不看了！
//        vector<vector<frequency>> ret;
//        vector<Sequence> sequences;
//        Sequence root_seq;
//        for(int j=0;j<trees.size();j++){
//            Node node = trees[j]->tree.root;
//            if(!node.isFrequent(threshold)){continue;}
//            vector<int> empty;
//            root_seq.nodes.push_back(&node);
//            root_seq.freq += node.freq;
//            sequences.push_back(root_seq);
//        }
//        while(!sequences.empty()){
//            vector<Sequence> sequences_new;
//            for(auto sequence:sequences){
//                vector<int> v = sequence.seq;
//                for(auto node:sequence.nodes){
//
//                }
//            }
//        }
//        return ret;
//    }

    std::vector<std::vector<frequency>> mining_tree_sub(std::vector<std::vector<frequency>> &inputs,int threshold){
        std::vector<std::vector<frequency>> ret;
        std::vector<frequency> tmp;
        for(auto input:inputs){
            int length = mining_node_sub(&tree.root, threshold,MappedSubstring(0,0,-1), input,0);
            if(length>1){
                std::vector<frequency> tmp(input.begin(),input.begin()+length);
                ret.push_back(tmp);
            }
        }
        return ret;
    }
    void mining_tree_add(std::vector<std::vector<frequency>> &inputs) {
        for(int i=0;i<inputs.size();i++){
            mining_node_add(&tree.root,MappedSubstring(0,0,-1), inputs[i],0);
        }
    }

    struct expand_node {
        SuffixTree * tree;
        Transition * base_transition;
        int current_pos;
        vector<int> phrase;
//        MappedSubstring phrase;
//        expand_node(SuffixTree * suffixTree, Transition & base_transition, int current_pos, vector<int> phrase) {
//            this->tree = suffixTree;
//            this->base_transition = base_transition;
////            this->base_transition->tgt = base_transition->tgt;
//            this->current_pos = current_pos;
//            for (int term: phrase) {
//                this->phrase.emplace_back(term);
//            }
//        }
        expand_node(SuffixTree * suffixTree, Transition * base_transition, int current_pos, vector<int> phrase) {
            this->tree = suffixTree;
            this->base_transition = base_transition;
            this->current_pos = current_pos;
            for (int term: phrase) {
                this->phrase.emplace_back(term);
            }
        }

        /**
         * 获得当前节点的下一个节点
         * 因为是压缩的后缀树，如果当前指针指向transition中间，往后挪一位；如果指向transition末尾，返回下一些transition
         * 相关的node
         * @return
         */
        vector<expand_node> get_next_node() {
            vector<expand_node> result;
            if (current_pos + base_transition->sub.l >= tree->haystack[base_transition->sub.ref_str].size() -2) { // 已经达到句子的末尾了
                return result;
            }
            if (current_pos < (base_transition->sub.r - base_transition->sub.l)) { // 如果当前指针指向transition中间，往后挪一位
                vector<int> new_phrase;
                for (int term: phrase) {
                    new_phrase.emplace_back(term);
                }
                new_phrase.emplace_back(tree->haystack[base_transition->sub.ref_str][base_transition->sub.l + current_pos + 1]);
                expand_node next_node = expand_node(tree, this->base_transition, current_pos + 1, new_phrase);
                result.emplace_back(next_node);
            } else { // 如果指向transition末尾，返回下一些transition
                for (auto & edge: base_transition->tgt->g) {
                    vector<int> new_phrase;
                    for (int term: phrase) {
                        new_phrase.emplace_back(term);
                    }
                    new_phrase.emplace_back(tree->haystack[edge.second.sub.ref_str][edge.second.sub.l]);
                    expand_node next_node = expand_node(tree, &(edge.second), 0, new_phrase);
                    result.emplace_back(next_node);
                }
            }
            return result;
        }
    };

    /**
     * 利用广搜实现多后缀树的高频词汇挖掘
     * @param trees
     * @param threshold
     */
public:static vector<vector<int>>  mining_tree_add_threshold(vector<SuffixTree*> trees, int threshold) {
        queue<expand_node> nodes;
        list<phrase_frequency> frequency_list;
        vector<vector<int>> result;
        int current_pos = 0;
        /* 对每棵树，根结点入队列 */
        for (SuffixTree* current_tree: trees) {
            Node &root = current_tree->tree.root;
            for (auto &edge: root.g) {
                if (edge.first < 0) {
                    continue;
                }
                vector<int> phrase;
                phrase.push_back(current_tree->haystack[edge.second.sub.ref_str][edge.second.sub.l]);
                nodes.push(expand_node(current_tree, &(edge.second), current_pos, phrase));
            }
        }
        while (!nodes.empty()) {
            /* 对队列中每个节点，做 frequency 运算 */
            map<vector<int>, int> phrase2frequency;
            int current_size = nodes.size();
            for (int i = 0; i < current_size; i++) {
                expand_node current_node = nodes.front();
                if (phrase2frequency.empty() || phrase2frequency.count(current_node.phrase) == 0) {
                    phrase2frequency.insert(
                            pair<vector<int>, int>(current_node.phrase, current_node.base_transition->tgt->freq));
                } else {
                    phrase2frequency[current_node.phrase] =
                            phrase2frequency[current_node.phrase] + current_node.base_transition->tgt->freq;
                }
                nodes.push(current_node);
                nodes.pop();
            }
            /* 对队列中的每个节点，如果frequency满足条件，子节点入队列，自己入list；不满足条件，pop，结束 */
            for (int i = 0; i < current_size; i++) {
                expand_node current_node = nodes.front();
//                if (current_node.phrase.size() == 2 && current_node.phrase[0] == 21905 && current_node.phrase[1] == 293011) {
//                    cout << current_node.tree->id << "   " << current_node.base_transition->tgt->freq << endl;
//                }
                if (phrase2frequency[current_node.phrase] >= threshold) { // frequency满足条件，子节点入队列，自己入list
                    vector<expand_node> sub_nodes = current_node.get_next_node();
                    for (expand_node sub_node: sub_nodes) {
                        nodes.push(sub_node);
                    }
                    result.emplace_back(current_node.phrase);
//                    frequency_list.push_front(phrase_frequency(current_node.phrase,
//                                                               phrase2frequency[current_node.phrase]));
                }
                nodes.pop();
            }
        }
//        return frequency_list;
        vector<vector<int>> res;
        res.insert(res.end(), result.begin(), result.end());
        set<vector<int>> new_res(res.begin(), res.end());
        result.assign(new_res.begin(), new_res.end());
        return result;
    }



    static vector<vector<int>>  mining_sorted_tree_add_threshold(vector<SuffixTree*> trees, int threshold) {
        vector<Transition*> trans;

//        queue<Node *> nodes;
        list<phrase_frequency> frequency_list;
        vector<vector<int>> result;
        int current_pos = 0;
        int size = trees.size();
        for (SuffixTree *current_tree:trees) {
            trans.emplace_back(current_tree->tree.first_trans);
        }
        // 横向：R1, R2, ... Rn, lower bound, upper bound   纵向：x1, x2, ..., xn
        map<vector<int>, vector<int>> phrase2frequency;
        // 统计trees当前最低的frequency
        vector<int> min_frequency;
        for (int i = 0; i < size; i++) {
            min_frequency.emplace_back(0);
        }
        int current_level = 1;
        int finish = 0;
        while (!finish) {
            int total_upper_bound = INT_MAX;
            finish = 1;
            /* 遍历当前层的所有suffix tree，更新每个phrase的出现情况 */
            for (int i = 0; i < size; i++) {
                SuffixTree * current_tree = trees[i];
                Transition * current_trans = trans[i];
                if (current_trans == nullptr) {
                    min_frequency[i] = 0;
                    continue;
                }
                int frequency = current_trans->tgt->freq;
                // 更新trans queue
                if (current_trans->next_trans != nullptr) {
                    trans[i] = current_trans->next_trans;
                    finish = 0;
                } else {
                    trans[i] = nullptr;
                }
                // 更新min_frequency
                min_frequency[i] = frequency;
                // 更新 phrase出现的情况
                vector<vector<int>> phrases = current_tree->get_phrase(*current_trans);
                for (const vector<int>& phrase: phrases) {
//                    if (phrase.size() == 2 && phrase[0] == 21905 && phrase[1] =xiao si= 293011) {
//                        cout << frequency;
//                    }
                    if (phrase2frequency.empty() || phrase2frequency.count(phrase) == 0) { // current phrase没有被统计过
                        // 纵向：x1, x2, ..., xn
                        vector<int> count;
                        for (int j = 0; j < size; j++) {
                            if (i == j) {
                                count.emplace_back(frequency);
                            } else {
                                count.emplace_back(0);
                            }
                        }
                        // lower bound & upper bound
                        count.emplace_back(0);
                        count.emplace_back(0);
                        phrase2frequency.insert(
                                pair<vector<int>, vector<int>>(phrase, count));
                    } else {
                        phrase2frequency[phrase][i] = phrase2frequency[phrase][i] + frequency;
                    }
                }
            }
            /* 更新 upper bound * lower bound */
            for (auto iter = phrase2frequency.begin(); iter != phrase2frequency.end(); ++iter) {
                int lower_bound = 0, upper_bound = 0;
                for (int i = 0; i < size; i++) {
                    lower_bound += iter->second[i];
                    if (iter->second[i] == 0) {
                        upper_bound += min_frequency[i];
                    } else {
                        upper_bound += iter->second[i];
                    }
                }
                iter->second[size] = lower_bound;
                iter->second[size+1] = upper_bound;
                if (total_upper_bound > upper_bound) {
                    total_upper_bound = upper_bound;
                }

            }
            /* 当UB<K,中止当层，进入下一层，然后回溯之前遍历过的map;如果LB>K,直接加入result;如果 LB < K & UB >K ,再去确认一番 */
            if (total_upper_bound < threshold) {
                break;
            }
        }

        for (auto iter = phrase2frequency.begin(); iter != phrase2frequency.end(); ++iter) {
            if (iter->second[size] >= threshold) {  // 如果LB>K,直接加入result
                result.emplace_back(iter->first);
            } else if (iter->second[size + 1] >= threshold) {  // 如果 LB < K & UB >K, 再去确认一番

                int frequency = 0;
                for (SuffixTree *current_tree:trees) {
                    frequency += current_tree->match_phrase(iter->first, 0, iter->first.size()-1, current_tree->tree.root, 0);
                }
                if (frequency >= threshold) {
                    result.emplace_back(iter->first);
                }
            }
        }

        vector<vector<int>> res;
        res.insert(res.end(), result.begin(), result.end());
        set<vector<int>> new_res(res.begin(), res.end());
        result.assign(new_res.begin(), new_res.end());
        return result;
    }

    /**
     *
     * @param freq_lists {[term,freq],[term,freq]...},{[term,freq],[term,freq]...}
     * @param threshold
     * @return
     */
    static vector<int>  mining_sorted_list_add_threshold(vector<vector<vector<int>>> freq_lists, int threshold) {
        vector<Transition*> trans;

//        queue<Node *> nodes;
        list<phrase_frequency> frequency_list;
        vector<int> result;
        int current_pos = 0;
        int size = freq_lists.size();
        int total_upper_bound = INT_MAX;

        // 横向：R1, R2, ... Rn, lower bound, upper bound   纵向：x1, x2, ..., xn
        map<int, vector<int>> term2frequency;
        int current_level = 0;
        for (int i = 0; 1; i++) { // 循环每一层
            /* 遍历每个list, 记录当前层 term的出现情况 */
            for (int j = 0; j < size; j++) {
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
                    term2frequency[term][i] = term2frequency[term][i] + freq;
                }
            }
            /* 更新LB & UB */
            for (auto iter = term2frequency.begin(); iter != term2frequency.end(); ++iter) {
                int lower_bound = 0, upper_bound = 0;
                for (int j = 0; j < size; j++) {
                    lower_bound += iter->second[j];
                    if (iter->second[i] == 0) {
                        upper_bound += freq_lists[j][i][1];
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
                result.emplace_back(term);
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
                    result.emplace_back(iter->first);
                }
            }
        }

        return result;
    }

    std::vector<std::vector<frequency>> mining_tree1(int threshold) {
        std::vector<frequency> v;
        std::vector<std::vector<frequency>> res;
        mining_node(&tree.root, threshold, v, MappedSubstring(0,0,-1),res);
        return res;
    }

    bool save(std::string path){
        if(access(path.data(),2)!=0){
            mkdir(path.data(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        }
        std::ofstream  ofs_haystack(path+"/haystack");
         ofs_haystack << last_index;
        for (auto it = haystack.begin();it!=haystack.end();it++){
            ofs_haystack  <<" "<< it->first << " " << it->second.size();
            for (auto its = it->second.begin();its != it->second.end();its++){
                ofs_haystack << " " << *its;
            }
        }
        tree.save(path);
        ofs_haystack.close();
        return true;
    }
    long load(std::string path){
        int key=0,length=0,temp=0;
        std::ifstream ifs_haystack;
        ifs_haystack.open(path+"/haystack");
        if (!ifs_haystack) {
            return 0;
        }
        if (!ifs_haystack.is_open())
            cout << "error 1" <<path<< endl;
        ifs_haystack.seekg(0, std::ios::end);
        length = ifs_haystack.tellg();
        ifs_haystack.seekg(0, std::ios::beg);
        char * token = new char[length];
        char * copy = token;
        ifs_haystack.read(token, length);
        ifs_haystack.close();
        token = strtok(token," ");
        last_index = atoi(token);
        token = strtok(NULL," ");
        try {
            for (int i = last_index; i ; i-- ){
                key = atoi(token);
                token = strtok(NULL," ");
                length = atoi(token);
                token = strtok(NULL," ");
                string value;
                while (length--){
                    temp = atoi(token);
                    token = strtok(NULL," ");
                    value.push_back(temp);
                }
                haystack[key] = value;
            }
        } catch(char *str)
        {
            std::cout << str << std::endl;
        }
        delete[] copy;
        return length + tree.load(path);
    }


};

#endif // _SUFFIX_TREE_HPP_INCLUDED_

