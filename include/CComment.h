#ifndef CComment_H
#define CComment_H

#include <CCommentParser.h>

class CComment : public CCommentParser {
 public:
  CComment();

  bool isReverse() { return reverse_; }
  void setReverse(bool reverse) { reverse_ = reverse; }

  bool isQuiet() { return quiet_; }
  void setQuiet(bool quiet) { quiet_ = quiet; }

  bool isSpell() { return spell_; }
  void setSpell(bool spell) { spell_ = spell; }

  bool isDoxygen() const { return doxygen_; }
  void setDoxygen(bool b) { doxygen_ = b; }

  void init();
  void term();

  void initFile();
  void termFile();

  void output_stats();

  void put_normal (char c) override;
  void put_comment(char c) override;

  void set_comment_type(CommentType type) override;

  bool is_word_char(char c);

 private:
  bool        reverse_      { false };
  bool        quiet_        { false };
  bool        spell_        { false };
  bool        doxygen_      { false };
  int         output_count_ { 0 };
  int         skip_count_   { 0 };
  std::string word_;
  bool        in_word_      { false };
  CommentType commentType_  { CommentType::NONE };
};

#endif
