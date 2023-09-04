//
// Created by 吴小宁 on 2021/8/24.
//


#include "storage.h"
#include "util.h"
#include <vector>
#include <stack>
#include <map>
//#include <tr1/unordered_set>
#include <unordered_set>

typedef unsigned char byte;

const unsigned BRANCH_FACTOR = 32;
const double REINSERT_FACTOR = 0.3;
const double SPLIT_FACTOR = 0.4;
const unsigned NEAR_MINIMUM_OVERLAP_FACTOR = 32;

class Node: public Buffer
{
public:
    // the node's level in the tree, level 0 is the leaf level
    unsigned level;
    // number of entries in the node
    unsigned numChildren;

    Rectangle mbr;

    // node's entries
    Object objects[BRANCH_FACTOR];

    // check whether the node is a leaf node or not
    bool isLeaf() const
    {
        return level == 0;
    };

    int suffixId;
};

class RTree
{
protected:
    Storage *storage;
    size_t nodeSize;
    unsigned branchFactor;
    double fillFactor;

    // the implementation of the r*-tree insert function
    void insertData(const Object &obj, unsigned desiredLevel,
                    byte *overflowArray);

    // the implementation of the r*-tree delete function
    bool deleteData(const Object &obj, stack<unsigned> &path, unsigned id);

    // r*-tree reinsert function
    void reinsert(Node *node, stack<Node *> path, unsigned desiredLevel,
                  unsigned position, byte *overflowArray);

    // r*-tree split function
    void split(Node *node, Object &a, Object &b);

    // delete an object from a node
    void deleteObject(Node *node, unsigned id);

    // condense the tree after a deletetion
    void condenseTree(Node* node, stack<unsigned> &path, stack<unsigned> &needReinsertion);

    // adjust the MBR for a node after r*-tree split
    void adjustNode(Node *node, Object &a, vector <EntryValue> &entries, unsigned startIndex, unsigned endIndex);

    // adjust the MBR for a node after r*-tree split
    void adjustNode(Node *node);

    // the implementation of range query function for both r-tree and r*-tree
    void rangeQuery(vector<Object> &objects, const Rectangle &range,
                    uintptr_t id);

    void rangeQuery(vector<vector<Object>> &objects, const Rectangle &range,
                    uintptr_t id);

    void rangeQueryInnerNodes(vector<Object> &objects, const Rectangle &range,
                              uintptr_t id);

    void rangeQueryInnerNodes(vector<vector<Object>> &objects, const Rectangle &range,
                              uintptr_t id);

    bool retrieve(unordered_set<unsigned> &ids, unsigned oid, unsigned id);
public:
    unsigned nodeNum;

    RTree(Storage *storage);

    // create r*-tree
    void create(unsigned bf, double ff);

    // r*-tree insert function
    void insertData(const Object &obj);

    // r*-tree delete function
    void deleteData(const Object &obj);

    // range query function for both r-tree and r*-tree
    void rangeQuery(vector<Object> &objects, const Rectangle &range);

    void rangeQuery(vector<vector<Object>> &objects, const Rectangle &range);

    void rangeQueryInnerNodes(vector<Object> &objects, const Rectangle &range);

    void rangeQueryInnerNodes(vector<vector<Object>> &objects, const Rectangle &range);

    // 在R 树上查询得到所有包含对应时空对象的节点，并更新节点对应的suffix tree
    void insertSpatialObjectInnerNode(SpatialObject &object, uintptr_t id);

    void insertSpatialObjectInnerNode(SpatialObject &object);

    void insertSpatialObjectLeafNode(SpatialObject &object);

    void insertSpatialObjectLeafNode(SpatialObject &object, uintptr_t id);

    // top k nearest neighbour function for both r-tree and r*-tree
    void kNNQuery(multimap<double, Object> &objects,
                  const Point &point, unsigned k);

    void retrieve(unordered_set<unsigned> &ids, unsigned oid);

    int get_height();
};


