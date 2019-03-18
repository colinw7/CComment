#include <CDoxyCheck.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>
#include <sys/types.h>

int
main(int argc, char **argv)
{
  bool quiet = false;
  bool debug = false;

  std::vector<std::string> files;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if      (argv[i][1] == 'q')
        quiet = true;
      else if (argv[i][1] == 'D')
        debug = true;
      else
        std::cerr << "Illegal option " << argv[i] << "\n";
    }
    else
      files.push_back(argv[i]);
  }

  CDoxyCheck check;

  check.setQuiet(quiet);
  check.setDebug(debug);

  uint num_files = files.size();

  for (uint i = 0; i < num_files; i++)
    check.processFile(files[i]);

  return 0;
}

//------

CDoxyCheck::
CDoxyCheck()
{
}

void
CDoxyCheck::
start_comment()
{
  commentStr_ = "";
}

void
CDoxyCheck::
end_comment()
{
  if (isDebug())
    std::cerr << "Comment: " << commentStr_ << "\n";;

  Token token(TokenType::COMMENT, commentStr_, lineNum_);

  token.commentType = commentType_;

  tokens_.push_back(token);

  commentStr_ = "";
  textStr_    = "";

  textLines_.clear();
}

void
CDoxyCheck::
put_normal(char c)
{
  if (c == '\n') {
    if (lastChar_ == '\\') {
      textStr_ += c;
    }
    else {
      parseText(textStr_, lineNum_);

      textLines_.push_back(TextLine(textStr_, lineNum_));

      ++lineNum_;

      textStr_  = "";
      lastChar_ = '\0';
    }
  }
  else {
    textStr_ += c;
  }

  lastChar_ = c;
}

