pragma lity ^1.2.0;

contract C {
    struct Customer{
        int age;
        int discount;
        int id;
    }
    struct Car{
        bool freeParking;
        int ownerID;
    }

    Customer[] customers;
    Car[] cars;

    rule "Give 10% discount to customers older than 60"
    when {
        $customer : Customer( age > 60 );
    } then {
        $customer.discount = 1;
    }

    rule "Give free parking to customers older than 60"
        extends "Give 10% discount to customers older than 60"
    when {
        $car : Car ( ownerID == $customer.id );
    } then {
        $car.freeParking = true ;
    }

    function setup() public {

        customers.push(Customer(70, 0, 0));
        customers.push(Customer(50, 0, 1));
        cars.push(Car(false, 0));
        cars.push(Car(false, 1));

        for(uint i=0; i<2; i++){
            factInsert customers[i];
            factInsert cars[i];
        }

        fireAllRules;
    }

    function queryDiscount(uint cusID) public view returns (int) {
        return customers[cusID].discount;
    }

    function queryFreeParking(uint carID) public view returns (bool) {
        return cars[carID].freeParking;
    }
}
