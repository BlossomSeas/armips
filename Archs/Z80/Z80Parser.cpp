#include "Archs/Z80/CZ80Instruction.h"
#include "Archs/Z80/Z80.h"
#include "Archs/Z80/Z80Opcodes.h"
#include "Archs/Z80/Z80Parser.h"
#include "Core/Expression.h"
#include "Parser/DirectivesParser.h"
#include "Parser/Tokenizer.h"

#define CHECK(exp) if (!(exp)) return false;

const Z80RegisterDescriptor Z80Regs8[] = {
	{ L"b", Z80_REG8_B }, { L"c", Z80_REG8_C },
	{ L"d", Z80_REG8_D }, { L"e", Z80_REG8_E },
	{ L"h", Z80_REG8_H }, { L"l", Z80_REG8_L },
	{ L"a", Z80_REG8_A },
};

const Z80RegisterDescriptor Z80Regs16SP[] = {
	{ L"bc", Z80_REG16_BC }, { L"de", Z80_REG16_DE },
	{ L"hl", Z80_REG16_HL }, { L"sp", Z80_REG16_SP },
};

const Z80RegisterDescriptor Z80Regs16AF[] = { // kinda hacky
	{ L"bc", Z80_REG16_BC }, { L"de", Z80_REG16_DE },
	{ L"hl", Z80_REG16_HL }, { L"af", Z80_REG16_AF },
};

const Z80RegisterDescriptor Z80HLIncDec16[] = {
	{ L"hli", 0 }, { L"hld", 1 },
};

const Z80RegisterDescriptor Z80Conds[] = {
	{ L"nz", Z80_COND_NZ }, { L"z", Z80_COND_Z },
	{ L"nc", Z80_COND_NC }, { L"c", Z80_COND_C },
};

const DirectiveMap Z80Directives = { };

std::unique_ptr<CAssemblerCommand> Z80Parser::parseDirective(Parser& parser)
{
	return parser.parseDirective(Z80Directives);
}

bool Z80Parser::parseRegisterTable(Parser& parser, Z80RegisterValue& dest, const Z80RegisterDescriptor* table, size_t count, int allowed)
{
	const Token& token = parser.peekToken();
	CHECK(token.type == TokenType::Identifier);

	const std::wstring stringValue = token.getStringValue();
	for (size_t i = 0; i < count; i++)
	{
		if (allowed & (1 << table[i].num) && stringValue == table[i].name)
		{
			dest.name = stringValue;
			dest.num = table[i].num;
			parser.eatToken();
			return true;
		}
	}

	return false;
}

bool Z80Parser::parseRegister8(Parser& parser, Z80RegisterValue& dest, int allowed)
{
	return parseRegisterTable(parser, dest, Z80Regs8, std::size(Z80Regs8), allowed);
}

bool Z80Parser::parseRegister16SP(Parser& parser, Z80RegisterValue& dest, int allowed)
{
	return parseRegisterTable(parser, dest, Z80Regs16SP, std::size(Z80Regs16SP), allowed);
}

bool Z80Parser::parseRegister16AF(Parser& parser, Z80RegisterValue& dest, int allowed)
{
	return parseRegisterTable(parser, dest, Z80Regs16AF, std::size(Z80Regs16AF), allowed);
}

bool Z80Parser::parseCondition(Parser& parser, Z80RegisterValue& dest)
{
	return parseRegisterTable(parser, dest, Z80Conds, std::size(Z80Conds), Z80_REG_BIT_ALL);
}

bool Z80Parser::parseHLIncDec(Parser& parser, Z80RegisterValue& dest)
{
	CHECK(parser.matchToken(TokenType::LParen));

	// hli / hld
	if (!parseRegisterTable(parser, dest, Z80HLIncDec16, std::size(Z80HLIncDec16), Z80_REG_BIT_ALL))
	{
		// hl+ / hl-
		CHECK(parseRegister16SP(parser, dest, Z80_REG_BIT(Z80_REG16_HL)));

		const Token& token = parser.nextToken();
		if (token.type == TokenType::Plus)
		{
			dest.name = L"hl+";
			dest.num = 0;
		}
		else if (token.type == TokenType::Minus)
		{
			dest.name = L"hl-";
			dest.num = 1;
		}
		else
		{
			return false;
		}

	}

	CHECK(parser.matchToken(TokenType::RParen));

	return true;
}

bool Z80Parser::parseMemoryRegister16(Parser& parser, Z80RegisterValue& dest, int allowed)
{
	CHECK(parser.matchToken(TokenType::LParen));
	CHECK(parseRegister16SP(parser, dest, allowed));
	CHECK(parser.matchToken(TokenType::RParen));

	return true;
}

bool Z80Parser::parseMemoryImmediate(Parser& parser, Expression& dest)
{
	CHECK(parser.matchToken(TokenType::LParen));
	dest = parser.parseExpression();
	CHECK(dest.isLoaded());
	CHECK(parser.matchToken(TokenType::RParen));

	return true;
}

