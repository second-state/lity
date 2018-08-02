contract C {
    struct Person {
        int age;
        bool eligible;
    }

    rule "myFirstRule" when {
        p: Person(age > 10, eligible);
    } then {
        p.eligible = false;
    }

    rule "mySecondRule" when {
        p: Person(age > 20, eligible);
    } then {
        p.eligible = false;
    }
}
// ----
