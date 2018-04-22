#include <CComment.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <sys/types.h>

int
main(int argc, char **argv)
{
  bool reverse = false;
  bool quiet   = false;
  bool spell   = false;

  std::vector<std::string> files;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if      (strcmp(&argv[i][1], "r") == 0 ||
               strcmp(&argv[i][1], "-reverse") == 0)
        reverse = true;
      else if (strcmp(&argv[i][1], "q") == 0 ||
               strcmp(&argv[i][1], "-quiet") == 0)
        quiet = true;
      else if (strcmp(&argv[i][1], "S") == 0 ||
               strcmp(&argv[i][1], "-spell") == 0)
        spell = true;
      else
        std::cerr << "Illegal option " << argv[i] << std::endl;
    }
    else
      files.push_back(argv[i]);
  }

  CComment comment;

  comment.setReverse(reverse);
  comment.setQuiet  (quiet);
  comment.setSpell  (spell);

  comment.init();

  uint num_files = files.size();

  for (uint i = 0; i < num_files; i++)
    comment.processFile(files[i]);

  comment.term();

  return 0;
}
