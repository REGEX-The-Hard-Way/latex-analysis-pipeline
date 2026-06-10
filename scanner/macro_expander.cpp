/**
 * @file macro_expander.cpp
 *
 * Expands LaTeX \newcommand / \renewcommand / \providecommand macros in
 * a UTF-8 text stream so downstream tools that don't run TeX can still
 * see fully-resolved input. Implementation lives in latex_macros.h.
 *
 * Recognized definitions:
 *   \newcommand{\name}{body}
 *   \newcommand{\name}[N]{body}
 *   \newcommand{\name}[N][default]{body}   (optional first arg)
 *   \renewcommand{...}{...}
 *   \providecommand{...}{...}
 *
 * Usage:
 *   macro-expander input.tex [output.tex]
 *   macro-expander < input.tex > output.tex
 */

#include <fstream>
#include <iostream>

#include "latex_macros.h"

int main(int argc, char** argv) {
  try {
    if (argc > 3) {
      std::cerr << "Usage: " << argv[0] << " [input.tex] [output.tex]"
                << std::endl;
      return 1;
    }
    std::ifstream fin;
    std::ofstream fout;
    std::istream* in = &std::cin;
    std::ostream* out = &std::cout;
    if (argc >= 2) {
      fin.open(argv[1]);
      if (!fin) {
        std::cerr << "could not open input: " << argv[1] << std::endl;
        return 1;
      }
      in = &fin;
    }
    if (argc >= 3) {
      fout.open(argv[2]);
      if (!fout) {
        std::cerr << "could not open output: " << argv[2] << std::endl;
        return 1;
      }
      out = &fout;
    }
    meta_tools::latex::Expander e(*in, *out);
    e.run();
  } catch (const std::exception& ex) {
    std::cerr << "macro-expander error: " << ex.what() << std::endl;
    return 1;
  }
  return 0;
}
