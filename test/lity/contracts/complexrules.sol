pragma lity ^1.2.0;
contract C {
    struct Person {
        uint age;
        uint money;
    }

    rule "moneyGT10" when {
        p: Person(money > 10);
    } then {
        p.money += 100;
    }

    rule "moneyLT10" when {
        p: Person(money < 10);
    } then {
        p.money += 13;
    }

    rule "ageLT18" when {
        p: Person(age < 18);
    } then {
        p.money += 10;
    }

    mapping (address => uint) addr2idx;
    Person[] ps;

    function numberOfPeople() public view returns (uint) {
        return ps.length;
    }
    function balance(uint i) public view returns (uint) {
        return ps[i].money;
    }
    function addPerson(uint age) public {
        ps.push(Person(age, 0));
        addr2idx[msg.sender] = factInsert ps[ps.length-1];
        fireAllRules;
    }
}
