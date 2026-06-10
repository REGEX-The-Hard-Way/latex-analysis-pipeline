/**
 * @file latex_macros.h
 *
 * Header-only LaTeX macro expander shared by `macro-expander` and
 * `sentence-splitter`. See macro_expander.cpp for the user-facing
 * documentation.
 *
 * Supports:
 *   \newcommand{\name}{body}
 *   \newcommand{\name}[N]{body}
 *   \newcommand{\name}[N][default]{body}   (optional first arg)
 *   \renewcommand{...}{...}
 *   \providecommand{...}{...}
 */

#ifndef META_TOOLS_LATEX_MACROS_H_
#define META_TOOLS_LATEX_MACROS_H_

#include <cctype>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace meta_tools {
namespace latex {

// Sentinel used to encode #N parameter slots inside a macro body.
// 0x01 (SOH) cannot legitimately appear in TeX source.
constexpr char PARAM_SENTINEL = '\x01';

struct Macro {
  int argcount = 0;
  bool has_default = false;
  std::string default_arg;
  std::string body;  // PARAM_SENTINEL + ('0'+i) marks #i+1
};

class Reader {
 public:
  explicit Reader(std::istream& in) : in_(&in) {}

  int get() {
    if (!pushback_.empty()) {
      int c = pushback_.back();
      pushback_.pop_back();
      return c;
    }
    return in_->get();
  }

  void unget(int c) { pushback_.push_back(c); }

  void unget_string(const std::string& s) {
    for (auto it = s.rbegin(); it != s.rend(); ++it) {
      pushback_.push_back(static_cast<unsigned char>(*it));
    }
  }

 private:
  std::istream* in_;
  std::vector<int> pushback_;
};

inline bool is_letter(int c) {
  return c != EOF && std::isalpha(static_cast<unsigned char>(c));
}

class Expander {
 public:
  Expander(std::istream& in, std::ostream& out) : reader_(in), out_(&out) {}

  void run() {
    while (true) {
      int c = reader_.get();
      if (c == EOF) break;
      if (c == '%') {
        out_->put(static_cast<char>(c));
        while ((c = reader_.get()) != EOF) {
          out_->put(static_cast<char>(c));
          if (c == '\n') break;
        }
        continue;
      }
      if (c == '\\') {
        int n = reader_.get();
        if (is_letter(n)) {
          std::string name = read_command_name(n);
          handle_command(name);
        } else if (n == EOF) {
          out_->put('\\');
        } else {
          out_->put('\\');
          out_->put(static_cast<char>(n));
        }
        continue;
      }
      out_->put(static_cast<char>(c));
    }
  }

 private:
  Reader reader_;
  std::ostream* out_;
  std::unordered_map<std::string, Macro> macros_;

  std::string read_command_name(int first) {
    std::string name(1, static_cast<char>(first));
    int c;
    while (is_letter(c = reader_.get())) {
      name.push_back(static_cast<char>(c));
    }
    if (c != EOF) reader_.unget(c);
    return name;
  }

  int next_non_comment() {
    while (true) {
      int c = reader_.get();
      if (c != '%') return c;
      while ((c = reader_.get()) != EOF && c != '\n') {
        // skip
      }
    }
  }

  int next_non_space() {
    int c;
    while ((c = next_non_comment()) != EOF
           && std::isspace(static_cast<unsigned char>(c))) {
    }
    return c;
  }

  // Read content of a {...} group; the leading '{' must already be consumed.
  std::string read_brace_group() {
    std::string out;
    int depth = 0;
    while (true) {
      int c = reader_.get();
      if (c == EOF) {
        throw std::runtime_error{"unterminated brace group"};
      }
      if (c == '\\') {
        int n = reader_.get();
        out.push_back('\\');
        if (n != EOF) out.push_back(static_cast<char>(n));
        continue;
      }
      if (c == '{') {
        ++depth;
        out.push_back('{');
      } else if (c == '}') {
        if (depth == 0) return out;
        --depth;
        out.push_back('}');
      } else {
        out.push_back(static_cast<char>(c));
      }
    }
  }

  // Read content of a [..] group; the leading '[' must already be consumed.
  // Honors backslash escapes and nested brackets.
  std::string read_bracket_group() {
    std::string out;
    int depth = 0;
    while (true) {
      int c = reader_.get();
      if (c == EOF) {
        throw std::runtime_error{"unterminated bracket group"};
      }
      if (c == '\\') {
        int n = reader_.get();
        out.push_back('\\');
        if (n != EOF) out.push_back(static_cast<char>(n));
        continue;
      }
      if (c == '[') {
        ++depth;
        out.push_back('[');
      } else if (c == ']') {
        if (depth == 0) return out;
        --depth;
        out.push_back(']');
      } else {
        out.push_back(static_cast<char>(c));
      }
    }
  }

  void handle_command(const std::string& name) {
    if (name == "newcommand" || name == "renewcommand"
        || name == "providecommand") {
      learn_macro(/*only_if_undefined=*/name == "providecommand");
      return;
    }
    auto it = macros_.find(name);
    if (it == macros_.end()) {
      out_->put('\\');
      *out_ << name;
      return;
    }
    expand(name, it->second);
  }

