/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <libsolidity/ast/Types.h>
#include <libsolidity/ast/ASTVisitor.h>

#include <map>
#include <memory>

namespace dev
{
namespace solidity
{

using FunctionSpecification = std::pair<std::string, std::shared_ptr<FunctionType const>>;
using FunctionSpecifications = std::vector<FunctionSpecification>;
using FunctionSignatureHash = std::pair<std::string, u256>;
using FunctionSignatureHashes = std::vector<FunctionSignatureHash>;

class ErrorReporter;

class BasicContractStandard; // forward declaration

class ContractStandardChecker
{
public:
	enum Mode
	{
		/// Notify user in both cases of compatible and non-compatible.
		WarnAll,
		/// Notify user if the contract is compatible to given standard,
		/// otherwise, notify user only if user have implemented at least one required function.
		WarnIfAtLeastOneFunctionImplemented,
	};

	/// @param _errorReporter provides the error logging functionality.
	/// @param _contractStandardStr specifies which contract standard to be checked,
	ContractStandardChecker(ErrorReporter& _errorReporter, boost::optional<std::string> _contractStandardStr = boost::none);

	/// @returns true iff no errors are found.
	/// Currently always return true since contract standard violation is not an error
	bool checkContractStandard(ContractDefinition const& _contract);

private:
	ErrorReporter& m_errorReporter;
	std::vector<std::shared_ptr<BasicContractStandard>> m_contractStandards;
	Mode m_mode = WarnAll;
};

class BasicContractStandard: private boost::noncopyable, public std::enable_shared_from_this<BasicContractStandard>, private ASTConstVisitor
{
public:
	/// Check if a contract definition is compatible to this standard.
	virtual bool checkCompatibility(ContractDefinition const& _contract, ErrorReporter& _reporter, ContractStandardChecker::Mode _mode) = 0;
	virtual std::string name() const = 0;
protected:
	bool checkFunctionExistence
	(
		ContractDefinition const& _contract,
		FunctionSpecifications const& _requiredFunctions,
		ErrorReporter& _reporter,
		ContractStandardChecker::Mode const _mode
	);
	bool checkFunctionCallExistence
	(
		ContractDefinition const& _contract,
		FunctionSignatureHashes const& _requiredFunctionCalls,
		ErrorReporter& _reporter
	);
private:
	virtual bool visit(ContractDefinition const& _contract) override;
	virtual bool visit(MemberAccess const& _ma) override;
	/// Mark an ASTNode as found while traversing AST
	std::function<void(ASTNode const*)> m_touch;
};

class ERC20ContractStandard: public BasicContractStandard
{
public:
	virtual bool checkCompatibility(ContractDefinition const& _contract, ErrorReporter& _reporter, ContractStandardChecker::Mode _mode) override;
	virtual std::string name() const override;
protected:
	/// @returns the functions which must be defined in ERC20 standard.
	FunctionSpecifications const& requiredFunctions() const;
};

// Note that an ERC223 contract is not necessary an ERC20 contract, because ERC223 don't have approve* functions
class ERC223ContractStandard: public BasicContractStandard
{
public:
	virtual bool checkCompatibility(ContractDefinition const& _contract, ErrorReporter& _reporter, ContractStandardChecker::Mode _mode) override;
	virtual std::string name() const override;
protected:
	/// @returns the functions which must be defined in ERC223 standard.
	FunctionSpecifications const& requiredFunctions() const;

	/// @returns the function (signatures) which must be called somewhere in the contract
	FunctionSignatureHashes const& requiredFunctionCalls() const;
};

class ERC721ContractStandard: public BasicContractStandard
{
public:
	virtual bool checkCompatibility(ContractDefinition const& _contract, ErrorReporter& _reporter, ContractStandardChecker::Mode _mode) override;
	virtual std::string name() const override;
protected:
	/// @returns the functions which must be defined in ERC721 standard.
	FunctionSpecifications const& requiredFunctions() const;
};

// ERC827 is ERC20 compatible, i.e every ERC827 contract should also implement ERC20 interface
class ERC827ContractStandard: public ERC20ContractStandard
{
public:
	virtual bool checkCompatibility(ContractDefinition const& _contract, ErrorReporter& _reporter, ContractStandardChecker::Mode _mode) override;
	virtual std::string name() const override;
protected:
	/// @returns the functions which must be defined in ERC827 standard.
	FunctionSpecifications const& requiredFunctions() const;
};

// ERC884 is ERC20 compatible, i.e every ERC884 contract should also implement ERC20 interface
class ERC884ContractStandard: public ERC20ContractStandard
{
public:
	virtual bool checkCompatibility(ContractDefinition const& _contract, ErrorReporter& _reporter, ContractStandardChecker::Mode _mode) override;
	virtual std::string name() const override;
protected:
	/// @returns the functions which must be defined in ERC884 standard.
	FunctionSpecifications const& requiredFunctions() const;
};

}
}
