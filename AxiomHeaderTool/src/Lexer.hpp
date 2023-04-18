#pragma once

// A simple Lexer meant to demonstrate a few theoretical concepts. It can
// support several parser concepts and is very fast (though speed is not its
// design goal).
//
// J. Arrieta, Nabla Zero Labs
//
// This code is released under the MIT License.
//
// Copyright 2018 Nabla Zero Labs
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish ,distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <string>

class Token {
public:
	enum class Kind {
		Number,
		Identifier,
		LeftParen,
		RightParen,
		LeftSquare,
		RightSquare,
		LeftCurly,
		RightCurly,
		LessThan,
		GreaterThan,
		Equal,
		Plus,
		Minus,
		Asterisk,
		Slash,
		Hash,
		Dot,
		Comma,
		Colon,
		Semicolon,
		SingleQuote,
		DoubleQuote,
		Comment,
		Pipe,
		End,
		Unexpected,
	};

	Token(Kind kind) noexcept : m_kind{kind} {}

	Token(Kind kind, const char* beg, std::size_t len) noexcept
		: m_kind{kind}, m_lexeme(beg, len) {}

	Token(Kind kind, const char* beg, const char* end) noexcept
		: m_kind{kind}, m_lexeme(beg, std::distance(beg, end)) {}

	Kind kind() const noexcept { return m_kind; }

	void kind(Kind kind) noexcept { m_kind = kind; }

	bool is(Kind kind) const noexcept { return m_kind == kind; }

	bool is_not(Kind kind) const noexcept { return m_kind != kind; }

	bool is_one_of(Kind k1, Kind k2) const noexcept { return is(k1) || is(k2); }

	template <typename... Ts>
	bool is_one_of(Kind k1, Kind k2, Ts... ks) const noexcept {
		return is(k1) || is_one_of(k2, ks...);
	}

	std::string_view lexeme() const noexcept { return m_lexeme; }

	void lexeme(std::string_view lexeme) noexcept {
		m_lexeme = std::move(lexeme);
	}

private:
	Kind             m_kind{};
	std::string_view m_lexeme{};
};

class Lexer {
public:
	Lexer(const char* beg) noexcept : m_beg{beg} {}

	Token next() noexcept;

private:
	Token identifier() noexcept;
	Token number() noexcept;
	Token slash_or_comment() noexcept;
	Token atom(Token::Kind) noexcept;

	char peek() const noexcept { return *m_beg; }
	char get() noexcept { return *m_beg++; }

	const char* m_beg = nullptr;
};
