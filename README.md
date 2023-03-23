# trie_test
Some experiments with tries

### details
Some C++ tries encoding a dictionary. Trying to see if quasi-random (aka low-discrepancy) sequences outperform normal random numbers. In my brief testing, it seems that reasonably high load factor situations can see benefits. 

Inspired by https://arxiv.org/abs/2209.06038 William Kuszmaul. A Hash Table Without Hash Functions, and How to Get the Most out of Your Random Bits. FOCS, 2022. What the code calls a "FlatTrie" is what the paper calls a "rotated radix trie". There's a #define for using a normal vs flat trie and a 0/1 define for using quasi or normal random numbers.
