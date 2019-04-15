#include <CDoxyCheck.h>
#include <iostream>
#include <cstring>
#include <cassert>
#include <sys/types.h>

int
main(int argc, char **argv)
{
  bool quiet       = false;
  bool checkClass  = false;
  bool checkStruct = false;
  bool debug       = false;

  std::vector<std::string> files;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      std::string arg = &argv[i][1];

      if      (arg == "q" || arg == "quiet")
        quiet = true;
      else if (arg == "class")
        checkClass = true;
      else if (arg == "struct")
        checkStruct = true;
      else if (arg == "D" || arg == "debug")
        debug = true;
      else if (arg == "h" || arg == "help")
        std::cerr << "Usage: CDoxyCheck [-q] [-struct] [-class] [-D] [-h]\n";
      else
        std::cerr << "Illegal option " << argv[i] << "\n";
    }
    else
      files.push_back(argv[i]);
  }

  if (! checkClass && ! checkStruct)
    checkClass = true;

  //---

  CDoxyCheck check;

  check.setQuiet(quiet);

  check.setCheckClass(checkClass);
  check.setCheckStruct(checkStruct);

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

bool
CDoxyCheck::
processFile(const std::string &fileName)
{
  // init
  fileName_ = fileName;

  lines_.clear();

  commentType_ = CommentType::NONE;

  token_ = "";

  tokens_.clear();

  //---

  // read lines
  FILE *fp = fopen(fileName_.c_str(), "r");
  if (! fp) return false;

  int  c;
  char lastChar = '\0';

  Line line;
  int  lineNum = 1;

  line.lineNum = lineNum;

  while ((c = fgetc(fp)) != EOF) {
    if (c == '\n') {
      ++lineNum;

      if (lastChar != '\\') {
        lines_.push_back(line);

        line.str     = "";
        line.lineNum = lineNum;
        lastChar     = '\0';
      }
      else {
        line.str = line.str.substr(0, line.str.size() - 1);
      }
    }
    else {
      line.str += char(c);
    }

    lastChar = c;
  }

  fclose(fp);

  //---

  // process lines
  if (isDebug()) std::cerr << "Process Lines\n";

  processLines();

  //---

  // check comments
  if (isDebug()) std::cerr << "\nCheck Comments\n";

  checkComments();

  //---

  fileName_ = "";
  lineNum_  = 0;

  return true;
}

void
CDoxyCheck::
processLines()
{
  inComment_ = false;

  for (const auto &line : lines_)
    processLine(line);
}

