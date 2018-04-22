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

  file_ = file;

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
    if      (c == '"') {
      put_normal(c);

      state_.in_string1 = true;
    }
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

      start_comment();

      put_comment('/');
      put_comment(c  );

      if (c == '*')
        state_.in_comment1 = true;
      else
        state_.in_comment2 = true;
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