void
CDoxyCheck::
parseText(const std::string &str, int lineNum)
{
  auto isOperatorChar = [](char c) {
    return (strchr("()[]+-*/%<>=&|!~^.?:", c) != 0);
  };

  auto readOperator = [](const std::string &str, int &i, int len) {
    std::string token;

    if (i >= len)
      return token;

    switch (str[i]) {
      case '(':
      case ')':
      case '[':
      case ']':
      case '~':
      case ',':
      case '?':
      case ':':
        token += str[i++];

        break;
      case '=':
        token += str[i++];

        if (i < len) {
          switch (str[i]) {
            case '=':
              token += str[i++];

              break;
            default:
              break;
          }
        }

        break;
      case '!':
        token += str[i++];

        if (i < len) {
          switch (str[i]) {
            case '=':
              token += str[i++];

              break;
            default:
              break;
          }
        }

        break;
      case '+':
        token += str[i++];

        if (i < len) {
          switch (str[i]) {
            case '+':
            case '=':
              token += str[i++];

              break;
            default:
              break;
          }
        }

        break;
      case '-':
        token += str[i++];

        if (i < len) {
          switch (str[i]) {
            case '>':
            case '-':
            case '=':
              token += str[i++];

              break;
            default:
              break;
          }
        }

        break;
      case '*':
        token += str[i++];

        if (i < len) {
          switch (str[i]) {
            case '=':
              token += str[i++];

              break;
            default:
              break;
          }
        }

        break;
      case '/':
        token += str[i++];

        if (i < len) {
          switch (str[i]) {
            case '=':
              token += str[i++];

              break;
            default:
              break;
          }
        }

        break;
      case '%':
        token += str[i++];

        if (i < len) {
          switch (str[i]) {
            case '=':
              token += str[i++];

              break;
            default:
              break;
          }
        }

        break;
      case '&':
        token += str[i++];

        if (i < len) {
          switch (str[i]) {
            case '&':
            case '=':
              token += str[i++];

              break;
            default:
              break;
          }
        }

        break;
      case '^':
        token += str[i++];

        if (i < len) {
          switch (str[i]) {
            case '=':
              token += str[i++];

              break;
            default:
              break;
          }
        }

        break;
      case '|':
        token += str[i++];

        if (i < len) {
          switch (str[i]) {
            case '|':
            case '=':
              token += str[i++];

              break;
            default:
              break;
          }
        }

        break;
      case '<':
        token += str[i++];

        if (i < len) {
          switch (str[i]) {
            case '=':
              token += str[i++];

              break;
            case '<':
              token += str[i++];

              if (i < len) {
                switch (str[i]) {
                  case '=':
                    token += str[i++];

                    break;
                  default:
                    break;
                }
              }

              break;
            default:
              break;
          }
        }

        break;
      case '>':
        token += str[i++];

        if (i < len) {
          switch (str[i]) {
            case '=':
              token += str[i++];

              break;
            case '>':
              token += str[i++];

              if (i < len) {
                switch (str[i]) {
                  case '=':
                    token += str[i++];

                    break;
                  default:
                    break;
                }
              }

              break;
            default:
              break;
          }
        }

        break;
      case '.':
        token += str[i++];

        if (i < len) {
          switch (str[i]) {
            case '.':
              if (i < len) {
                switch (str[i + 1]) {
                  case '.':
                    token += str[i++];
                    token += str[i++];

                    break;
                  default:
                    break;
                }
              }

              break;
            default:
              break;
          }
        }

        break;
      default:
        break;
    }

    return token;
  };

  auto isSeparatorChar = [](char c) {
    return (strchr("{};,", c) != 0);
  };

  auto readSeparator = [](const std::string &str, int &i, int len) {
    std::string token;

    if (i >= len)
      return token;

    switch (str[i]) {
      case '{':
      case '}':
      case ';':
      case ',':
        token += str[i++];
        break;
      default:
        break;
    }

    return token;
  };

  auto readDoubleQuotedString = [](const std::string &str, int &i, int len) {
    std::string token;

    if (i >= len || str[i] != '\"')
      return token;

    ++i;

    while (i < len) {
      if      (str[i] == '\\') {
        ++i;

        if (i < len)
          ++i;
      }
      else if (str[i] == '\"')
        break;
      else
        ++i;
    }

    if (i >= len)
      return token;

    ++i;

    return token;
  };

  auto readSingleQuotedString = [](const std::string &str, int &i, int len) {
    std::string token;

    if (i >= len || str[i] != '\'')
      return token;

    ++i;

    while (i < len) {
      if      (str[i] == '\\') {
        ++i;

        if (i < len)
          ++i;
      }
      else if (str[i] == '\'')
        break;
      else
        ++i;
    }

    if (i >= len)
      return token;

    ++i;

    return token;
  };

  auto isNumber = [](const std::string &str, int &i, int len) {
    if (i < len && ::isdigit(str[i]))
      return true;

    if (i < len - 1 && str[i] == '.' && ::isdigit(str[i + 1]))
      return true;

    return false;
  };

  auto readNumber = [](const std::string &str, int &i, int len) {
    std::string token;

    //------

    if (i < len && (str[i] == '+' || str[i] == '-'))
      token += str[i++];

    //------

    while (i < len && ::isdigit(str[i]))
      token += str[i++];

    //------

    if (i < len && str[i] == '.') {
      token += str[i++];

      while (i < len && ::isdigit(str[i]))
        token += str[i++];
    }

    //------

    if (i < len && (str[i] == 'e' || str[i] == 'E')) {
      token += str[i++];

      if (i < len && (str[i] == '+' || str[i] == '-'))
        token += str[i++];

      if (i >= len || ! ::isdigit(str[i]))
        return token;

      while (i < len && ::isdigit(str[i]))
        token += str[i++];
    }

    //------

    return token;
  };

  auto isIdentifier = [](const std::string &str, int i, int len) {
    if (i < len && (str[i] == '_' || ::isalpha(str[i])))
      return true;

    return false;
  };

  auto readIdentifier = [](const std::string &str, int &i, int len) {
    std::string token;

    if (i < len && (str[i] == '_' || ::isalpha(str[i]))) {
      token = str[i++];

      while (i < len && (str[i] == '_' || ::isalnum(str[i])))
        token += str[i++];
    }

    return token;
  };

  auto skipSpace = [](const std::string &str, int &i, int len) {
    while (i < len && isspace(str[i]))
      ++i;
  };

  auto addToken = [&](TokenType type) {
    tokens_.push_back(Token(type, token_, lineNum));

    token_ = "";
  };

  //---

  int i   = 0;
  int len = str.size();

  skipSpace(str, i, len);

  if (i < len && str[i] == '#')
    return;

  while (i < len) {
    if      (str[i] == '\"') {
      token_ = readDoubleQuotedString(str, i, len);

      if (isDebug()) std::cerr << "String: " << token_ << "\n";;
      addToken(TokenType::STRING);
    }
    else if (str[i] == '\'') {
      token_ = readSingleQuotedString(str, i, len);

      if (isDebug()) std::cerr << "String: " << token_ << "\n";;
      addToken(TokenType::STRING);
    }
    else if (isNumber(str, i, len)) {
      token_ = readNumber(str, i, len);

      if (isDebug()) std::cerr << "Number: " << token_ << "\n";
      addToken(TokenType::NUMBER);
    }
    else if (isOperatorChar(str[i])) {
      token_ = readOperator(str, i, len);

      if (isDebug()) std::cerr << "Operator: " << token_ << "\n";
      addToken(TokenType::OPERATOR);
    }
    else if (isSeparatorChar(str[i])) {
      token_ = readSeparator(str, i, len);

      if (isDebug()) std::cerr << "Separator: " << token_ << "\n";
      addToken(TokenType::SEPARATOR);
    }
    else if (isIdentifier(str, i, len)) {
      token_ = readIdentifier(str, i, len);

      if (isDebug()) std::cerr << "Identifier: " << token_ << "\n";
      addToken(TokenType::IDENTIFIER);
    }
    else if (isspace(str[i])) {
      skipSpace(str, i, len);
    }
    else {
      assert(false);
    }
  }
}