void
CDoxyCheck::
processLine(const Line &line)
{
  const std::string& str     = line.str;
  int                lineNum = line.lineNum;

  //---

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

  auto isComment = [](const std::string &str, int &i, int len) {
    return (i < len - 1 && str[i] == '/' && (str[i + 1] == '/' || str[i + 1] == '*'));
  };

  auto readComment = [](const std::string &str, int &i, int len,
                        CommentType &type, bool &complete) {
    type     = CommentType::NONE;
    complete = true;

    std::string token;

    assert(str[i] == '/');

    token += str[i++];

    char c1 = str[i++];

    assert(c1 == '/' || c1 == '*');

    token += c1;

    if (c1 == '/') {
      type = CommentType::CPP_NORMAL;

      while (i < len)
        token += str[i++];

      if      (token[2] == '/') {
        type = CommentType::CPP_BLOCK;
      }
      else if (token[2] == '!') {
        if (token[3] == '<')
          type = CommentType::QT_AFTER;
        else
          type = CommentType::QT;
      }
    }
    else {
      complete = false;

      type = CommentType::C_NORMAL;

      while (i < len) {
        if (i < len - 1 && str[i] == '*' && str[i + 1] == '/') {
          token += str[i++];
          token += str[i++];
          complete = true;
          break;
        }
        else
          token += str[i++];
      }

      if      (token[2] == '*') {
        if (token[3] == '<')
          type = CommentType::JAVADOC_AFTER;
        else
          type = CommentType::JAVADOC;
      }
      else if (token[2] == '!') {
        if (token[3] == '<')
          type = CommentType::QT_AFTER;
        else
          type = CommentType::QT;
      }
    }

    return token;
  };

  auto continuedComment = [](const std::string &str, int &i, int len, bool &complete) {
    std::string token;

    complete = false;

    while (i < len) {
      if (i < len - 1 && str[i] == '*' && str[i + 1] == '/') {
        token += str[i++];
        token += str[i++];
        complete = true;
        break;
      }
      else
        token += str[i++];
    }

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
    Token token(type, token_, lineNum);

    if (type == TokenType::COMMENT)
      token.commentType = commentType_;

    tokens_.push_back(token);

    token_       = "";
    commentType_ = CommentType::NONE;
  };

  //---

  int i   = 0;
  int len = str.size();

  skipSpace(str, i, len);

  if (i < len && str[i] == '#')
    return;

  while (i < len) {
    if      (inComment_) {
      bool complete;

      std::string token = continuedComment(str, i, len, complete);

      token_ += token;

      if (complete) {
        if (isDebug()) std::cerr << "Comment: " << token_ << "\n";
        addToken(TokenType::COMMENT);
        inComment_ = false;
      }
    }
    else if (str[i] == '\"') {
      token_ = readDoubleQuotedString(str, i, len);

      if (isDebug()) std::cerr << "String: " << token_ << "\n";
      addToken(TokenType::STRING);
    }
    else if (str[i] == '\'') {
      token_ = readSingleQuotedString(str, i, len);

      if (isDebug()) std::cerr << "String: " << token_ << "\n";
      addToken(TokenType::STRING);
    }
    else if (isComment(str, i, len)) {
      commentType_ = CommentType::NONE;

      bool complete;

      token_ = readComment(str, i, len, commentType_, complete);

      if (complete) {
        if (isDebug()) std::cerr << "Comment: " << token_ << "\n";
        addToken(TokenType::COMMENT);
      }
      else
        inComment_ = true;
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
checkComments()
{
  auto printPrivacy = [](ScopePrivacy privacy, bool newline=true) {
    if      (privacy == ScopePrivacy::PUBLIC   ) std::cerr << "PUBLIC";
    else if (privacy == ScopePrivacy::PROTECTED) std::cerr << "PROTECTED";
    else if (privacy == ScopePrivacy::PRIVATE  ) std::cerr << "PRIVATE";

    if (newline) std::cerr << "\n";
  };

  //---

  ScopeData scopeData;

  ScopeDatas scopeDatas;

  CommentType commentType { CommentType::NONE };

  //---

  auto printScopeStack = [&]() {
    std::cerr << "Stack (#" << scopeDatas.size() << ")";

    for (auto &scopeData1 : scopeDatas) {
      std::cerr << " ";

      printPrivacy(scopeData1.privacy, false);
    }

    std::cerr << " ";

    printPrivacy(scopeData.privacy);
  };

  auto pushScope = [&](ScopePrivacy privacy) {
    scopeDatas.push_back(scopeData);

    scopeData = ScopeData();

    scopeData.privacy = privacy;

    commentType = CommentType::NONE;

    if (isDebug()) printScopeStack();
  };

  auto popScope = [&](int lineNum) {
    if (! scopeDatas.empty()) {
      scopeData = scopeDatas.back();

      scopeDatas.pop_back();
    }
    else {
      if (isDebug()) std::cout << fileName_ << ":" << lineNum << " Mismatched {}\n";
    }

    commentType = CommentType::NONE;

    if (isDebug()) printScopeStack();
  };

  //---

  ScopePrivacy nextPrivacy = ScopePrivacy::PUBLIC;

  int i   = 0;
  int len = tokens_.size();

  while (i < len) {
    const Token &token1 = tokens_[i];

    if (isDebug()) std::cerr << "Token: " << token1.str << "\n";

    if      (token1.type == TokenType::COMMENT) {
      if (commentType != CommentType::QT)
        commentType = token1.commentType;
    }
    else if (token1.type == TokenType::IDENTIFIER && token1.str == "template") {
      if (i >= len - 1)
        break;

      const Token &token2 = tokens_[i + 1];

      if (token2.type == TokenType::OPERATOR && token2.str == "<") {
        i += 2;

        int brackets = 1;

        while (i < len) {
          const Token &token3 = tokens_[i];

          if      (token3.type == TokenType::OPERATOR && token2.str == "<")
            ++brackets;
          else if (token3.type == TokenType::OPERATOR && token2.str == ">") {
            --brackets;

            if (brackets == 0)
              break;
          }

          ++i;
        }
      }
    }
    else if (token1.type == TokenType::IDENTIFIER && token1.str == "enum") {
      if (i >= len - 1)
        break;

      const Token &token2 = tokens_[i + 1];

      if (token2.type == TokenType::IDENTIFIER && token2.str == "class") {
        ++i;

        if (isDebug()) std::cerr << "Token: " << token2.str << "\n";
      }
    }
    else if (token1.type == TokenType::IDENTIFIER && token1.str == "class") {
      if (i >= len - 2)
        break;

      const Token &token2 = tokens_[i + 1];
      const Token &token3 = tokens_[i + 2];

      if (token2.type == TokenType::IDENTIFIER &&
          (token3.type != TokenType::SEPARATOR || token3.str != ";")) {
        if (isCheckClass())
          checkCommented(token1, token2, commentType, scopeData);

        nextPrivacy = ScopePrivacy::PRIVATE;
      }
    }
    else if (token1.type == TokenType::IDENTIFIER && token1.str == "struct") {
      if (i >= len - 2)
        break;

      const Token &token2 = tokens_[i + 1];
      const Token &token3 = tokens_[i + 2];

      if (token2.type == TokenType::IDENTIFIER &&
          (token3.type != TokenType::SEPARATOR || token3.str != ";")) {
        if (isCheckStruct())
          checkCommented(token1, token2, commentType, scopeData);

        nextPrivacy = ScopePrivacy::PUBLIC;
      }
    }
    else if (token1.type == TokenType::IDENTIFIER &&
             (token1.str == "public" || token1.str == "protected" || token1.str == "private")) {
      if (i >= len - 1)
        break;

      const Token &token2 = tokens_[i + 1];

      if (token2.type == TokenType::OPERATOR && token2.str == ":") {
        if      (token1.str == "public")
          scopeData.privacy = ScopePrivacy::PUBLIC;
        else if (token1.str == "protected")
          scopeData.privacy = ScopePrivacy::PROTECTED;
        else if (token1.str == "private")
          scopeData.privacy = ScopePrivacy::PRIVATE;

        if (isDebug()) printScopeStack();
      }
    }
    else if (token1.type == TokenType::SEPARATOR && token1.str == "{") {
      pushScope(nextPrivacy);

      nextPrivacy = ScopePrivacy::PUBLIC;
    }
    else if (token1.type == TokenType::SEPARATOR && token1.str == "}") {
      popScope(token1.lineNum);
    }
    else {
      commentType = CommentType::NONE;
    }

    ++i;
  }
}

void
CDoxyCheck::
checkCommented(const Token &token1, const Token &token2, CommentType commentType,
               const ScopeData &scopeData)
{
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

  if (scopeData.privacy == ScopePrivacy::PUBLIC) {
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
