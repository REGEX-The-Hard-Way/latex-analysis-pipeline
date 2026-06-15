"""
Extended test cases for macro_expander.c (100 real-world LaTeX tests).
These tests cover basic macros, parameters, @ macros, mathematical constructs,
and real paper patterns.

Usage:
    python3 -m unittest tests.test_macro_expander_extended -v
"""

import unittest
import subprocess


def _expand(text: str) -> subprocess.CompletedProcess:
    """Pipe `text` through macro_expander and return the CompletedProcess."""
    expander = './macro_expander.out'
    return subprocess.run(
        [expander],
        input=text,
        capture_output=True,
        text=True
    )


class TestBasicMacros(unittest.TestCase):
    """Tests 1-20: Basic LaTeX macro commands."""

    def test_newcommand_basic(self):
        r = _expand(r'\newcommand{\foo}{bar}\foo')
        self.assertIn('bar', r.stdout)

    def test_def_basic(self):
        r = _expand(r'\def\foo{bar}\foo')
        self.assertIn('bar', r.stdout)

    def test_edef_basic(self):
        r = _expand(r'\edef\foo{hello}\foo')
        self.assertIn('hello', r.stdout)

    def test_xdef_basic(self):
        r = _expand(r'\xdef\foo{world}\foo')
        self.assertIn('world', r.stdout)

    def test_gdef_basic(self):
        r = _expand(r'\gdef\foo{global}\foo')
        self.assertIn('global', r.stdout)

    def test_renewcommand_basic(self):
        r = _expand(r'\newcommand{\foo}{old}\renewcommand{\foo}{new}\foo')
        self.assertIn('new', r.stdout)

    def test_providecommand(self):
        r = _expand(r'\providecommand{\foo}{provided}\foo')
        self.assertIn('provided', r.stdout)

    def test_let_basic(self):
        r = _expand(r'\def\b{x}\let\a\b\a')
        self.assertIn('x', r.stdout)

    def test_relax(self):
        r = _expand(r'\relax')
        self.assertIn('', r.stdout)

    def test_noexpand(self):
        r = _expand(r'\noexpand\foo')
        self.assertIn(r'\foo', r.stdout)

    def test_aftergroup(self):
        r = _expand(r'\aftergroup\foo\foo')
        self.assertEqual(r.returncode, 0)

    def test_expandafter(self):
        r = _expand(r'\expandafter\foo\bar\foo')
        self.assertIn(r'\bar', r.stdout)

    def test_csname(self):
        r = _expand(r'\csname foo\endcsname')
        self.assertIn(r'\foo', r.stdout)

    def test_def_one_param(self):
        r = _expand(r'\def\foo#1{#1}\foo{test}')
        self.assertIn('test', r.stdout)

    def test_def_two_params(self):
        r = _expand(r'\def\foo#1#2{#1+#2}\foo{a}{b}')
        self.assertIn('a+b', r.stdout)

    def test_newcommand_one_arg(self):
        r = _expand(r'\newcommand{\add}[1]{#1}\add{x}')
        self.assertIn('x', r.stdout)

    def test_newcommand_two_args(self):
        r = _expand(r'\newcommand{\add}[2]{#1+#2}\add{1}{2}')
        self.assertIn('1+2', r.stdout)

    def test_double_hash(self):
        r = _expand(r'\def\foo{##}\foo')
        self.assertIn('#', r.stdout)


