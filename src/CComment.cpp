#include <CComment.h>
#include <CSpell.h>
#include <iostream>
#include <cstdio>

CComment::
CComment()
{
}

void
CComment::
init()
{
}

void
CComment::
term()
{
#if 0
  for (const auto &ls : fileMispelled_) {
    std::cerr << ls.first << "\n";

    for (const auto &cs : ls.second) {
      for (const auto &s : cs.second)
        std::cerr << " " << s;
    }

    std::cerr << "\n";
  }
#endif

  for (const auto &sf : mispelled_) {
    std::cerr << sf.first;

    std::cerr << " (";

    bool first = true;

    for (const auto &fd : sf.second) {
      if (! first)
        std::cerr << ", ";

      std::cerr << fd.file;

      first = false;
    }

    std::cerr << ")\n";
  }
}

void
CComment::
initFile()
{
  if (isSpell())
    CSpellInit();
}

void
CComment::
termFile()
{
  if (! isQuiet())
    output_stats();

  if (isSpell())
    CSpellTerm();
}

void
CComment::
output_stats()
{
  double percent = 0.0;

  if (output_count_ + skip_count_ > 0)
    percent = (100.0 * output_count_)/(output_count_ + skip_count_);

  char percent_str[32];

  sprintf(percent_str, "%-2.2f", percent);

  std::cout << file_ << " " << percent_str << " Percent" << std::endl;
}

void
CComment::
put_normal(char c)
{
  if (c == '\n' || ! isReverse()) {
    if (! isQuiet())
      fputc(c, stdout);

    if (c != '\n')
      ++output_count_;
  }
  else
    ++skip_count_;
}

void
CComment::
set_comment_type(CommentType type)
{
  commentType_ = type;
}

void
CComment::
put_comment(char c)
{
  if (c == '\n' || isReverse()) {
    if (isDoxygen()) {
      if (commentType_ == CommentType::C_NORMAL || commentType_ == CommentType::CPP_NORMAL)
        return;
    }

    if (! isQuiet())
      fputc(c, stdout);

    if (c != '\n') {
      if (isSpell()) {
        if (is_word_char(c)) {
          word_     += c;
          in_word_   = true;
        }
        else {
          if (in_word_) {
            if (CSpellCheckWord(word_) == 0)
              mispelled(word_);

            word_    = "";
            in_word_ = false;
          }
        }
      }

      ++output_count_;
    }
  }
  else
    ++skip_count_;
}

bool
CComment::
is_word_char(char c)
{
  return (isalpha(c) || c == '\'');
}

void
CComment::
mispelled(const std::string &word)
{
  fileMispelled_[file_][lineNum_].push_back(word);

  mispelled_[word].push_back(FileData(file_, lineNum_, charNum_));

  //std::cerr << "Mispelled: '" << word << "' " << file_ << "@" << lineNum_ << "\n";
}
