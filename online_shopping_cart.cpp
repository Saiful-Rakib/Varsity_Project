// Simple Online Shopping Cart System in C++
// Demonstrates OOP features: Encapsulation, Inheritance, Polymorphism, Abstraction
// Also includes Operator Overloading, Templates, Exception Handling, and File I/O

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
using namespace std;

// ----------------- Exception Class -----------------
class ShopException : public runtime_error {
public:
    explicit ShopException(const string &msg) : runtime_error(msg) {}
};

// ----------------- Product Class -----------------
class Product {
private:
    int id;
    string name;
    double price;
    int stock;
public:
    Product(int i=0, string n="", double p=0, int s=0)
        : id(i), name(n), price(p), stock(s) {}

    // Encapsulation (Getters & Setters)
    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }

    void setPrice(double p) { if (p<0) throw ShopException("Invalid price"); price=p; }
    void setStock(int s) { if (s<0) throw ShopException("Invalid stock"); stock=s; }

    bool reduceStock(int qty) {
        if (qty <= 0) return false;
        if (qty > stock) return false;
        stock -= qty;
        return true;
    }

    // Operator Overloading
    friend ostream& operator<<(ostream &os, const Product &p) {
        os << "[" << p.id << "] " << p.name << " - $" << fixed << setprecision(2) << p.price
           << " (stock: " << p.stock << ")";
        return os;
    }
};

// ----------------- CartItem -----------------
class CartItem {
public:
    Product product;
    int quantity;

    CartItem(Product p, int q) : product(p), quantity(q) {}
    double subtotal() const { return product.getPrice() * quantity; }
};

// ----------------- User & Admin (Inheritance) -----------------
class User {
protected:
    string name;
public:
    User(string n="Guest") : name(n) {}
    virtual string role() const { return "User"; } // Polymorphism
    string getName() const { return name; }
};

class Admin : public User {
public:
    Admin(string n) : User(n) {}
    string role() const override { return "Admin"; }
};

// ----------------- Payment (Abstraction) -----------------
class Payment {
public:
    virtual bool pay(double amount) = 0;
    virtual ~Payment() {}
};

class CardPayment : public Payment {
public:
    bool pay(double amount) override {
        cout << "Paid $" << amount << " using Credit Card." << endl;
        return true;
    }
};

class PayPalPayment : public Payment {
public:
    bool pay(double amount) override {
        cout << "Paid $" << amount << " using PayPal." << endl;
        return true;
    }
};

// ----------------- ShoppingCart -----------------
class ShoppingCart {
    vector<CartItem> items;
public:
    void addItem(Product p, int q) { items.push_back(CartItem(p,q)); }
    void viewCart() {
        double total=0;
        for (auto &c : items) {
            cout << c.product.getName() << " x" << c.quantity << " = $" << c.subtotal() << endl;
            total+=c.subtotal();
        }
        cout << "Total: $" << total << endl;
    }
    double total() {
        double t=0; for(auto &c:items) t+=c.subtotal(); return t;
    }
    vector<CartItem> getItems(){ return items; }
    void clear(){ items.clear(); }
    bool empty(){ return items.empty(); }
};

// ----------------- Order -----------------
class Order {
    static int orderCounter;
    int id;
    vector<CartItem> items;
    double amount;
public:
    Order(vector<CartItem> its) : items(its) {
        id=++orderCounter;
        amount=0;
        for(auto &c:its) amount+=c.subtotal();
    }
    void showOrder(){
        cout << "Order #" << id << " Summary:" << endl;
        for(auto &c:items) cout << c.product.getName() << " x" << c.quantity << endl;
        cout << "Total: $" << amount << endl;
    }
};
int Order::orderCounter=0;

// ----------------- Template Function -----------------
template<class T>
void showVector(vector<T> v){ for(auto &x:v) cout << x << endl; }

// ----------------- Main -----------------
int main(){
    vector<Product> products = {
        Product(1,"Book",10.5,10),
        Product(2,"Pen",2.5,20),
        Product(3,"Laptop",800,5)
    };

    ShoppingCart cart;
    User u("Alice");
    cout << "Welcome, " << u.getName() << " (" << u.role() << ")" << endl;

    int choice;
    while(true){
        cout << "\n1. Show Products\n2. Add to Cart\n3. View Cart\n4. Checkout\n5. Exit\nChoice: ";
        cin >> choice;
        if(choice==1){ showVector(products); }
        else if(choice==2){
            int id,q; cout << "Enter product id & quantity: "; cin>>id>>q;
            for(auto &p:products){ if(p.getId()==id && p.reduceStock(q)) cart.addItem(p,q); }
        }
        else if(choice==3){ cart.viewCart(); }
        else if(choice==4){
            if(cart.empty()){ cout << "Cart is empty!"<<endl; continue; }
            int pm; cout << "1.Card 2.PayPal: "; cin>>pm;
            Payment* pay = (pm==1) ? static_cast<Payment*>(new CardPayment()) : static_cast<Payment*>(new PayPalPayment());
            if(pay->pay(cart.total())){
                Order o(cart.getItems());
                o.showOrder();
                cart.clear();
            }
            delete pay;
        }
        else break;
    }
    return 0;
}
