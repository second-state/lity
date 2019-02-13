contract C {
    struct Person {
        int age;
        bool eligible;
        address payable addr;
    }

    rule "myRule" when {
        p: Person(age > 10, eligible);
    } then {
        p.addr.transfer(10);
        p.eligible = false;
    }
}
// ----
