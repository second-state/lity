pragma lity ^1.2.0;

contract C {

    struct S {
        uint256 value;
        // manually mark visited rules as done, this is workaround before no-loop's implementation
        bool[30] doneFizz;
        bool[30] doneBuzz;
    }

    uint256 N;

    S x;

    uint256[] result;

    uint256 fizz = 115792089237316195423570985008687907853269984665640564039457584007913129639933; // -3
    uint256 buzz = 115792089237316195423570985008687907853269984665640564039457584007913129639931; // -5

    rule "increaseValue" salience 0
    when {
        s: S(value+1 <= N);
    } then {
        s.value++;
        update s;
    }

    rule "ruleNumber" salience 10
    when {
        s: S(value <= N, !doneFizz[value], !doneBuzz[value]);
    } then {
        result.push(s.value);
    }

    // salience higher than buzz, because when the value is a multiple of 15,
    // we want to print "fizzbuzz", not "buzzfizz"
    rule "ruleFizz" salience 30
    when {
        s: S(value % 3 == 0, value <= N, !doneFizz[value]);
    } then {
        result.push(fizz);
        s.doneFizz[s.value] = true;
        update s;
    }

    rule "ruleBuzz" salience 20
    when {
        s: S(value % 5 == 0, value <= N, !doneBuzz[value]);
    } then {
        result.push(buzz);
        s.doneBuzz[s.value] = true;
        update s;
    }

    function setup(uint256 n) public returns (bool) {
        x.value = 0;
        N = n;
        factInsert x;
        return true;
    }

    function calc() public returns (bool) {
        fireAllRules;
        return true;
    }

    function getValue() public view returns (uint256) {
        return x.value;
    }

    function getN() public view returns (uint256) {
        return N;
    }

    function getResult() public view returns (uint256[] memory) {
        return result;
    }

    function () external payable { }
}

// ----
