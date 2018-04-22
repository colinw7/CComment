#ifndef CConvertComment_H
#define CConvertComment_H

#include <CCommentParser.h>

class CConvertComment : public CCommentParser {
 public:
  CConvertComment();

  void init();
  void term();

  void initFile();
  void termFile();

  void put_normal (char c);
  void put_comment(char c);

 private:
  void flushComment();

 private:
  std::string comment_;
  bool        in_comment_ { false };
};

#endif
