#ifndef CDoxyCheck_H
#define CDoxyCheck_H

#include <string>
#include <vector>

/*!
 * Check if classes in header files have doxygen comments
 */
class CDoxyCheck {
 public:
  CDoxyCheck();

  bool isQuiet() const { return quiet_; }
  void setQuiet(bool b) { quiet_ = b; }

  bool isCheckClass() const { return checkClass_; }
  void setCheckClass(bool b) { checkClass_ = b; }

  bool isCheckStruct() const { return checkStruct_; }
  void setCheckStruct(bool b) { checkStruct_ = b; }

  bool isDebug() const { return debug_; }
  void setDebug(bool b) { debug_ = b; }

  bool processFile(const std::string &fileName);

 private:
  enum class CommentType {
    NONE,
    C_NORMAL,
    CPP_NORMAL,
    CPP_BLOCK,
    JAVADOC,
    JAVADOC_AFTER,
    QT,
    QT_AFTER
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

  enum class ScopePrivacy {
    PUBLIC,
    PRIVATE,
    PROTECTED
  };

 private:
  struct Line {
    std::string str;
    int         lineNum { 0 };

    Line(const std::string &str="", int lineNum=0) :
     str(str), lineNum(lineNum) {
    }
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

  struct ScopeData {
    ScopePrivacy privacy { ScopePrivacy::PUBLIC };
  };

  using Lines      = std::vector<Line>;
  using Tokens     = std::vector<Token>;
  using ScopeDatas = std::vector<ScopeData>;

 private:
  void processLines();

  void processLine(const Line &line);

  void checkComments();

  void checkCommented(const Token &token1, const Token &token2, CommentType commentType,
                      const ScopeData &scopeData);

 private:
  bool        quiet_       { false };
  bool        checkClass_  { false };
  bool        checkStruct_ { false };
  bool        debug_       { false };
  std::string fileName_;
  Lines       lines_;
  int         lineNum_     { 1 };
  CommentType commentType_ { CommentType::NONE };
  std::string token_;
  Tokens      tokens_;
  bool        inComment_   { false };
};

#endif
