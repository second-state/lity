#include <boost/test/unit_test.hpp>
#include <test/lity/ContractExecutionFramework.h>

using namespace std;
using namespace dev::test;

namespace dev
{
namespace solidity
{
namespace test
{

static map<string, unique_ptr<bytes>> s_compiledContracts;

class RuleAttributeTestFramework: public ContractExecutionFramework
{
protected:
	void deployContract(string _contractFileName)
	{
		if (!s_compiledContracts[_contractFileName])
			s_compiledContracts[_contractFileName].reset(new bytes(compileContractFile(
				"test/lity/contracts/" + _contractFileName,
				"C")));

		bytes args = encodeArgs();
		sendMessage(*(s_compiledContracts[_contractFileName]) + args, true);
		BOOST_REQUIRE(!m_output.empty());
	}
};

/// This is a test suite that tests optimised code!
BOOST_FIXTURE_TEST_SUITE(LityRules, RuleAttributeTestFramework)

BOOST_AUTO_TEST_CASE(fizzbuzzTest) // this test is for `salience` keyword
{
	deployContract("fizzbuzz.sol");

	int N = 20;

	BOOST_REQUIRE(callContractFunction("setup(uint256)", u256(N)) == encodeArgs(true));
	BOOST_REQUIRE(callContractFunction("calc()") == encodeArgs(true));

	vector<bytes> v;
	bytes ans;

	for (int i = 0; i <= N; i++) {
		bool pushed = false;
		if (i%3 == 0) {
			v.push_back(encode(-3));
			ans += v.back();
			pushed = true;
		}
		if (i%5 == 0) {
			v.push_back(encode(-5));
			ans += v.back();
			pushed = true;
		}
		if (!pushed) {
			v.push_back(encode(i));
			ans += v.back();
		}
	}

	BOOST_REQUIRE(callContractFunction("getResult()") == encodeArgs(0x20, v.size()) + ans);
}

BOOST_AUTO_TEST_SUITE_END()

}
}
} // end namespaces
