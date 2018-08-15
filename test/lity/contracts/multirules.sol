contract C {
    struct Person {
        uint age;
        uint balance;
    }

    rule "ageLT18" when {
        p: Person(age < 18);
    } then {
        p.balance += 10;
    }

    rule "ageGT20" when {
        p: Person(age > 20);
    } then {
        p.balance += 7;
    }

    mapping (address => uint) addr2idx;
    Person[] ps;

    function numberOfPeople() public view returns (uint) {
        return ps.length;
    }
    function balance(uint i) public view returns (uint) {
        return ps[i].balance;
    }
    function addPerson(uint age) public {
        ps.push(Person(age, 0));
        addr2idx[msg.sender] = factInsert ps[ps.length-1];
        fireAllRules;
    }
}
