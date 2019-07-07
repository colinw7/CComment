#include <CCommentParser.h>
#include <cstdio>
#include <cassert>

CCommentParser::
CCommentParser()
{
}

bool
CCommentParser::
processFile(const std::string &file)
{
  return processCFile(file);
}

bool
CCommentParser::
processCFile(const std::string &file)
{
  bool rc = false;

  file_    = file;
  lineNum_ = 0;
  charNum_ = 0;

  initFile();

  fp_ = fopen(file_.c_str(), "r");

  if (fp_) {
    state_.init();

    int c;

    while ((c = getChar()) != EOF) {
      if (! processCChar(c))
        break;
    }

    fclose(fp_);

    rc = true;
  }

  termFile();

  file_ = "";

  return rc;
}

bool
CCommentParser::
processCChar(int c)
{
  if (c == '\n') {
    ++lineNum_;

    charNum_ = 0;
  }
  else
    ++charNum_;

  //---

  // in double quoted string
  if      (state_.in_string1) {
    if     (c == '\\') {
      put_normal(c);

      c = getChar();

      put_normal(c);
    }
    else if (c == '"') {
      put_normal(c);

      state_.in_string1 = false;
    }
    else {
      put_normal(c);
    }
  }
  // in single quoted string
  else if (state_.in_string2) {
    if     (c == '\\') {
      put_normal(c);

      c = getChar();

      put_normal(c);
    }
    else if (c == '\'') {
      put_normal(c);

      state_.in_string2 = false;
    }
    else {
      put_normal(c);
    }
  }
  // in C comment
  else if (state_.in_comment1) {
    if (c == '*') {
      c = getChar();

      while (c == '*') {
        put_comment(c);

        c = getChar();
      }

      if (c == EOF)
        return false;

      put_comment('*');
      put_comment(c  );

      if (c != '/')
        return true;

      end_comment();

      state_.in_comment1 = false;
    }
    else {
      put_comment(c);
    }

    return true;
  }
  // in C++ comment
  else if (state_.in_comment2) {
    if (c == '\n') {
      end_comment();

      state_.in_comment2 = false;
    }

    if (state_.in_comment2)
      put_comment(c);
    else
      put_normal(c);

    return true;
  }
  else {
    // start double quoted string
    if      (c == '"') {
      put_normal(c);

      state_.in_string1 = true;
    }
    // start single quoted string
    else if (c == '\'') {
      put_normal(c);

      state_.in_string2 = true;
    }
    else if (c == '/') {
      c = getChar();

      if (c != '*' && c != '/') {
        put_normal('/');
        put_normal(c  );

        return true;
      }

      // C or C++ comment
      start_comment();

      commentBuffer_.push_back('/');
      commentBuffer_.push_back(c  );

      if (c == '*') {
        state_.in_comment1 = true;
        state_.commentType = CommentType::C_NORMAL;

        int c1 = getChar();

        if (c1 != EOF)
          commentBuffer_.push_back(c1);

        if      (c1 == '*') {
          int c2 = getChar();

          if (c2 != EOF)
            commentBuffer_.push_back(c2);

          if      (isspace(c2)) {
            state_.commentType = CommentType::JAVADOC;
          }
          else if (c2 == '<') {
            int c3 = getChar();

            if (c3 != EOF)
              commentBuffer_.push_back(c2);

            if (isspace(c3)) {
              state_.commentType = CommentType::JAVADOC_AFTER;
            }
          }
          else if (c2 == '/') {
            end_comment();
          }
        }
        else if (c1 == '!') {
          int c2 = getChar();

          if (c2 != EOF)
            commentBuffer_.push_back(c2);

          if      (isspace(c2)) {
            state_.commentType = CommentType::QT;
          }
          else if (c2 == '<') {
            int c3 = getChar();

            if (c3 != EOF)
              commentBuffer_.push_back(c2);

            if (isspace(c3)) {
              state_.commentType = CommentType::QT_AFTER;
            }
          }
        }
      }
      else {
        state_.in_comment2 = true;
        state_.commentType = CommentType::CPP_NORMAL;

        int c1 = getChar();

        if (c1 != EOF)
          commentBuffer_.push_back(c1);

        if      (c1 == '/') {
          int c2 = getChar();

          if (c2 != EOF)
            commentBuffer_.push_back(c2);

          if (isspace(c2)) {
            state_.commentType = CommentType::CPP_BLOCK;
          }
        }
        else if (c1 == '!') {
          int c2 = getChar();

          if (c2 != EOF)
            commentBuffer_.push_back(c2);

          if      (isspace(c2)) {
            state_.commentType = CommentType::QT;
          }
          else if (c2 == '<') {
            int c3 = getChar();

            if (c3 != EOF)
              commentBuffer_.push_back(c2);

            if (isspace(c3)) {
              state_.commentType = CommentType::QT_AFTER;
            }
          }
        }
      }

      set_comment_type(state_.commentType);

      for (const auto &c : commentBuffer_)
        put_comment(c);

      commentBuffer_.clear();
    }
    else {
      put_normal(c);
    }
  }

  return true;
}

bool
CCommentParser::
processHtmlFile(const std::string &file)
{
  bool rc = false;

  file_ = file;

  initFile();

  fp_ = fopen(file_.c_str(), "r");

  if (fp_ == NULL)
    goto done;

  {
  int c;

  bool in_comment = false;

  while ((c = getChar()) != EOF) {
    if (in_comment) {
      if (c == '-') {
        c = getChar();

        while (c == '-') {
          put_comment(c);

          c = getChar();
        }

        if (c == EOF)
          break;

        put_comment('-');
        put_comment(c  );

        if (c != '>')
          continue;

        end_comment();

        in_comment = false;
      }
      else {
        put_comment(c);
      }

      continue;
    }
    else {
      if (c == '<') {
        int c1 = getChar();

        if (c1 == '!') {
          int c2 = getChar();

          if (c2 == '-') {
            int c3 = getChar();

            if (c3 == '-') {
              start_comment();

              in_comment = true;

              put_comment('<');
              put_comment('!');
              put_comment('-');
              put_comment('-');
            }
            else {
              put_normal(c);
              put_normal(c1);
              put_normal(c2);
              put_normal(c3);
            }
          }
          else {
            put_normal(c);
            put_normal(c1);
            put_normal(c2);
          }
        }
        else {
          put_normal(c);
          put_normal(c1);
        }
      }
      else {
        put_normal(c);
      }
    }
  }
  }

  fclose(fp_);

  rc = true;

 done:
  termFile();

  file_ = "";

  return rc;
}

int
CCommentParser::
getChar() const
{
  assert(fp_);

  return fgetc(fp_);
}

void
CCommentParser::
put_normal(char c)
{
  fputc(c, stdout);
}

void
CCommentParser::
put_comment(char c)
{
  fputc(c, stdout);
}
