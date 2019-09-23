#include <libsolidity/codegen/ENIHandler.h>
#include <libsolidity/codegen/CompilerUtils.h>
#include <libsolidity/codegen/LValue.h>
#include <libsolidity/ast/ASTForward.h>
#include <libsolidity/parsing/Token.h>
#include <libsolidity/ast/AST.h>
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

//===----------------------------------------------------------------------===//
// Helper functions
//===----------------------------------------------------------------------===//

/// Convert std::string to bytes.
bytes toBytes(std::string pStr) {
	bytes ret;
	for (auto ch : pStr) {
		ret.push_back(ch);
	}
	return ret;
}

/// Convert bytes to std::string.
std::string toString(bytes& pBytes) {
	std::string ret("");
	for (auto b : pBytes) {
		ret.push_back((char)(b+'0'));
	}
	return ret;
}

/// Convert the type of literal to byte representation. This will produce ENI type section.
uint8_t literalTypeToString(Token pType) {
	switch (pType) {
		case Token::StringLiteral:
			return 4;
		case Token::Number:
			return 9;
		case Token::TrueLiteral:
		case Token::FalseLiteral:
			return 0;
		default:
			return 0xff;
	}
}

/// Convert the type of identifier to byte representation. This will produce ENI type section.
uint8_t identifierTypeToString(TypePointer pType) {
	if (pType->category() == Type::Category::Array) {
		auto arrayType = dynamic_cast<ArrayType const*>(pType.get());
		if (arrayType->isString())
			return 0x6B;
		else if (arrayType->isByteArray())
			solUnimplementedAssert(false, "Unsupported type of bytes array");
		else
			solAssert(false, "Unsupported type of unknown type of array");
	}
	switch (pType->category()) {
		case Type::Category::Integer:
			return 9;
		case Type::Category::RationalNumber:
			return 9;
		case Type::Category::StringLiteral:
			return 4;
		case Type::Category::Bool:
			return 0;
		default:
			solUnimplementedAssert(false, "Unsupported unknown type");
			return 0xff;
			break;
	}
}

CompilerUtils ENIHandler::utils() {
	return CompilerUtils(*m_Context);
}

void ENIHandler::storeSizeOfTypeSection() {
	/// Allocate memory
	/// stack pre: typeSectionOffset
	*m_Context << u256(0x20);
	utils().allocateMemory();
	/// stack post: typeSectionOffset <mem_start>
	/// stack pre: typeSectionOffset <mem_start> length
	*m_Context << u256(m_TypeSection.size()) << Instruction::SWAP1 << Instruction::MSTORE;
	/// stack post: typeSectionOffset
}

void ENIHandler::storeTypeSection() {
	/// Allocate memory
	/// stack pre: typeSectionOffset
	*m_Context << u256(0x20);
	utils().allocateMemory();
	/// stack post: typeSectionOffset <mem_start> typeSection
	*m_Context << h256::Arith(h256(m_TypeSection, h256::AlignLeft)) << Instruction::SWAP1 << Instruction::MSTORE;
	/// stack post: typeSectionOffset
}

void ENIHandler::storeSizeOfDataSection() {
	/// Calculate size of data section
	/// stack: typeSectionOffset dataSectionOffset
	*m_Context << Instruction::DUP1;
	/// stack: typeSectionOffset dataSectionOffset dataSectionOffset
	utils().fetchFreeMemoryPointer();
	/// stack: typeSectionOffset dataSectionOffset dataSectionOffset curFreeMemoryPtr
	*m_Context << Instruction::DUP2 << Instruction::SWAP1 << Instruction::SUB;
	/// stack: typeSectionOffset dataSectionOffset dataSectionOffset (freeMemoryPtr-dataSectionOffset)
	*m_Context << u256(0x20) << Instruction::SWAP1 << Instruction::SUB;
	/// stack: typeSectionOffset dataSectionOffset dataSectionOffset dataSectionSize
	*m_Context << Instruction::SWAP1 << Instruction::MSTORE;
	/// stack: typeSectionOffset dataSectionOffset
	*m_Context << Instruction::SWAP1;
	/// stack post: dataSectionOffset typeSectionOffset
}

