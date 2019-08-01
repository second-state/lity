pragma lity ^1.2.0;

contract C {

    struct S {
        uint256 value;
    }

    S x;

    rule "increaseValue" salience 0 no_loop true
    when {
        s: S();
    } then {
        s.value++;
        update s;
    }

    rule "increaseValue2" salience 10 no_loop true
    when {
        s: S();
    } then {
        s.value += 2;
        update s;
    }

    function calc() public returns (uint256) {
        x.value = 1;
        factInsert x;
        fireAllRules; // should not terminate
        return x.value;
    }

    function () external payable { }
}

// ----
