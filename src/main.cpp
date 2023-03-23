#include <iostream>

#include <algorithm> // shuffle
#include <array> // array
#include <memory> // unique_ptr & shared_ptr
#include <fstream> // input
#include <vector> // word buffer
#include <chrono> // timing info
#include <random>
#include <tuple>

#include <unordered_map>
#include <map>

#include <cmath>

#define USE_FLAT_TRIE
#define QUASI (1)

int reduce(int word, int p) {
    return word < p ? word : word % p;
}

auto rng = std::default_random_engine{};
std::uniform_real_distribution<> dis(0, 1);

const double INV_GOLDEN_RATIO = 0.6180339887498948482;
double random_num = 0;

#define ARRAY_SIZE (26)
class Trie
{
public:
    using TriePtr = std::unique_ptr<Trie>;
    std::array<TriePtr, ARRAY_SIZE> flat;
    bool is_leaf = false;
    int offset = 0;

    Trie() {
        random_num += INV_GOLDEN_RATIO;
#if QUASI
        offset = static_cast<int>(std::round((random_num - static_cast<long>(random_num)) * (ARRAY_SIZE - 1)));
#else
        offset = static_cast<int>(std::round((dis(rng)) * (ARRAY_SIZE - 1)));
#endif
    }

    void add(const std::string&);

    void remove(const std::string&);
    void prune();

    bool find(const std::string&);
    bool find_prefix(const std::string&);
private:
    Trie* find_node(const std::string&);
    bool has_children();

};


void Trie::add(const std::string& key)
{
    auto curr = this;
    for (const auto& c : key) {
        auto ck = reduce(c + curr->offset, ARRAY_SIZE);
        if (curr->flat[ck] == nullptr) {
            curr->flat[ck] = std::make_unique<Trie>();
        }
        curr = curr->flat[ck].get();
    }
    curr->is_leaf = true;
}

Trie* Trie::find_node(const std::string& key)
{
    auto curr = this;
    for (const auto& c : key) {
        auto ck = reduce(c + curr->offset, ARRAY_SIZE);
        if (curr->flat[ck] == nullptr)
            return nullptr;
        curr = curr->flat[ck].get();
    }
    return curr;
}

bool Trie::find(const std::string& key)
{
    auto curr = find_node(key);
    return curr != nullptr && curr->is_leaf;
}

bool Trie::find_prefix(const std::string& key)
{
    auto curr = find_node(key);
    return curr != nullptr && curr->has_children();
}

bool Trie::has_children()
{
    return std::find_if(std::begin(flat), std::end(flat),
        [](const TriePtr& a) {return a != nullptr;}
    ) != std::end(flat);
}

void Trie::prune()
{
    for (auto& c : flat) {
        if (c == nullptr)
            continue;
        c->prune();
        if ((!c->is_leaf) && (!c->has_children())) {
            c.reset();
        }
    }
}

void Trie::remove(const std::string& key)
{
    auto curr = find_node(key);
    if (curr != nullptr)
        curr->is_leaf = false;
}

class Node {
public:
    bool is_leaf = false;
    double offset = 0;

    void set_offset() {
#if QUASI
        random_num += INV_GOLDEN_RATIO;
        offset = random_num - static_cast<long>(random_num);
#else
        offset = dis(rng);
#endif
    }
    int ioffset(int max_size) {
        return static_cast<int>(std::round(offset * max_size));
    }
};

class FlatTrie
{
public:
    std::vector<std::map<std::string, Node>> flat;
    Node root;

    FlatTrie(int size)
        : flat(size)
    {

    }
    void add(const std::string&);

    void remove(const std::string&);
    void prune();

    bool find(const std::string&);
    bool find_prefix(const std::string&);
private:
    int find_node(const std::string&);
    bool has_children(int curr_i, const std::string&);

};

void FlatTrie::add(const std::string& key)
{
    auto curr = root;
    auto asize = flat.size();
    auto ck = 0;
    for (size_t i = 0; i < key.size(); i++) {
        auto c = key[i];
        ck = reduce(c + curr.ioffset(asize), asize);
        auto subkey = key.substr(0, i + 1);
        if (flat[ck].find(subkey) == flat[ck].end()) {
            auto new_node = Node();
            new_node.set_offset();
            flat[ck][subkey] = new_node;
        }
        curr = flat[ck][subkey];
    }
    flat[ck][key].is_leaf = true;
}

int FlatTrie::find_node(const std::string& key)
{

    auto curr = root;
    auto asize = flat.size();
    auto ck = 0;
    for (size_t i = 0; i < key.size(); i++) {
        auto c = key[i];
        ck = reduce(c + curr.ioffset(asize), asize);
        auto subkey = key.substr(0, i + 1);

        if (flat[ck].find(subkey) == flat[ck].end()) {
            return -1;
        }
        curr = flat[ck][subkey];
    }
    return ck;
}

bool FlatTrie::find(const std::string& key)
{
    auto curr = find_node(key);
    return curr >= 0 && flat[curr][key].is_leaf;
}