void
CDoxyCheck::
put_comment(char c)
{
  if (c == '\n')
    ++lineNum_;

  commentStr_ += c;
}

void
CDoxyCheck::
set_comment_type(CommentType type)
{
  commentType_ = type;
}

void
CDoxyCheck::
initFile()
{
  fileName_ = filename();
  lineNum_  = 0;

  commentType_ = CommentType::NONE;
  commentStr_  = "";

  textLines_.clear();

  textStr_  = "";
  lastChar_ = '\0';

  token_ = "";

  tokens_.clear();
}

void
CDoxyCheck::
termFile()
{
  if (textStr_ != "") {
    textLines_.push_back(TextLine(textStr_, lineNum_));

    textStr_ = "";
  }

  checkComment();
}

void
CDoxyCheck::
checkComment()
{
  bool isPublic = true;

  int i   = 0;
  int len = tokens_.size();

  CommentType commentType { CommentType::NONE };

  while (i < len) {
    const Token &token1 = tokens_[i];

    if      (token1.type == TokenType::COMMENT) {
      if (commentType != CommentType::QT)
        commentType = token1.commentType;
    }
    else if (token1.type == TokenType::IDENTIFIER && token1.str == "enum") {
      if (i >= len - 1)
        break;

      const Token &token2 = tokens_[i + 1];

      if (token2.type == TokenType::IDENTIFIER && token2.str == "class")
        ++i;
    }
    else if (token1.type == TokenType::IDENTIFIER && token1.str == "class") {
      if (i >= len - 2)
        break;

      const Token &token2 = tokens_[++i];
      const Token &token3 = tokens_[++i];

      if (token2.type == TokenType::IDENTIFIER &&
          (token3.type == TokenType::SEPARATOR && token3.str != ";")) {
        bool        fail = false;
        std::string errorMsg;

        if      (commentType == CommentType::NONE) {
          fail     = true;
          errorMsg = "no comment";
        }
        else if (commentType != CommentType::QT) {
          fail     = true;
          errorMsg = "no doxygen comment";
        }
        else {
          fail     = false;
          errorMsg = "doxygen comment";
        }

        if (isPublic) {
          if (! isQuiet()) {
            std::cout << fileName_ << ":" << token1.lineNum << " ";

            std::cout << "class " << token2.str;

            std::cout << " : " << (fail ? "FAIL" : "PASS") << " : " << errorMsg;

            std::cout << "\n";
          }
          else {
            if (fail) {
              std::cout << fileName_ << ":" << token1.lineNum << " ";

              std::cout << "class " << token2.str;

              std::cout << " : " << errorMsg;

              std::cout << "\n";
            }
          }
        }
      }
    }
    else if (token1.type == TokenType::IDENTIFIER &&
             (token1.str == "public" || token1.str == "protected" || token1.str == "private")) {
      if (i >= len - 1)
        break;

      const Token &token2 = tokens_[++i];

      if (token2.type == TokenType::OPERATOR && token2.str == ":") {
        isPublic = (token1.str == "public");
      }
    }
    else {
      commentType = CommentType::NONE;
    }

    ++i;
  }
}
