#pragma once


#include <libsolidity/ast/ASTForward.h>
#include <libsolidity/parsing/Token.h>
#include <libsolidity/ast/Types.h>
#include <libsolidity/ast/ASTAnnotations.h>
#include <libsolidity/ast/ASTEnums.h>

#include <libevmasm/SourceLocation.h>
#include <libevmasm/Instruction.h>

#include <libdevcore/FixedHash.h>
#include <json/json.h>

#include <boost/noncopyable.hpp>

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace dev
{
namespace solidity
{

/// Forward declaration.
class CompilerContext;
class CompilerUtils;

class ENIHandler {
private:
	enum ENIParameterType : uint8_t {
		kLiteral = 0,
		kIdentifier = 1
	};

	struct LiteralInfo {
		Token m_Type;
		std::string m_Value;

		LiteralInfo() : m_Type(), m_Value() {}
		explicit LiteralInfo(Token pType, std::string pValue)
			: m_Type(pType), m_Value(pValue) {}
	};

	struct IdentifierInfo {
		TypePointer m_Type;
		Declaration const* m_Declaration;
		Expression const* m_Expression;

		IdentifierInfo() : m_Type(), m_Declaration(nullptr), m_Expression(nullptr) {}
		explicit IdentifierInfo(TypePointer pType, Declaration const* pDeclaration, Expression const* pExpression)
			: m_Type(pType), m_Declaration(pDeclaration), m_Expression(pExpression) {}
	};

	struct ENIParameter {
		ENIParameterType m_Type;
		LiteralInfo m_LiteralInfo;
		IdentifierInfo m_IdentifierInfo;
	};

public:
	ENIHandler() :
		m_TypeSection(),
		m_ReturnSection(),
		m_FunctionName(),
		m_Parameters(),
		m_Context(nullptr) {}

	virtual ~ENIHandler() {}

	/// Appends ENI function parameters
	void appendLiteral(Token, std::string);
	void appendIdentifier(TypePointer, Declaration const&, Expression const&);

	/// Clear all ENI function parameters
	void clearENIObjects();

	/// Set current CompilerContext.
	void setContext(CompilerContext* pContext) { m_Context = pContext; }

	/// Pack ENI parameters to memory.
	void packedToMemory();

	/// Get CompilerUtils from current CompilerContext.
	CompilerUtils utils();

private:
	/// Initialize size of data section
	/// stack pre: typeSectionOffset
	/// stack post: typeSectionOffset dataSectionOffset
	void initSizeOfDataSection();
	/// Calculate and store size of data section
	/// stack pre: typeSectionOffset dataSectionOffset
	/// stack post: dataSectionOffset typeSectionOffset
	void storeSizeOfDataSection();
	/// Calculate and store size of type section
	/// stack pre: typeSectionOffset
	/// stack post: typeSectionOffset
	void storeSizeOfTypeSection();
	/// Calculate and store size of return section
	/// stack pre: typeSectionOffset
	/// stack post: typeSectionOffset
	void storeSizeOfReturnSection();
	/// Store type section into memory
	/// stack pre: typeSectionOffset
	/// stack post: typeSectionOffset
	void storeTypeSection();
	/// Store return section into memory
	/// stack pre: typeSectionOffset
	/// stack post: typeSectionOffset
	void storeReturnSection();
	/// Handle literal type of parameters
	void handleLiteral(LiteralInfo&);
	/// Handle identifer type of parameters
	void handleIdentifier(IdentifierInfo&);

private:
	bytes m_TypeSection;
	bytes m_ReturnSection;
	/// Store current ENI function name.
	std::string m_FunctionName;
	/// Store literals and identifiers of ENI function parameters.
	std::vector<ENIParameter> m_Parameters;
	/// Use CompilerContext to access identifiers.
	CompilerContext* m_Context;
};

}
}
