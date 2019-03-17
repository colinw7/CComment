#include <CDoxyCheck.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <sys/types.h>

int
main(int argc, char **argv)
{
  bool quiet = false;

  std::vector<std::string> files;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == 'q')
        quiet = true;
      else
        std::cerr << "Illegal option " << argv[i] << "\n";
    }
    else
      files.push_back(argv[i]);
  }

  CDoxyCheck check;

  check.setQuiet(quiet);

  uint num_files = files.size();

  for (uint i = 0; i < num_files; i++)
    check.processFile(files[i]);

  return 0;
}

//------

CDoxyCheck::
CDoxyCheck()
{
}

void
CDoxyCheck::
start_comment()
{
  checkComment();

  commentStr_ = "";
}

void
CDoxyCheck::
end_comment()
{
  textStr_ = "";

  textLines_.clear();
}

void
CDoxyCheck::
put_normal(char c)
{
  if (c == '\n') {
    textLines_.push_back(TextLine(textStr_, lineNum_));

    ++lineNum_;

    textStr_ = "";
  }
  else {
    textStr_ += c;
  }
}

void
CDoxyCheck::
put_comment(char c)
{
  if (c == '\n')
    ++lineNum_;

  commentStr_ += c;
}

void
CDoxyCheck::
set_comment_type(CommentType type)
{
  commentType_ = type;
}

void
CDoxyCheck::
initFile()
{
  fileName_ = filename();
  lineNum_  = 0;
}

void
CDoxyCheck::
termFile()
{
  if (textStr_ != "") {
    textLines_.push_back(TextLine(textStr_, lineNum_));

    textStr_ = "";
  }

  checkComment();
}

void
CDoxyCheck::
checkComment()
{
  int lineNum  = 0;
  int numLines = textLines_.size();

  while (lineNum < numLines) {
    std::string textStr = textLines_[lineNum].str;

    int i = 0;

    auto skipSpace = [&]() {
      while (textStr[i] != '\0' && isspace(textStr[i]))
        ++i;
    };

    skipSpace();

    std::string word;

    while (textStr[i] != '\0' && isalpha(textStr[i]))
      word += textStr[i++];

    if (word == "class") {
      skipSpace();

      std::string name;

      while (textStr[i] != '\0' && ! isspace(textStr[i]) &&
             textStr[i] != '{' && textStr[i] != ';')
        name += textStr[i++];

      skipSpace();

      if (textStr[i] != ';') {
        int textLineNum = textLines_[lineNum].lineNum;

        bool        fail = false;
        std::string errorMsg;

        if      (commentStr_ == "") {
          fail     = true;
          errorMsg = "no comment";
        }
        else if (commentType_ != CommentType::QT) {
          fail     = true;
          errorMsg = "no doxygen comment";
        }
        else {
          fail     = false;
          errorMsg = "doxygen comment";
        }

        if (! isQuiet()) {
          std::cout << fileName_ << ":" << textLineNum << " ";

          std::cout << "Class " << name;

          std::cout << " : " << (fail ? "FAIL" : "PASS") << " : " << errorMsg;

          std::cout << "\n";
        }
        else {
          if (fail) {
            std::cout << fileName_ << ":" << textLineNum << " ";

            std::cout << "Class " << name;

            std::cout << " : " << errorMsg;

            std::cout << "\n";
          }
        }
      }
    }

    ++lineNum;
  }
}