bool FlatTrie::find_prefix(const std::string& key)
{
    auto curr = find_node(key);
    return curr >= 0 && has_children(curr, key);
}

bool FlatTrie::has_children(int curr_i, const std::string& key)
{
    auto curr = flat[curr_i][key];
    const auto asize = flat.size();
    for (int c = 0; c < ARRAY_SIZE; c++) {
        auto ck = reduce(c + curr.ioffset(asize), asize);
        auto newkey = key + static_cast<char>(c);
        if (flat[ck].find(newkey) != flat[ck].end()) {
            return true;
        }
    }
    return false;
}

void FlatTrie::prune()
{

}


void FlatTrie::remove(const std::string& key)
{
    auto curr = root;
    auto asize = flat.size();
    auto ck = 0;
    std::vector<std::pair<int,std::string>> purge_list;
    for (size_t i = 0; i < key.size(); i++) {
        auto c = key[i];
        ck = reduce(c + curr.ioffset(asize), asize);
        auto subkey = key.substr(0, i + 1);

        if (flat[ck].find(subkey) == flat[ck].end()) {
            break;
        }
        curr = flat[ck][subkey];
        purge_list.push_back({ck,subkey});
    }
    for(int i =purge_list.size()-1; i >= 0; i--) {
        auto empty = true;
        auto start = purge_list[i];
        auto curr = flat[start.first][start.second];
        for(int c =0; c < ARRAY_SIZE; c++) {
            auto newkey = start.second + static_cast<char>(c);

            ck = reduce(c + curr.ioffset(asize), asize);
            auto goal_node = flat[ck].find(newkey);
            if (goal_node != flat[ck].end() && goal_node->second.is_leaf) {
                empty = false;
            }

        }
        if(empty) {
            flat[start.first].erase(start.second);
        }
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> words;//= {"cat","dog"};

    std::ifstream infile("words_alpha.txt");
    std::string line;
    while (std::getline(infile, line)) {
        words.push_back(line.substr(0, line.size() - 1));
    }

    for (auto& w : words) {
        for (auto& c : w) {
            c = std::min('z', std::max('a', c));
            c -= 'a';
        }
    }
#ifdef USE_FLAT_TRIE
    FlatTrie head(words.size() * 0.5);
    std::cout << "using a flat trie with " << ((QUASI) ? "quasi-random" : "random") << std::endl;
#else
    Trie head;
    std::cout << "using a normal trie with " << ((QUASI) ? "quasi-random" : "random") << std::endl;
#endif
    auto tot_t0 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1; i++) {
        //head = {};
        //std::shuffle(std::begin(words), std::end(words), rng);

        auto t0 = std::chrono::high_resolution_clock::now();
        for (auto& w : words) {
            head.add(w);
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        head.prune();
        auto t2 = std::chrono::high_resolution_clock::now();
        std::vector<bool> results(words.size());

        for (auto& w : words) {
            results.push_back(head.find(w));
        }
        auto t3 = std::chrono::high_resolution_clock::now();

        if (i == 0) {
            std::cout << "time to add: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << std::endl;
            std::cout << "time to prune: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
            std::cout << "time to find: " << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count() << std::endl;
        }
    }
    auto tot_t1 = std::chrono::high_resolution_clock::now();
    std::cout << "TOTAL " << std::chrono::duration_cast<std::chrono::milliseconds>(tot_t1 - tot_t0).count() << std::endl;

    for (int i = 1; i < argc; ++i) {
        auto word = std::string(argv[i]);
        auto word_p = word;
        for (auto& c : word) {
            c -= 'a';
        }
        std::cout << word_p << '\t' << head.find(word) << '\t' << head.find_prefix(word) << std::endl;
    }

#if 0
    for (const auto& node : head.flat)
    {
        for (const auto& myPair : node) {
            std::string outw = myPair.first;
            for (auto& c : outw) {
                c += 'a';
            }
            std::cout << outw << "\t" << myPair.second.is_leaf << '\n';
        }
    }
#endif
    std::cin.get();


    auto t4 = std::chrono::high_resolution_clock::now();
    for (auto& w : words) {
        head.remove(w);
    }
    auto t5 = std::chrono::high_resolution_clock::now();
    head.prune();
    auto t6 = std::chrono::high_resolution_clock::now();
    head.prune();
    auto t7 = std::chrono::high_resolution_clock::now();

    std::cout << "time to remove: " << std::chrono::duration_cast<std::chrono::milliseconds>(t5 - t4).count() << std::endl;
    std::cout << "time to prune2: " << std::chrono::duration_cast<std::chrono::milliseconds>(t6 - t5).count() << std::endl;
    std::cout << "time to prune3: " << std::chrono::duration_cast<std::chrono::milliseconds>(t7 - t6).count() << std::endl;

    for (int i = 1; i < argc; ++i) {
        auto word = std::string(argv[i]);
        auto word_p = word;
        for (auto& c : word) {
            c -= 'a';
        }
        std::cout << word_p << '\t' << head.find(word) << '\t' << head.find_prefix(word) << std::endl;
    }

    std::cin.get();
    return 0;
}