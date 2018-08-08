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

static unique_ptr<bytes> s_compiledContract;

class SmokeTestFramework: public ContractExecutionFramework
{
protected:
	void deployContract()
	{
		if (!s_compiledContract)
			s_compiledContract.reset(new bytes(compileContractFile(
				"test/lity/contracts/smoke.sol",
				"Smoke")));

		bytes args = encodeArgs();
		sendMessage(*s_compiledContract + args, true);
		BOOST_REQUIRE(!m_output.empty());
	}
};

/// This is a test suite that tests optimised code!
BOOST_FIXTURE_TEST_SUITE(LitySmoke, SmokeTestFramework)

BOOST_AUTO_TEST_CASE(creation)
{
	deployContract();
	BOOST_REQUIRE(callContractFunction("numberOfElements()") == encodeArgs(u256(0)));
}

BOOST_AUTO_TEST_SUITE_END()

}
}
} // end namespaces
