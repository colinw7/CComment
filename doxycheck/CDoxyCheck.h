#ifndef CDoxyCheck_H
#define CDoxyCheck_H

#include <CComment.h>

/*!
 * Check if classes in header files have doxygen comments
 */
class CDoxyCheck : public CCommentParser {
 public:
  CDoxyCheck();

  bool isQuiet() const { return quiet_; }
  void setQuiet(bool b) { quiet_ = b; }

  void initFile() override;
  void termFile() override;

  void start_comment() override;
  void end_comment  () override;

  void put_normal (char c) override;
  void put_comment(char c) override;

  void set_comment_type(CommentType type) override;

 private:
  void checkComment();

 private:
  struct TextLine {
    std::string str;
    int         lineNum { 0 };

    TextLine(const std::string &str, int lineNum) :
     str(str), lineNum(lineNum) {
    }
  };

  using Lines = std::vector<TextLine>;

  bool        quiet_       { false };
  CommentType commentType_ { CommentType::NONE };
  std::string commentStr_;
  Lines       textLines_;
  std::string textStr_;
  std::string fileName_;
  int         lineNum_     { 1 };
};

#endif