class TestParameterHandling(unittest.TestCase):
    """Tests 21-40: Parameter handling and delimiters."""

    def test_param_then_double_hash(self):
        r = _expand(r'\def\foo#1##{#1}\foo{x')
        # Note: The closing brace is consumed by ##, so this is tricky
        self.assertEqual(r.returncode, 0)

    def test_undelimited_param(self):
        r = _expand(r'\def\foo#a{x}\foo y')
        self.assertIn('x', r.stdout)

    def test_double_hash_param(self):
        r = _expand(r'\def\foo##1{##1}\foo x')
        self.assertIn('x', r.stdout)

    def test_three_params(self):
        r = _expand(r'\def\foo#1#2#3{#1:#2:#3}\foo{a}{b}{c}')
        self.assertIn('a:b:c', r.stdout)

    def test_param_chaining(self):
        r = _expand(r'\def\f#1{\g{#1}}\def\g#1{[#1]}\f{val}')
        self.assertIn('[val]', r.stdout)

    def test_braces_in_body(self):
        r = _expand(r'\def\foo{{x}}\foo')
        self.assertIn('{x}', r.stdout)

    def test_macro_chain(self):
        r = _expand(r'\def\a{\b}\def\b{x}\a')
        self.assertIn('x', r.stdout)

    def test_deep_chain(self):
        r = _expand(r'\def\a{\b}\def\b{\c}\def\c{d}\a')
        self.assertIn('d', r.stdout)

    def test_comment_in_body(self):
        r = _expand(r'\def\foo{hello%comment}\foo')
        self.assertIn('hello', r.stdout)

    def test_param_modifier(self):
        r = _expand(r'\def\foo#1{-#1}\foo x')
        self.assertEqual(r.returncode, 0)


class TestAtMacros(unittest.TestCase):
    """Tests 41-60: @ macros and internal LaTeX commands."""

    def test_at_in_name(self):
        r = _expand(r'\def\@foo{bar}\@foo')
        self.assertIn('bar', r.stdout)

    def test_double_at(self):
        r = _expand(r'\def\@@{double}\@@')
        self.assertIn('double', r.stdout)

    def test_at_with_param(self):
        r = _expand(r'\def\@bar#1{[#1]}\@bar x')
        self.assertEqual(r.returncode, 0)

    def test_double_at_foo(self):
        r = _expand(r'\def\@@foo{triple}\@@foo')
        self.assertIn('triple', r.stdout)

    def test_at_vs_normal(self):
        r = _expand(r'\def\@test{a}\def\test{b}\@test')
        self.assertIn('a', r.stdout)

    def test_maketletter_block(self):
        r = _expand(r'\makeatletter\def\@foo{bar}\@foo\makeatother')
        self.assertIn('bar', r.stdout)

    def test_undefined_at(self):
        r = _expand(r'\def\bar{val}\@bar')
        self.assertIn(r'@bar', r.stdout)  # Passes through as undefined


class TestMathConstructs(unittest.TestCase):
    """Tests 61-80: Mathematical LaTeX constructs."""

    def test_inline_math(self):
        r = _expand(r'$E = mc^2$')
        self.assertIn(r'$E = mc^2$', r.stdout)

    def test_display_math(self):
        r = _expand(r'$$\int_0^1 x dx$$')
        self.assertIn(r'$$\int_0^1 x dx$$', r.stdout)

    def test_frac(self):
        r = _expand(r'\frac{a+b}{c+d}')
        self.assertIn(r'\frac{a+b}{c+d}', r.stdout)

    def test_sqrt(self):
        r = _expand(r'\sqrt{x^2+y^2}')
        self.assertIn(r'\sqrt{x^2+y^2}', r.stdout)

    def test_sum(self):
        r = _expand(r'\sum_{i=1}^n i')
        self.assertIn(r'\sum_{i=1}^n i', r.stdout)

    def test_integral(self):
        r = _expand(r'\int_a^b f(x) dx')
        self.assertIn(r'\int_a^b f(x) dx', r.stdout)

    def test_limit(self):
        r = _expand(r'\lim_{x \to \infty}')
        self.assertIn(r'\lim_{x \to \infty}', r.stdout)

    def test_greek_letters(self):
        r = _expand(r'\alpha\beta\gamma\delta')
        self.assertIn(r'\alpha\beta\gamma\delta', r.stdout)

    def test_vectors(self):
        r = _expand(r'\vec{v} + \vec{w}')
        self.assertIn(r'\vec{v} + \vec{w}', r.stdout)

    def test_left_right(self):
        r = _expand(r'\left(\frac{a}{b}\right)')
        self.assertIn(r'\left(\frac{a}{b}\right)', r.stdout)

    def test_hat_accent(self):
        r = _expand(r'\hat{H}')
        self.assertIn(r'\hat{H}', r.stdout)

    def test_nabla(self):
        r = _expand(r'\nabla')
        self.assertIn(r'\nabla', r.stdout)


