#include <libsolidity/codegen/CompilerContext.h>
#include <libsolidity/codegen/DynArrUtils.h>
#include <libsolidity/codegen/CompilerUtils.h>

using namespace std;

namespace dev
{
namespace solidity
{

// Stack pre : reference
// Stack post:
void DynArrUtils::clearArray()
{
	m_context << 0 << Instruction::DUP2 << 32 << Instruction::ADD;
	// stack: refer 0 refer+32
	m_context << Instruction::MSTORE;
	// stack: refer
	m_context << 0 << Instruction::DUP2 << 64 << Instruction::ADD;
	// stack: refer 0 refer+64
	m_context << Instruction::MSTORE;
	// stack: refer
	m_context << 0 << Instruction::SWAP1;
	// stack: 0 refer
	m_context << Instruction::MSTORE;
}

// TODO: item with elementSize
// Stack pre : reference item
// Stack post:
void DynArrUtils::pushItem()
{
	eth::AssemblyItem noRealloc = m_context.newTag();

	// stack: refer item
	m_context << Instruction::DUP2;
	getLen();
	// stack: refer item len
	m_context << Instruction::DUP3;
	getCap();
	// stack: refer item len cap
	m_context << Instruction::EQ;
	// stack: len==cap
	m_context << 1 << Instruction::XOR;
	// stack: len!=cap
	m_context.appendConditionalJumpTo(noRealloc);

	// stack: refer item
	getCap();
	// stack: refer item cap
	m_context << 1 << Instruction::ADD << 2 << Instruction::MUL;
	// stack: refer item (cap+1)*2
	m_context << Instruction::DUP3 << Instruction::SWAP1;
	// stack: refer item refer (cap+1)*2
	reAlloc();

	m_context << noRealloc;
	// stack: refer item
	m_context << Instruction::DUP2;
	// stack: refer item refer
	incrLen();
	// stack: refer item
	m_context << Instruction::DUP2 << Instruction::DUP1;
	// stack: refer item refer refer
	getLen();
	m_context << 1 << Instruction::SUB;
	// stack: refer item refer len'-1
	accessIndex(false);
	// stack: refer item memAddrToBePlaced
	m_context << Instruction::MSTORE; // TODO: item with elementSize
	m_context << Instruction::POP;
}

// Stack pre : reference
// Stack post:
void DynArrUtils::popItem()
{
	m_context << Instruction::DUP1;
	getLen();
	// stack refer len
	m_context << 1 << Instruction::SUB;
	// stack refer len'
	m_context << Instruction::DUP2 << 32 << Instruction::ADD;
	// stack refer len' refer+32
	m_context << Instruction::MSTORE;
	// stack refer
	m_context << Instruction::POP;
}

// Stack pre : reference cap'
// Stack post:
void DynArrUtils::reAlloc()
{
	m_context << Instruction::DUP1;
	// stack: refer cap' cap'
	m_context << Instruction::DUP3;
	// stack: ... cap' refer
	m_context << 64 << Instruction::ADD;
	// stack: ... cap' refer+64
	m_context << Instruction::MSTORE;
	// stack: refer cap'
	utils().allocateMemory();
	// stack: refer dataPtr'
	
	m_context << Instruction::DUP2 << Instruction::MLOAD;
	// stack: refer dataPtr' dataPtr
	m_context << Instruction::DUP3 << 32 << Instruction::ADD;
	// stack: refer dataPtr' dataPtr refer+32
	m_context << Instruction::MLOAD;
	// stack: refer dataPtr' dataPtr len
	m_context << 32*elementSize << Instruction::MUL;
	// stack: refer dataPtr' dataPtr len*32*eleSize
	m_context << Instruction::SWAP2 << Instruction::SWAP1;
	// stack: refer len*32*eleSize dataPtr' dataPtr
	utils().memoryCopy32();
	// stack: refer
	m_context << Instruction::POP;
}

// Stack pre : reference
// Stack post:
void DynArrUtils::incrLen()
{
	getLen();
	// stack: refer len
	m_context << 1 << Instruction::ADD;
	// stack: refer len'
	m_context << Instruction::DUP2 << 32 << Instruction::ADD;
	// stack: refer len' refer+32
	m_context << Instruction::MSTORE << Instruction::POP;
}

// Stack pre: reference index
// Stack post: memory_offset
void DynArrUtils::accessIndex(bool _doBoundsCheck)
{
	if(_doBoundsCheck)
	{
		// TODO
	}
	m_context << 32*elementSize << Instruction::MUL;
	// refer index*32*eleSize
	m_context << Instruction::SWAP1 << Instruction::MLOAD;
	// 32*index*eleSize dataPtr
	m_context << Instruction::ADD << Instruction::MLOAD;
}

/// Stack pre : reference
/// Stack post:
/// Stack state of f:
///   Stack pre : elmtMemAddr
///   Stack post:
void DynArrUtils::forEachDo(std::function<void(CompilerContext&)> f)
{
	eth::AssemblyItem loopStart = m_context.newTag();
	eth::AssemblyItem loopEnd = m_context.newTag();
	m_context << Instruction::DUP1 << Instruction::MLOAD;
	// stack: refer
	m_context << Instruction::DUP2;
	getLen();
	// stack: refer len
	m_context << 0;
	// stack: refer len i
	m_context << loopStart;                                  // loop:
	m_context << Instruction::DUP2 << Instruction::DUP2;
	// stack: ... len i
	m_context << Instruction::LT;       //   if i>=len
	m_context << 1 << Instruction::XOR;
	// stack: ... !(len>i)
	m_context.appendConditionalJumpTo(loopEnd);              //     break
	// stack: refer len i
	m_context << Instruction::DUP3 << Instruction::DUP2;
	// stack: ... refer i
	accessIndex(false);
	// stack: ... elmtMemAddr
	f(m_context);
	// stack: refer len i
	m_context << 1 << Instruction::ADD;                      //   i++
	m_context.appendJumpTo(loopStart);
	m_context << loopEnd;                                    // loopEnd:
	// stack: refer len i
	for(int i=0; i<3; i++)
		m_context << Instruction::POP;
}

CompilerUtils DynArrUtils::utils()
{
	return CompilerUtils(m_context);
}


}
}