#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>

using namespace std;

const long long MOD = 10e9 + 7;

const int DatabaseSize = 5000; // number of elements to be inserted
const double P = 0.05;   // Probability of False positivity 
const int BitArraySize = -(DatabaseSize * log(P)) / (log(2) * log(2));


struct BloomFilter
{
    bitset<BitArraySize> filter;
    const int NumberOfHashFunc = (BitArraySize * 1.0 / DatabaseSize) * log(2);
    int* FuncCoef; //The base values for hash calculation
};

//Calculates the hash value of a given string using the polynomial rolling hash algorithm.
long long hashString(string s, int BASE)
{
    long long hash = 0;
    for (int i = 0; i < s.size(); i++)
        hash = (hash * BASE + (s[i] - ' ' + 1)) % MOD;

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

//Generate BASE numbers for Polynomial Rolling Hash
void GenerateFuncCoefficient(BloomFilter &bf, int start)
{ // get prime numbers, good for polynomial rolling hash
    bf.FuncCoef = new int[bf.NumberOfHashFunc];
    int num = start;
    int count = 0;
    while (count < bf.NumberOfHashFunc)
    {
        if (isPrime(num))
            bf.FuncCoef[count++] = num;
        num++;
    }
}

// Time Complexity: O(d*NumberOfHashFunc) where d is the length of string s
//Add a string to the bloom filter
void addToBF(string s, BloomFilter &bf)
{
    for (int i = 0; i < bf.NumberOfHashFunc; i++)
    {
        int index = hashString(s, bf.FuncCoef[i]) % BitArraySize;
        bf.filter[index] = 1;   // Turn on the bit at index
    }
}
// Time Complexity: O(d*NumberOfHashFunc) where d is the length of string s
// Check if a string is in the bloom filter
bool isInBF(string s, BloomFilter bf)
{
    for (int i = 0; i < bf.NumberOfHashFunc; i++)
    {
        int index = hashString(s, bf.FuncCoef[i]) % BitArraySize;
        if (bf.filter[index] == 0)
            return false;
    }
    return true;
}