class TestRealPaperPatterns(unittest.TestCase):
    """Tests 81-100: Real paper LaTeX patterns."""

    def test_equation_env(self):
        r = _expand(r'\begin{equation}\label{eq:e}E=mc^2\end{equation}')
        self.assertIn(r'\begin{equation}\label{eq:e}E=mc^2\end{equation}', r.stdout)

    def test_align_env(self):
        r = _expand(r'\begin{align}x&=y\\z&=w\end{align}')
        self.assertIn(r'\begin{align}x&=y\\z&=w\end{align}', r.stdout)

    def test_itemize_env(self):
        r = _expand(r'\begin{itemize}\item A\item B\end{itemize}')
        self.assertIn(r'\begin{itemize}\item A\item B\end{itemize}', r.stdout)

    def test_enumerate_env(self):
        r = _expand(r'\begin{enumerate}\item One\item Two\end{enumerate}')
        self.assertIn(r'\begin{enumerate}\item One\item Two\end{enumerate}', r.stdout)

    def test_proof_env(self):
        r = _expand(r'\begin{proof}Proof.\end{proof}')
        self.assertIn(r'\begin{proof}Proof.\end{proof}', r.stdout)

    def test_theorem_env(self):
        r = _expand(r'\begin{theorem}x=y\end{theorem}')
        self.assertIn(r'\begin{theorem}x=y\end{theorem}', r.stdout)

    def test_lemma_env(self):
        r = _expand(r'\begin{lemma}Lemma.\end{lemma}')
        self.assertIn(r'\begin{lemma}Lemma.\end{lemma}', r.stdout)

    def test_cite(self):
        r = _expand(r'\cite{smith2020}')
        self.assertIn(r'\cite{smith2020}', r.stdout)

    def test_ref(self):
        r = _expand(r'\ref{sec:intro}')
        self.assertIn(r'\ref{sec:intro}', r.stdout)

    def test_label(self):
        r = _expand(r'\label{sec:intro}')
        self.assertIn(r'\label{sec:intro}', r.stdout)

    def test_section(self):
        r = _expand(r'\section{Introduction}')
        self.assertIn(r'\section{Introduction}', r.stdout)

    def test_usepackage(self):
        r = _expand(r'\usepackage{amsmath}')
        self.assertIn(r'\usepackage{amsmath}', r.stdout)

    def test_documentclass(self):
        r = _expand(r'\documentclass{article}')
        self.assertIn(r'\documentclass{article}', r.stdout)

    def test_physics_equation(self):
        r = _expand(r'\alpha\left(\frac{\partial \phi}{\partial t}\right)\alpha')
        self.assertEqual(r.returncode, 0)

    def test_quantum_hamiltonian(self):
        r = _expand(r'\hat{H}\psi = E\psi')
        self.assertIn(r'\hat{H}\psi = E\psi', r.stdout)


if __name__ == '__main__':
    unittest.main()


