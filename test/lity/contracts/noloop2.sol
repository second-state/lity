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

    rule "increaseValue2" salience 10 lock_on_active true
    when {
        s: S();
    } then {
        s.value += 2;
        update s;
    }

    function calc() public returns (uint256) {
        x.value = 1;
        factInsert x;
        fireAllRules;
        return x.value; // should be 4
    }

    function () external { }
}

// ----
