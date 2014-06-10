#include "thirdparty/re2/util/benchmark.h"
#include "thirdparty/re2/re2/re2.h"

using namespace re2;

static int match(const char* name, int argc, const char** argv) {
  if(argc == 1)
    return 1;
  for(int i = 1; i < argc; i++)
    if(RE2::PartialMatch(name, argv[i]))
      return 1;
  return 0;
}

int main(int argc, const char** argv) {
  for(int i = 0; i < nbenchmarks; i++) {
    Benchmark* b = benchmarks[i];
    if(match(b->name, argc, argv))
      for(int j = b->threadlo; j <= b->threadhi; j++)
        for(int k = max(b->lo, 1); k <= max(b->hi, 1); k<<=1)
          RunBench(b, j, k);
  }
}
