
#include <iostream>
#include <map>
#include <memory>
#include <string>

struct DeathlyHallows
{
  bool elder_wand;
  bool resurrection_stone;
  bool invisibility_cloak;
};

std::ostream& operator<<(std::ostream& stream, const DeathlyHallows& hallows)
{
  auto bprint = [](bool v, const std::string& name) {
    return (v ? "[x] " : "[ ] ") + name;
  };

  stream << bprint(hallows.elder_wand, "ElderWand"); 
  stream << "    ";
  stream << bprint(hallows.resurrection_stone, "Resurrection Stone");
  stream << "    ";
  stream << bprint(hallows.invisibility_cloak, "Invisibility Cloak");
  return stream;
}

auto he_who_must_not_be_named()
{
  // C++ authorizes the definition of a class inside a function.
  // That class cannot be named elsewhere, but since C++11, we 
  // can return an object that is an instance of this class by 
  // using 'auto' as return type.
  struct Voldemort
  {
    DeathlyHallows deathly_hallows;
  };

  return Voldemort{ {true, false, false} };
}

int main()
{
  // We cannot name the type of 'he_who_must_not_be_named()' so we have to use 'auto'.
  // In particular, we cannot write: `Voldemort you_know_who = he_who_must_not_be_named();`.
  auto you_know_who = he_who_must_not_be_named();
  std::cout << you_know_who.deathly_hallows << std::endl;

  // Well, actually, we can give a name to a "Voldemort type" by a combination of 
  // 'using' and 'decltype'. 😊
  // Dangerous code, though! At least in this context; naming the dark lord may have consequences... 😱
  using DarkLord = decltype(he_who_must_not_be_named());
  DarkLord tom_riddle{ {true, true, true} };
  std::cout << tom_riddle.deathly_hallows << std::endl; // Unlimited Power!

  void factory_with_voldemort_types();
  factory_with_voldemort_types();
}


// One plausibly reasonable use case for Voldemort types would be for 
// an anonymous concrete creator in a factory design pattern (as suggested 
// here http://videocortex.io/2017/Bestiary/#-voldemort-types).
// Read more about the factory design pattern: https://refactoring.guru/design-patterns/factory-method

class Product
{
public:
  virtual ~Product() = default;
};

class Car : public Product { };

class FactoryInterface
{
public:
  virtual ~FactoryInterface() = default;
  virtual Product* getProduct() = 0;
};

using Factory = std::map<std::string, std::unique_ptr<FactoryInterface>>;

void register_car_factory(Factory& factory)
{
  // Since no one needs to know the actual class that creates cars,
  // we can use a voldemort type to considerably restrict access to the class.
  class CarFactory : public FactoryInterface
  {
    Product* getProduct() override { return new Car; }
  };

  factory["car"] = std::make_unique<CarFactory>();
}

Product* build_car(Factory& factory)
{
  return factory.at("car")->getProduct();
}

void factory_with_voldemort_types() 
{   
  Factory factory; 
  register_car_factory(factory);
  Product* p = build_car(factory);
  std::cout << (dynamic_cast<Car*>(p) ? true : false) << std::endl;
}
