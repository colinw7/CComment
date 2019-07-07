#ifndef CCommentParser_H
#define CCommentParser_H

#include <string>
#include <vector>

class CCommentParser {
 public:
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

 public:
  CCommentParser();

  virtual ~CCommentParser() { }

  const std::string &filename() const { return file_; }

  bool processFile(const std::string &file);

  bool processCFile   (const std::string &file);
  bool processHtmlFile(const std::string &file);

  bool processCChar(int c);

  virtual void initFile() { }
  virtual void termFile() { }

  virtual void start_comment() { }
  virtual void end_comment  () { }

  virtual void put_normal (char c);
  virtual void put_comment(char c);

  virtual void set_comment_type(CommentType) { }

  virtual int getChar() const;

 protected:
  struct State {
    bool        in_comment1 { false };
    bool        in_comment2 { false };
    bool        in_string1  { false };
    bool        in_string2  { false };
    CommentType commentType { CommentType::NONE };

    void init() {
      in_comment1 = false;
      in_comment2 = false;
      in_string1  = false;
      in_string2  = false;
      commentType = CommentType::NONE;
    }
  };

  using CharBuffer = std::vector<char>;

  std::string file_;
  int         lineNum_ { 0 };
  int         charNum_ { 0 };
  FILE*       fp_      { nullptr };
  State       state_;
  CharBuffer  commentBuffer_;
};

#endif
