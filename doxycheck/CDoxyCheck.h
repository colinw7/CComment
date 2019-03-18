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

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  void initFile() override;
  void termFile() override;

  void start_comment() override;
  void end_comment  () override;

  void put_normal (char c) override;
  void put_comment(char c) override;

  void set_comment_type(CommentType type) override;

  void parseText(const std::string &str, int lineNum);

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

  struct ParseData {
    std::string token;

    void reset() {
      token = "";
    }
  };

  enum class TokenType {
    NONE,
    STRING,
    NUMBER,
    IDENTIFIER,
    OPERATOR,
    SEPARATOR,
    COMMENT
  };

  struct Token {
    TokenType   type        { TokenType::NONE };
    std::string str;
    int         lineNum     { 0 };
    CommentType commentType { CommentType::NONE };

    Token(TokenType type=TokenType::NONE, const std::string &str="", int lineNum=-1) :
     type(type), str(str), lineNum(lineNum) {
    }
  };

  using Lines  = std::vector<TextLine>;
  using Tokens = std::vector<Token>;

  bool        quiet_       { false };
  bool        debug_       { false };
  CommentType commentType_ { CommentType::NONE };
  std::string commentStr_;
  Lines       textLines_;
  std::string textStr_;
  char        lastChar_    { '\0' };
  std::string fileName_;
  int         lineNum_     { 1 };
  std::string token_;
  Tokens      tokens_;
};

#endif
