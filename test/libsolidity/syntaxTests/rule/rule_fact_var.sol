contract C {
    struct Person {
        int age;
    }

    rule "myRule" when {
        p: Person(age > 10);
    } then {
    }
}
// ----
