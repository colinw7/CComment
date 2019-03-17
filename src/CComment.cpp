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
  if (isSpell())
    CSpellInit();
}

void
CComment::
term()
{
}

void
CComment::
initFile()
{
}

void
CComment::
termFile()
{
  if (! isQuiet())
    output_stats();
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
              std::cerr << "Mispelled: " << word_ << std::endl;

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