void ENIHandler::storeSizeOfReturnSection() {
	/// Allocate memory
	/// stack pre: typeSectionOffset
	*m_Context << u256(0x20);
	utils().allocateMemory();
	/// stack post: typeSectionOffset <mem_start>
	/// stack pre: typeSectionOffset <mem_start> length
	*m_Context << u256(m_ReturnSection.size()) << Instruction::SWAP1 << Instruction::MSTORE;
	/// stack post: typeSectionOffset
}

void ENIHandler::storeReturnSection() {
	/// Allocate memory
	/// stack pre: typeSectionOffset
	*m_Context << u256(0x20);
	utils().allocateMemory();
	/// stack post: typeSectionOffset <mem_start> returnSection
	*m_Context << h256::Arith(h256(m_ReturnSection, h256::AlignLeft)) << Instruction::SWAP1 << Instruction::MSTORE;
	/// stack post: typeSectionOffset
}

void ENIHandler::initSizeOfDataSection() {
	/// Allocate memory
	/// stack pre: typeSectionOffset
	*m_Context << u256(0x20);
	utils().allocateMemory();
	*m_Context << Instruction::DUP1;
	/// stack post: typeSectionOffset dataSectionOffset dataSectionOffset 0
	*m_Context << u256(0x0) << Instruction::SWAP1 << Instruction::MSTORE;
	/// stack post: typeSectionOffset dataSectionOffset
}

void ENIHandler::handleIdentifierPointer(TypePointer pType) {
	if (pType->category() == Type::Category::Array) {
		auto arrayType = dynamic_cast<ArrayType const*>(pType.get());
		if (arrayType->isString()) {
			/// stack: <stringOffset>
			*m_Context << u256(0x20);
			/// stack: <stringOffset> <pointerSize>
			utils().allocateMemory();
			/// stack: <stringOffset> <eniPointerOffset>
			*m_Context << Instruction::MSTORE;
		} else if (arrayType->isByteArray())
			solUnimplementedAssert(false, "Unsupported type of bytes array identifier");
		else
			solAssert(false, "Unsupported type of unknown type array");
	} else {
		switch (pType->category()) {
			case Type::Category::Integer:
				*m_Context << u256(0x20);
				utils().allocateMemory();
				*m_Context << Instruction::MSTORE;
				break;
			case Type::Category::RationalNumber:
				solUnimplementedAssert(false, "Unsupported type of RationalNumber identifier");
				break;
			case Type::Category::Bool:
				solUnimplementedAssert(false, "Unsupported type of Boolean identifier");
				break;
			default:
				solAssert(false, "Unsupported type of unknown type of identifier");
				break;
		}
	}
}


void ENIHandler::handleLiteralPointer(Token token, std::string value) {
	switch (token) {
		case Token::StringLiteral:
			{
				/// Store Literal String
				/// Allocate memory
				/// stack pre: typeSectionOffset dataSectionOffset
				bytesConstRef data(value);
				u256 storageSize(32 + ((data.size() + 31) / 32) * 32);
				*m_Context << storageSize;
				utils().allocateMemory();
				// stack pre: typeSectionOffset dataSectionOffset mem_start
				*m_Context << Instruction::DUP1 << u256(data.size());
				// stack pre: typeSectionOffset dataSectionOffset mem_start mem_start data_len
				*m_Context << Instruction::SWAP1 << Instruction::MSTORE;
				// stack post: typeSectionOffset dataSectionOffset mem_start
				*m_Context << u256(0x20) << Instruction::ADD;
				// stack: typeSectionOffset dataSectionOffset mem_next
				for (unsigned i = 0; i < data.size(); i += 32)
				{
					// stack: typeSectionOffset dataSectionOffset mem_cur
					*m_Context << h256::Arith(h256(data.cropped(i), h256::AlignLeft));
					// stack: typeSectionOffset dataSectionOffset mem_cur data
					*m_Context << Instruction::DUP2 << Instruction::MSTORE;
					// stack: typeSectionOffset dataSectionOffset mem_cur
					*m_Context << u256(0x20) << Instruction::ADD;
					// stack: typeSectionOffset dataSectionOffset mem_next
				}
				*m_Context << Instruction::POP;
				// stack: typeSectionOffset dataSectionOffset
			}
			break;
		case Token::Number:
			/// Store Literal Number
			/// Allocate memory
			/// stack pre: typeSectionOffset dataSectionOffset
			*m_Context << u256(0x20);
			/// stack pre: typeSectionOffset dataSectionOffset 32bytes
			utils().allocateMemory();
			/// stack post: typeSectionOffset dataSectionOffset <mem_start>
			*m_Context << u256(value);
			/// stack pre: typeSectionOffset dataSectionOffset <mem_start> literal_number
			*m_Context << Instruction::SWAP1 << Instruction::MSTORE;
			/// stack post: typeSectionOffset dataSectionOffset
			break;
		default:
			solAssert(false, "Unsupported type of unknown data type");
			break;
	}
}

