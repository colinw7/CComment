#include <CConvertComment.h>
#include <vector>
#include <iostream>
#include <sys/types.h>

int
main(int argc, char **argv)
{
  std::vector<std::string> files;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      std::cerr << "Illegal option " << argv[i] << std::endl;
    }
    else
      files.push_back(argv[i]);
  }

  CConvertComment convert;

  convert.init();

  uint num_files = files.size();

  for (uint i = 0; i < num_files; i++)
    convert.processFile(files[i]);

  convert.term();

  return 0;
}
