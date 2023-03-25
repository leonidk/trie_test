# trie_test
Some experiments with tries

### details
Some C++ tries encoding a dictionary. Trying to see if quasi-random (aka low-discrepancy) sequences outperform normal random numbers. In my brief testing, it seems that reasonably high load factor situations can see benefits. 

Inspired by https://arxiv.org/abs/2209.06038 William Kuszmaul. A Hash Table Without Hash Functions, and How to Get the Most out of Your Random Bits. FOCS, 2022. What the code calls a "FlatTrie" is what the paper calls a "rotated radix trie". There's a #define for using a normal vs flat trie and a 0/1 define for using quasi or normal random numbers.

### minor results
For example, with the default settings of a flat trie that is 2.8x times larger than the dictionary size, we obtained the following runtimes in milliseconds for adding/removing/finding all words in the included dictionary (370,000 words). 

|              | Add  | Find | Remove |
|--------------|------|------|--------|
| Random       | 1258 | 1197 | 1254   |
| Quasi-Random | 905  | 883  | 890    |