  void learn_macro(bool only_if_undefined) {
    int c = next_non_space();
    if (c != '{') {
      throw std::runtime_error{"expected '{' after \\newcommand"};
    }
    c = next_non_space();
    if (c != '\\') {
      throw std::runtime_error{
          "expected \\name inside \\newcommand{...}"};
    }
    c = reader_.get();
    if (!is_letter(c)) {
      throw std::runtime_error{"expected letter after \\ in macro name"};
    }
    std::string name = read_command_name(c);
    c = next_non_space();
    if (c != '}') {
      throw std::runtime_error{"expected '}' after \\name"};
    }

    Macro m;
    c = next_non_space();
    if (c == '[') {
      int d = reader_.get();
      if (!std::isdigit(static_cast<unsigned char>(d))) {
        throw std::runtime_error{"expected digit inside [..] arg count"};
      }
      m.argcount = d - '0';
      int e = reader_.get();
      if (e != ']') {
        throw std::runtime_error{"expected ']' after arg count"};
      }
      c = next_non_space();
      // Optional default for argument #1: \newcommand{\foo}[N][default]{...}
      if (c == '[') {
        m.default_arg = read_bracket_group();
        m.has_default = true;
        c = next_non_space();
      }
    }
    if (c != '{') {
      throw std::runtime_error{"expected '{' to start macro body"};
    }
    m.body = encode_body(read_brace_group(), m.argcount, name);

    if (only_if_undefined) {
      macros_.emplace(name, std::move(m));
    } else {
      macros_[name] = std::move(m);
    }
  }

  std::string encode_body(const std::string& raw, int argcount,
                          const std::string& macro_name) {
    std::string out;
    out.reserve(raw.size());
    for (std::size_t i = 0; i < raw.size(); ++i) {
      char c = raw[i];
      if (c == '\\' && i + 1 < raw.size()) {
        out.push_back('\\');
        out.push_back(raw[i + 1]);
        ++i;
        continue;
      }
      if (c == '#' && i + 1 < raw.size()) {
        char n = raw[i + 1];
        if (n == '#') {
          out.push_back('#');
          ++i;
          continue;
        }
        if (std::isdigit(static_cast<unsigned char>(n))) {
          int idx = n - '0';
          if (idx < 1 || idx > argcount) {
            throw std::runtime_error{
                "\\" + macro_name + " body references #"
                + std::to_string(idx) + " but only "
                + std::to_string(argcount) + " arg(s) declared"};
          }
          out.push_back(PARAM_SENTINEL);
          out.push_back(static_cast<char>('0' + (idx - 1)));
          ++i;
          continue;
        }
      }
      out.push_back(c);
    }
    return out;
  }

  void expand(const std::string& name, const Macro& m) {
    std::vector<std::string> args(static_cast<std::size_t>(m.argcount));
    int first_arg_index = 0;
    if (m.has_default && m.argcount > 0) {
      // Look for an optional [..] before the mandatory args. We must
      // remember any whitespace/comments we consume while peeking so we
      // can restore them if no '[' is found (otherwise non-optional
      // call sites lose their formatting).
      std::string skipped;
      int c;
      while ((c = reader_.get()) != EOF
             && (std::isspace(static_cast<unsigned char>(c)) || c == '%')) {
        skipped.push_back(static_cast<char>(c));
        if (c == '%') {
          while ((c = reader_.get()) != EOF && c != '\n') {
            skipped.push_back(static_cast<char>(c));
          }
          if (c == '\n') skipped.push_back('\n');
          else if (c == EOF) break;
        }
      }
      if (c == '[') {
        args[0] = read_bracket_group();
      } else {
        if (c != EOF) reader_.unget(c);
        reader_.unget_string(skipped);
        args[0] = m.default_arg;
      }
      first_arg_index = 1;
    }
    for (int i = first_arg_index; i < m.argcount; ++i) {
      int c = next_non_space();
      if (c != '{') {
        throw std::runtime_error{
            "expected '{' for argument #" + std::to_string(i + 1)
            + " of \\" + name};
      }
      args[static_cast<std::size_t>(i)] = read_brace_group();
    }
    std::string out;
    out.reserve(m.body.size());
    for (std::size_t i = 0; i < m.body.size(); ++i) {
      if (m.body[i] == PARAM_SENTINEL && i + 1 < m.body.size()) {
        std::size_t idx
            = static_cast<std::size_t>(m.body[i + 1] - '0');
        if (idx < args.size()) out += args[idx];
        ++i;
      } else {
        out.push_back(m.body[i]);
      }
    }
    reader_.unget_string(out);
  }
};

// Convenience: expand all \newcommand-style macros in `input` and
// return the resulting string.
inline std::string expand(const std::string& input) {
  std::istringstream in(input);
  std::ostringstream out;
  Expander e(in, out);
  e.run();
  return out.str();
}

}  // namespace latex
}  // namespace meta_tools

#endif  // META_TOOLS_LATEX_MACROS_H_
