#include <string>
#include <tuple>
#include <boost/test/unit_test.hpp>
#include <test/libsolidity/SolidityExecutionFramework.h>

using namespace std;
using namespace dev::test;

namespace dev
{
namespace solidity
{
namespace test
{

static char const* contractCode = R"DELIMITER(
pragma solidity ^0.4.0;

contract MoneyGiveAway {
	struct Person {
		uint age;
		bool eligible;
		address addr;
	}
	rule "moneyToAdult" when {
		p: Person(age >= 18, eligible);
	} then {
		p.addr.transfer(1 wei);
		p.eligible = false;
	}
	Person[] ps;
	mapping (address => uint256) addr2idx;
	function numberOfPeople() public view returns (uint256) {
		return ps.length;
	}
	function addPerson(uint age) public {
		ps.push(Person(age, true, msg.sender));
		addr2idx[msg.sender] = factInsert ps[ps.length-1];
	}
	function deletePerson() public {
		factDelete addr2idx[msg.sender];
	}
	function pay() public {
		fireAllRules;
	}
}

)DELIMITER";

static unique_ptr<bytes> s_compiledContract;

class RulesTestFramework: public SolidityExecutionFramework
{
protected:
	void deployContract(u256 const& _value = 0)
	{
		if (!s_compiledContract)
			s_compiledContract.reset(new bytes(compileContract(contractCode, "MoneyGiveAway")));

		bytes args = encodeArgs();
		sendMessage(*s_compiledContract + args, true, _value);
		BOOST_REQUIRE(!m_output.empty());
	}
};

/// This is a test suite that tests optimised code!
BOOST_FIXTURE_TEST_SUITE(LityRules, RulesTestFramework)

BOOST_AUTO_TEST_CASE(creation)
{
	deployContract();
	BOOST_REQUIRE(callContractFunction("numberOfPeople()") == encodeArgs(u256(0)));
}

BOOST_AUTO_TEST_SUITE_END()

}
}
} // end namespaces
