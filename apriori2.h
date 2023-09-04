//
// Created by 王璐 on 2020/11/18.
//
#ifndef Apriori2_H
#define Apriori2_H
#include<vector>
#include<string>
#include<map>
#include<set>
#include<fstream>
#include<iostream>
#include<iterator>

using namespace std;
class Apriori2
{
public:
    Apriori2();
    Apriori2(int minSupport);
    ~Apriori2();

    void ReadData(const string &fileName);//读入数据
    void Apriori2Freq();//生成频繁项集

    void showData();
    void printFreq();//输出频繁项集
private:

    int minSupport;//最小支持度
    vector<set<string> > dataSet;  //原始数据集
    vector<set<string> > freqSet;//频繁项集
    map<set<string>, int> supportLabel;//支持度数

    set<string> Split(const string &str);//切分读入的行
    bool isSubset(set<string> &a, set<string> &b);//判断a是b的子集

    vector<set<string> > Apriori2GenC1();//产生一项候选集
    vector<set<string> > Apriori2GenCk(map<set<string>, int> &frqList);//产生多项候选集
    map<set<string>, int> findFreq(vector<set<string> >&s);//筛选候选集产生频繁项集
    bool CanGen(const set<string>& a, const set<string>& b);



};

#endif
Apriori2::Apriori2()
{
    this->minSupport = 2;
}

Apriori2::Apriori2(int minSupport)
{
    this->minSupport = minSupport;
}

Apriori2::~Apriori2()
{

}

void Apriori2::ReadData(const string &fileName)
{
    ifstream inf(fileName);
    string str;
    while (getline(inf, str))
    {
        set<string> line = Split(str);

        dataSet.push_back(line);
    }
}

set<string> Apriori2::Split(const string &str)
{
    set<string> v;
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
            v.insert(string(str,pos,i-pos));
            isStringHead = false;
        }
        if (i == str.size()-1)
        {
            v.insert(string(str,pos,v.size()-pos));
        }
    }

    return v;
}

void Apriori2::showData()
{
    for (vector<set<string> >::size_type i = 0; i != dataSet.size(); ++i)
    {
        for (set<string>::iterator iter = dataSet[i].begin(); iter != dataSet[i].end(); ++iter)
        {
            cout << *iter << " ";
        }
        cout << endl;
    }
}


vector<set<string> > Apriori2::Apriori2GenC1()
{

    vector<set<string> > C1;
    set<string> temp;
    set<string> allString;
    for (vector<set<string> >::size_type i = 0; i != dataSet.size()-1; ++i)
    {

        for (set<string>::iterator iter = dataSet[i].begin(); iter != dataSet[i].end(); ++iter)
        {

            if (allString.find(*iter) == allString.end())
            {
                temp.insert(*iter);
                C1.push_back(temp);
                temp.clear();
            }
            allString.insert(*iter);

        }
    }

    return C1;
}

//生成频繁项集
map<set<string>, int> Apriori2::findFreq(vector<set<string> >&s)
{

    map<set<string>, int> support;
    map<set<string>, int> retSupport;
    for (vector<set<string> >::size_type i = 0; i != s.size(); ++i)
    {

        for (vector<set<string> >::size_type j = 0; j != dataSet.size(); ++j)
        {
            if (isSubset(s[i], dataSet[j]))
            {
                ++support[s[i]];
            }
        }
    }
    for (map<set<string>, int>::iterator mIter = support.begin(); mIter != support.end(); ++mIter)
    {

        if (mIter->second > minSupport)
        {
            retSupport.insert(*mIter);
        }
    }
    return retSupport;
}
//判断a是否为b的子集
bool Apriori2::isSubset(set<string> &a, set<string> &b)
{
    for (set<string>::iterator aIter = a.begin(); aIter != a.end(); ++aIter)
    {

        if (b.find(*aIter) == b.end())
        {
            return false;
        }
    }
    return true;
}




vector<set<string> > Apriori2::Apriori2GenCk(map<set<string> ,int> &freqList)
{
    vector<set<string> >  retv;
    set<string> temp;
    for (map<set<string>, int>::iterator mIterA = freqList.begin(); mIterA != freqList.end(); ++mIterA)
    {
        for (map<set<string>, int>::iterator mIterB = mIterA; mIterB != freqList.end(); ++mIterB)
        {
            if (CanGen(mIterA->first, mIterB->first))//判断是否能连接生成候选项
            {

                temp.insert(mIterA->first.begin(),mIterA->first.end());
                temp.insert(mIterB->first.begin(), mIterB->first.end());
                retv.push_back(temp);
                temp.clear();
            }
        }
    }

    return retv;
}

bool Apriori2::CanGen(const set<string> &a, const set<string> &b)
{
    int notSameCount = 0;
    for (set<string>::iterator sIterA = a.begin(); sIterA != a.end(); ++sIterA)
    {
        if (b.find(*sIterA) == b.end())
        {
            notSameCount++;
        }
    }
    if (notSameCount == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Apriori2::Apriori2Freq()
{

    vector<set<string> > C1 = Apriori2GenC1();

    map<set<string>, int> supportDk = findFreq(C1);

    vector<set<string> > ck;
    supportLabel.insert(supportDk.begin(), supportDk.end());

    while (supportDk.size() != 0)
    {


        ck = Apriori2GenCk(supportDk);

        supportDk.clear();
        supportDk = findFreq(ck);

        supportLabel.insert(supportDk.begin(), supportDk.end());

        ck.clear();
    }
}

void Apriori2::printFreq()
{
    cout << supportLabel.size()<<"  supportLabel"<<endl;
    for (map<set<string>, int>::iterator mIter = supportLabel.begin(); mIter != supportLabel.end(); ++mIter)
    {
        cout << mIter->second<<" ";
        for (set<string>::iterator siter = mIter->first.begin(); siter != mIter->first.end(); ++siter)
        {
            cout << *siter<<"\\";
        }
        cout << endl;

    }

}