bool Z80Parser::parseFF00PlusC(Parser& parser)
{
	CHECK(parser.matchToken(TokenType::LParen));

	// 0xFF00+ optional
	const Token& token = parser.peekToken();
	if (token.type == TokenType::Integer)
	{
		CHECK(token.intValue == 0xFF00);
		parser.eatToken();

		CHECK(parser.matchToken(TokenType::Plus));
	}

	Z80RegisterValue tempReg;
	CHECK(parseRegister8(parser, tempReg, Z80_REG_BIT(Z80_REG8_C)));

	CHECK(parser.matchToken(TokenType::RParen));

	return true;
}

bool Z80Parser::parseSPImmediate(Parser& parser, Expression& dest, bool& isNegative)
{
	isNegative = false;

	Z80RegisterValue tempReg;
	CHECK(parseRegister16SP(parser, tempReg, Z80_REG_BIT(Z80_REG16_SP)));

	const Token& token = parser.peekToken();
	if (token.type != TokenType::Plus && token.type != TokenType::Minus)
	{
		// Treat as +0
		dest = createConstExpression(0);
		return true;
	}
	parser.eatToken();
	isNegative = token.type == TokenType::Minus;

	dest = parser.parseExpression();
	CHECK(dest.isLoaded());

	return true;
}

bool Z80Parser::parseOpcodeParameter(Parser& parser, unsigned char paramType, Z80RegisterValue& destReg, Expression& destImm, bool& isNegative)
{
	switch (paramType)
	{
	case Z80_PARAM_REG8_MEMHL:
		if (parseRegister8(parser, destReg, Z80_REG8_BIT_ALL))
		{
			return true;
		}
		if (parseMemoryRegister16(parser, destReg, Z80_REG_BIT(Z80_REG16_HL)))
		{
			destReg.num = Z80_REG8_MEMHL;
			return true;
		}
		return false;
	case Z80_PARAM_REG16_SP:
		return parseRegister16SP(parser, destReg, Z80_REG16_BIT_ALL);
	case Z80_PARAM_REG16_AF:
		return parseRegister16AF(parser, destReg, Z80_REG16_BIT_ALL);
	case Z80_PARAM_A:
		return parseRegister8(parser, destReg, Z80_REG_BIT(Z80_REG8_A));
	case Z80_PARAM_MEMBC_MEMDE:
		return parseMemoryRegister16(parser, destReg, Z80_REG_BIT(Z80_REG16_BC) | Z80_REG_BIT(Z80_REG16_DE));
	case Z80_PARAM_HL:
		return parseRegister16SP(parser, destReg, Z80_REG_BIT(Z80_REG16_HL));
	case Z80_PARAM_MEMHL:
		return parseMemoryRegister16(parser, destReg, Z80_REG_BIT(Z80_REG16_HL));
	case Z80_PARAM_HLI_HLD:
		return parseHLIncDec(parser, destReg);
	case Z80_PARAM_SP:
		return parseRegister16SP(parser, destReg, Z80_REG_BIT(Z80_REG16_SP));
	case Z80_PARAM_IMMEDIATE:
		destImm = parser.parseExpression();
		return destImm.isLoaded();
	case Z80_PARAM_MEMIMMEDIATE:
		return parseMemoryImmediate(parser, destImm);
	case Z80_PARAM_FF00_C:
		return parseFF00PlusC(parser);
	case Z80_PARAM_SP_IMM:
		return parseSPImmediate(parser, destImm, isNegative);
	case Z80_PARAM_CONDITION:
		return parseCondition(parser, destReg);
	default:
		return false;
	}
}

bool Z80Parser::parseOpcodeParameterList(Parser& parser, const tZ80Opcode opcode, Z80OpcodeVariables& vars)
{
	bool isNegative = false;
	if (opcode.lhs)
	{
		CHECK(parseOpcodeParameter(parser, opcode.lhs, vars.LeftParam, vars.ImmediateExpression, isNegative));
	}
	if (opcode.rhs)
	{
		CHECK(parser.matchToken(TokenType::Comma));
		CHECK(parseOpcodeParameter(parser, opcode.rhs, vars.RightParam, vars.ImmediateExpression, isNegative));
	}
	vars.IsNegative = isNegative;

	return true;
}

std::unique_ptr<CZ80Instruction> Z80Parser::parseOpcode(Parser& parser)
{
	if (parser.peekToken().type != TokenType::Identifier)
		return nullptr;

	const Token& token = parser.nextToken();

	Z80OpcodeVariables vars;
	bool paramFail = false;

	const std::wstring stringValue = token.getStringValue();
	for (int z = 0; Z80Opcodes[z].name != nullptr; z++)
	{
		if ((Z80Opcodes[z].flags & Z80_GAMEBOY) && Z80.GetVersion() != ZARCH_GAMEBOY)
			continue;

		if (stringValue == Z80Opcodes[z].name)
		{
			TokenizerPosition tokenPos = parser.getTokenizer()->getPosition();

			if (parseOpcodeParameterList(parser, Z80Opcodes[z], vars))
			{
				// success, return opcode
				return std::make_unique<CZ80Instruction>(Z80Opcodes[z], vars);
			}

			parser.getTokenizer()->setPosition(tokenPos);
			paramFail = true;
		}
	}

	if (paramFail)
		parser.printError(token, L"Z80 parameter failure in %S", stringValue);
	else
		parser.printError(token, L"Invalid Z80 opcode: %S", stringValue);

	return nullptr;
}