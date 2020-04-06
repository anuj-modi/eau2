// #include <assert.h>

// #include "application/application.h"
// #include "catch.hpp"
// #include "dataframe/row.h"
// #include "dataframe/visitor.h"
// #include "util/string.h"

// class FileReader : public Writer {
//    public:
//     char* buf_;
//     size_t end_ = 0;
//     size_t i_ = 0;
//     FILE* file_;

//     /** Creates the reader and opens the file for reading.  */
//     FileReader(const char* file_name) : Writer() {
//         file_ = fopen(file_name, "r");
//         assert(file_ != nullptr);
//         buf_ = new char[BUFSIZE + 1];  //  null terminator
//         fillBuffer_();
//         skipWhitespace_();
//     }

//     ~FileReader() {
//         delete[] buf_;
//         fclose(file_);
//     }

//     /** Reads next word and stores it in the row. Actually read the word.
//      While reading the word, we may have to re-fill the buffer  */
//     void visit(Row& r) override {
//         assert(i_ < end_);
//         assert(!isspace(buf_[i_]));
//         size_t wStart = i_;
//         while (true) {
//             if (i_ == end_) {
//                 if (feof(file_)) {
//                     ++i_;
//                     break;
//                 }
//                 i_ = wStart;
//                 wStart = 0;
//                 fillBuffer_();
//             }
//             if (isspace(buf_[i_])) break;
//             ++i_;
//         }
//         buf_[i_] = 0;
//         String* word = new String(buf_ + wStart, i_ - wStart);
//         r.set(0, word);
//         ++i_;
//         skipWhitespace_();
//     }

//     /** Returns true when there are no more words to read.  There is nothing
//        more to read if we are at the end of the buffer and the file has
//        all been read.     */
//     bool done() override {
//         return (i_ >= end_) && feof(file_);
//     }

//     static const size_t BUFSIZE = 1024;

//     /** Reads more data from the file. */
//     void fillBuffer_() {
//         size_t start = 0;
//         // compact unprocessed stream
//         if (i_ != end_) {
//             start = end_ - i_;
//             memcpy(buf_, buf_ + i_, start);
//         }
//         // read more contents
//         end_ = start + fread(buf_ + start, sizeof(char), BUFSIZE - start, file_);
//         i_ = start;
//     }

//     /** Skips spaces.  Note that this may need to fill the buffer if the
//         last character of the buffer is space itself.  */
//     void skipWhitespace_() {
//         while (true) {
//             if (i_ == end_) {
//                 if (feof(file_)) return;
//                 fillBuffer_();
//             }
//             // if the current character is not whitespace, we are done
//             if (!isspace(buf_[i_])) return;
//             // otherwise skip it
//             ++i_;
//         }
//     }
// };

// class Adder : public Reader {
//    public:
//     std::unordered_map<std::string, int> map_;

//     Adder(std::unordered_map<std::string, int>& map) : Reader() {
//         map_ = map;
//     }

//     void visit(Row& r) override {
//         std::string w = std::string(r.get_string(0)->c_str());
//         if (map_.find(w) == map_.end()) {
//             map_[w] = 1;
//         } else {
//             map_[w] += 1;
//         }
//     }
// };

// class Merger : public Reader {
//    public:
//     std::unordered_map<std::string, int> map_;

//     Merger(std::unordered_map<std::string, int>& map) : Reader() {
//         map_ = map;
//     }

//     void visit(Row& r) override {
//         std::string w = std::string(r.get_string(0)->c_str());
//         if (map_.find(w) == map_.end()) {
//             map_[w] = r.get_int(1);
//         } else {
//             map_[w] += r.get_int(1);
//         }
//     }
// };

// class Summer : public Writer {
//    public:
//     std::unordered_map<std::string, int>::iterator it_;
//     std::unordered_map<std::string, int> map_;

//     Summer(std::unordered_map<std::string, int>& map) : Writer() {
//         map_ = map;
//         it_ = map_.begin();
//     }

//     virtual ~Summer() {}

//     /**
//      * Visits the given row.
//      * @arg r  the row
//      */
//     virtual void visit(Row& r) {
//         String* key = new String(it_->first.c_str());
//         r.set(0, key);
//         r.set(1, it_->second);
//         it_++;
//     }

//     /**
//      * Marks when the writer is done visiting the data frame.
//      * @return true if done
//      */
//     virtual bool done() {
//         return it_ == map_.end();
//     }
// };

// /****************************************************************************
//  * Calculate a word count for given file:
//  *   1) read the data (single node)
//  *   2) produce word counts per homed chunks, in parallel
//  *   3) combine the results
//  **********************************************************author: pmaj ****/
// class WordCount : public Application {
//    public:
//     static const size_t BUFSIZE = 1024;
//     Key in;
//     std::unordered_map<std::string, int> all;
//     FileReader fr_;

//     WordCount(size_t idx, KVStore* kv, const char* file_name)
//         : Application(idx, kv), in("data"), fr_(file_name) {}

//     /** The master nodes reads the input, then all of the nodes count. */
//     void run() override {
//         if (node_num_ == 0) {
//             delete DataFrame::fromVisitor(&in, &kd_, "S", fr_);
//         }
//         local_count();
//         reduce();
//     }

//     /** Returns a key for given node.  These keys are homed on master node
//      *  which then joins them one by one. */
//     Key* mk_key(size_t idx) {
//         StrBuff sb = StrBuff();
//         sb.c(node_num_);
//         Key* k = new Key(sb.get()->c_str());
//         return k;
//     }

//     /** Compute word counts on the local node and build a data frame. */
//     void local_count() {
//         DataFrame* words = (kd_.waitAndGet(in));
//         p("Node ").p(node_num_).pln(": starting local count...");
//         std::unordered_map<std::string, int> map = std::unordered_map<std::string, int>();
//         Adder add(map);
//         words->local_map(add);
//         delete words;
//         Summer cnt(map);
//         delete DataFrame::fromVisitor(mk_key(node_num_), &kd_, "SI", cnt);
//     }

//     /** Merge the data frames of all nodes */
//     void reduce() {
//         if (node_num_ != 0) return;
//         pln("Node 0: reducing counts...");
//         std::unordered_map<std::string, int> map = std::unordered_map<std::string, int>();
//         Key* own = mk_key(0);
//         merge(kd_.get(*own), map);
//         for (size_t i = 1; i < kv_->num_nodes(); ++i) {  // merge other nodes
//             Key* ok = mk_key(i);
//             merge(kd_.waitAndGet(*ok), map);
//             delete ok;
//         }
//         p("Different words: ").pln(map.size());
//         delete own;
//     }

//     void merge(DataFrame* df, std::unordered_map<std::string, int>& m) {
//         Adder merger(m);
//         df->map(merger);
//         delete df;
//     }
// };

// // check output of word count
// TEST_CASE("run word count app") {
//     // TODO word count test
// }
