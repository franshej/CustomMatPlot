#include "cmp_datamodels.h"
#include "cmp_test_helper.hpp"
#include <string>

using namespace cmp;

// Test observer class
class TestObserver : public Observer<int> {
 public:
  void observableValueUpdated(ObserverId id, const int& newValue) override {
    lastUpdatedId = id;
    lastValue = newValue;
    updateCount++;
  }

  ObserverId lastUpdatedId;
  int lastValue = 0;
  int updateCount = 0;
};

SECTION(ObservableTest, "Observable class tests") {

  TEST("Single observer receives updates") {
    // Create an Observable with initial value 0
    Observable<int> observable(ObserverId::XLim, 0);

    // Create a TestObserver instance
    TestObserver observer;

    // Add the observer to the observable
    observable.addObserver(observer);

    // Update the observable's value
    observable = 42;

    // Check if observer received the update
    expect(observer.lastUpdatedId == ObserverId::XLim);
    expectEquals(observer.lastValue, 42);
    expectEquals(observer.updateCount, 2);
  }

  TEST("Multiple observers receive updates") {
    Observable<int> observable(ObserverId::XLim, 0);
    TestObserver observer1;
    TestObserver observer2;

    // Add multiple observers
    observable.addObserver(observer1, observer2);

    // Update the observable's value
    observable = 100;

    // Check if both observers received the update
    expectEquals(observer1.lastValue, 100);
    expectEquals(observer1.updateCount, 2);
    expectEquals(observer2.lastValue, 100);
    expectEquals(observer2.updateCount, 2);
  }

  TEST("Observer not notified after value remains the same") {
    Observable<int> observable(ObserverId::XLim, 50);
    TestObserver observer;
    observable.addObserver(observer);

    // Assign the same value
    observable = 50;

    // Observer should still be notified
    expectEquals(observer.updateCount, 2);
  }

  TEST("Observable conversion to value type") {
    Observable<int> observable(ObserverId::XLim, 10);

    // Use the observable as an int
    int value = observable;

    expectEquals(value, 10);
  }

  TEST("Pointer-like access to observable's value") {
    Observable<std::string> observable(ObserverId::XLim, "initial");
    expectEquals(*(observable.operator->()), std::string("initial"));

    // Modify the value 
    observable = std::string("updated");
    expectEquals(*observable.operator->(), std::string("updated"));
  }

  TEST("Multiple observables with same observer") {
    TestObserver observer;
    Observable<int> observable1(ObserverId::XLim, 0);
    Observable<int> observable2(ObserverId::YLim, 0);
    
    observable1.addObserver(observer);
    expectEquals(observer.updateCount, 1);
    
    observable2.addObserver(observer);
    expectEquals(observer.updateCount, 2);
    
    observable1 = 42;
    expectEquals(static_cast<int>(observer.lastUpdatedId), static_cast<int>(ObserverId::XLim));
    expectEquals(observer.lastValue, 42);
    expectEquals(observer.updateCount, 3);

    observable2 = 100;
    expectEquals(static_cast<int>(observer.lastUpdatedId), static_cast<int>(ObserverId::YLim));
    expectEquals(observer.lastValue, 100);
    expectEquals(observer.updateCount, 4);
  }

  TEST("Observable cleanup when going out of scope") {
    TestObserver observer1;

    {
      Observable<int> observable(ObserverId::XLim, 0);
      {
        TestObserver observer2;
        observable.addObserver(observer1, observer2);
        observable = 1337;

        expectEquals(observer1.lastValue, 1337);
        expectEquals(observer1.updateCount, 2);
        expectEquals(observer2.lastValue, 1337);
        expectEquals(observer2.updateCount, 2);
      }

      observable = 42;
      expectEquals(observer1.updateCount, 3);
      expectEquals(observer1.lastValue, 42);
    }
  }
}
