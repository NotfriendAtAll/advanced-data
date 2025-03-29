#include <cstdint>
#include <string>
#include <utility>
enum class IteratorType {
  SkiplistIterator,
};

class BaseIterator {
public:
  using valuetype = std::pair<std::string, std::string>;
  using pvaluetype=valuetype*;
  using rvaluetype=valuetype&;
  virtual ~BaseIterator() = default;
  virtual bool valid() const = 0;
  virtual BaseIterator &operator++() = 0;
  virtual bool operator==(const BaseIterator &other) const = 0;
  virtual bool operator!=(const BaseIterator &other) const = 0;
  virtual IteratorType type() const = 0;
  virtual bool isEnd() = 0;
  virtual bool operator*() const = 0;
  virtual uint64_t getseq() const = 0;
};
