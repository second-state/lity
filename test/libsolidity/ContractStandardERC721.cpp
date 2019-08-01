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
 * Unit tests for ERC721 standard detection
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

class ERC721ContractFramework: public AnalysisFramework
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
		m_compiler.setContractStandard("ERC721");
		return AnalysisFramework::parseAnalyseAndReturnError(
			_source,
			_reportWarnings,
			_insertVersionPragma,
			_allowMultipleErrors
		);
	}
};

BOOST_FIXTURE_TEST_SUITE(ContractStandardERC721, ERC721ContractFramework)

BOOST_AUTO_TEST_CASE(erc721_normal_contract)
{
	string text = R"(
contract C { function f(uint a, uint b) public pure returns (uint) { return a + b; } }
	)";
	CHECK_INFO(text, "Missing 'Transfer'");
}
BOOST_AUTO_TEST_CASE(erc721_standard)
{
	string text = R"(
contract E {
	event Transfer(address indexed _from, address indexed _to, uint256 _tokenId);
	event Approval(address indexed _owner, address indexed _approved, uint256 _tokenId);
	event ApprovalForAll(address indexed _owner, address indexed _operator, bool _approved);
	function balanceOf(address _owner) external view returns (uint256);
	function ownerOf(uint256 _tokenId) external view returns (address);
	function safeTransferFrom(address _from, address _to, uint256 _tokenId, bytes calldata data) external payable;
	function safeTransferFrom(address _from, address _to, uint256 _tokenId) external payable;
	function transferFrom(address _from, address _to, uint256 _tokenId) external payable;
	function approve(address _approved, uint256 _tokenId) external payable;
	function setApprovalForAll(address _operator, bool _approved) external;
	function getApproved(uint256 _tokenId) external view returns (address);
	function isApprovedForAll(address _owner, address _operator) external view returns (bool);
	function supportsInterface(bytes4 interfaceID) external view returns (bool);
}
	)";
	CHECK_INFO(text, "E is compatible");
}
BOOST_AUTO_TEST_CASE(erc721_standard_wrong_modification_level)
{
	string text = R"(
contract E {
	event Transfer(address indexed _from, address indexed _to, uint256 _tokenId);
	event Approval(address indexed _owner, address indexed _approved, uint256 _tokenId);
	event ApprovalForAll(address indexed _owner, address indexed _operator, bool _approved);
	function balanceOf(address _owner) external view returns (uint256);
	function ownerOf(uint256 _tokenId) external view returns (address);
	function safeTransferFrom(address _from, address _to, uint256 _tokenId, bytes calldata data) external payable;
	function safeTransferFrom(address _from, address _to, uint256 _tokenId) external payable;
	function transferFrom(address _from, address _to, uint256 _tokenId) external; // missing payable
	function approve(address _approved, uint256 _tokenId) external payable;
	function setApprovalForAll(address _operator, bool _approved) external;
	function getApproved(uint256 _tokenId) external view returns (address);
	function isApprovedForAll(address _owner, address _operator) external view returns (bool);
	function supportsInterface(bytes4 interfaceID) external view returns (bool);
}
	)";
	CHECK_INFO(text, "Missing 'transferFrom'");
}

BOOST_AUTO_TEST_SUITE_END()
}
}
}
