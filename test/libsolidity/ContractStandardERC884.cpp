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
/**
 * Unit tests for ERC884 standard detection
 */

#include <test/libsolidity/AnalysisFramework.h>

#include <boost/test/unit_test.hpp>

#include <string>

using namespace std;

namespace dev
{
namespace solidity
{
namespace test
{

class ERC884ContractFramework: public AnalysisFramework
{
protected:
	virtual std::pair<SourceUnit const*, ErrorList>
	parseAnalyseAndReturnError(
		std::string const& _source,
		bool _reportWarnings = false,
		bool _insertVersionPragma = true,
		bool _allowMultipleErrors = true
	) override
	{
		m_compiler.setContractStandard("ERC884");
		return AnalysisFramework::parseAnalyseAndReturnError(
			_source,
			_reportWarnings,
			_insertVersionPragma,
			_allowMultipleErrors
		);
	}
};

BOOST_FIXTURE_TEST_SUITE(ContractStandardERC884, ERC884ContractFramework)

BOOST_AUTO_TEST_CASE(erc884_normal_contract)
{
	string text = R"(
contract C { function f(uint a, uint b) public pure returns (uint) { return a + b; } }
	)";
	CHECK_INFO(text, "Missing 'VerifiedAddressAdded'");
}
BOOST_AUTO_TEST_CASE(erc884_standard)
{
	string text = R"(
contract C20 {
  function totalSupply() public view returns (uint256);
  function balanceOf(address who) public view returns (uint256);
  function transfer(address to, uint256 value) public returns (bool);
  function allowance(address owner, address spender) public view returns (uint256);
  function transferFrom(address from, address to, uint256 value) public returns (bool);
  function approve(address spender, uint256 value) public returns (bool);
  event Approval(address indexed owner, address indexed spender, uint256 value);
  event Transfer(address indexed from, address indexed to, uint256 value);
}
contract X is C20 {
	event VerifiedAddressAdded( address indexed addr, bytes32 hash, address indexed sender);
	event VerifiedAddressRemoved(address indexed addr, address indexed sender);
	event VerifiedAddressUpdated( address indexed addr, bytes32 oldHash, bytes32 hash, address indexed sender);
	event VerifiedAddressSuperseded( address indexed original, address indexed replacement, address indexed sender);
	function addVerified(address addr, bytes32 hash) public;
	function removeVerified(address addr) public;
	function updateVerified(address addr, bytes32 hash) public;
	function cancelAndReissue(address original, address replacement) public;
	function transfer(address to, uint256 value) public returns (bool);
	function transferFrom(address from, address to, uint256 value) public returns (bool);
	function isVerified(address addr) public view returns (bool);
	function isHolder(address addr) public view returns (bool);
	function hasHash(address addr, bytes32 hash) public view returns (bool);
	function holderCount() public view returns (uint);
	function holderAt(uint256 index) public view returns (address);
	function isSuperseded(address addr) public view returns (bool);
	function getCurrentFor(address addr) public view returns (address);
}
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"X is compatible",
		"C20 is not compatible",
	}));
}
BOOST_AUTO_TEST_CASE(erc884_standard_wrong)
{
	string text = R"(
contract E {
	event Transfer(address indexed _from, address indexed _to, uint256 _tokenId);
	event Approval(address indexed _owner, address indexed _approved, uint256 _tokenId);
	event ApprovalForAll(address indexed _owner, address indexed _operator, bool _approved);
	function balanceOf(address _owner) external view returns (uint256);
	function ownerOf(uint256 _tokenId) external view returns (address);
	function safeTransferFrom(address _from, address _to, uint256 _tokenId, bytes data) external payable;
	function safeTransferFrom(address _from, address _to, uint256 _tokenId) external payable;
	function transferFrom(address _from, address _to, uint256 _tokenId) external; // missing payable
	function approve(address _approved, uint256 _tokenId) external payable;
	function setApprovalForAll(address _operator, bool _approved) external;
	function getApproved(uint256 _tokenId) external view returns (address);
	function isApprovedForAll(address _owner, address _operator) external view returns (bool);
	function supportsInterface(bytes4 interfaceID) external view returns (bool);
}
	)";
	CHECK_INFO(text, "Missing 'VerifiedAddressAdded'");
}
BOOST_AUTO_TEST_CASE(erc884_standard_wrong_modification_level)
{
	string text = R"(
contract C20 {
  function totalSupply() public view returns (uint256);
  function balanceOf(address who) public view returns (uint256);
  function transfer(address to, uint256 value) public returns (bool);
  function allowance(address owner, address spender) public view returns (uint256);
  function transferFrom(address from, address to, uint256 value) public returns (bool);
  function approve(address spender, uint256 value) public returns (bool);
  event Approval(address indexed owner, address indexed spender, uint256 value);
  event Transfer(address indexed from, address indexed to, uint256 value);
}
contract X is C20 {
	event VerifiedAddressAdded( address indexed addr, bytes32 hash, address indexed sender);
	event VerifiedAddressRemoved(address indexed addr, address indexed sender);
	event VerifiedAddressUpdated( address indexed addr, bytes32 oldHash, bytes32 hash, address indexed sender);
	event VerifiedAddressSuperseded( address indexed original, address indexed replacement, address indexed sender);
	function addVerified(address addr, bytes32 hash) public;
	function removeVerified(address addr) public;
	function updateVerified(address addr, bytes32 hash) public;
	function cancelAndReissue(address original, address replacement) public;
	function transfer(address to, uint256 value) public returns (bool);
	function transferFrom(address from, address to, uint256 value) public returns (bool);
	function isVerified(address addr) public view returns (bool);
	function isHolder(address addr) public view returns (bool);
	function hasHash(address addr, bytes32 hash) public view returns (bool);
	function holderCount() public view returns (uint);
	function holderAt(uint256 index) public view returns (address);
	function isSuperseded(address addr) public view returns (bool);
	function getCurrentFor(address addr) public returns (address); // should be a view function
}
	)";
	CHECK_INFO_ALLOW_MULTI(text, (std::vector<string>{
		"Missing 'getCurrentFor'",
		"C20 is not compatible",
	}));
}

BOOST_AUTO_TEST_SUITE_END()
}
}
}
