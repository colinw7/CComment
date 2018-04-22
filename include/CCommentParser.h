#ifndef CCommentParser_H
#define CCommentParser_H

#include <string>

class CCommentParser {
 public:
  CCommentParser();

  virtual ~CCommentParser() { }

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

  virtual int getChar() const;

 protected:
  struct State {
    bool in_comment1 { false };
    bool in_comment2 { false };
    bool in_string1  { false };
    bool in_string2  { false };

    void init() {
      in_comment1 = false;
      in_comment2 = false;
      in_string1  = false;
      in_string2  = false;
    }
  };

  std::string file_;
  FILE*       fp_ { nullptr };
  State       state_;
};

#endif
