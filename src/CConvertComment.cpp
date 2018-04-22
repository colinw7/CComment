#include <CConvertComment.h>
#include <iostream>
#include <sys/types.h>

CConvertComment::
CConvertComment()
{
}

void
CConvertComment::
init()
{
}

void
CConvertComment::
term()
{
}

void
CConvertComment::
initFile()
{
}

void
CConvertComment::
termFile()
{
  flushComment();
}

void
CConvertComment::
put_normal(char c)
{
  if (in_comment_)
    flushComment();

  std::cout << c;
}

void
CConvertComment::
put_comment(char c)
{
  in_comment_ = true;

  comment_ += c;
}

void
CConvertComment::
flushComment()
{
  uint len = comment_.size();

  if (len > 1 && comment_[0] == '/' && comment_[1] == '*')
    comment_[1] = '/';

  if (len > 1 && comment_[len - 1] == '/' && comment_[len - 2] == '*') {
    len -= 2;

    while (len > 0 && isspace(comment_[len - 1]))
      --len;

    comment_ = comment_.substr(0, len);
  }

  for (uint i = 0; i < len; ++i) {
    if (comment_[i] == '\n') {
      std::cout << std::endl;
      std::cout << "//";

      if (i + 1 < len && isspace(comment_[i + 1])) ++i;
      if (i + 1 < len && isspace(comment_[i + 1])) ++i;
    }
    else
      std::cout << comment_[i];
  }

  in_comment_ = false;
  comment_    = "";
}
