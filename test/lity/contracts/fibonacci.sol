
// if you changed this file, you should also consider update
// the corresponding example in docs/rule-examples.rst

contract C {
    struct E {
        int256 index;
        int256 value;
    }

    rule "buildFibonacci" when {
        x1: E(value != -1);
        x2: E(value != -1, index == x1.index+1);
        x3: E(value == -1, index == x2.index+1);
    } then {
        x3.value = x1.value+x2.value;
        update x3;
    }

    E[] es;

    constructor() public {
        es.push(E(0, 0));
        factInsert es[es.length - 1];
        es.push(E(1, 1));
        factInsert es[es.length - 1];
        for (int i = 2 ; i < 10 ; i++) {
            es.push(E(i, -1));
            factInsert es[es.length - 1];
        }
    }

    function calc() public returns (bool) {
        fireAllRules;
        return true;
    }

    function get(uint256 x) public view returns (int256) {
        return es[x].value;
    }

    function () external payable { }
}
