
#pragma once

#include <memory>
#include <functional>

namespace dev
{
namespace solidity
{

class CompilerContext;
class Type;
using TypePointer = std::shared_ptr<Type const>;

/**
 * Class that provides code generation for dynamic memory (int/reference) arrays.
 * memory address of the array struct must be put on stack
 * array struct layout: [dataPtr len cap]
 *   dataPtr points to memory address of the data
 *   len is the length of this array (the number of elements)
 *   cap is the capacity allocated of this array
 * if len is greater than cap after push, 
 * 2*cap-size mem space would be allocated
 * and data would be copied to that space
 */
class DynArrUtils
{
public:
	explicit DynArrUtils(CompilerContext& _context, int _elementSize): m_context(_context), elementSize(_elementSize) {}

	/// Stack pre : reference
	/// Stack post:
	void clearArray();

	/// This might reallocates memory.
	/// Stack pre : referrence item
	/// Stack post:
	void pushItem();

	/// Stack pre : referrence
	/// Stack post:
	void popItem();
	
	/// Stack pre : referrence
	/// Stack post: len'
	void incrLen();

	/// Stack pre : referrence
	/// Stack post: len
	void getLen() { m_context << 32 << Instruction::ADD << Instruction::MLOAD; }

	/// reallocate memory space and copy data to the new space
	/// Stack pre : refer cap'
	/// Stack post:
	void reAlloc();

	/// Note that one more MLOAD is required to get the item value(ref).
	/// Stack pre: reference index
	/// Stack post: memory_offset
	void accessIndex(bool _doBoundsCheck = true);
	void forEachDo(std::function<void(CompilerContext&)> f);
private:

	/// Stack pre : referrence
	/// Stack post: cap
	void getCap() { m_context << 64 << Instruction::ADD << Instruction::MLOAD; }
	CompilerUtils utils();

	CompilerContext& m_context;
	const int elementSize; //< Size of element (number of Word)
};

}
}