void  ENIHandler::appendTypePointer(TypePointer pType)
{
	// Unused member.
	m_ParameterTypes.push_back(pType);

	// Make type pointer to decide how to deal with data, so it is not required to judge if it is Literal.
	m_TypeSection.push_back(identifierTypeToString(pType));
}

void ENIHandler::packedToMemoryPrepare() {
	/// Memory layout
	/// 32 bytes       | length of type section
	/// dynamic length | bytes of type section
	/// 32 bytes       | length of return section
	/// dynamic length | bytes of return section
	/// 32 bytes       | length of data section
	/// dynamic length | data section
	//
	/// After ENI instruction is done, the following memory will be:
	/// 32 bytes       | length of return section
	/// dynamic length | return section


	/// Return type of string memory
	m_ReturnSection.push_back((char)4);

	/// Handle type section
	//
	/// Copy freeMemoryPtr
	/// stack pre:
	utils().fetchFreeMemoryPointer();
	/// stack post: freeMemoryPtr

	/// Calculate and store size of type section
	/// stack pre: typeSectionOffset
	/// stack post: typeSectionOffset
	storeSizeOfTypeSection();

	/// Store type section into memory
	/// stack pre: typeSectionOffset
	/// stack post: typeSectionOffset
	storeTypeSection();

	/// Store length of return section
	/// stack pre: typeSectionOffset
	/// stack post: typeSectionOffset
	storeSizeOfReturnSection();

	/// Store return section
	/// stack pre: typeSectionOffset
	/// stack post: typeSectionOffset
	storeReturnSection();

	/// Store length of data section
	/// stack pre: typeSectionOffset
	initSizeOfDataSection();
	/// stack post: typeSectionOffset dataSectionOffset
}

void ENIHandler::packData(TypePointer pType, Expression const &pExpr )
{
	// TODO: Do it exist better way to judge isLiteral?
	const string magicWord = "literal";
	bool isLiteral = pType->toString().substr(0, magicWord.size()) == magicWord;

	if( isLiteral )
	{
		auto lit = dynamic_cast<Literal const *>(&pExpr);
		handleLiteralPointer(lit->token(), lit->value());
	}
	else // assume it is identifier
	{
		handleIdentifierPointer(pType);
	}
}

void ENIHandler::packedToMemoryEnd() {
	/// Calculate and store size of data section
	/// stack pre: typeSectionOffset dataSectionOffset
	/// stack post: dataSectionOffset typeSectionOffset
	storeSizeOfDataSection();

	/// Set the name of ENI function
	/// stack pre: dataSectionOffset typeSectionOffset
	*m_Context << h256::Arith(h256(toBytes(m_FunctionName), h256::AlignLeft));
	/// stack post: dataSectionOffset typeSectionOffset eni_function_name
	*m_Context << Instruction::ENI;
}

void ENIHandler::clearENIObjects() {
	m_FunctionName = std::string("");
	m_TypeSection.clear();
	m_ParameterTypes.clear();
	m_ReturnSection.clear();
}

}
}
