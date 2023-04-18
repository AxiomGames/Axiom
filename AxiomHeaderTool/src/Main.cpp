#include <iostream>
#include <filesystem>
#include <ostream>
#include <sstream>
#include <fstream>
#include <stack>

#include "Lexer.hpp"
#include "ReflectionTypes.hpp"
#include "TypeGenerators.hpp"

using Path = std::filesystem::path;

std::string ReadFileToString(const Path& path)
{
	std::string lines;

	std::ifstream stream(path.string());
	std::string line;

	if (stream.is_open())
	{
		while (std::getline(stream, line))
		{
			lines += line;
			//lines += "\n";
		}

		stream.close();
		return lines;
	}

	return lines;
}

void ParseSourceCode(const Path& path);

int main(int argc, const char** argv)
{
	for (int i = 0; i < argc; ++i)
	{
		//std::cout << argv[i] << std::endl;

		if (std::string(argv[i]) == "--root_dir")
		{
			ParseSourceCode(argv[i + 1]);
		}
	}

	Enum e;
	e.Name = "TestEnum";
	e.FullNamespaceName = "TestEnum";
	e.Values.push_back({"One", "1", 0});

	std::string enumTestString;
	enumTestString += "HPP FILE: \n";
	GenerateEnum_hpp(enumTestString, e);
	enumTestString += "CPP FILE: \n";
	GenerateEnum_cpp(enumTestString, e);
	std::cout << enumTestString;

	//std::cout << std::filesystem::current_path().string().c_str() << std::endl;
	return 0;
}

void GenerateHeaderCode(const Path& path);

void ParseSourceCode(const Path& path)
{
	std::cout << path << std::endl;

	for (const auto& it: std::filesystem::recursive_directory_iterator(path))
	{
		const Path& sourceFilePath = it.path();
		Path sourceFileExtension = sourceFilePath.extension();

		if (sourceFileExtension == ".hpp" || sourceFileExtension == ".h")
		{
			if (sourceFilePath.stem() == "Material")
			{
				GenerateHeaderCode(sourceFilePath);
				break;
			}
		}
	}
}

std::ostream& operator<<(std::ostream& os, const Token::Kind& kind)
{
	static const char* const names[]{
		"Number", "Identifier", "LeftParen", "RightParen", "LeftSquare",
		"RightSquare", "LeftCurly", "RightCurly", "LessThan", "GreaterThan",
		"Equal", "Plus", "Minus", "Asterisk", "Slash",
		"Hash", "Dot", "Comma", "Colon", "Semicolon",
		"SingleQuote", "DoubleQuote", "Comment", "Pipe", "End",
		"Unexpected",
	};
	return os << names[static_cast<int>(kind)];
}

struct LexerPattern
{
	Token::Kind Type;
	const char* TypeValue;

	constexpr LexerPattern(Token::Kind type, const char* typeValue)
		: Type(type), TypeValue(typeValue) {}
};

static constexpr LexerPattern EnumPattern(Token::Kind::Identifier, "AX_ENUM");
static constexpr LexerPattern StructPattern(Token::Kind::Identifier, "AX_STRUCT");
static constexpr LexerPattern ClassPattern(Token::Kind::Identifier, "AX_CLASS");

static const char* const TypeNames[] {
	"namespace",
	"enum",
	"struct",
	"class",
	"union"
};

enum class Types
{
	Namespace,
	Enum,
	Struct,
	Class,
	Union,
	
	Empty
};

struct Scope
{
	Types Type = Types::Empty;
	std::string TypeName;
};

void GenerateHeaderCode(const Path& path)
{
	std::string code = ReadFileToString(path);

	Lexer lex(code.c_str());
	std::vector<std::pair<Token::Kind, std::string_view>> tokens;

	for (auto token = lex.next(); not token.is_one_of(Token::Kind::End, Token::Kind::Unexpected); token = lex.next())
	{
		tokens.emplace_back(token.kind(), token.lexeme());
	}

	std::stack<Scope> stackScope;
	Scope* currentScope = nullptr;

	Types currentType = Types::Empty;
	std::string currentTypeName;

	for (const auto& [kind, lexedValue] : tokens)
	{
		switch (kind)
		{
			case Token::Kind::Identifier:
			{
				// Read current type and store it
				currentType = Types::Empty;
				currentTypeName.clear();

				for (int i = 0; i < _countof(TypeNames); ++i)
				{
					if (lexedValue == TypeNames[i])
					{
						currentType = (Types) i;
						currentTypeName = TypeNames[i];
						break;
					}
				}

				// check if it is something to reflect



				break;
			}

			case Token::Kind::LeftCurly:
			{
				currentScope = &stackScope.emplace(currentType, currentTypeName);
				break;
			}
			case Token::Kind::RightCurly:
			{
				stackScope.pop();

				if (stackScope.empty() == false)
				{
					currentScope = &stackScope.top();
				}
				break;
			}

			default: break;
		}

		std::cout << std::setw(12) << kind << " |" << lexedValue << "|\n";
	}
}