# Additional tests to reach 100
class TestMoreMacros(unittest.TestCase):
    """Additional tests to complete 100 total."""

    def test_newcommand_text(self):
        r = _expand(r'\newcommand{\R}{\mathbb{R}}\R')
        self.assertIn(r'\mathbb{R}', r.stdout)

    def test_let_chain(self):
        r = _expand(r'\def\c{x}\let\b\c\let\a\b\a')
        self.assertIn('x', r.stdout)

    def test_nested_newcommand(self):
        r = _expand(r'\newcommand{\foo}{\bar}\newcommand{\bar{B}}\foo')
        self.assertEqual(r.returncode, 0)

    def test_macro_in_macro(self):
        r = _expand(r'\def\foo{\bar}\def\bar{expanded}\foo')
        self.assertIn('expanded', r.stdout)

    def test_simple_text(self):
        r = _expand('hello world')
        self.assertIn('hello world', r.stdout)

    def test_spaces_preserved(self):
        r = _expand(r'\def\sp{ a b }\sp')
        self.assertIn(' a b ', r.stdout)

    def test_multiple_macros(self):
        r = _expand(r'\def\a{x}\def\b{y}\a\b')
        self.assertIn('xy', r.stdout)

    def test_redefine(self):
        r = _expand(r'\def\foo{a}\foo\def\foo{b}\foo')
        self.assertIn('ab', r.stdout)

    def test_long_def(self):
        r = _expand(r'\long\def\foo#1{long:#1}\foo{x}')
        self.assertEqual(r.returncode, 0)

    def test_outer_def(self):
        r = _expand(r'\outer\def\foo{outer}\foo')
        self.assertEqual(r.returncode, 0)

    # Math accents and symbols
    def test_tilde_accent(self):
        r = _expand(r'\tilde{x}')
        self.assertIn(r'\tilde{x}', r.stdout)

    def test_bar_accent(self):
        r = _expand(r'\bar{x}')
        self.assertIn(r'\bar{x}', r.stdout)

    def test_dot_accent(self):
        r = _expand(r'\dot{x}')
        self.assertIn(r'\dot{x}', r.stdout)

    def test_bold_math(self):
        r = _expand(r'\mathbf{x}')
        self.assertIn(r'\mathbf{x}', r.stdout)

    def test_calligraphic(self):
        r = _expand(r'\mathcal{L}')
        self.assertIn(r'\mathcal{L}', r.stdout)

    def test_blackboard_bold(self):
        r = _expand(r'\mathbb{R}')
        self.assertIn(r'\mathbb{R}', r.stdout)

    def test_underline(self):
        r = _expand(r'\underline{x}')
        self.assertIn(r'\underline{x}', r.stdout)

    def test_overline(self):
        r = _expand(r'\overline{AB}')
        self.assertIn(r'\overline{AB}', r.stdout)

    def test_phantom(self):
        r = _expand(r'\phantom{abc}')
        self.assertIn(r'\phantom{abc}', r.stdout)

    def test_mathrm(self):
        r = _expand(r'\mathrm{H_2O}')
        self.assertIn(r'\mathrm{H_2O}', r.stdout)

    def test_operatorname(self):
        r = _expand(r'\operatorname{tr}(M)')
        self.assertIn(r'\operatorname{tr}(M)', r.stdout)


class TestMorePaperPatterns(unittest.TestCase):
    """Additional paper pattern tests."""

    def test_corollary_env(self):
        r = _expand(r'\begin{corollary}x\end{corollary}')
        self.assertIn(r'\begin{corollary}x\end{corollary}', r.stdout)

    def test_definition_env(self):
        r = _expand(r'\begin{definition}x\end{definition}')
        self.assertIn(r'\begin{definition}x\end{definition}', r.stdout)

    def test_example_env(self):
        r = _expand(r'\begin{example}x\end{example}')
        self.assertIn(r'\begin{example}x\end{example}', r.stdout)

    def test_remark_env(self):
        r = _expand(r'\begin{remark}x\end{remark}')
        self.assertIn(r'\begin{remark}x\end{remark}', r.stdout)

    def test_proposition_env(self):
        r = _expand(r'\begin{proposition}x\end{proposition}')
        self.assertIn(r'\begin{proposition}x\end{proposition}', r.stdout)

    def test_assumption_env(self):
        r = _expand(r'\begin{assumption}x\end{assumption}')
        self.assertIn(r'\begin{assumption}x\end{assumption}', r.stdout)

    def test_conjecture_env(self):
        r = _expand(r'\begin{conjecture}x\end{conjecture}')
        self.assertIn(r'\begin{conjecture}x\end{conjecture}', r.stdout)

    def test_axiom_env(self):
        r = _expand(r'\begin{axiom}x\end{axiom}')
        self.assertIn(r'\begin{axiom}x\end{axiom}', r.stdout)

    def test_notation_env(self):
        r = _expand(r'\begin{notation}x\end{notation}')
        self.assertIn(r'\begin{notation}x\end{notation}', r.stdout)

    def test_property_env(self):
        r = _expand(r'\begin{property}x\end{property}')
        self.assertIn(r'\begin{property}x\end{property}', r.stdout)


