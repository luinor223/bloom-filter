#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>

using namespace std;

const long long MOD = 10e9 + 7;

const int DatabaseSize = 1000; // number of elements to be inserted
const double P = 0.05;   // Probability of False positivity 
const int BitArraySize = -(DatabaseSize * log(P)) / (log(2) * log(2));


struct BloomFilter
{
    bitset<BitArraySize> filter;
    const int FuncNum = (BitArraySize * 1.0 / DatabaseSize) * log(2);
    int* FuncCoef;
};

long long hashString(string s, int BASE) // Polynomial rolling hash
{
    long long hash = 0;
    for (int i = 0; i < s.size(); i++)
        hash = (hash * BASE + (s[i] + 1)) % MOD;

    return hash;
}
bool isPrime(int num)
{
    if (num < 2)
        return false;

    for (int i = 2; i * i <= num; ++i)
    {
        if (num % i == 0)
            return false;
    }
    return true;
}

void GenerateFuncCoefficient(BloomFilter &bf, int start)
{ // get prime numbers, good for polynomial rolling hash
    bf.FuncCoef = new int[bf.FuncNum];
    int num = start;
    int count = 0;
    while (count < bf.FuncNum)
    {
        if (isPrime(num))
            bf.FuncCoef[count++] = num;
        num++;
    }
}

// Time Complexity: O(d*FuncNum) where d is the length of string s
void add(string s, BloomFilter &bf)
{
    for (int i = 0; i < bf.FuncNum; i++)
    {
        int index = hashString(s, bf.FuncCoef[i]) % BitArraySize;
        bf.filter[index] = 1;
        // cout << "flipped the "<< index << "th bit" << endl;
    }
}
// Time Complexity: O(d*FuncNum) where d is the length of string s
bool lookUp(string s, BloomFilter bf)
{
    for (int i = 0; i < bf.FuncNum; i++)
    {
        int index = hashString(s, bf.FuncCoef[i]) % BitArraySize;
        if (bf.filter[index] == 0)
            return false;
    }
    return true;
}