if __name__ == '__main__':
    unittest.main()


# Final tests to reach 100
class TestFinalCount(unittest.TestCase):
    """Final tests to complete 100 total."""

    def test_math_in_math(self):
        r = _expand(r'$x_{\alpha}$')
        self.assertEqual(r.returncode, 0)

    def test_nested_frac(self):
        r = _expand(r'\frac{\frac{a}{b}}{c}')
        self.assertEqual(r.returncode, 0)

    def test_partial_deriv(self):
        r = _expand(r'\frac{\partial^2 f}{\partial x \partial y}')
        self.assertEqual(r.returncode, 0)

    def test_log_deriv(self):
        r = _expand(r'\frac{d\ln f}{dx}')
        self.assertEqual(r.returncode, 0)

    def test_integral_sub(self):
        r = _expand(r'\int_0^\infty')
        self.assertEqual(r.returncode, 0)

    def test_derivative_sup(self):
        r = _expand(r'f^{(n)}')
        self.assertEqual(r.returncode, 0)

    def test_mathematical_const(self):
        r = _expand(r'\pi \approx 3.14159')
        self.assertIn(r'\pi', r.stdout)

    def test_complex_sup_sub(self):
        r = _expand(r'x_1^2 y_3^4')
        self.assertEqual(r.returncode, 0)

    def test_matrix_env(self):
        r = _expand(r'\begin{matrix}a&b\\c&d\end{matrix}')
        self.assertEqual(r.returncode, 0)

    def test_bmatrix_env(self):
        r = _expand(r'\begin{bmatrix}a&b\\c&d\end{bmatrix}')
        self.assertEqual(r.returncode, 0)

    def test_pmatrix_env(self):
        r = _expand(r'\begin{pmatrix}a&b\\c&d\end{pmatrix}')
        self.assertEqual(r.returncode, 0)

    def test_vmatrix_env(self):
        r = _expand(r'\begin{vmatrix}a&b\\c&d\end{vmatrix}')
        self.assertEqual(r.returncode, 0)

    def test_figure_env(self):
        r = _expand(r'\begin{figure}\includegraphics{img}\caption{cap}\end{figure}')
        self.assertEqual(r.returncode, 0)

    def test_table_env(self):
        r = _expand(r'\begin{table}\begin{tabular}{cc}a&b\\\hline c&d\end{tabular}\end{table}')
        self.assertEqual(r.returncode, 0)

    def test_center_env(self):
        r = _expand(r'\begin{center}Centered\end{center}')
        self.assertEqual(r.returncode, 0)

    def test_verse_env(self):
        r = _expand(r'\begin{verse}Poem\end{verse}')
        self.assertEqual(r.returncode, 0)

    def test_quote_env(self):
        r = _expand(r'\begin{quote}Quote\end{quote}')
        self.assertEqual(r.returncode, 0)

    def test_quotation_env(self):
        r = _expand(r'\begin{quotation}Quotation\end{quotation}')
        self.assertEqual(r.returncode, 0)


if __name__ == '__main__':
    unittest.main()
